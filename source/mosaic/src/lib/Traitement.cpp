#include "Traitement.hpp"
#include "Pixel.hpp"
#include <string>
#include <cmath>
#include <vector>

double gradient_norme(ImageBase &img, int x, int y) {
    int width = img.getWidth();
    int height = img.getHeight();

    double sumX = 0.0, sumY = 0.0;

    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int idx = ny * width + nx;
                double intensity = img.getColor() ? (img.getData()[idx * 3] + img.getData()[idx * 3 + 1] + img.getData()[idx * 3 + 2]) / 3.0 : img.getData()[idx];
                sumX += intensity * i;
                sumY += intensity * j;
            }
        }
    }

    return std::sqrt(sumX * sumX + sumY * sumY);
}

// METHODS
ImageBase Traitement::seuillage(ImageBase &image, int *seuil, int nSeuil) {
    ImageBase imOut(image.getWidth(), image.getHeight(), image.getColor());


    if (image.getColor()) {
        for(int x = 0; x < image.getHeight(); ++x)
            for(int y = 0; y < image.getWidth(); ++y) {
                int step = 255 / nSeuil;
                for (int i = 0; i < nSeuil; ++i) {
                    if (image[x*3][y*3] < seuil[i])
                        imOut[x*3][y*3] = i * step;
                    else if (i == nSeuil - 1) 
                        imOut[x][y] = 255;

                    if (image[x*3][y*3 + 1] < seuil[i])
                        imOut[x*3][y*3 + 1] = i * step;
                    else if (i == nSeuil - 1)
                        imOut[x][y] = 255;

                    if (image[x*3][y*3 + 2] < seuil[i])
                        imOut[x*3][y*3 + 2] = i * step;
                    else if (i == nSeuil - 1)
                        imOut[x][y] = 255;
                }
            }
    }
    else {

        for(int x = 0; x < image.getHeight(); ++x)
            for(int y = 0; y < image.getWidth(); ++y) {
                int step = 255 / nSeuil;
                for (int i = 0; i < nSeuil; ++i) {
                    if (image[x][y] < seuil[i]) imOut[x][y] = i * step;
                    else if (i == nSeuil - 1) imOut[x][y] = 255;
                }
            }
    }

    return imOut;
}

ImageBase Traitement::seuillage_auto(ImageBase &image) {
    int hist[256] = {0};

    std::cout << "Seuillage automatique..." << std::endl;

    if (image.getColor()) {
        for (int y = 0; y < image.getHeight(); y++) {
            for (int x = 0; x < image.getWidth(); x++) {
                hist[(int)image[y*3][x*3+0]]++;
                hist[(int)image[y*3][x*3+1]]++;
                hist[(int)image[y*3][x*3+2]]++;
            }
        }
    } else {
        for (int y = 0; y < image.getHeight(); y++) {
            for (int x = 0; x < image.getWidth(); x++) {
                hist[(int)image[x][y]]++;
            }
        }
    }

    int totalPixels = image.getWidth() * image.getHeight() * (image.getColor() ? 3 : 1);
    int sum = 0;
    for (int i = 0; i < 256; i++) sum += hist[i] * i;

    int sumB = 0, wB = 0, wF = 0;
    double maxVariance = 0.0;
    int threshold = 0;

    for (int i = 0; i < 256; i++) {
        wB += hist[i];
        if (wB == 0) continue;

        wF = totalPixels - wB;
        if (wF == 0) break;

        sumB += hist[i] * i;
        double mB = static_cast<double>(sumB) / wB;
        double mF = static_cast<double>(sum - sumB) / wF;

        double varianceBetween = static_cast<double>(wB) * wF * (mB - mF) * (mB - mF);
        if (varianceBetween > maxVariance) {
            maxVariance = varianceBetween;
            threshold = i;
        }
    }

    std::cout << "Seuil optimal trouvé : " << threshold << std::endl;

    return Traitement::seuillage(image, new int[1]{threshold}, 1);
}

ImageBase Traitement::griser(ImageBase &image) {
    std::cout << "Grisage..." << std::endl;

    if (!image.getColor())
        return image;

    int width = image.getWidth();
    int height = image.getHeight();

    ImageBase imOut(width, height, false);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            int idxColor = idx * 3;

            Pixel p(&image.getData()[idxColor], x, y);
            p.to_grey();
            
            imOut.getData()[idx] = p.N;
        }
    }

    std::cout << "Grisage terminé." << std::endl;

    return imOut;
}

