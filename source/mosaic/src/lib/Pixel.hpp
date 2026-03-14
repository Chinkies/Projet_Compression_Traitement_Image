#pragma once

#include"ImageBase.h"

class Pixel {
public:
    unsigned char R, G, B, N;
    bool color;
    unsigned int x, y;

    Pixel() : R{0}, G{0}, B{0}, N{0}, color{false}, x{0}, y{0} {};

    Pixel(unsigned char R, unsigned char G, unsigned char B, unsigned int x, unsigned int y) 
    : R{R}, G{G}, B{B}, x{x}, y{y} {color = true;};

    Pixel(unsigned char *data, unsigned int x, unsigned int y);

    Pixel(unsigned char N, unsigned int x, unsigned int y)
    : N{N}, x{x}, y{y} {color = false;};


    // OPERATORS 
    Pixel operator-(const Pixel &p) const;

    void operator=(const Pixel &p);

    void operator+=(const Pixel &p);

    void operator/=(const unsigned int &val);

    bool operator==(const Pixel &p) const;

    bool operator!=(const Pixel &p) const;

    // METHODS
    unsigned int dist(const Pixel &p) const;

    bool check_pixel(ImageBase *img);

    void to_grey();
};