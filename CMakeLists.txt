cmake_minimum_required(VERSION 3.10)
project(MosaiqueApp)

# ---------------------------------------------------------
# 1. STANDARDS C++
# ---------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# 2. RECHERCHE DES PACKAGES (Système)
# ---------------------------------------------------------
# Trouve OpenGL
find_package(OpenGL REQUIRED)

# Trouve GLFW3 (doit être installé sur ton système)
find_package(glfw3 REQUIRED)

# ---------------------------------------------------------
# 3. DÉFINITION DES SOURCES
# ---------------------------------------------------------

# Tes fichiers à toi
set(MY_SOURCES
    source/mosaic/src/main.cpp
    source/mosaic/src/lib/ImageBase.cpp
    source/mosaic/src/lib/Pixel.cpp
    source/mosaic/src/lib/Traitement.cpp
    source/mosaic/src/imagesInfo.cpp
    source/mosaic/src/Mask.cpp
    source/mosaic/src/Mosaique2.cpp
    source/super_pixel/src/snic_mex.cpp
    # Ajoute ici tes .cpp (ex: lib/ImageBase.cpp) si nécessaire
)

# Fichiers sources de Dear ImGui (Core)
set(IMGUI_SOURCES
    source/mosaic/src/lib/imgui/imgui.cpp
    source/mosaic/src/lib/imgui/imgui_draw.cpp
    source/mosaic/src/lib/imgui/imgui_widgets.cpp
    source/mosaic/src/lib/imgui/imgui_tables.cpp
    source/mosaic/src/lib/imgui/imgui_demo.cpp
)

# Fichiers sources des Backends ImGui (Pont avec GLFW/OpenGL)
set(IMGUI_BACKENDS_SOURCES
    source/mosaic/src/lib/imgui/backends/imgui_impl_glfw.cpp
    source/mosaic/src/lib/imgui/backends/imgui_impl_opengl3.cpp
)

# Chargeur OpenGL (GLAD)
set(GLAD_SOURCES
    source/mosaic/src/lib/glad/src/glad.c
)

# ---------------------------------------------------------
# 4. INCLUDES (Chemins vers les headers .h)
# ---------------------------------------------------------
include_directories(
    .
    source/mosaic/src/lib/
    source/mosaic/src/lib/imgui/
    source/mosaic/src/lib/imgui/backends/
    source/mosaic/src/lib/glad/include/
    # Ajoute le chemin vers glad/include si nécessaire
)

# ---------------------------------------------------------
# 5. CRÉATION DE L'EXÉCUTABLE
# ---------------------------------------------------------
add_executable(${PROJECT_NAME} 
    ${MY_SOURCES} 
    ${IMGUI_SOURCES} 
    ${IMGUI_BACKENDS_SOURCES} 
    ${GLAD_SOURCES}
)

# ---------------------------------------------------------
# 6. LIENS (Linker)
# ---------------------------------------------------------
# On lie l'exécutable avec les bibliothèques système
target_link_libraries(${PROJECT_NAME} 
    glfw 
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS} # Nécessaire pour charger les fonctions dynamiques sur Linux
)

# Pour Windows : évite parfois des problèmes de console
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
endif()