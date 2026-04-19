#pragma once
#include "ImageBase.h"
#include <iostream>
#include "Pixel.hpp"

class Traitement {  
    private:

    public:

    // METHODS
        static ImageBase seuillage(ImageBase &image, int *seuil, int nSeuil);
        static ImageBase seuillage_auto(ImageBase &image);
        static ImageBase griser(ImageBase &image);
        static void histograme(ImageBase &image, const char *filename);
        static void histograme(ImageBase &image, int ligne, const char *filename);
        static Pixel avg_voisins(ImageBase &img, ImageBase &binaryImg, int x, int y, int rayon);
        static Pixel avg_voisins(ImageBase &img, int x, int y, int rayon);
        static ImageBase floutage(ImageBase &img, int percentage = 95);
        static ImageBase floutage_fond(ImageBase &img, int percentage = 95);
        static bool test_voisins(ImageBase &img, int x, int y, int rayon, int percentage_voisins);
        static ImageBase erode(ImageBase &img, int percentage = 1, int percentage_voisins = 100, bool white = false);
        static ImageBase gradient_image(ImageBase &img);
        static ImageBase hysteresis(ImageBase &img, int SB, int SH);
        static float variance(ImageBase &img);
        static ImageBase resizeImage(ImageBase& img, int newWidth, int newHeight);
        static ImageBase RGB_to_LAB(ImageBase &img);
        static ImageBase rotateImage(ImageBase &img, int angle); // Rotation simple de 90 180 et 270 degré
};