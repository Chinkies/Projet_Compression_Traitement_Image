#pragma once

#include "lib/ImageBase.h"
#include "lib/Pixel.hpp"
#include <vector>
#include <string>

struct ImgInfo {
    double R = 0, B = 0, G = 0;
    double N = 0;
    std::string name;

    std::string getBinPath() const
    {
        int separatorPos = name.find_last_of(':');
        return name.substr(0, separatorPos);
    }
    int getBinId() const
    {
        int separatorPos = name.find_last_of(':');
        return std::stoi(name.substr(separatorPos + 1));
    }

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

extern std::vector<ImgInfo> imgInfos;
void initImgInfos();
void initImgInfosFromBin(const std::string& _path);