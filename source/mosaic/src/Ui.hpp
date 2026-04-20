#include "imgui.h"
#include <string>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <iostream>

// Inclusions de tes headers
#include "lib/ImageBase.h"
#include "Mosaique2.hpp"
#include "imagesInfo.hpp"
#include "Mask.hpp"

std::string OpenFileDialog() {
    char buffer[128];
    std::string result = "";
    // On appelle Zenity (explorateur de fichiers standard sous Linux)
    // --file-selection : ouvre l'explorateur
    // --filter : limite aux formats d'images
    FILE* pipe = popen("zenity --file-selection --title='Choisir une image' --file-filter='Images (ppm, pgm, jpg, png) | *.ppm *.pgm *.jpg *.png'", "r");
    if (!pipe) return "";
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    // On retire le caractère de nouvelle ligne à la fin
    if (!result.empty() && result.back() == '\n') result.pop_back();
    
    return result;
}

std::string OpenFolderDialog() {
    char buffer[1024];
    std::string result = "";
    // L'option --directory est la clé ici
    FILE* pipe = popen("zenity --file-selection --directory --title='Choisir le dossier de la base de données'", "r");
    if (!pipe) return "";
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);

    if (!result.empty() && result.back() == '\n') result.pop_back();
    
    return result;
}

// Fonction qui contient toute la logique de l'interface graphique
void RenderMosaicUI() {
    // === 1. ÉTAT DE L'APPLICATION (Variables statiques pour persister entre les frames) ===
    static char inputImagePath[256] = "ImageIn.ppm";
    static char dbPath[256] = "database.bin";
    
    static int mosaicMode = 0; // 0 = Standard, 1 = Quadtree, 2 = SNIC
    static int topK = 10;
    static bool repetition = false;

    // Paramètres spécifiques
    static float percentGrid = 0.02f;
    static int seuilVariance = 500;
    static int grilleMin = 2;
    static int numSuperPixels = 6000;
    static float snicCompactness = 10.0f;

    // Variables de résultats
    static double psnr1 = 0.0, ssim1 = 0.0;
    static double psnr2 = 0.0, ssim2 = 0.0;
    static int numTilesGenerated = 0;
    static bool hasGeneratedPass1 = false;
    static bool hasGeneratedPass2 = false;
    static bool isProcessing = false;

    // Données lourdes persistantes
    static ImageBase imIn;
    static ImageBase imOut1;
    static ImageBase imOut2;
    static std::vector<Tile> currentTiles;
    static std::vector<ImgInfo> currentRegionInfos;
    static std::vector<int> currentDistances;
    static std::vector<int> currentSnicLabels;


    // === 2. FENÊTRE IMGUI ===
    ImGui::Begin("Générateur de Mosaïque");

    // --- Section Fichiers ---
    ImGui::Text("Fichiers sources");

    // 1. Image d'entrée (Fichier)
    ImGui::InputText("##inputPath", inputImagePath, IM_ARRAYSIZE(inputImagePath));
    ImGui::SameLine(); 
    if (ImGui::Button("Parcourir...##img")) {
        std::string path = OpenFileDialog(); // Ta fonction précédente pour les fichiers
        if (!path.empty()) strncpy(inputImagePath, path.c_str(), sizeof(inputImagePath)-1);
    }
    ImGui::SameLine(); ImGui::Text("Image cible (.ppm)");

    // 2. Dossier de la BDD (Dossier)
    static char dbFolderPath[512] = "./database"; // Buffer pour le chemin du dossier
    ImGui::InputText("##dbFolder", dbFolderPath, IM_ARRAYSIZE(dbFolderPath));
    ImGui::SameLine();
    if (ImGui::Button("Choisir Dossier...##bin")) {
        std::string path = OpenFolderDialog(); // La nouvelle fonction pour dossiers
        if (!path.empty()) {
            strncpy(dbFolderPath, path.c_str(), sizeof(dbFolderPath)-1);
        }
    }
    ImGui::SameLine(); ImGui::Text("Dossier des .bin");

    // Bouton de chargement
    if (ImGui::Button("Charger la Base de Données", ImVec2(ImGui::GetContentRegionAvail().x, 25))) {
        // Ici, on passe le chemin du dossier à ton algo
        // Supposons que tu aies une fonction qui scanne le dossier :
        initImgInfosFromBin(dbFolderPath); 
        ImGui::OpenPopup("Chargement Terminé");
    }

    // Petit feedback visuel
    if (ImGui::BeginPopupModal("Chargement Terminé", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("La base de données a été chargée avec succès !\nChemin : %s", dbFolderPath);
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    // --- Section Algorithme ---
    ImGui::Text("Choix de l'algorithme");
    ImGui::RadioButton("Grille standard", &mosaicMode, 0); ImGui::SameLine();
    ImGui::RadioButton("Quadtree (Variance)", &mosaicMode, 1); ImGui::SameLine();
    ImGui::RadioButton("SNIC (Superpixels)", &mosaicMode, 2);

    ImGui::Separator();

    // --- Section Paramètres ---
    ImGui::Text("Paramètres globaux");
    ImGui::InputInt("Top-K (Imagettes candidates)", &topK);
    ImGui::Checkbox("Autoriser les répétitions d'images", &repetition);

    ImGui::Text("Paramètres spécifiques :");
    if (mosaicMode == 0) {
        ImGui::SliderFloat("Taille grille (%)", &percentGrid, 0.01f, 0.20f, "%.3f");
    } 
    else if (mosaicMode == 1) {
        ImGui::InputInt("Seuil de Variance", &seuilVariance);
        ImGui::InputInt("Subdivisions minimales", &grilleMin);
    } 
    else if (mosaicMode == 2) {
        ImGui::InputInt("Nombre de Superpixels", &numSuperPixels);
        ImGui::SliderFloat("Compacité", &snicCompactness, 10.0f, 75.0f, "%.1f");
    }

    ImGui::Separator();

    // --- Section Actions (Première passe) ---
    if (ImGui::Button("Générer la Mosaïque (Pass 1)", ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
        // Nettoyage des anciennes données
        currentTiles.clear();
        currentRegionInfos.clear();
        currentDistances.clear();
        currentSnicLabels.clear();
        hasGeneratedPass2 = false;

        // Chargement de l'image
        imIn.load(inputImagePath);
        int width = imIn.getWidth();
        int height = imIn.getHeight();
        
        bool* used = new bool[imgInfos.size()](); // Initialisé à false
        
        // Lancement de l'algorithme choisi
        if (mosaicMode == 0) {
            mosaique(imIn, currentTiles, currentRegionInfos, currentDistances, percentGrid, repetition, topK);
            imOut1 = constructMosaicFromTiles(currentTiles, imIn);
        } 
        else if (mosaicMode == 1) {
            int tailleMin = std::max(width, height) * 0.01f; // 1%
            mosaique2(imIn, currentTiles, currentDistances, currentRegionInfos, 0, 0, width, height, seuilVariance, tailleMin, grilleMin, used, repetition);
            imOut1 = constructMosaicFromTiles(currentTiles, imIn);
        } 
        else if (mosaicMode == 2) {
            mosaiqueSNICPolygon(imIn, currentTiles, currentDistances, currentRegionInfos, currentSnicLabels, numSuperPixels, snicCompactness, repetition, topK);
            imOut1 = constructMosaicFromLabels(currentTiles, currentSnicLabels, imIn);
        }

        // Calcul des métriques
        numTilesGenerated = currentTiles.size();
        psnr1 = calculatePSNR(imIn, imOut1);
        ssim1 = calculateSSIM(imIn, imOut1);
        
        imOut1.save("mosaique_pass1.ppm");
        
        delete[] used;
        hasGeneratedPass1 = true;
    }

    // --- Section Résultats & Seconde Passe ---
    if (hasGeneratedPass1) {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Mosaïque générée ! (%d tiles)", numTilesGenerated);
        ImGui::Text("PSNR: %.2f dB  |  SSIM: %.4f", psnr1, ssim1);

        ImGui::Spacing();
        if (ImGui::Button("Sauvegarder la grille", ImVec2(ImGui::GetContentRegionAvail().x / 2, 25))) {
            bool isSNIC = (mosaicMode == 2);
            // Assure-toi que saveMosaicGrid est bien déclarée (la fonction de notre échange précédent)
            saveMosaicGrid(imIn, currentTiles, currentSnicLabels, isSNIC, "grille_debug.ppm"); 
        }

        ImGui::Spacing();
        ImGui::Separator();
        
        if (ImGui::Button("Lancer l'optimisation (Pass 2)", ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
            SecondPass(currentTiles, currentDistances, currentRegionInfos, imIn, 20, topK);

            if (mosaicMode == 2) {
                imOut2 = constructMosaicFromLabels(currentTiles, currentSnicLabels, imIn);
            } else {
                imOut2 = constructMosaicFromTiles(currentTiles, imIn);
            }

            psnr2 = calculatePSNR(imIn, imOut2);
            ssim2 = calculateSSIM(imIn, imOut2);
            imOut2.save("mosaique_pass2.ppm");
            
            hasGeneratedPass2 = true;
        }
    }

    if (hasGeneratedPass2) {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Optimisation terminée !");
        ImGui::Text("PSNR: %.2f dB  |  SSIM: %.4f", psnr2, ssim2);
    }

    ImGui::End();
}
