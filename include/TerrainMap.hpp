#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "TerrainType.hpp"

class TerrainMap
{
public:
    TerrainMap();

    void render(sf::RenderTarget& target) const;

    [[nodiscard]] sf::FloatRect getBounds() const;
    [[nodiscard]] TerrainType getTerrainAt(const sf::Vector2f& worldPosition) const;

private:
    [[nodiscard]] sf::Color getTerrainColor(TerrainType terrainType) const;

private:
    unsigned int m_widthInTiles{ 60 };
    unsigned int m_heightInTiles{ 40 };
    float m_tileSize{ 50.0f };
    std::vector<TerrainType> m_vTiles;
};