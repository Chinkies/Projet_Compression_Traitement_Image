# Generateur de Mosaique d'Images

Ce logiciel permet de creer des mosaiques d'images a partir d'une image source et d'une base de donnees d'imagettes. Il propose une interface graphique interactive utilisant Dear ImGui.

## Dependances (Linux)

Pour compiler et faire fonctionner ce projet, vous devez installer les paquets suivants :

- cmake : Outil de configuration de compilation
- g++ : Compilateur C++
- libglfw3-dev : Gestion des fenetres et des entrees
- libgl1-mesa-dev : Bibliotheque OpenGL
- zenity : Pour l'ouverture des explorateurs de fichiers natifs

Commande d'installation :
sudo apt update && sudo apt install cmake g++ libglfw3-dev libgl1-mesa-dev xorg-dev zenity

## Structure des dossiers

Le projet doit respecter l'organisation suivante :
- main.cpp : Code principal et boucle de rendu
- Ui.hpp : Gestion de l'interface utilisateur
- glad.c : Fichier de chargement OpenGL
- /imgui : Sources de la bibliotheque Dear ImGui
- /lib : Algorithmes de traitement d'image et classes de base

## Compilation

La compilation s'effectue a l'aide de CMake. Suivez ces etapes :

1. Creez un repertoire de build :
   mkdir build
   cd build

2. Generez les fichiers de compilation :
   cmake ..

3. Compilez le projet :
   make

4. Lancez le programme :
   ./MosaiqueApp

## Fonctionnement de l'interface

1. Fichiers : Selectionnez l'image d'entree (.ppm) et le dossier contenant les fichiers de la base de donnees (.bin).
2. Algorithmes : Choisissez entre Grille standard, Quadtree (basé sur la variance) ou SNIC (basé sur les superpixels).
3. Parametres : Ajustez les reglage specifiques (taille de grille, seuil de variance, nombre de superpixels, etc.).
4. Execution : Lancez la "Pass 1" pour generer la mosaique initiale, puis la "Pass 2" pour optimiser le resultat.

Les images resultantes sont sauvegardees automatiquement dans le dossier courant sous les noms 'mosaique_pass1.ppm' et 'mosaique_pass2.ppm'.