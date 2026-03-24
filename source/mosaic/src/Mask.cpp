#include "Mask.hpp"

ImageBase cut_image(ImageBase &img, ImageBase &mask) {
    int width = mask.getWidth();
    int height = mask.getHeight();

    ImageBase resized = Traitement::resizeImage(img, width, height);
    ImageBase cut(width, height, img.getColor());

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            Pixel maskPixel = mask.getPixel(x, y);
            if (maskPixel.N == 255) { // Si le pixel du masque est blanc
                Pixel p = resized.getPixel(x, y);
                cut.setPixelTo(x, y, p); // on met le pixel correspondant 
            }
        }
    }

    return cut;
}