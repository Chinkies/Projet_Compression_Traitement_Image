#include "Mosaique2.hpp"
#include "../../super_pixel/src/snic_mex.hpp"

double calculatePSNR(ImageBase &imgOriginal, ImageBase &imgCompressed) {
    if (imgOriginal.getWidth() != imgCompressed.getWidth() || 
        imgOriginal.getHeight() != imgCompressed.getHeight()) {
        std::cerr << "Erreur: les images doivent avoir les mêmes dimensions" << std::endl;
        return -1.0;
    }

    double mse = 0.0;  // Mean Squared Error
    unsigned long nbPixels = imgOriginal.getWidth() * imgOriginal.getHeight() * 3;  // RGB

    unsigned char *data1 = imgOriginal.getData();
    unsigned char *data2 = imgCompressed.getData();

    for (unsigned long i = 0; i < nbPixels; ++i) {
        int diff = (int)data1[i] - (int)data2[i];
        mse += diff * diff;
    }

    mse /= nbPixels;

    if (mse == 0) {
        return 100.0;  // Images identiques
    }

    double psnr = 10.0 * log10((255.0 * 255.0) / mse);
    return psnr;
}

double calculateSSIM(ImageBase &imgOriginal, ImageBase &imgCompared) {
    if (imgOriginal.getWidth() != imgCompared.getWidth() ||
        imgOriginal.getHeight() != imgCompared.getHeight() ||
        imgOriginal.getColor() != imgCompared.getColor()) {
        std::cerr << "Erreur: les images doivent avoir les mêmes dimensions et le même type" << std::endl;
        return -1.0;
    }

    const int channels = imgOriginal.getColor() ? 3 : 1;
    const unsigned long nbValues =
        static_cast<unsigned long>(imgOriginal.getWidth()) *
        static_cast<unsigned long>(imgOriginal.getHeight()) *
        static_cast<unsigned long>(channels);

    if (nbValues == 0) {
        std::cerr << "Erreur: image vide" << std::endl;
        return -1.0;
    }

    unsigned char *data1 = imgOriginal.getData();
    unsigned char *data2 = imgCompared.getData();

    double muX = 0.0;
    double muY = 0.0;
    for (unsigned long i = 0; i < nbValues; ++i) {
        muX += static_cast<double>(data1[i]);
        muY += static_cast<double>(data2[i]);
    }
    muX /= static_cast<double>(nbValues);
    muY /= static_cast<double>(nbValues);

    double sigmaX2 = 0.0;
    double sigmaY2 = 0.0;
    double sigmaXY = 0.0;

    for (unsigned long i = 0; i < nbValues; ++i) {
        const double x = static_cast<double>(data1[i]) - muX;
        const double y = static_cast<double>(data2[i]) - muY;
        sigmaX2 += x * x;
        sigmaY2 += y * y;
        sigmaXY += x * y;
    }

    sigmaX2 /= static_cast<double>(nbValues);
    sigmaY2 /= static_cast<double>(nbValues);
    sigmaXY /= static_cast<double>(nbValues);

    const double L = 255.0;
    const double C1 = (0.01 * L) * (0.01 * L);
    const double C2 = (0.03 * L) * (0.03 * L);

    const double numerator = (2.0 * muX * muY + C1) * (2.0 * sigmaXY + C2);
    const double denominator =
        (muX * muX + muY * muY + C1) * (sigmaX2 + sigmaY2 + C2);

    if (denominator == 0.0) {
        return 1.0;
    }

    return numerator / denominator;
}
// version qui traite séparéments les composantes R, G, B plutôt que de faire une moyenne des 3
// Modification: maintenant rajoute une tile dans le tableau des tiles et renvoie la distance entre la tile et la région cible
int get_corresponding_image(Tile &tile, bool color, bool *used, int R, int G, int B, bool repetition) {

    int dR = 255, dG = 255, dB = 255;
    int bestIdx = -1;
    int idx = 0;
    
    if (!repetition){
        for (auto& imgInfo : imgInfos) {

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
        }

        used[bestIdx] = true;

        std::string path = imgInfos[bestIdx].getBinPath();
        int id = imgInfos[bestIdx].getBinId();

        tile.imgInfoIDx = bestIdx;


    } else {
        for (auto& imgInfo : imgInfos) {

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
        }

        std::string path = imgInfos[bestIdx].getBinPath();
        int id = imgInfos[bestIdx].getBinId();

        tile.imgInfoIDx = bestIdx;

    }

    return dB+dG+dR;
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

int get_distance(ImageBase &img1, ImageBase &img2) {
    int distance = 0;
    ImageBase resized = resizeImage(img2, img1.getWidth(), img1.getHeight());
    for (int y = 0; y < img1.getHeight(); ++y) {
        for (int x = 0; x < img1.getWidth(); ++x) {
            Pixel p1 = img1.getPixel(x, y);
            Pixel p2 = resized.getPixel(x, y);
            if (img1.getColor()) {
                distance += std::abs(p1.R - p2.R) + std::abs(p1.G - p2.G) + std::abs(p1.B - p2.B);
            } else {
                distance += std::abs(p1.N - p2.N);
            }
        }
    }
    return distance;
}

ImageBase chargeTileRegion(Tile &tile, ImageBase &img) {
    ImageBase tileRegion(tile.width, tile.height, img.getColor());
    for (int y = 0; y < tile.height; ++y) {
        for (int x = 0; x < tile.width; ++x) {
            tileRegion.setPixelTo(x, y, img.getPixel(tile.x + x, tile.y + y));
        }
    }
    return tileRegion;
}

ImageBase chargeTile(Tile &tile, ImageBase &img) {
    std::string path = imgInfos[tile.imgInfoIDx].getBinPath();
    int id = imgInfos[tile.imgInfoIDx].getBinId();

    ImageBase tileImg;
    tileImg.loadFromBin(const_cast<char*>(path.c_str()), id);

    ImageBase resized = resizeImage(tileImg, tile.width, tile.height);
    return resized;
}

void tileSwap(Tile &tile1, Tile &tile2) {
    int tempIdx = tile1.imgInfoIDx;

    tile1.imgInfoIDx = tile2.imgInfoIDx;

    tile2.imgInfoIDx = tempIdx;
}


// A faire : trouver une meilleure heuristique
void SecondPass(std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo, int seuil) {
    int count = 0;

    for (size_t i = 0; i < tiles.size(); i++) {
        if (distances[i] > seuil) { // Seulement pour les tiles dont la distance est supérieure au seuil
            int idx = tiles[i].imgInfoIDx;

            for (size_t j = i+1; j < tiles.size(); j++) {
                if (distances[j] > seuil) {
                    int idx2 = tiles[j].imgInfoIDx;
                    int d1 = distances[i] + distances[j]; // Distance totale avant le swap

                    // Calcul de la distance Region cible avec imagette en cours
                    int R1 = std::abs(imgInfos[idx].R - RegionInfo[j].R);
                    int G1 = std::abs(imgInfos[idx].G - RegionInfo[j].G);
                    int B1 = std::abs(imgInfos[idx].B - RegionInfo[j].B);

                    // Calcul de la distance Region en cours avec imagette cible
                    int R2 = std::abs(imgInfos[idx2].R - RegionInfo[i].R);
                    int G2 = std::abs(imgInfos[idx2].G - RegionInfo[i].G);
                    int B2 = std::abs(imgInfos[idx2].B - RegionInfo[i].B);

                    int d2 = (R1+G1+B1) + (R2+G2+B2); // Distance totale après le swap

                    if (d2 < d1) {
                        tileSwap(tiles[i], tiles[j]);

                        distances[j] = R1+G1+B1;
                        distances[i] = R2+G2+B2;

                        count++;
                    }
                }
            }
        }
    }

    std::cout << "Nombre de swaps effectués : " << count << std::endl;
}

// Fonction qui construit la mosaique finale à partir des tiles sélectionnées
ImageBase constructMosaicFromTiles(std::vector<Tile>& tiles, ImageBase &img) {
    ImageBase mosaic(img.getWidth(), img.getHeight(), img.getColor());

    for (auto& tile : tiles) {
        ImageBase tileImg = chargeTile(tile, img);

        for (int y = 0; y < tile.height; ++y) {
            for (int x = 0; x < tile.width; ++x) {
                mosaic.setPixelTo(tile.x + x, tile.y + y, tileImg.getPixel(x, y));
            }
        }
    }
    return mosaic;
}

// Fonction qui construit la mosaique finale spécifiquement pour la version SNIC (tile à forme variable)
ImageBase constructMosaicFromLabels(std::vector<Tile>& tiles, const std::vector<int>& labels, ImageBase &img) {
    ImageBase mosaic(img.getWidth(), img.getHeight(), img.getColor());
    int width = img.getWidth();

    std::vector<ImageBase> loadedTiles(tiles.size());
    for (size_t i = 0; i < tiles.size(); ++i){
        if (tiles[i].imgInfoIDx != -1 && tiles[i].width > 0 && tiles[i].height > 0) {
            loadedTiles[i] = chargeTile(tiles[i], img);
        }
    }

    for (int y = 0; y < img.getHeight(); ++y){
        for (int x = 0; x < img.getWidth(); ++x){
            int index = y * width + x;
            int label = labels[index];

            if (label >= 0 && label < (int)tiles.size() && tiles[label].imgInfoIDx != -1) {
                Tile& t = tiles[label];

                int localX = x - t.x;
                int localY = y - t.y;

                if (localX >= 0 && localX < t.width && localY >= 0 && localY < t.height) {
                    mosaic.setPixelTo(x, y, loadedTiles[label].getPixel(localX, localY));
                }
            }
        }
    }
    return mosaic;
}

void mosaique(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<ImgInfo> &RegionInfo, std::vector<int> &distances, float percent, bool repetion) {
    bool used[imgInfos.size()];
    for (size_t i = 0; i < imgInfos.size(); ++i) {
        used[i] = false;
    }

    int width = imIn.getWidth();
    int height = imIn.getHeight();

    int gridWidth = static_cast<int>(width * percent);
    int gridHeight = static_cast<int>(height * percent);

    if (gridWidth <= 0) gridWidth = 1;
    if (gridHeight <= 0) gridHeight = 1;

    std::cout << "Number of cells : " << (width / gridWidth) * (height / gridHeight) << std::endl;

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

            ImgInfo regionInfo;
            regionInfo.R = R;
            regionInfo.G = G;
            regionInfo.B = B;
            RegionInfo.push_back(regionInfo);

            Tile tile = {x0, y0, tileWidth, tileHeight, -1};
            int distance = get_corresponding_image(tile, imIn.getColor(), used, R, G, B, repetion);
            distances.push_back(distance);
            tiles.push_back(tile);

            //std::cout << "Processed tile at (" << x0 << ", " << y0 << ") with distance " << distance << "\33[2K\r";
            
        }
    }
    
}

