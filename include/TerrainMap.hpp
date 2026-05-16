#pragma once

#include <vector>
#include <unordered_set>
#include <SFML/Graphics.hpp>

#include "TerrainType.hpp"

class TerrainMap
{
public:
    TerrainMap();

    void render(sf::RenderTarget& target) const;
    bool loadFromImage(const std::string& filePath);

    [[nodiscard]] sf::FloatRect getBounds() const;
    [[nodiscard]] TerrainType getTerrainAt(const sf::Vector2f& worldPosition) const;

    // Utility function to clean terrain map images created in external tools like Paint.
    // Unknown or interpolated colors are replaced with the closest valid terrain color,
    // preventing terrain detection errors caused by anti aliasing or color artifacts.
    //
    // Currently integrated directly into the game project for simplicity and reuse of
    // the existing SFML setup. Could later be moved into a dedicated tools pipeline
    // or standalone map utility.
    bool cleanImageFile(const std::string& inputFilePath, const std::string& outputFilePath) const;
    [[nodiscard]] int calculateColorDistanceSquared(const sf::Color& a, const sf::Color& b) const;
    [[nodiscard]] sf::Color findClosestValidColor(const sf::Color& color) const;
    [[nodiscard]] bool isValidTerrainColor(const sf::Color& color) const;
    [[nodiscard]] sf::Color findMostCommonValidNeighborColor(const sf::Image& image, unsigned int pixelX, unsigned int pixelY) const;

private:
    [[nodiscard]] sf::Color getTerrainColor(TerrainType terrainType) const;
    [[nodiscard]] TerrainType colorToTerrainType(const sf::Color& color) const;
    [[nodiscard]] bool isColorClose(const sf::Color& a, const sf::Color& b, int tolerance) const;
    
private:
    unsigned int m_widthInTiles{ 60 };
    unsigned int m_heightInTiles{ 40 };
    float m_tileSize{ 1.0f };
    std::vector<TerrainType> m_vTiles;
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    mutable std::unordered_set<std::uint32_t> m_loggedUnknownColors;
};