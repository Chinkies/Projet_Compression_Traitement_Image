/******************************************************************************
* ICAR_Library
*
* Fichier : ImageBase.cpp
*
* Description : Voir le fichier .h
*
* Auteur : Mickael Pinto
*
* Mail : mickael.pinto@live.fr
*
* Date : Octobre 2012
*
*******************************************************************************/

#include "ImageBase.h"
#include "image_ppm.h"

ImageBase::ImageBase(void)
{
	isValid = false;
	init();
}

ImageBase::ImageBase(int imWidth, int imHeight, bool isColor)
{
	isValid = false;
	init();

	color = isColor;
	height = imHeight;
	width = imWidth;
	nTaille = height * width * (color ? 3 : 1);
	
	if(nTaille == 0)
		return;
	
	allocation_tableau(data, OCTET, nTaille);
	dataD = (double*)malloc(sizeof(double) * nTaille);
	isValid = true;
}

ImageBase::ImageBase(const ImageBase &copy)
{
	data = 0;
	dataD = 0;
	this->copy(copy);
}


ImageBase::~ImageBase(void)
{
	reset();
}

void ImageBase::init()
{
	if(isValid)
	{
		free(data);
		free(dataD);
	}

	data = 0;
	dataD = 0;
	height = width = nTaille = 0;
	isValid = false;
}

void ImageBase::reset()
{
	if(isValid)
	{
		free(data);
		free(dataD);
	}
	isValid = false;
}

void ImageBase::load(char *filename)
{
	init();

	int l = strlen(filename);

	if(l <= 4) // Le fichier ne peut pas etre que ".pgm" ou ".ppm"
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut être que '.pgm' ou '.ppm'");
		exit(0);
	}

	int nbPixel = 0;

	if( strcmp(filename + l - 3, "pgm") == 0) // L'image est en niveau de gris
	{
		color = false;
		lire_nb_lignes_colonnes_image_pgm(filename, &height, &width);
		nbPixel = height * width;
  
		nTaille = nbPixel;
		allocation_tableau(data, OCTET, nTaille);
		lire_image_pgm(filename, data, nbPixel);
	}
	else if( strcmp(filename + l - 3, "ppm") == 0) // L'image est en couleur
	{
		color = true;
		lire_nb_lignes_colonnes_image_ppm(filename, &height, &width);
		nbPixel = height * width;
  
		nTaille = nbPixel * 3;
		allocation_tableau(data, OCTET, nTaille);
		lire_image_ppm(filename, data, nbPixel);
	}
	else 
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut être que .pgm ou .ppm");
		exit(0);
	}
	
	dataD = (double*)malloc(sizeof(double) * nTaille);

	isValid = true;
}


void ImageBase::readFromBin(char nom_image[], OCTET *pt_image, 
	const int index, const int width, const int height)
{
	int area = width * height;
	int imgSize = 1 + (3 * area);
	
    std::ifstream file(nom_image, std::ios::binary);

	if (!file) 
	{
		printf("Impossible d'ouvrir %s \n", nom_image);
		exit(EXIT_FAILURE);
	}

	// On se positionne sur la bonne image (index)
	std::streampos offset = static_cast<std::streampos>(index) * imgSize;
	file.seekg(offset);

	if (!file.good())
	{
		printf("Index hors limit du fichier");
		return;
	}

	// On ignore le label
	file.ignore(1);
	
	std::vector<OCTET> image(3*imgSize);
	file.read(reinterpret_cast<char*>(image.data()), image.size());
	
	for (int i = 0; i < area; ++i) 
	{
        pt_image[i * 3 + 0] = image[i];                 
        pt_image[i * 3 + 1] = image[i + area];   
        pt_image[i * 3 + 2] = image[i + 2 * area];
    }

	file.close();
}

// TODO : Rendre generique pour la taille de l'image (width & height)
// Only for bin to ppm file
// Work for 
void ImageBase::loadFromBin(char *filename, const int index)
{
	init();

	int nbPixel = 0; 

	color = true;

	height = 32;
	width = 32;
	nbPixel = height * width;
	
	nTaille = nbPixel * 3;
	allocation_tableau(data, OCTET, nTaille);

	//void ImageBase::readFromBin(char nom_image[], OCTET *pt_image, 
	//const int index, const int width, const int height)

	readFromBin(filename, data, index, width, height);

	dataD = (double*)malloc(sizeof(double) * nTaille);

	isValid = true;
}

