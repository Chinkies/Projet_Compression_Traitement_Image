#include "Mosaique2.hpp"


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
            resized.setPixel(resized.getPixel(srcX, srcY));
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

void mosaique2(ImageBase &imIn, ImageBase &imOut, int x0, int y0, int seuilVariance, int tailleMin) {
    int width = imIn.getWidth();
    int height = imIn.getHeight();

    int demiWidth = width / 2;
    int demiHeight = height / 2;

    ImageBase r1(demiWidth, demiHeight, imIn.getColor());
    ImageBase r2(demiWidth, demiHeight, imIn.getColor());
    ImageBase r3(demiWidth, demiHeight, imIn.getColor());
    ImageBase r4(demiWidth, demiHeight, imIn.getColor());

    int m1 = 0, m2 = 0, m3 = 0, m4 = 0;

    for (int y = 0; y < demiHeight; ++y) {
        for (int x = 0; x < demiWidth; ++x) {
            int idxIn = (y0 + y) * imIn.getWidth() + (x0 + x);
            int idx = y * demiWidth + x;

            r1.getData()[idx] = imIn.getData()[idxIn];
            m1 += r1.getData()[idx];

            r2.getData()[idx] = imIn.getData()[idxIn + demiWidth];
            m2 += r2.getData()[idx];

            r3.getData()[idx] = imIn.getData()[idxIn + demiWidth * imIn.getWidth()];
            m3 += r3.getData()[idx];

            r4.getData()[idx] = imIn.getData()[idxIn + demiWidth * imIn.getWidth() + demiWidth];
            m4 += r4.getData()[idx];
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

    if (v1 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0, y0, seuilVariance, tailleMin);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m1);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                }
            }
        }
    }

    if (v2 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0 + demiWidth, y0, seuilVariance, tailleMin);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m1);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                }
            }
        }
    }

    if (v3 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0, y0 + demiHeight, seuilVariance, tailleMin);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m1);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                }
            }
        }
    }

    if (v4 > seuilVariance && demiWidth >= tailleMin && demiHeight >= tailleMin) {
        mosaique2(imIn, imOut, x0 + demiWidth, y0 + demiHeight, seuilVariance, tailleMin);
    } else {
        // Mettre l'image correspondante dans imOut
        ImageBase img = get_corresponding_image(imIn.getColor(), m1);
        if (img.getValidity()) {
            ImageBase resized = resizeImage(img, demiWidth, demiHeight);
            for (int y = 0; y < demiHeight; ++y) {
                for (int x = 0; x < demiWidth; ++x) {
                    int idxOut = (y0 + y) * imOut.getWidth() + (x0 + x);
                    imOut.getData()[idxOut] = resized.getData()[y * demiWidth + x];
                }
            }
        }
    }
}