#include <iostream>
#include <array>

#include "TerrainMap.hpp"

TerrainMap::TerrainMap()
    : m_sprite(m_texture)
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
    target.draw(m_sprite);
}

bool TerrainMap::loadFromImage(const std::string& filePath)
{
    sf::Image image;
    if (!image.loadFromFile(filePath))
    {
        std::cout << "Failed to load terrain image: " << filePath << std::endl;
        return false;
    }

    m_widthInTiles = image.getSize().x;
    m_heightInTiles = image.getSize().y;

    m_vTiles.clear();
    m_vTiles.resize(static_cast<std::size_t>(m_widthInTiles * m_heightInTiles));

    for (unsigned int y = 0; y < m_heightInTiles; ++y)
    {
        for (unsigned int x = 0; x < m_widthInTiles; ++x)
        {
            const sf::Color color = image.getPixel({ x, y });
            const TerrainType terrain = colorToTerrainType(color);

            m_vTiles[x + y * m_widthInTiles] = terrain;
        }
    }

    // --- Build render image using game colors ---
    sf::Image renderImage;
    renderImage.resize({ m_widthInTiles, m_heightInTiles });

    for (unsigned int y = 0; y < m_heightInTiles; ++y)
    {
        for (unsigned int x = 0; x < m_widthInTiles; ++x)
        {
            const TerrainType terrain = m_vTiles[x + y * m_widthInTiles];
            const sf::Color renderColor = getTerrainColor(terrain);

            renderImage.setPixel({ x, y }, renderColor);
        }
    }

    // --- Use render image for texture ---
    if (!m_texture.loadFromImage(renderImage))
    {
        std::cout << "Failed to create terrain texture from render image: " << filePath << std::endl;
        return false;
    }

    m_texture.setSmooth(false);
    m_sprite.setTexture(m_texture, true);

    return true;
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
        return sf::Color(20, 120, 20);
    case TerrainType::Water:
        return sf::Color(40, 80, 180);
    case TerrainType::Dirt:
        return sf::Color(120, 80, 40);
    case TerrainType::Mountain:
        return sf::Color(20, 20, 20);
    }

    return sf::Color::Magenta;
}

bool TerrainMap::isColorClose(const sf::Color& a, const sf::Color& b, int tolerance) const
{
    return
        std::abs(static_cast<int>(a.r) - static_cast<int>(b.r)) <= tolerance &&
        std::abs(static_cast<int>(a.g) - static_cast<int>(b.g)) <= tolerance &&
        std::abs(static_cast<int>(a.b) - static_cast<int>(b.b)) <= tolerance;
}

TerrainType TerrainMap::colorToTerrainType(const sf::Color& color) const
{
    const int tolerance = 20;

    if (isColorClose(color, sf::Color{ 128,128,128 }, tolerance))
    {
        return TerrainType::Road;
    }
    if (isColorClose(color, sf::Color{ 0,255,0 }, tolerance))
    {
        return TerrainType::Grass;
    }
    if (isColorClose(color, sf::Color{ 0,0,255 }, tolerance))
    {
        return TerrainType::Water;
    }
    if (isColorClose(color, sf::Color{ 139,69,19 }, tolerance))
    {
        return TerrainType::Dirt;
    }
    if (isColorClose(color, sf::Color{ 0,0,0 }, tolerance))
    {
        return TerrainType::Mountain;
    }

    const std::uint32_t key =
        (static_cast<std::uint32_t>(color.r) << 16) |
        (static_cast<std::uint32_t>(color.g) << 8) |
        static_cast<std::uint32_t>(color.b);

    if (m_loggedUnknownColors.insert(key).second)
    {
        std::cout << "Unknown color encountered: ("
            << static_cast<int>(color.r) << ", "
            << static_cast<int>(color.g) << ", "
            << static_cast<int>(color.b) << ")" << std::endl;
    }

    return TerrainType::Grass;
}



// Temporary terrain map cleaning utility integrated into the game project.
// Intended to become part of a dedicated external tools pipeline later.
int TerrainMap::calculateColorDistanceSquared(const sf::Color& a, const sf::Color& b) const
{
    const int redDifference = static_cast<int>(a.r) - static_cast<int>(b.r);
    const int greenDifference = static_cast<int>(a.g) - static_cast<int>(b.g);
    const int blueDifference = static_cast<int>(a.b) - static_cast<int>(b.b);

    return redDifference * redDifference +
        greenDifference * greenDifference +
        blueDifference * blueDifference;
}

