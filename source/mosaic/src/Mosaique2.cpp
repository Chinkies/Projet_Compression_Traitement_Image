#include "Mosaique2.hpp"
#include "../../super_pixel/src/snic_mex.hpp"

//TODO : faire en sorte de trouver l'image la plus proche et non la première
ImageBase get_corresponding_image_old(bool color, int m) {

    for (const auto& imgInfo : imgInfos) {

        std::string path = imgInfo.getBinPath();
        int id = imgInfo.getBinId();

        //TODO : ne fonctionne qu'avec des images couleurs
        if (color) {
            double avgColor = (imgInfo.R + imgInfo.G + imgInfo.B) / 3.0;
            if (std::abs(avgColor - m) < 10) { // Seuil de 10 pour trouver une image similaire
                ImageBase img;
                //img.load(const_cast<char*>(imgInfo.name.c_str()));
                img.loadFromBin(const_cast<char*>(path.c_str()), id);
                return img;
            }
        } else if (std::abs(imgInfo.N - m) < 10) { // Seuil de 10 pour trouver une image similaire
                ImageBase img;
                img.load(const_cast<char*>(imgInfo.name.c_str()));
                return img;
            }
        }
    return ImageBase();
}

// version qui traite séparéments les composantes R, G, B plutôt que de faire une moyenne des 3
ImageBase get_corresponding_image(bool color, bool *used, int R, int G, int B, bool repetition) {

    int dR = 255, dG = 255, dB = 255;
    int bestIdx = -1;
    int idx = 0;
    
    if (!repetition){
        for (const auto& imgInfo : imgInfos) {

            //TODO : ne fonctionne qu'avec des images couleurs
            if (color) {
                if (!used[idx] && std::abs(imgInfo.B - B) < dB && std::abs(imgInfo.G - G) < dG && std::abs(imgInfo.R - R) < dR) {
                    dB = std::abs(imgInfo.B - B);
                    dG = std::abs(imgInfo.G - G);
                    dR = std::abs(imgInfo.R - R);
                    bestIdx = idx;
                }
            } 
            idx++;
        }

        if (bestIdx == -1) {
            std::cout << "Aucune image correspondante trouvée pour R=" << R << " G=" << G << " B=" << B << std::endl;
            return ImageBase();
        }

        used[bestIdx] = true;

        std::string path = imgInfos[bestIdx].getBinPath();
        int id = imgInfos[bestIdx].getBinId();

        ImageBase img;
        //img.load(const_cast<char*>(imgInfo.name.c_str()));
        img.loadFromBin(const_cast<char*>(path.c_str()), id);
        return img;
    } else {
        for (const auto& imgInfo : imgInfos) {

            //TODO : ne fonctionne qu'avec des images couleurs
            if (color) {
                if (std::abs(imgInfo.B - B) < dB && std::abs(imgInfo.G - G) < dG && std::abs(imgInfo.R - R) < dR) {
                    dB = std::abs(imgInfo.B - B);
                    dG = std::abs(imgInfo.G - G);
                    dR = std::abs(imgInfo.R - R);
                    bestIdx = idx;
                }
            } 
            idx++;
        }

        if (bestIdx == -1) {
            std::cout << "Aucune image correspondante trouvée pour R=" << R << " G=" << G << " B=" << B << std::endl;
            return ImageBase();
        }

        std::string path = imgInfos[bestIdx].getBinPath();
        int id = imgInfos[bestIdx].getBinId();

        ImageBase img;
        //img.load(const_cast<char*>(imgInfo.name.c_str()));
        img.loadFromBin(const_cast<char*>(path.c_str()), id);
        return img;
    }
}

ImageBase resizeImage(ImageBase& img, int newWidth, int newHeight) {
    ImageBase resized(newWidth, newHeight, img.getColor());
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int srcX = x * img.getWidth() / newWidth;
            int srcY = y * img.getHeight() / newHeight;
            resized.setPixelTo(x, y, img.getPixel(srcX, srcY));
        }
    }
    return resized;
}

