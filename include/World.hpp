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
    void handleRightClick(const sf::Vector2f& worldPosition);

    void stopSelectedUnits();
private:
    void moveSelectedUnitsTo(const sf::Vector2f& targetPosition);

    Unit* findUnitAt(const sf::Vector2f& worldPosition);
    const Unit* findUnitAt(const sf::Vector2f& worldPosition) const;

    Unit* findEnemyUnitAt(const sf::Vector2f& worldPosition);
    const Unit* findEnemyUnitAt(const sf::Vector2f& worldPosition) const;

    std::vector<Unit> m_vUnits;
};