void mosaique2(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo, int x0, int y0,
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
            Pixel p1 = imIn.getPixel((x0 + x), (y0 + y));
            Pixel p2 = imIn.getPixel((x0 + demiWidth + x), (y0 + y));
            Pixel p3 = imIn.getPixel((x0 + x), (y0 + demiHeight + y));
            Pixel p4 = imIn.getPixel((x0 + demiWidth + x), (y0 + demiHeight + y));

            r1.setPixelTo(x, y, p1);
            R1 += p1.R;
            G1 += p1.G;
            B1 += p1.B;

            r2.setPixelTo(x, y, p2);
            R2 += p2.R;
            G2 += p2.G;
            B2 += p2.B;

            r3.setPixelTo(x, y, p3);
            R3 += p3.R;
            G3 += p3.G;
            B3 += p3.B;

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
        mosaique2(imIn, tiles, distances, RegionInfo, x0, y0, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        Tile tile = {x0, y0, demiWidth, demiHeight, -1};
        ImgInfo regionInfo;
        regionInfo.R = R1;
        regionInfo.G = G1;
        regionInfo.B = B1;
        int distance = get_corresponding_image(tile, imIn.getColor(), used, R1, G1, B1, repetition);

        distances.push_back(distance);
        RegionInfo.push_back(regionInfo);
        tiles.push_back(tile);
    }

    if ((v2 > seuilVariance || grilleMin > 0) && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, tiles, distances, RegionInfo, x0 + demiWidth, y0, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        Tile tile = {x0 + demiWidth, y0, demiWidth + regionWidth%2, demiHeight, -1};
        ImgInfo regionInfo;
        regionInfo.R = R2;
        regionInfo.G = G2;
        regionInfo.B = B2;
        int distance = get_corresponding_image(tile, imIn.getColor(), used, R2, G2, B2, repetition);
        
        distances.push_back(distance);
        RegionInfo.push_back(regionInfo);
        tiles.push_back(tile);
    }

    if ((v3 > seuilVariance || grilleMin > 0) && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, tiles, distances, RegionInfo, x0, y0 + demiHeight, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        Tile tile = {x0, y0 + demiHeight, demiWidth, demiHeight + regionHeight%2, -1};
        ImgInfo regionInfo;
        regionInfo.R = R3;
        regionInfo.G = G3;
        regionInfo.B = B3;
        int distance = get_corresponding_image(tile, imIn.getColor(), used, R3, G3, B3, repetition);
        
        distances.push_back(distance);
        RegionInfo.push_back(regionInfo);
        tiles.push_back(tile);
    }

    if ((v4 > seuilVariance || grilleMin > 0) && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, tiles, distances, RegionInfo, x0 + demiWidth, y0 + demiHeight, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1, used, repetition);
    } else {
        // Mettre l'image correspondante dans imOut
        Tile tile = {x0 + demiWidth, y0 + demiHeight, demiWidth + regionWidth%2, demiHeight + regionHeight%2, -1};
        ImgInfo regionInfo;
        regionInfo.R = R4;
        regionInfo.G = G4;
        regionInfo.B = B4;
        int distance = get_corresponding_image(tile, imIn.getColor(), used, R4, G4, B4, repetition);
        
        distances.push_back(distance);
        RegionInfo.push_back(regionInfo);
        tiles.push_back(tile);
    }
}