void Traitement::histograme(ImageBase &image, const char* filename) {
    FILE *f;
    f = fopen(filename, "w");
    unsigned int hist[256] = {0};

    if (!image.getColor()){
        for (int y = 0; y < image.getHeight(); y++) {
            for (int x = 0; x < image.getWidth(); x++) {
                hist[(int)image[x][y]]++;
            }
        }

        for (int i = 0; i < 256; i++) {
            std::cout << i << "\t" << hist[i] << std::endl;
            fprintf(f, "%d %d\n", i, hist[i]);
        }
    } else {
        unsigned int R[256] = {0};
        unsigned int G[256] = {0};
        unsigned int B[256] = {0};

        for (int y = 0; y < image.getHeight(); y++) {
            for (int x = 0; x < image.getWidth(); x++) {
                R[(int)image[y*3][x*3+0]]++;
                G[(int)image[y*3][x*3+1]]++;
                B[(int)image[y*3][x*3+2]]++;
            }
        }

        for (int i = 0; i < 256; i++) {
            std::cout << i << "\t" << R[i] << "\t" << G[i] << "\t" << B[i] <<std::endl;
            fprintf(f, "%d %d %d %d\n", i, R[i], G[i], B[i]);
        }
    }

    fclose(f);

    // Génération du graphe PNG avec gnuplot
    std::string pngFilename = std::string(filename) + ".png";
    FILE* gp = popen("gnuplot", "w");
    
    if (gp != nullptr) {
        fprintf(gp, "set terminal png size 1200,800\n");
        fprintf(gp, "set output '%s'\n", pngFilename.c_str());
        fprintf(gp, "set ylabel 'Fréquence'\n");
        fprintf(gp, "set grid\n");
        fprintf(gp, "set style fill solid 0.5\n");
        
        if (!image.getColor()) {
            fprintf(gp, "set xlabel 'Niveau de gris'\n");
            fprintf(gp, "set title 'Histogramme (Niveaux de gris)'\n");
            fprintf(gp, "plot '%s' using 1:2 with boxes title 'Intensité'\n", filename);
        } else {
            fprintf(gp, "set title 'Histogramme (RGB)'\n");
            fprintf(gp, "set xlabel 'Niveau de couleur'\n");
            fprintf(gp, "plot '%s' using 1:2 with lines lw 2 lc rgb 'red' title 'Rouge', \\\n", filename);
            fprintf(gp, "     '%s' using 1:3 with lines lw 2 lc rgb 'green' title 'Vert', \\\n", filename);
            fprintf(gp, "     '%s' using 1:4 with lines lw 2 lc rgb 'blue' title 'Bleu'\n", filename);
        }
        
        fflush(gp);
        pclose(gp);
        
        std::cout << "Graphe PNG généré : " << pngFilename << std::endl;
    } else {
        std::cerr << "Erreur : impossible d'ouvrir gnuplot" << std::endl;
    }
}

void Traitement::histograme(ImageBase &image, int ligne, const char* filename) {
    FILE *f;
    f = fopen(filename, "w");
    unsigned int hist[256] = {0};

    if (!image.getColor()){
        for (int x = 0; x < image.getWidth(); x++) {
            hist[(int)image[x][ligne]]++;
        }

        for (int i = 0; i < 256; i++) {
            std::cout << i << "\t" << hist[i] << std::endl;
            fprintf(f, "%d %d\n", i, hist[i]);
        }
    } else {
        unsigned int R[256] = {0};
        unsigned int G[256] = {0};
        unsigned int B[256] = {0};

        for (int x = 0; x < image.getWidth(); x++) {
            R[(int)image[ligne*3][x*3+0]]++;
            G[(int)image[ligne*3][x*3+1]]++;
            B[(int)image[ligne*3][x*3+2]]++;
        }

        for (int i = 0; i < 256; i++) {
            std::cout << i << "\t" << R[i] << "\t" << G[i] << "\t" << B[i] <<std::endl;
            fprintf(f, "%d %d %d %d\n", i, R[i], G[i], B[i]);
        }
    }

    fclose(f);

    // Génération du graphe PNG avec gnuplot
    std::string pngFilename = std::string(filename) + ".png";
    FILE* gp = popen("gnuplot", "w");
    
    if (gp != nullptr) {
        fprintf(gp, "set terminal png size 1200,800\n");
        fprintf(gp, "set output '%s'\n", pngFilename.c_str());
        fprintf(gp, "set ylabel 'Fréquence'\n");
        fprintf(gp, "set grid\n");
        fprintf(gp, "set style fill solid 0.5\n");
        
        if (!image.getColor()) {
            fprintf(gp, "set xlabel 'Niveau de gris'\n");
            fprintf(gp, "set title 'Histogramme (Niveaux de gris)'\n");
            fprintf(gp, "plot '%s' using 1:2 with boxes title 'Intensité'\n", filename);
        } else {
            fprintf(gp, "set title 'Histogramme (RGB)'\n");
            fprintf(gp, "set xlabel 'Niveau de couleur'\n");
            fprintf(gp, "plot '%s' using 1:2 with lines lw 2 lc rgb 'red' title 'Rouge', \\\n", filename);
            fprintf(gp, "     '%s' using 1:3 with lines lw 2 lc rgb 'green' title 'Vert', \\\n", filename);
            fprintf(gp, "     '%s' using 1:4 with lines lw 2 lc rgb 'blue' title 'Bleu'\n", filename);
        }
        
        fflush(gp);
        pclose(gp);
        
        std::cout << "Graphe PNG généré : " << pngFilename << std::endl;
    } else {
        std::cerr << "Erreur : impossible d'ouvrir gnuplot" << std::endl;
    }
}

