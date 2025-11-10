#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

using namespace sf;
using namespace std;

class ResourceManager {
private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;

    ResourceManager() = default;  // constructor privado (singleton)

public:
    // --- Texturas ---
    const Texture& getTexture(const std::string& filename);

    // --- Fuentes ---
    const Font& getFont(const std::string& filename);

    // --- Singleton ---
    static ResourceManager& instance();

    // --- No copiable ---
    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;
};
