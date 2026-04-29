#include "Soldier.hpp"

Soldier::Soldier(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats)
    : Unit(position, radius, moveSpeed, faction, UnitType::Soldier, stats)
{
}

void Soldier::renderBody(sf::RenderTarget& target) const
{
    sf::CircleShape shape(m_radius);
    shape.setOrigin({ m_radius, m_radius });
    shape.setPosition(m_position);

    shape.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

    target.draw(shape);
}