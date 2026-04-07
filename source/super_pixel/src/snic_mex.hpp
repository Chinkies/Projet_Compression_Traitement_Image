//=================================================================================
//  snic_mex.cpp
//
//
//  AUTORIGHTS
//  Copyright (C) 2016 Ecole Polytechnique Federale de Lausanne (EPFL), Switzerland.
//
//  Created by Radhakrishna Achanta on 05/November/16 (firstname.lastname@epfl.ch)
//
//
//  Code released for research purposes only. For commercial purposes, please
//  contact the author.
//=================================================================================
/*Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of EPFL nor the names of its contributors may
 be used to endorse or promote products derived from this software
 without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  @inproceedings{snic_cvpr17, author = {Achanta, Radhakrishna and Susstrunk, Sabine},
  title = {Superpixels and Polygons using Simple Non-Iterative Clustering},
  booktitle = {IEEE Conference on Computer Vision and Pattern Recognition (CVPR)}, year = {2017} }
 */

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