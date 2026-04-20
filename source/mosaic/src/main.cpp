#include <iostream>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "lib/ImageBase.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Mosaique2.hpp"
#include "imagesInfo.hpp"
#include "Mask.hpp"
#include "Ui.hpp"


/* int main(int argc, char **argv) {
	char cNomImgLue[250], databaseLue[250];
	char S;
	int topK, saveGrid;


	if (argc != 6) 
	{
		printf("Usage: ImageIn.ppm/pgm database.bin type_de_mosaique(1, 2 ou 3)\n"); 
		return 1;
	}
	sscanf (argv[1],"%s",cNomImgLue);
	sscanf (argv[2],"%s",databaseLue);
	sscanf (argv[3],"%c",&S);
	sscanf (argv[4],"%d",&topK);
	sscanf (argv[5],"%d",&saveGrid);

	ImageBase imIn;
	imIn.load(cNomImgLue);

    ImageBase imOut(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	ImageBase imOut2(imIn.getWidth(), imIn.getHeight(), imIn.getColor());

    // Initialisation tableau des valeurs moyennes des images
	initImgInfosFromBin(databaseLue);

    int seuilVariance = 500; // Seuil de variance pour la subdivision
    int tailleMin = std::max(imIn.getWidth(), imIn.getHeight()) * (5.f/100.f); // Taille minimale d'une région pour la subdivision, 1% de la taille de l'image
	int grilleMin = 2; // Nombre de subdivisions minimum

	std::vector<Tile> tiles;	// Contient les imagettes de la mosaïque
	std::vector<ImgInfo> RegionInfo;	// Contient les infos de chaque région de l'image d'entrée
	std::vector<int> distances;	// Contient la distance entre chaque région de l'image d'entrée et l'imagette
	std::vector<int> snicLabels; // Contient les labels nécessaire à la construction des tiles SNIC

	//bool used[imgInfos.size()] = {false};
	bool* used = new bool[imgInfos.size()]();
	
	switch(S) {
		case '1':
			mosaique(imIn, tiles, RegionInfo, distances, 0.06, false, topK);
			break;
		case '2':
    		mosaique2(imIn, tiles, distances, RegionInfo, 0, 0, imIn.getWidth(), imIn.getHeight(), seuilVariance, 16, grilleMin, used, false);
			break;
		case '3':
			mosaiqueSNICPolygon(imIn, tiles, distances, RegionInfo, snicLabels, 3000, topK, false);
			break;
		default:
			std::cerr << "Type de mosaïque invalide. Utilisez '1', '2' ou '3'." << std::endl;
			return 1;
	}

	if (S == '3') {
		imOut = constructMosaicFromLabels(tiles, snicLabels, imIn);
	} else {
		imOut = constructMosaicFromTiles(tiles, imIn);
	}

	if (saveGrid == 1) {
		bool isSnic = (S == '3');
		saveMosaicGrid(imIn, tiles, snicLabels, isSnic, "debug_grille.ppm");
	}

	std::cout << "Nbr de tiles : " << tiles.size() << "\n\n";
    double PSNR = calculatePSNR(imIn, imOut);
	double SSIM = calculateSSIM(imIn, imOut);
	std::cout << "Avant 2nd pass\n\tPSNR : " << PSNR << " dB" << std::endl;
	std::cout << "\tSSIM : " << SSIM << std::endl;
	imOut.save("mosaique.ppm");

    SecondPass(tiles, distances, RegionInfo, imIn, 20, topK);

	if (S == '3') {
		imOut2 = constructMosaicFromLabels(tiles, snicLabels, imIn);
	} else {
    	imOut2 = constructMosaicFromTiles(tiles, imIn);
	}

    double PSNR2 = calculatePSNR(imIn, imOut2);
	double SSIM2 = calculateSSIM(imIn, imOut2);
	std::cout << "Après 2nd pass\n\tPSNR : " << PSNR2 << " dB" << std::endl;
	std::cout << "\tSSIM : " << SSIM2 << std::endl;
	imOut2.save("mosaique2ndPass.ppm");


	/*if (PSNR2 > PSNR) {
		imOut2.save("mosaique2ndPass.ppm");
	} else {
		imOut.save("mosaique.ppm");
	}

    delete[] used;
	return 0;
} */
int main() {
    // --- 1. INITIALISATION DE GLFW ---
    if (!glfwInit()) {
        std::cerr << "Erreur : Impossible d'initialiser GLFW" << std::endl;
        return -1;
    }

    // Paramètres de la fenêtre (Version OpenGL 3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Mosaique Generator", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Active la V-Sync pour une interface fluide

    // --- 2. CHARGEMENT D'OPENGL (via GLAD) ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : Impossible d'initialiser GLAD" << std::endl;
        return -1;
    }

    // --- 3. INITIALISATION IMGUI ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // Style visuel d'ImGui

    // Configuration des ponts (Backends)
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // --- 4. BOUCLE PRINCIPALE ---
    while (!glfwWindowShouldClose(window)) { // <--- Voici le vrai nom de la fonction
        // On récupère les évènements (clic, clavier, etc.)
        glfwPollEvents();

        // Prépare une nouvelle frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- APPEL DE TON INTERFACE ---
        RenderMosaicUI(); 
        // ------------------------------

        // Rendu final (Dessin)
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        // Couleur de fond de la fenêtre
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // --- 5. NETTOYAGE ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}