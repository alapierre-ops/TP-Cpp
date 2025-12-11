# Configuration de CMake pour TP C++ Moderne esimed
cmake_minimum_required(VERSION 3.29)

# Nom du projet et de l'exécutable généré
project(mon_projet)

# Configuration du SDK macOS pour LLVM/Homebrew
if(APPLE AND NOT MSVC)
    # Trouver le SDK macOS
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(MACOS_SDK_PATH)
        set(CMAKE_OSX_SYSROOT ${MACOS_SDK_PATH})
        message(STATUS "Using macOS SDK: ${MACOS_SDK_PATH}")
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
# Forcer l'utilisation de C++ 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Activer le scan de modules C++ (nécessaire pour les modules C++20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
# Tout doit être parfait
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
# Ne pas utiliser les extensions de CMake
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
endif()
include(cmake/compile_options.cmake)

# Pour "import std" - doit être inclus AVANT la création des exécutables
# car les modules qui utilisent "import std" ont besoin que std-cxx-modules existe
include(cmake/std.cmake)

# --- SFML ---
# SFML 3 utilise des composants avec des noms en PascalCase (Graphics, Window, System, ...)
find_package(SFML 3.0.2 COMPONENTS Graphics Window System Audio REQUIRED)

# Répertoire contenant les sources des exercices
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

#######################################################
# Partie à mettre à jour manuellement
# Liste des fichiers sources des modules des exercices
# Exécutable principal
add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
)
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        ${SRC_DIR}/sprites.ixx
        ${SRC_DIR}/game_types.ixx
        ${SRC_DIR}/game.ixx
)
compile_options(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SFML::Graphics SFML::Window SFML::System SFML::Audio)