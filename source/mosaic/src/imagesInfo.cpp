#include "imagesInfo.hpp"

#include <filesystem>

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
