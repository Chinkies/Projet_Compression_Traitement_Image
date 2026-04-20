#include <iostream>
#include <vector>
#include <cmath>

#include "lib/ImageBase.h"
#include "Mosaique2.hpp"
#include "imagesInfo.hpp"
#include "Mask.hpp"

int main(int argc, char **argv) {
	char cNomImgLue[250], databaseLue[250];
	char S;
	int topK, saveGrid;


	if (argc != 6) 
	{
		printf("Usage: ImageIn.ppm/pgm database.bin type_de_mosaique(1, 2 ou 3)\n"); 
		return 1;
	}
	sscanf (argv[1],"%s",cNomImgLue);
	sscanf (argv[2],"%s",databaseLue);
	sscanf (argv[3],"%c",&S);
	sscanf (argv[4],"%d",&topK);
	sscanf (argv[5],"%d",&saveGrid);

	ImageBase imIn;
	imIn.load(cNomImgLue);

    ImageBase imOut(imIn.getWidth(), imIn.getHeight(), imIn.getColor());
	ImageBase imOut2(imIn.getWidth(), imIn.getHeight(), imIn.getColor());

    // Initialisation tableau des valeurs moyennes des images
	initImgInfosFromBin(databaseLue);

    int seuilVariance = 500; // Seuil de variance pour la subdivision
    int tailleMin = std::max(imIn.getWidth(), imIn.getHeight()) * (5.f/100.f); // Taille minimale d'une région pour la subdivision, 1% de la taille de l'image
	int grilleMin = 2; // Nombre de subdivisions minimum

	std::vector<Tile> tiles;	// Contient les imagettes de la mosaïque
	std::vector<ImgInfo> RegionInfo;	// Contient les infos de chaque région de l'image d'entrée
	std::vector<int> distances;	// Contient la distance entre chaque région de l'image d'entrée et l'imagette
	std::vector<int> snicLabels; // Contient les labels nécessaire à la construction des tiles SNIC

	//bool used[imgInfos.size()] = {false};
	bool* used = new bool[imgInfos.size()]();
	
	switch(S) {
		case '1':
			mosaique(imIn, tiles, RegionInfo, distances, 0.06, false, topK);
			break;
		case '2':
    		mosaique2(imIn, tiles, distances, RegionInfo, 0, 0, imIn.getWidth(), imIn.getHeight(), seuilVariance, 16, grilleMin, used, false);
			break;
		case '3':
			mosaiqueSNICPolygon(imIn, tiles, distances, RegionInfo, snicLabels, 6000, topK, false);
			break;
		default:
			std::cerr << "Type de mosaïque invalide. Utilisez '1', '2' ou '3'." << std::endl;
			return 1;
	}

	if (S == '3') {
		imOut = constructMosaicFromLabels(tiles, snicLabels, imIn);
	} else {
		imOut = constructMosaicFromTiles(tiles, imIn);
	}

	if (saveGrid == 1) {
		bool isSnic = (S == '3');
		saveMosaicGrid(imIn, tiles, snicLabels, isSnic, "debug_grille.ppm");
	}

	std::cout << "Nbr de tiles : " << tiles.size() << "\n\n";
    double PSNR = calculatePSNR(imIn, imOut);
	double SSIM = calculateSSIM(imIn, imOut);
	std::cout << "Avant 2nd pass\n\tPSNR : " << PSNR << " dB" << std::endl;
	std::cout << "\tSSIM : " << SSIM << std::endl;
	imOut.save("mosaique.ppm");

    SecondPass(tiles, distances, RegionInfo, imIn, 20, topK);

	if (S == '3') {
		imOut2 = constructMosaicFromLabels(tiles, snicLabels, imIn);
	} else {
    	imOut2 = constructMosaicFromTiles(tiles, imIn);
	}

    double PSNR2 = calculatePSNR(imIn, imOut2);
	double SSIM2 = calculateSSIM(imIn, imOut2);
	std::cout << "Après 2nd pass\n\tPSNR : " << PSNR2 << " dB" << std::endl;
	std::cout << "\tSSIM : " << SSIM2 << std::endl;
	imOut2.save("mosaique2ndPass.ppm");


	/*if (PSNR2 > PSNR) {
		imOut2.save("mosaique2ndPass.ppm");
	} else {
		imOut.save("mosaique.ppm");
	}*/

    delete[] used;
	return 0;
}