Pixel Traitement::avg_voisins(ImageBase &img, ImageBase &binaryImg, int x, int y, int rayon) {
    int count = 0;
    unsigned int sumR = 0, sumG = 0, sumB = 0, sumN = 0;

    Pixel avg;
    avg.color = img.getColor();

    for (int j = -1*rayon; j <= rayon; j++) {
        for (int i = -1*rayon; i <= rayon; i++) {
            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < img.getWidth() && ny >= 0 && ny < img.getHeight()) {
                int idx = ny * img.getWidth() + nx;
                if (avg.color) {
                    if (binaryImg.getData()[idx] == 255) {
                        sumR += img.getData()[idx * 3];
                        sumG += img.getData()[idx * 3 + 1];
                        sumB += img.getData()[idx * 3 + 2];
                    }
                    else {
                        idx = y * img.getWidth() + x; // Si le pixel n'est pas dans le binaire, on prend la valeur du pixel central
                        sumR += img.getData()[idx * 3];
                        sumG += img.getData()[idx * 3 + 1];
                        sumB += img.getData()[idx * 3 + 2];
                    }
                    count++;

                }
                else if (!avg.color) {
                    sumN += img.getData()[idx];
                    count++;
                    
                }
            }
        }
    }

    count = std::max(count, 1); // Eviter la division par zéro

    if (avg.color) {
        avg.R = static_cast<unsigned char>(sumR / count);
        avg.G = static_cast<unsigned char>(sumG / count);
        avg.B = static_cast<unsigned char>(sumB / count);
    } else {
        avg.N = static_cast<unsigned char>(sumN / count);
    }

    return avg;
}

Pixel Traitement::avg_voisins(ImageBase &img, int x, int y, int rayon) {
    int count = 0;
    unsigned int sumR = 0, sumG = 0, sumB = 0, sumN = 0;

    Pixel avg;
    avg.color = img.getColor();

    for (int j = -1*rayon; j <= rayon; j++) {
        for (int i = -1*rayon; i <= rayon; i++) {
            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < img.getWidth() && ny >= 0 && ny < img.getHeight()) {
                int idx = ny * img.getWidth() + nx;
                if (avg.color) {
                    sumR += img.getData()[idx * 3];
                    sumG += img.getData()[idx * 3 + 1];
                    sumB += img.getData()[idx * 3 + 2];
                }
                else {
                    sumN += img.getData()[idx];
                }
                count++;
            }
        }
    }

    if (avg.color) {
        avg.R = static_cast<unsigned char>(sumR / count);
        avg.G = static_cast<unsigned char>(sumG / count);
        avg.B = static_cast<unsigned char>(sumB / count);
    } else {
        avg.N = static_cast<unsigned char>(sumN / count);
    }

    return avg;
}

ImageBase Traitement::floutage(ImageBase &img, int percentage) {
    int width = img.getWidth();
    int height = img.getHeight();

    ImageBase imOut(width, height, img.getColor());

    int rayon = int(std::max(width, height) / percentage);


    std::cout << "Floutage..." << std::endl;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            Pixel avg = Traitement::avg_voisins(img, x, y, rayon);

            if (imOut.getColor()) {
                imOut.getData()[idx * 3] = static_cast<unsigned char>(avg.R);
                imOut.getData()[idx * 3 + 1] = static_cast<unsigned char>(avg.G);
                imOut.getData()[idx * 3 + 2] = static_cast<unsigned char>(avg.B);
            } else {
                imOut.getData()[idx] = static_cast<unsigned char>(avg.N);
            }
        }
    }

    return imOut;
}