void mosaiqueSNICPolygon(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo,
    std::vector<int> &outLabels, int numberSuperPixel, double compactness, bool repetition)
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

    outLabels.assign(labels, labels + size);

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


    for (int i = 0; i < nbrSuperPixel; ++i)
    {
        // si le superpixel ne contient aucun pixel, on met un label factice pour garder l'alignement index = label
        if (allSuperPixel[i].nbrPixel == 0) {
            Tile emptyTile = {0, 0, 0, 0, -1};
            tiles.push_back(emptyTile);
            distances.push_back(0);
            RegionInfo.push_back(ImgInfo());
            continue;
        }

        meansR = allSuperPixel[i].sumR / allSuperPixel[i].nbrPixel;
        meansG = allSuperPixel[i].sumG / allSuperPixel[i].nbrPixel;
        meansB = allSuperPixel[i].sumB / allSuperPixel[i].nbrPixel;

        boundingBoxWidth = allSuperPixel[i].maxX - allSuperPixel[i].minX + 1;
        boundingBoxHeight = allSuperPixel[i].maxY - allSuperPixel[i].minY + 1;
        
        //imagette = get_corresponding_image(color, used, meansR, meansG, meansB, repetition);

        ImgInfo regionInfo;
        regionInfo.R = meansR;
        regionInfo.G = meansG;
        regionInfo.B = meansB;
        RegionInfo.push_back(regionInfo);

        Tile tile = {allSuperPixel[i].minX, allSuperPixel[i].minY,
            boundingBoxWidth, boundingBoxHeight, -1};

        int distance = get_corresponding_image(tile, imIn.getColor(), used, meansR, meansG, meansB, repetition);

        distances.push_back(distance);
        tiles.push_back(tile);
        
        if (!imagette.getValidity()) continue;

        /* resized = resizeImage(imagette, boundingBoxWidth, boundingBoxHeight);
        
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
        } */
    }

    delete[] rImIn; delete[] gImIn; delete[] bImIn;
    delete[] lImInLAB; delete[] aImInLAB; delete[] bImInLAB;
    delete[] labels;
    delete[] used;
}