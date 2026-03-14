#include <iostream>
#include <vector>
#include <cmath>

#include "lib/ImageBase.h"
#include "Mosaique2.hpp"
#include "imagesInfo.hpp"

int main(int argc, char **argv) {
	char cNomImgLue[250];
	if (argc != 2) 
	{
		printf("Usage: ImageIn.ppm/pgm \n"); 
		return 1;
	}
	sscanf (argv[1],"%s",cNomImgLue) ;
	
	ImageBase imIn;
	imIn.load(cNomImgLue);

    ImageBase imOut(imIn.getWidth(), imIn.getHeight(), imIn.getColor());

    // Initialisation tableau des valeurs moyennes des images
    initImgInfos();

    int seuilVariance = 500; // Seuil de variance pour la subdivision
    int tailleMin = 16; // Taille minimale d'une région pour la subdivision

    mosaique2(imIn, imOut, 0, 0, seuilVariance, tailleMin);

}