#include "imagesInfo.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

std::vector<ImgInfo> imgInfos;

void initImgInfos() {
    imgInfos.clear();

    const std::string path = "source/mosaic/media/db";
    if (!fs::exists(path) || !fs::is_directory(path)) {
        return;
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        std::string filePath = entry.path().string();

        ImgInfo info;
        ImageBase image;
        image.load(const_cast<char*>(filePath.c_str()));

        info.name = filePath;

        int height = image.getHeight();
        int width = image.getWidth();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                info += image.getPixel(x, y);
            }
        }

        info /= static_cast<unsigned int>(height * width);
        imgInfos.push_back(info);
    }
}


// Les images CIFAR-10 sont en 32*32 rgb
// le format du fichier binaire est :
// - une image par ligne
// - premier octet -> catégorie (voiture, animal etc ...)
// - 1024 octets pour R, 1024 G, 1024 B
// - soit 3073 octets par images et 3072 sans le label

void initImgInfosFromBin(const std::string& _filePath)
{
    imgInfos.clear();

    std::ifstream file(_filePath, std::ios::binary);
    if (!file.is_open()){
        std::cerr << "Fichier impossible à ouvrir" << std::endl;
        return;
    }

    int side = 32;
    int area = side * side;
    // taile de la ligne (label + area * 3)
    int imgSize = 1 + area * 3;

    std::vector<int> currentImg(imgSize);
    int id = 0;

    while (file.read(reinterpret_cast<char*>(currentImg.data()), imgSize))
    {
        ImgInfo currentImgInfo;

        currentImgInfo.name = _filePath + ":" + std::to_string(id);

        for (int i = 0; i < area; ++i)
        {
            currentImgInfo.R += currentImg[1 + i];
            currentImgInfo.G += currentImg[1 + area + i];
            currentImgInfo.B += currentImg[1 + 2 * area + i];
        }

        currentImgInfo /= static_cast<int>(area);
        imgInfos.push_back(currentImgInfo);

        id++;
    }

    file.close();

    std::cout << "Image : " << id << " from " 
        << _filePath << " loaded successfully" << std::endl;
}