/*
    Mosaique2 est plus poussé qu'une mosaique naïve, elle subdivise l'image en fonction d'un seuil de variance
    pour avoir des régions plus homogènes, et ainsi trouver des images plus similaires à ces régions.

    Je me suis inspiré du TP noté de traitement d'image.

    Malheureusement je n'ai pas de base de donnée d'image pour pouvoir tester.
*/

void mosaique(ImageBase &imIn, ImageBase &imOut, float percent, bool repetion) {
    bool used[imgInfos.size()];
    for (size_t i = 0; i < imgInfos.size(); ++i) {
        used[i] = false;
    }
    int width = imIn.getWidth();
    int height = imIn.getHeight();

    int gridWidth = static_cast<int>(width * percent);
    int gridHeight = static_cast<int>(height * percent);

    std::cout << "Number of cells : " << (width / gridWidth) * (height / gridHeight) << std::endl;

    if (gridWidth <= 0) gridWidth = 1;
    if (gridHeight <= 0) gridHeight = 1;

    for (int y0 = 0; y0 < height; y0 += gridHeight) {
        for (int x0 = 0; x0 < width; x0 += gridWidth) {
            int tileWidth = std::min(gridWidth, width - x0);
            int tileHeight = std::min(gridHeight, height - y0);

            int R = 0, G = 0, B = 0;
            for (int y = y0; y < y0 + tileHeight; ++y) {
                for (int x = x0; x < x0 + tileWidth; ++x) {
                    Pixel p = imIn.getPixel(x, y);
                    if (imIn.getColor()) {
                        R += p.R;
                        G += p.G;
                        B += p.B;
                    } else {
                        R += p.N;
                    }
                }
            }

            R /= (tileWidth * tileHeight);
            G /= (tileWidth * tileHeight);
            B /= (tileWidth * tileHeight);

            ImageBase imagette = get_corresponding_image(imIn.getColor(), used, R, G, B, repetion);
            if (!imagette.getValidity()) {
                continue;
            }

            ImageBase resized = resizeImage(imagette, tileWidth, tileHeight);

            for (int y = 0; y < tileHeight; ++y) {
                for (int x = 0; x < tileWidth; ++x) {
                    imOut.setPixelTo(x0 + x, y0 + y, resized.getPixel(x, y));
                }
            }
        }
    }
}

