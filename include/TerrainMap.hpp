#pragma once

#include <vector>
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
};