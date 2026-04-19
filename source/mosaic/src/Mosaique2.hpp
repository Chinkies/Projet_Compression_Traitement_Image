#pragma once

#include "lib/ImageBase.h"
#include "lib/Pixel.hpp"
#include "lib/Traitement.hpp"
#include "imagesInfo.hpp"

struct Tile{
    int x, y; 
    int width, height;
    int imgInfoIDx;
    int rotation; //0 90 180 ou 270

    Tile &operator=(const Tile &tile) {
        this->x = tile.x;
        this->y = tile.y;
        this->width = tile.width;
        this->height = tile.height;
        this->imgInfoIDx = tile.imgInfoIDx;
        this->rotation = tile.rotation;

        return *this;
    }
};

struct Candidate {
    int index;
    int difference;
};

double calculatePSNR(ImageBase &imgOriginal, ImageBase &imgCompressed);
double calculateSSIM(ImageBase &imgOriginal, ImageBase &imgCompared);
int get_corresponding_image(Tile &tile, bool color, bool *used, int R, int G, int B, bool repetition = false);
ImageBase resizeImage(ImageBase& img, int newWidth, int newHeight);
void mosaique2(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo, int x0, int y0,
    int regionWidth, int regionHeight, int seuilVariance, int tailleMin, int grilleMin, bool used[], bool repetition);
void mosaique(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<ImgInfo> &RegionInfo, std::vector<int> &distances, float percent, bool repetion, int topK = 10);
void mosaiqueSNICPolygon(ImageBase &imIn, std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo,
    std::vector<int> &outLabels, int numberSuperPixel, double compactness, bool repetition, int topK = 10);
ImageBase constructMosaicFromTiles(std::vector<Tile>& tiles, ImageBase &img);
ImageBase constructMosaicFromLabels(std::vector<Tile>& tiles, const std::vector<int>& labels, ImageBase &img);
void SecondPass(std::vector<Tile> &tiles, std::vector<int> &distances, std::vector<ImgInfo> &RegionInfo, ImageBase &imIn, int seuil, int topK = 10);

// topK correspond au nombre d'image que l'on garde (image proche en distance RGB) pour tester pixel à pixel
int getBestRotation(Tile& tile, ImgInfo& regionInfo, ImageBase& imIn, bool used[], bool repetition, int topK = 10);
int getBestRotationForSpecificImage(ImageBase& regionTarget, Tile& tile);