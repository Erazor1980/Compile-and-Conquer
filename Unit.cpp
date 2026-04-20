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

    target.draw(shape);
}

const sf::Vector2f& Unit::getPosition() const
{
    return m_position;
}