#include "Pixel.hpp"
#include "ImageBase.h"
#include <cmath>

Pixel::Pixel(unsigned char *data, unsigned int x, unsigned int y) {
    this->x = x;
    this->y = y;
    color = true;
    R = data[0];
    G = data[1];
    B = data[2];
}


unsigned int Pixel::dist(const Pixel &p) const {
    if (color)
        return abs((int)(R - p.R)) + abs((int)(G - p.G)) + abs((int)(B - p.B));
    else
        return abs((int)(N - p.N));
}

bool Pixel::check_pixel(ImageBase *img) {
    if (x <= 0 || x >= img->getWidth() ||
        y <= 0 || y >= img->getHeight()) return false;
    else return true;
}

Pixel Pixel::operator-(const Pixel &p) const {
    if (color)
        return {R - p.R, G - p.G, B - p.B, x, y};
    else
        return {N - p.N, x, y};
}

void Pixel::operator=(const Pixel &p) {
    color = p.color;
    x = p.x;
    y = p.y;
    if (color) {
        R = p.R;
        G = p.G;
        B = p.B;
    }
    else {
        N = p.N;
    }
}

void Pixel::operator+=(const Pixel &p) {
    if (color) {
        R += p.R;
        G += p.G;
        B += p.B;
    }
    else N += p.N;
}

void Pixel::operator/=(const unsigned int &val) {
    if (color) {
        R /= val;
        G /= val;
        B /= val;
    }
    else N /= val;
}

bool Pixel::operator==(const Pixel &p) const {
    if (color)
        return (R == p.R) && (G == p.G) && (B == p.B);
    else
        return (N == p.N);
}

bool Pixel::operator!=(const Pixel &p) const {
    if (color)
        return ((R != p.R) || (G != p.G) || (B != p.B));
    else 
        return (N != p.N);
}

void Pixel::to_grey() {
    if (color) {
        N = (R + G + B) / 3;
        color = false;
    }
}