void mosaique2(ImageBase &imIn, ImageBase &imOut, int x0, int y0,
    int regionWidth, int regionHeight, int seuilVariance, int tailleMin, int grilleMin, bool used[], bool repetition) {

    int demiWidth = regionWidth / 2;
    int demiHeight = regionHeight / 2;

    ImageBase r1(demiWidth, demiHeight, imIn.getColor());
    ImageBase r2(demiWidth, demiHeight, imIn.getColor());
    ImageBase r3(demiWidth, demiHeight, imIn.getColor());
    ImageBase r4(demiWidth, demiHeight, imIn.getColor());

    int R1 = 0, G1 = 0, B1 = 0;
    int R2 = 0, G2 = 0, B2 = 0;
    int R3 = 0, G3 = 0, B3 = 0;
    int R4 = 0, G4 = 0, B4 = 0;

    for (int y = 0; y < demiHeight; ++y) {
        for (int x = 0; x < demiWidth; ++x) {
            //int idxIn = (y0 + y) * imIn.getWidth() + (x0 + x);
            //int idx = y * demiWidth + x;
            Pixel p1 = imIn.getPixel((x0 + x), (y0 + y));
            Pixel p2 = imIn.getPixel((x0 + demiWidth + x), (y0 + y));
            Pixel p3 = imIn.getPixel((x0 + x), (y0 + demiHeight + y));
            Pixel p4 = imIn.getPixel((x0 + demiWidth + x), (y0 + demiHeight + y));

            //r1.getData()[idx] = imIn.getData()[idxIn];
            //m1 += r1.getData()[idx];
            r1.setPixelTo(x, y, p1);
            R1 += p1.R;
            G1 += p1.G;
            B1 += p1.B;

            //r2.getData()[idx] = imIn.getData()[idxIn + demiWidth];
            //m2 += r2.getData()[idx];
            r2.setPixelTo(x, y, p2);
            R2 += p2.R;
            G2 += p2.G;
            B2 += p2.B;

            //r3.getData()[idx] = imIn.getData()[idxIn + demiWidth * imIn.getWidth()];
            //m3 += r3.getData()[idx];
            r3.setPixelTo(x, y, p3);
            R3 += p3.R;
            G3 += p3.G;
            B3 += p3.B;

            //r4.getData()[idx] = imIn.getData()[idxIn + demiWidth * imIn.getWidth() + demiWidth];
            //m4 += r4.getData()[idx];
            r4.setPixelTo(x, y, p4);
            R4 += p4.R;
            G4 += p4.G;
            B4 += p4.B;
        }
    }

    R1 /= (demiWidth * demiHeight);
    G1 /= (demiWidth * demiHeight);
    B1 /= (demiWidth * demiHeight);
    
    R2 /= (demiWidth * demiHeight);
    G2 /= (demiWidth * demiHeight);
    B2 /= (demiWidth * demiHeight);

    R3 /= (demiWidth * demiHeight);
    G3 /= (demiWidth * demiHeight);
    B3 /= (demiWidth * demiHeight);

    R4 /= (demiWidth * demiHeight);
    G4 /= (demiWidth * demiHeight);
    B4 /= (demiWidth * demiHeight);

    float v1 = Traitement::variance(r1);
    float v2 = Traitement::variance(r2);
    float v3 = Traitement::variance(r3);
    float v4 = Traitement::variance(r4);

    //TODO : à corriger 
    if ((v1 > seuilVariance || grilleMin > 0) && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0, y0, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), used, R1, G1, B1, repetition);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    //int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    //imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                    imOut.setPixelTo((x0 + x), (y0 + y), resized.getPixel(x,y));
                }
            }
        }
    }

    if ((v2 > seuilVariance || grilleMin > 0) && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0 + demiWidth, y0, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), used, R2, G2, B2, repetition);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    //int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    //imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                    imOut.setPixelTo((x0 + demiWidth + x), (y0 + y), resized.getPixel(x,y));
                }
            }
        }
    }

    if ((v3 > seuilVariance || grilleMin > 0) && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0, y0 + demiHeight, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), used, R3, G3, B3, repetition);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    //int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    //imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                    imOut.setPixelTo((x0 + x), (y0 + demiHeight + y), resized.getPixel(x,y));
                }
            }
        }
    }

    if ((v4 > seuilVariance || grilleMin > 0) && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0 + demiWidth, y0 + demiHeight, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), used, R4, G4, B4, repetition);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    //int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    //imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                    imOut.setPixelTo((x0 + demiWidth + x), (y0 + demiHeight + y), resized.getPixel(x,y));
                }
            }
        }
    }
}

// TO DO : intégrer un masque pour la découpe de l'image, pour permettre de faire des formes différentes.
// Detection des edges pour les imagettes
// Super-pixel
// Second pass pour swap