bool ImageBase::save(char *filename)
{
	if(!isValid)
	{
		printf("Sauvegarde de l'image impossible : L'image courante n'est pas valide");
		exit(0);
	}

	if(color)
		ecrire_image_ppm(filename, data,  height, width);
	else
		ecrire_image_pgm(filename, data,  height, width);

	return true;
}

ImageBase *ImageBase::getPlan(PLAN plan)
{
	if( !isValid || !color )
		return 0;

	ImageBase *greyIm = new ImageBase(width, height, false);
	
	switch(plan)
	{
	case PLAN_R:
		planR(greyIm->data, data, height * width);
		break;
	case PLAN_G:
		planV(greyIm->data, data, height * width);
		break;
	case PLAN_B:
		planB(greyIm->data, data, height * width);
		break;
	default:
		printf("Il n'y a que 3 plans, les valeurs possibles ne sont donc que 'PLAN_R', 'PLAN_G', et 'PLAN_B'");
		exit(0);
		break;
	}

	return greyIm;
}

void ImageBase::copy(const ImageBase &copy)
{
	reset();
	
	isValid = false;
	init();
	
	color = copy.color;
	height = copy.height;
	width = copy.width;
	nTaille = copy.nTaille;
	isValid = copy.isValid;
	
	if(nTaille == 0)
		return;
	
	allocation_tableau(data, OCTET, nTaille);
	dataD = (double*)malloc(sizeof(double) * nTaille);
	isValid = true;

	for(int i = 0; i < nTaille; ++i)
	{
		data[i] = copy.data[i];
	}

	if (copy.dataD != nullptr && dataD != nullptr) {
		for(int i = 0; i < nTaille; ++i) dataD[i] = copy.dataD[i];
	}
}

unsigned char *ImageBase::operator[](int l)
{
	if(!isValid)
	{
		printf("L'image courante n'est pas valide");
		exit(0);
	}
	
	if((!color && l >= height) || (color && l >= height*3))
	{
		printf("L'indice se trouve en dehors des limites de l'image");
		exit(0);
	}
	
	return data+l*width;
}

Pixel ImageBase::getPixel(int x, int y) {
	if (!isValid || data == nullptr) { // <--- AJOUT SÉCURITÉ
        // Retourne un pixel noir par défaut ou lance une erreur
        Pixel p; p.R = p.G = p.B = p.N = 0; p.x = x; p.y = y; return p;
    }
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw std::out_of_range("Pixel coordinates are out of bounds");
	}
	int index = (y * width + x) * (color ? 3 : 1);
	Pixel pixel;
	pixel.x = x;
	pixel.y = y;
	
	if (color) {
		pixel.R = data[index];
		pixel.G = data[index + 1];
		pixel.B = data[index + 2];
		pixel.color = true;
	} else {
		pixel.N = data[index];
		pixel.color = false;
	}
	return pixel;
}

void ImageBase::setPixel(const Pixel& pixel) {
	if (pixel.x < 0 || pixel.x >= width || pixel.y < 0 || pixel.y >= height) {
		throw std::out_of_range("Pixel coordinates are out of bounds");
	}
	int index = (pixel.y * width + pixel.x) * (color ? 3 : 1);
	
	if (color) {
		data[index] = pixel.R;
		data[index + 1] = pixel.G;
		data[index + 2] = pixel.B;
	} else {
		data[index] = pixel.N;
	}
}

void ImageBase::setPixelTo(int x, int y, const Pixel& pixel) 
{
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw std::out_of_range("coordinates x or y are out of bounds");
	}

	int index = (y * width + x) * (color ? 3 : 1);

	if (color) {
		data[index] = pixel.R;
		data[index + 1] = pixel.G;
		data[index + 2] = pixel.B;
	} else {
		data[index] = pixel.N;
	}
}

void ImageBase::operator=(const ImageBase &copy)
{
	this->copy(copy);
}