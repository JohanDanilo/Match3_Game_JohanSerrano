#include "ResourceManager.h"

const Texture& ResourceManager::getTexture(const string& filename) {
    if (textures.find(filename) == textures.end()) {
        if (!textures[filename].loadFromFile(filename)) {
            textures.erase(filename);
            throw runtime_error("Error cargando textura: " + filename);
        }
        textures[filename].setSmooth(true);
    }
    return textures.at(filename);
}

const Font& ResourceManager::getFont(const string& filename) {
    if (fonts.find(filename) == fonts.end()) {
        if (!fonts[filename].loadFromFile(filename)) {
            fonts.erase(filename);
            throw runtime_error("Error cargando fuente: " + filename);
        }
    }
    return fonts.at(filename);
}

ResourceManager& ResourceManager::instance() {
    static ResourceManager rm;
    return rm;
}