void mosaiqueSNICPolygone(ImageBase &imIn, ImageBase &imOut, int numberSuperPixel, double compactness, bool repetition)
{
    int width   = imIn.getWidth();
    int height  = imIn.getHeight();
    int size    = width * height;

    // === On converti les données de ImageBase au format lab utilisé par le code SNIC ===

    // On récupère les composantes rgb de l'image
    int* rImIn = new int[size];
    int* gImIn = new int[size];
    int* bImIn = new int[size];

    int index;
    Pixel p;

    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x)
        {
            index = y * width + x;
            p = imIn.getPixel(x, y);

            rImIn[index] = p.R;
            gImIn[index] = p.G;
            bImIn[index] = p.B;
        }
    }

    // On les convertits en lab
    double* lImInLAB = new double[size];
    double* aImInLAB = new double[size];
    double* bImInLAB = new double[size];

    rgbtolab(rImIn, gImIn, bImIn, size, lImInLAB, aImInLAB, bImInLAB);

    // === On utilise le code SNIC pour obtenir les superpixels de l'image ===
    //     - Le résultat de la fonction est stocké dans labels
    //     - labels[i] donne le superpixel auquel appartient le pixel i
    //     - nbrSuperPixel est le nombre total final de superpixel obtenu
    //       ce nombre peut être différent du nombre demandé

    int* labels = new int[size];
    int nbrSuperPixel = 0;

    runSNIC(lImInLAB, aImInLAB, bImInLAB, width, height, labels, &nbrSuperPixel, numberSuperPixel, compactness);

    // === On traite et stock les valeurs obtenues dans un vecteur de superpixel ===

    std::vector<SuperPixel> allSuperPixel(nbrSuperPixel);

    int currentLabel;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x)
        {
            index = y * width + x;
            currentLabel = labels[index];

            if (currentLabel >= 0 && currentLabel < nbrSuperPixel) 
            {
                p = imIn.getPixel(x, y);
                
                allSuperPixel[currentLabel].minX = std::min(allSuperPixel[currentLabel].minX, x);
                allSuperPixel[currentLabel].minY = std::min(allSuperPixel[currentLabel].minY, y);
                allSuperPixel[currentLabel].maxX = std::max(allSuperPixel[currentLabel].maxX, x);
                allSuperPixel[currentLabel].maxY = std::max(allSuperPixel[currentLabel].maxY, y);

                allSuperPixel[currentLabel].sumR += p.R;
                allSuperPixel[currentLabel].sumG += p.G;
                allSuperPixel[currentLabel].sumB += p.B;

                allSuperPixel[currentLabel].nbrPixel++;
            }
        }
    }

    // === On construit la mosaïque à partir des valeurs obtenues et traitées

    bool* used = new bool[imgInfos.size()](); // faudra utiliser ce genre de déclaration si la base de données devient immense

    int meansR, meansG, meansB, boundingBoxWidth, boundingBoxHeight, imageX, imageY;
    ImageBase imagette, resized;
    bool color = imIn.getColor();


    for (int i = 0; i < nbrSuperPixel; ++i)
    {
        // si le superpixel ne contient aucun pixel, on passe au suivant
        if (allSuperPixel[i].nbrPixel == 0) continue;

        meansR = allSuperPixel[i].sumR / allSuperPixel[i].nbrPixel;
        meansG = allSuperPixel[i].sumG / allSuperPixel[i].nbrPixel;
        meansB = allSuperPixel[i].sumB / allSuperPixel[i].nbrPixel;

        boundingBoxWidth = allSuperPixel[i].maxX - allSuperPixel[i].minX + 1;
        boundingBoxHeight = allSuperPixel[i].maxY - allSuperPixel[i].minY + 1;
        
        imagette = get_corresponding_image(color, used, meansR, meansG, meansB, repetition);
        
        if (!imagette.getValidity()) continue;

        resized = resizeImage(imagette, boundingBoxWidth, boundingBoxHeight);
        
        // on met le masque
        for (int y = 0; y < boundingBoxHeight; ++y){
            for (int x = 0; x < boundingBoxWidth; ++x)
            {
                imageX = allSuperPixel[i].minX + x;
                imageY = allSuperPixel[i].minY + y;

                if (imageX >= 0 && imageX < width && imageY >= 0 && imageY < height)
                {
                    index = imageY * width + imageX;
                    if (labels[index] == i)
                    {
                        imOut.setPixelTo(imageX, imageY, resized.getPixel(x, y));
                    }
                }
            }
        }
    }

    delete[] rImIn; delete[] gImIn; delete[] bImIn;
    delete[] lImInLAB; delete[] aImInLAB; delete[] bImInLAB;
    delete[] labels;
    delete[] used;
}