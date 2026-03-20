#pragma once

#include "lib/ImageBase.h"
#include "lib/Pixel.hpp"
#include "lib/Traitement.hpp"
#include "imagesInfo.hpp"

ImageBase get_corresponding_image_old(bool color, int m);
ImageBase get_corresponding_image(bool color, bool *used, int R, int G, int B, bool repetition = false);
ImageBase resizeImage(ImageBase& img, int newWidth, int newHeight);
void mosaique2(ImageBase &imIn, ImageBase &imOut, int x0, int y0,
    int regionWidth, int regionHeight, int seuilVariance, int tailleMin, int grilleMin, bool repetition = false);
void mosaique(ImageBase &imIn, ImageBase &imOut, float percent = 0.05, bool repetion = false);