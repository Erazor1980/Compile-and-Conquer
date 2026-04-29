#include "Aircraft.hpp"

Aircraft::Aircraft(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats)
    : Unit(position, radius, moveSpeed, faction, stats)
{
}

void Aircraft::renderBody(sf::RenderTarget& target) const
{
    sf::ConvexShape shape;
    shape.setPointCount(3);
    shape.setPoint(0, { m_radius * 2.4f, m_radius * 1.2f });
    shape.setPoint(1, { 0.0f, 0.0f });
    shape.setPoint(2, { 0.0f, m_radius * 2.4f });
    shape.setOrigin({ m_radius * 1.2f, m_radius * 1.2f });
    shape.setPosition(m_position);
    shape.setRotation(sf::degrees(m_facingAngleDegrees));
    shape.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

    target.draw(shape);
}