ImageBase Traitement::floutage_fond(ImageBase &img, int percentage) {
    int width = img.getWidth();
    int height = img.getHeight();

    ImageBase imOut(width, height, img.getColor());

    ImageBase imgGris(Traitement::griser(img));
    imgGris.save("../Media/gris.pgm");

    ImageBase imgBinaireRaw(Traitement::seuillage_auto(imgGris));
    imgBinaireRaw.save("../Media/binaire_raw.pgm");

    ImageBase imgBinaire(Traitement::erode(imgBinaireRaw, 99)); // Erosion pour réduire les zones blanches (objets)
    imgBinaire.save("../Media/binaire_erode.pgm");

    int rayon = int(std::max(width, height) / percentage);
    std::cout << "Floutage du fond..." << std::endl;

    for (int y = 0; y < height; y++) {
        if (y % 10 == 0) std::cout << "Progression: " << (y * 100) / height << "%\r" << std::flush;
        for (int x = 0; x < width; x++) {
            Pixel p;
            int idx = y * width + x;

            if (imgBinaire.getData()[idx] == 255)
                p = Traitement::avg_voisins(img, imgBinaire, x, y, rayon);
            else {
                if (img.getColor()) {
                    int idxColor = idx * 3;
                    p = Pixel(img.getData()[idxColor], img.getData()[idxColor + 1], img.getData()[idxColor + 2], x, y);
                } else {
                    p = Pixel(img.getData()[idx], x, y);
                }
            }

            if (imOut.getColor()) {
                imOut.getData()[idx * 3] = static_cast<unsigned char>(p.R);
                imOut.getData()[idx * 3 + 1] = static_cast<unsigned char>(p.G);
                imOut.getData()[idx * 3 + 2] = static_cast<unsigned char>(p.B);
            } else {
                imOut.getData()[idx] = static_cast<unsigned char>(p.N);
            }
        }
    }

    return imOut;
}

bool Traitement::test_voisins(ImageBase &img, int x, int y, int rayon, int percentage_voisins) {
    // Retourne true si TOUS les voisins ont la même valeur que le pixel central
    int idx = y * img.getWidth() + x;
    unsigned char centerValue = img.getData()[idx];
    int n = 0;
    int count = 0;

    for (int j = -rayon; j <= rayon; j++) {
        for (int i = -rayon; i <= rayon; i++) {
            int nx = x + i;
            int ny = y + j;

            if (nx >= 0 && nx < img.getWidth() && ny >= 0 && ny < img.getHeight()) {
                int nidx = ny * img.getWidth() + nx;
                if (img.getData()[nidx] != centerValue) {
                    count++;
                }
            }
        }
    }
    // Si pourcentage_voisins = 100, alors il faut que tous les voisins soient identiques
    return (count * 100) / ((2 * rayon + 1) * (2 * rayon + 1)) <= percentage_voisins;
}
    
ImageBase Traitement::erode(ImageBase &img, int percentage, int percentage_voisins, bool white) {
    int width = img.getWidth();
    int height = img.getHeight();
    percentage = 100 - percentage; // Inverser le pourcentage pour que 100% corresponde à une érosion totale

    ImageBase imOut(width, height, false);

    int rayon = int(std::max(width, height) / percentage);

    std::cout << "Érosion sur un rayon de " << rayon << "..." << std::endl;

    for (int y = 0; y < height; y++) {        
        if (y % 10 == 0) std::cout << "Progression: " << (y * 100) / height << "%\r" << std::flush;

        for (int x = 0; x < width; x++) {
            int idx = y * width + x;

            if (white) {
                // Érosion des blancs : garde blanc seulement si tous les voisins sont blancs
                if (img.getData()[idx] == 255 && Traitement::test_voisins(img, x, y, rayon, percentage_voisins)) {
                    imOut.getData()[idx] = 255;
                } else {
                    imOut.getData()[idx] = 0;
                }
            }
            else {
                // Érosion des noirs : garde noir seulement si tous les voisins sont noirs
                if (img.getData()[idx] == 0 && Traitement::test_voisins(img, x, y, rayon, percentage_voisins)) {
                    imOut.getData()[idx] = 0;
                } else {
                    imOut.getData()[idx] = 255;
                }
            }

        }
    }

    return imOut;
}