sf::Color TerrainMap::findClosestValidColor(const sf::Color& color) const
{
    const std::array<sf::Color, 5> validColors{
        sf::Color{ 128, 128, 128 },
        sf::Color{ 0, 255, 0 },
        sf::Color{ 0, 0, 255 },
        sf::Color{ 139, 69, 19 },
        sf::Color{ 0, 0, 0 }
    };

    sf::Color closestColor = validColors[0];
    int bestDistance = calculateColorDistanceSquared(color, closestColor);

    for (const sf::Color& validColor : validColors)
    {
        const int distance = calculateColorDistanceSquared(color, validColor);

        if (distance < bestDistance)
        {
            bestDistance = distance;
            closestColor = validColor;
        }
    }

    return closestColor;
}

bool TerrainMap::isValidTerrainColor(const sf::Color& color) const
{
    return
        color == sf::Color{ 128, 128, 128 } ||
        color == sf::Color{ 0, 255, 0 } ||
        color == sf::Color{ 0, 0, 255 } ||
        color == sf::Color{ 139, 69, 19 } ||
        color == sf::Color{ 0, 0, 0 };
}

sf::Color TerrainMap::findMostCommonValidNeighborColor(const sf::Image& image, unsigned int pixelX, unsigned int pixelY) const
{
    const std::array<sf::Color, 5> validColors{
        sf::Color{ 128, 128, 128 },
        sf::Color{ 0, 255, 0 },
        sf::Color{ 0, 0, 255 },
        sf::Color{ 139, 69, 19 },
        sf::Color{ 0, 0, 0 }
    };

    std::array<int, 5> counts{ 0, 0, 0, 0, 0 };

    const sf::Vector2u imageSize = image.getSize();

    for (int offsetY = -1; offsetY <= 1; ++offsetY)
    {
        for (int offsetX = -1; offsetX <= 1; ++offsetX)
        {
            if (offsetX == 0 && offsetY == 0)
            {
                continue;
            }

            const int neighborX = static_cast<int>(pixelX) + offsetX;
            const int neighborY = static_cast<int>(pixelY) + offsetY;

            if (neighborX < 0 || neighborY < 0 ||
                neighborX >= static_cast<int>(imageSize.x) ||
                neighborY >= static_cast<int>(imageSize.y))
            {
                continue;
            }

            const sf::Color neighborColor = image.getPixel({
                static_cast<unsigned int>(neighborX),
                static_cast<unsigned int>(neighborY)
                });

            for (std::size_t i = 0; i < validColors.size(); ++i)
            {
                if (neighborColor == validColors[i])
                {
                    ++counts[i];
                    break;
                }
            }
        }
    }

    int bestCount = 0;
    sf::Color bestColor = sf::Color::Transparent;

    for (std::size_t i = 0; i < validColors.size(); ++i)
    {
        if (counts[i] > bestCount)
        {
            bestCount = counts[i];
            bestColor = validColors[i];
        }
    }

    return bestColor;
}

bool TerrainMap::cleanImageFile(const std::string& inputFilePath, const std::string& outputFilePath) const
{
    sf::Image image;
    if (!image.loadFromFile(inputFilePath))
    {
        std::cout << "Failed to load terrain image for cleaning: " << inputFilePath << std::endl;
        return false;
    }

    const sf::Vector2u imageSize = image.getSize();
    std::size_t correctedPixelCount = 0;

    for (unsigned int y = 0; y < imageSize.y; ++y)
    {
        for (unsigned int x = 0; x < imageSize.x; ++x)
        {
            const sf::Color originalColor = image.getPixel({ x, y });

            if (isValidTerrainColor(originalColor))
            {
                continue;
            }

            sf::Color replacementColor = findMostCommonValidNeighborColor(image, x, y);

            if (replacementColor == sf::Color::Transparent)
            {
                replacementColor = findClosestValidColor(originalColor);
            }

            image.setPixel({ x, y }, replacementColor);
            ++correctedPixelCount;
        }
    }

    if (!image.saveToFile(outputFilePath))
    {
        std::cout << "Failed to save cleaned terrain image: " << outputFilePath << std::endl;
        return false;
    }

    std::cout << "Cleaned terrain image saved: " << outputFilePath
        << " corrected pixels: " << correctedPixelCount << std::endl;

    return true;
}