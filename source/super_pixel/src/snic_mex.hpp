#ifndef SNIC_MEX_HPP
#define SNIC_MEX_HPP

#include <cstring>
#include <cmath>
#include <cfloat>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct SuperPixel {
    int minX = 999999, minY = 999999, maxX = -1, maxY = -1;
    unsigned long int sumR = 0, sumG = 0, sumB = 0;
    int nbrPixel = 0;
};

void rgbtolab(int* rin, int* gin, int* bin, int sz, double* lvec, double* avec, double* bvec);

void FindSeeds(const int width, const int height, int& numk, vector<int>& kx, vector<int>& ky);

void runSNIC(
             double*		lv,
             double*		av,
             double*		bv,
             const int					width,
             const int					height,
             int*                       labels,
             int*						outnumk,
             const int                  innumk,
             const double               compactness);

#endif