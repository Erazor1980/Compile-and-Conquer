#include "Unit.hpp"

Unit::Unit(sf::Vector2f position, float radius, float moveSpeed)
    : m_position(position)
    , m_targetPosition(position)
    , m_radius(radius)
    , m_moveSpeed(moveSpeed)
{
}

void Unit::update(float deltaTime)
{
    if (!m_bHasMoveTarget)
    {
        return;
    }

    const sf::Vector2f toTarget = m_targetPosition - m_position;
    const float distanceSquared = (toTarget.x * toTarget.x) + (toTarget.y * toTarget.y);

    if (distanceSquared <= 0.0001f)
    {
        m_position = m_targetPosition;
        m_bHasMoveTarget = false;
        return;
    }

    const float distance = std::sqrt(distanceSquared);
    const float maxStep = m_moveSpeed * deltaTime;

    if (distance <= maxStep)
    {
        m_position = m_targetPosition;
        m_bHasMoveTarget = false;
        return;
    }

    const sf::Vector2f direction = toTarget / distance;
    m_position += direction * maxStep;
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

    if (m_bSelected && m_bHasMoveTarget)
    {
        sf::Vertex line[] =
        {
            sf::Vertex{ m_position, sf::Color::Red },
            sf::Vertex{ m_targetPosition, sf::Color::Red }
        };

        sf::CircleShape targetMarker(4.0f);
        targetMarker.setFillColor(sf::Color::Red);
        targetMarker.setOrigin({ 4.0f, 4.0f });
        targetMarker.setPosition(m_targetPosition);

        target.draw(line, 2, sf::PrimitiveType::Lines);
        target.draw(targetMarker);
    }
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
    //const float radiusSquared = m_selectionRadius * m_selectionRadius;
    const float radiusSquared = m_radius * m_radius;

    return distanceSquared <= radiusSquared;
}

void Unit::setMoveTarget(const sf::Vector2f& targetPosition)
{
    m_targetPosition = targetPosition;
    m_bHasMoveTarget = true;
}

void Unit::stopMovement()
{
    m_targetPosition = m_position;
    m_bHasMoveTarget = false;
}

bool Unit::hasMoveTarget() const
{
    return m_bHasMoveTarget;
}