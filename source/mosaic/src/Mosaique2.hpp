#pragma once

#include "lib/ImageBase.h"
#include "lib/Pixel.hpp"
#include "lib/Traitement.hpp"
#include "imagesInfo.hpp"

struct Tile{
    int x, y; 
    int width, height;
    int imgInfoIDx;

    Tile &operator=(const Tile &tile) {
        this->x = tile.x;
        this->y = tile.y;
        this->width = tile.width;
        this->height = tile.height;
        this->imgInfoIDx = tile.imgInfoIDx;

        return *this;
    }
};

double calculatePSNR(ImageBase &imgOriginal, ImageBase &imgCompressed);
double calculateSSIM(ImageBase &imgOriginal, ImageBase &imgCompared);
int get_corresponding_image(Tile &tile, bool color, bool *used, int R, int G, int B, bool repetition = false);
ImageBase resizeImage(ImageBase& img, int newWidth, int newHeight);
void mosaique2(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo, int x0, int y0,
    int regionWidth, int regionHeight, int seuilVariance, int tailleMin, int grilleMin, bool used[], bool repetition);
void mosaique(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<ImgInfo> &RegionInfo, std::vector<int> &distances, float percent, bool repetion);
void mosaiqueSNICPolygon(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo,
    std::vector<int> &outLabels, int numberSuperPixel, double compactness, bool repetition);
ImageBase constructMosaicFromTiles(std::vector<Tile>& tiles, ImageBase &img);
ImageBase constructMosaicFromLabels(std::vector<Tile>& tiles, const std::vector<int>& labels, ImageBase &img);
void SecondPass(std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo, int seuil);