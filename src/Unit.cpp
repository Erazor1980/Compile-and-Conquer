#include "Unit.hpp"

Unit::Unit(sf::Vector2f position, sf::Vector2f velocity, float radius)
    : m_position(position)
    , m_velocity(velocity)
    , m_radius(radius)
{
}

void Unit::update(float deltaTime)
{
    m_position += m_velocity * deltaTime;
}

void Unit::render(sf::RenderTarget& target) const
{
    sf::CircleShape shape(m_radius);
    shape.setFillColor(sf::Color::Green);
    shape.setOrigin({ m_radius, m_radius });
    shape.setPosition(m_position);

    if (m_bSelected)
    {
        shape.setOutlineThickness(2.0f);
        shape.setOutlineColor(sf::Color::Yellow);
    }
    else
    {
        shape.setOutlineThickness(0.0f);
    }

    target.draw(shape);
}

const sf::Vector2f& Unit::getPosition() const
{
    return m_position;
}

void Unit::setSelected(bool bSelected)
{
    m_bSelected = bSelected;
}

bool Unit::isSelected() const
{
    return m_bSelected;
}

bool Unit::contains(const sf::Vector2f& worldPosition) const
{
    const sf::Vector2f delta = worldPosition - m_position;
    const float distanceSquared = (delta.x * delta.x) + (delta.y * delta.y);
    const float radiusSquared = m_selectionRadius * m_selectionRadius;

    return distanceSquared <= radiusSquared;
}