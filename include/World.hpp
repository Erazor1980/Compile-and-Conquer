#pragma once

#include <vector>
#include "Unit.hpp"

class World
{
public:
    World();

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    void clearSelection();
    void selectUnitAt(const sf::Vector2f& worldPosition);
    void toggleUnitAt(const sf::Vector2f& worldPosition);
    void selectUnitsInRect(const sf::FloatRect& rect);
    void toggleUnitsInRect(const sf::FloatRect& rect);
private:
    std::vector<Unit> m_vUnits;
};
