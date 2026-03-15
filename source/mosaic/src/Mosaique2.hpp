#pragma once

#include "lib/ImageBase.h"
#include "lib/Pixel.hpp"
#include "lib/Traitement.hpp"
#include "imagesInfo.hpp"

ImageBase get_corresponding_image(bool color, int m);
ImageBase resizeImage(ImageBase& img, int newWidth, int newHeight);
void mosaique2(ImageBase &imIn, ImageBase &imOut, int x0, int y0,
    int regionWidth, int regionHeight, int seuilVariance, int tailleMin);