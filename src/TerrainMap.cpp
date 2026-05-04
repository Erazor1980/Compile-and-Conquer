#include "TerrainMap.hpp"

TerrainMap::TerrainMap()
{
    m_vTiles.resize(static_cast<std::size_t>(m_widthInTiles * m_heightInTiles), TerrainType::Grass);

    for (unsigned int x = 0; x < m_widthInTiles; ++x)
    {
        m_vTiles[x + 10 * m_widthInTiles] = TerrainType::Road;
    }

    for (unsigned int y = 0; y < m_heightInTiles; ++y)
    {
        m_vTiles[20 + y * m_widthInTiles] = TerrainType::Dirt;
    }
}

void TerrainMap::render(sf::RenderTarget& target) const
{
    sf::RectangleShape tileShape;
    tileShape.setSize({ m_tileSize, m_tileSize });

    for (unsigned int y = 0; y < m_heightInTiles; ++y)
    {
        for (unsigned int x = 0; x < m_widthInTiles; ++x)
        {
            const TerrainType terrainType = m_vTiles[x + y * m_widthInTiles];

            tileShape.setPosition({
                static_cast<float>(x) * m_tileSize,
                static_cast<float>(y) * m_tileSize
                });

            tileShape.setFillColor(getTerrainColor(terrainType));
            target.draw(tileShape);
        }
    }
}

sf::FloatRect TerrainMap::getBounds() const
{
    return sf::FloatRect{
        sf::Vector2f{ 0.0f, 0.0f },
        sf::Vector2f{
            static_cast<float>(m_widthInTiles) * m_tileSize,
            static_cast<float>(m_heightInTiles) * m_tileSize
        }
    };
}

TerrainType TerrainMap::getTerrainAt(const sf::Vector2f& worldPosition) const
{
    const int x = static_cast<int>(worldPosition.x / m_tileSize);
    const int y = static_cast<int>(worldPosition.y / m_tileSize);

    if (x < 0 || y < 0 || x >= static_cast<int>(m_widthInTiles) || y >= static_cast<int>(m_heightInTiles))
    {
        return TerrainType::Mountain;
    }

    return m_vTiles[static_cast<std::size_t>(x + y * m_widthInTiles)];
}

sf::Color TerrainMap::getTerrainColor(TerrainType terrainType) const
{
    switch (terrainType)
    {
    case TerrainType::Road:
        return sf::Color(120, 120, 120);
    case TerrainType::Grass:
        return sf::Color(40, 120, 40);
    case TerrainType::Water:
        return sf::Color(40, 80, 180);
    case TerrainType::Dirt:
        return sf::Color(120, 80, 40);
    case TerrainType::Mountain:
        return sf::Color(20, 20, 20);
    }

    return sf::Color::Magenta;
}