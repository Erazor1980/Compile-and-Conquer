#include <cmath>

#include "Tank.hpp"

Tank::Tank(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats)
    : Unit(position, radius, moveSpeed, faction, UnitType::Tank, stats)
{
}

void Tank::render(sf::RenderTarget& target) const
{
    const float tankLength = m_radius * 2.4f;
    const float tankHeight = m_radius * 1.6f;

    sf::RectangleShape body({ tankLength, tankHeight });
    body.setOrigin({ tankLength * 0.5f, tankHeight * 0.5f });
    body.setPosition(m_position);
    body.setRotation(sf::degrees(m_facingAngleDegrees));
    body.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

    if (m_bSelected)
    {
        body.setOutlineThickness(2.0f);
        body.setOutlineColor(sf::Color::Yellow);
    }

    target.draw(body);

    const float barrelLength = tankLength * 0.65f;
    const float barrelThickness = m_radius * 0.3f;

    sf::Color barrelColor;
    if (m_faction == UnitFaction::Player)
    {
        barrelColor = sf::Color(0, 120, 0);
    }
    else
    {
        barrelColor = sf::Color(120, 0, 0);
    }

    sf::RectangleShape barrel({ barrelLength, barrelThickness });
    barrel.setOrigin({ 0.0f, barrelThickness * 0.5f });
    barrel.setPosition(m_position);
    barrel.setRotation(sf::degrees(m_barrelAngleDegrees));
    barrel.setFillColor(barrelColor);

    target.draw(barrel);

    const float pivotRadius = m_radius * 0.35f;

    sf::CircleShape pivot(pivotRadius);
    pivot.setOrigin({ pivotRadius, pivotRadius });
    pivot.setPosition(m_position);
    pivot.setFillColor(sf::Color(100, 100, 100));

    target.draw(pivot);
}

void Tank::updateWeaponDirectionTo(const sf::Vector2f& targetPosition)
{
    const sf::Vector2f toTarget = targetPosition - m_position;
    const float distanceSquared = (toTarget.x * toTarget.x) + (toTarget.y * toTarget.y);

    if (distanceSquared <= 0.0001f)
    {
        return;
    }

    updateBarrelDirection(toTarget / std::sqrt(distanceSquared));
}

void Tank::resetWeaponDirectionToBody()
{
    m_barrelAngleDegrees = m_facingAngleDegrees;
}

void Tank::updateBarrelDirection(const sf::Vector2f& direction)
{
    if ((direction.x * direction.x) + (direction.y * direction.y) <= 0.0001f)
    {
        return;
    }

    m_barrelAngleDegrees = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
}