#include "Mosaique2.hpp"

//TODO : faire en sorte de trouver l'image la plus proche et non la première
ImageBase get_corresponding_image(bool color, int m) {

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

void mosaique(ImageBase &imIn, ImageBase &imOut, float percent) {
    int width = imIn.getWidth();
    int height = imIn.getHeight();

    int gridWidth = static_cast<int>(width * percent);
    int gridHeight = static_cast<int>(height * percent);

    if (gridWidth <= 0) gridWidth = 1;
    if (gridHeight <= 0) gridHeight = 1;

    for (int y0 = 0; y0 < height; y0 += gridHeight) {
        for (int x0 = 0; x0 < width; x0 += gridWidth) {
            int tileWidth = std::min(gridWidth, width - x0);
            int tileHeight = std::min(gridHeight, height - y0);

            int m = 0;
            for (int y = y0; y < y0 + tileHeight; ++y) {
                for (int x = x0; x < x0 + tileWidth; ++x) {
                    Pixel p = imIn.getPixel(x, y);
                    m += (p.R + p.G + p.B) / 3;
                }
            }

            m /= (tileWidth * tileHeight);

            ImageBase imagette = get_corresponding_image(imIn.getColor(), m);
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
    int regionWidth, int regionHeight, int seuilVariance, int tailleMin, int grilleMin) {

    int demiWidth = regionWidth / 2;
    int demiHeight = regionHeight / 2;

    ImageBase r1(demiWidth, demiHeight, imIn.getColor());
    ImageBase r2(demiWidth, demiHeight, imIn.getColor());
    ImageBase r3(demiWidth, demiHeight, imIn.getColor());
    ImageBase r4(demiWidth, demiHeight, imIn.getColor());

    int m1 = 0, m2 = 0, m3 = 0, m4 = 0;

    //TODO : à corriger m1 etc sont des int pour potentiellement des RGB
    //Donc la moyenne obtenue correspond au total (r+g+b) / 3
    //Alors que je pense que pour comparer il faut vérifier indépendament
    //les trois composantes individuellement
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
            m1 += imIn.getColor() ? (p1.R + p1.G + p1.B) / 3 : p1.N; // Faux pour le rgb 

            //r2.getData()[idx] = imIn.getData()[idxIn + demiWidth];
            //m2 += r2.getData()[idx];
            r2.setPixelTo(x, y, p2);
            m2 += imIn.getColor() ? (p2.R + p2.G + p2.B) / 3 : p2.N; // Faux pour le rgb 

            //r3.getData()[idx] = imIn.getData()[idxIn + demiWidth * imIn.getWidth()];
            //m3 += r3.getData()[idx];
            r3.setPixelTo(x, y, p3);
            m3 += imIn.getColor() ? (p3.R + p3.G + p3.B) / 3 : p3.N; // Faux pour le rgb 

            //r4.getData()[idx] = imIn.getData()[idxIn + demiWidth * imIn.getWidth() + demiWidth];
            //m4 += r4.getData()[idx];
            r4.setPixelTo(x, y, p4);
            m4 += imIn.getColor() ? (p4.R + p4.G + p4.B) / 3 : p4.N; // Faux pour le rgb 
        }
    }

    m1 /= (demiWidth * demiHeight);
    m2 /= (demiWidth * demiHeight);
    m3 /= (demiWidth * demiHeight);
    m4 /= (demiWidth * demiHeight);

    float v1 = Traitement::variance(r1);
    float v2 = Traitement::variance(r2);
    float v3 = Traitement::variance(r3);
    float v4 = Traitement::variance(r4);

    //TODO : à corriger 
    if ((v1 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) || grilleMin > 0) {
        mosaique2(imIn, imOut, x0, y0, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m1);
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

    if ((v2 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) || grilleMin > 0) {
        mosaique2(imIn, imOut, x0 + demiWidth, y0, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m2);
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

    if ((v3 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) || grilleMin > 0) {
        mosaique2(imIn, imOut, x0, y0 + demiHeight, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m3);
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

    if ((v4 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) || grilleMin > 0) {
        mosaique2(imIn, imOut, x0 + demiWidth, y0 + demiHeight, demiWidth, demiHeight, seuilVariance, tailleMin, grilleMin-1);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m4);
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