ImageBase Traitement::gradient_image(ImageBase &img) {
    int width = img.getWidth();
    int height = img.getHeight();

    ImageBase imOut(width, height, false);

    std::cout << "Calcul du gradient..." << std::endl;

    for (int y = 0; y < height; y++) {
        if (y % 10 == 0) std::cout << "Progression: " << (y * 100) / height << "%\r" << std::flush;

        for (int x = 0; x < width; x++) {
            double grad = gradient_norme(img, x, y);
            imOut.getData()[y * width + x] = static_cast<unsigned char>(std::min(255.0, grad));
        }
    }

    return imOut;
}

ImageBase Traitement::hysteresis(ImageBase &img, int SB, int SH) {
    int width = img.getWidth();
    int height = img.getHeight();

    ImageBase imOut(width, height, false);

    ImageBase gradient = Traitement::gradient_image(img);
    std::cout << "Hystérésis..." << std::endl;

    std::vector<int> stack;
    stack.reserve(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;
            unsigned char g = gradient.getData()[idx];
            if (g >= SH) {
                imOut.getData()[idx] = 255;
                stack.push_back(idx);
            } else {
                imOut.getData()[idx] = 0;
            }
        }
    }

    const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    while (!stack.empty()) {
        int idx = stack.back();
        stack.pop_back();

        int x = idx % width;
        int y = idx / width;

        for (int k = 0; k < 8; k++) {
            int nx = x + dx[k];
            int ny = y + dy[k];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int nidx = ny * width + nx;
            if (imOut.getData()[nidx] == 255) continue;

            unsigned char ng = gradient.getData()[nidx];
            if (ng >= SB) {
                imOut.getData()[nidx] = 255;
                stack.push_back(nidx);
            }
        }
    }

    return imOut;

}

float Traitement::variance(ImageBase &img) {
    int width = img.getWidth();
    int height = img.getHeight();
    long long sum = 0;
    long long sumSq = 0;
    int n = width * height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char val = img.getData()[y * width + x];
            sum += val;
            sumSq += val * val;
        }
    }

    double mean = static_cast<double>(sum) / n;
    double variance = static_cast<double>(sumSq) / n - mean * mean;

    return static_cast<float>(variance);
}

ImageBase Traitement::resizeImage(ImageBase& img, int newWidth, int newHeight) {
    ImageBase resized(newWidth, newHeight, img.getColor());
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int srcX = x * img.getWidth() / newWidth;
            int srcY = y * img.getHeight() / newHeight;
            resized.setPixelTo(x, y, img.getPixel(srcX, srcY));
        }
    }
    return resized;
}

static ImageBase RGB_to_LAB(ImageBase &img) {
    int width = img.getWidth();
    int height = img.getHeight();
    ImageBase LAB_image(width, height, true);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Pixel p = img.getPixel(x, y);
            double r = p.R / 255.0;
            double g = p.G / 255.0;
            double b = p.B / 255.0;

            // Convertir RGB en XYZ
            double X = r * 0.4124564 + g * 0.2126729 + b * 0.0193339;
            double Y = r * 0.2126729 + g * 0.7151522 + b * 0.1191920;
            double Z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;

            // Convertir XYZ en LAB
            double L = std::max(0.0, std::min(100.0, (Y > 0.008856) ? (116 * std::cbrt(Y) - 16) : (903.3 * Y)));
            double A = std::max(-128.0, std::min(127.0, (X > 0.008856) ? (116 * std::cbrt(X) - 16) : (903.3 * X)));
            double B = std::max(-128.0, std::min(127.0, (Z > 0.008856) ? (116 * std::cbrt(Z) - 16) : (903.3 * Z)));

            LAB_image.setPixelTo(x, y, Pixel(static_cast<unsigned char>(L), static_cast<unsigned char>(A + 128), static_cast<unsigned char>(B + 128), x, y));
        }
    }

    return LAB_image;
}

// Rotation simple (90, 180 et 270 degré)
ImageBase Traitement::rotateImage(ImageBase &img, int angle){
    if (angle == 0) return img;
    
    int width = img.getWidth();
    int height = img.getHeight();

    ImageBase res(
        (angle == 90 || angle == 270) ? height : width,
        (angle == 90 || angle == 270) ? width : height,
        img.getColor()
    );

    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            
            Pixel p = img.getPixel(x, y);
        
            if (angle == 90) {
                res.setPixelTo(height - 1 - y, x, p);
            } else if (angle == 180) {
                res.setPixelTo(width - 1 - x, height - 1 - y, p);
            } else if (angle == 270) {
                res.setPixelTo(y, width - 1 - x, p);
            }
        }
    }

    return res;
}