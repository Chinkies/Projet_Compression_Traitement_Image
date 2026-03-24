#include <iostream>
#include <vector>
#include <cmath>

#include "lib/ImageBase.h"
#include "Mosaique2.hpp"
#include "imagesInfo.hpp"
#include "Mask.hpp"

double calculatePSNR(ImageBase &imgOriginal, ImageBase &imgCompressed) {
    if (imgOriginal.getWidth() != imgCompressed.getWidth() || 
        imgOriginal.getHeight() != imgCompressed.getHeight()) {
        std::cerr << "Erreur: les images doivent avoir les mêmes dimensions" << std::endl;
        return -1.0;
    }

    double mse = 0.0;  // Mean Squared Error
    unsigned long nbPixels = imgOriginal.getWidth() * imgOriginal.getHeight() * 3;  // RGB

    unsigned char *data1 = imgOriginal.getData();
    unsigned char *data2 = imgCompressed.getData();

    for (unsigned long i = 0; i < nbPixels; ++i) {
        int diff = (int)data1[i] - (int)data2[i];
        mse += diff * diff;
    }

    mse /= nbPixels;

    if (mse == 0) {
        return 100.0;  // Images identiques
    }

    double psnr = 10.0 * log10((255.0 * 255.0) / mse);
    return psnr;
}

int main(int argc, char **argv) {
	char cNomImgLue[250], databaseLue[250];
	if (argc != 3) 
	{
		printf("Usage: ImageIn.ppm/pgm database.bin\n"); 
		return 1;
	}
	sscanf (argv[1],"%s",cNomImgLue) ;
	sscanf (argv[2],"%s",databaseLue) ;
	
	ImageBase imIn;
	imIn.load(cNomImgLue);

    ImageBase imOut(imIn.getWidth(), imIn.getHeight(), imIn.getColor());

    // Initialisation tableau des valeurs moyennes des images
    //initImgInfos();
	initImgInfosFromBin(databaseLue);

    int seuilVariance = 500; // Seuil de variance pour la subdivision
    int tailleMin = std::max(imIn.getWidth(), imIn.getHeight()) * (5/100); // Taille minimale d'une région pour la subdivision, 1% de la taille de l'image
	int grilleMin = 2; // Nombre de subdivisions minimum

	//mosaique(imIn, imOut, 0.02, false);

    bool used[imgInfos.size()] = {false};
    //mosaique2(imIn, imOut, 0, 0, imIn.getWidth(), imIn.getHeight(), seuilVariance, 16, grilleMin, used, false);
	ImageBase mask;

	mask.load("coeur.pgm");

	ImageBase cut = cut_image(imIn, mask);
	cut.save("cut.ppm");
	//float PSNR = calculatePSNR(imIn, imOut);
	//std::cout << "PSNR : " << PSNR << " dB" << std::endl;

	//imOut.save("mosaique1.ppm");

    //mosaique2(imIn, imOut, 0, 0, imIn.getWidth(), imIn.getHeight(), seuilVariance, tailleMin, grilleMin);

	//imOut.save("mosaique2.ppm");
}