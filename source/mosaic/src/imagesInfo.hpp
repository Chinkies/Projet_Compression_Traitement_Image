#pragma once

#include "lib/ImageBase.h"
#include "lib/Pixel.hpp"
#include "lib/Traitement.hpp"

#include <iostream>
#include <vector>
#include <filesystem>

#include "lib/ImageBase.hpp"
#include "lib/Pixel.hpp"

namespace fs = std::filesystem;

std::vector<ImgInfo> imgInfos;

struct ImgInfo {
    double R = 0, B = 0, G = 0;
    double N = 0;
    std::string name;

    void operator+=(const Pixel& pixel) {
        if (pixel.color) {
            R += pixel.R;
            G += pixel.G;
            B += pixel.B;
        } else N += pixel.N;
    }

    void operator/=(const unsigned int& val) {
        if (val == 0) return;
        R /= val;
        G /= val;
        B /= val;
        N /= val;
    }
    
    void operator=(const ImgInfo& info) {
        R = info.R;
        G = info.G;
        B = info.B;
        N = info.N;
        name = info.name;
    }

    void operator=(const Pixel& pixel) {
        if (pixel.color) {
            R = pixel.R;
            G = pixel.G;
            B = pixel.B;
        } else N = pixel.N;
    }

};

void initImgInfos() {
    
    std::string path = "media/db";

    // Parcours du dossier media/db
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            std::string fileName = entry.path().filename().string();

            // Traitement de l'image pour calculer les valeurs moyennes de R, G, B ou N si nuance de gris
            ImgInfo info;
            ImageBase image(filePath);

            info.name = fileName;

            int height = image.getHeight();
            int width = image.getWidth();

            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    info += image.getPixel(x, y)
                }
            }

            imgInfos.push_back(info);
        }
    }
}