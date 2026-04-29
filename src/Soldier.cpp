#include "Soldier.hpp"

Soldier::Soldier(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats)
    : Unit(position, radius, moveSpeed, faction, UnitType::Soldier, stats)
{
    m_gunAngleDegrees = m_facingAngleDegrees;
}

void Soldier::renderBody(sf::RenderTarget& target) const
{
    // draw body
    sf::CircleShape shape(m_radius);
    shape.setOrigin({ m_radius, m_radius });
    shape.setPosition(m_position);

    shape.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

    target.draw(shape);

    // draw gun
    const float length = m_radius * 1.5f;
    const float thickness = 3.0f;

    sf::Color gunColor;
    if (m_faction == UnitFaction::Player)
    {
        gunColor = sf::Color(0, 120, 0);
    }
    else
    {
        gunColor = sf::Color(120, 0, 0);
    }

    sf::RectangleShape gun({ length, thickness });

    // Ursprung links mittig → rotiert sauber um Zentrum
    gun.setOrigin({ 0.0f, thickness * 0.5f });

    gun.setPosition(m_position);
    gun.setRotation(sf::degrees(m_gunAngleDegrees));
    gun.setFillColor(gunColor);

    target.draw(gun);
}

void Soldier::updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime)
{
    const sf::Vector2f toTarget = targetPosition - m_position;
    const float distanceSquared = (toTarget.x * toTarget.x) + (toTarget.y * toTarget.y);

    if (distanceSquared <= 0.0001f)
    {
        return;
    }

    m_gunAngleDegrees = std::atan2(toTarget.y, toTarget.x) * 180.0f / 3.14159265f;
}

void Soldier::resetWeaponDirectionToBody(float deltaTime)
{
    m_gunAngleDegrees = m_facingAngleDegrees;
}