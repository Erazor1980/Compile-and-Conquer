#include <cmath>

#include "Tank.hpp"

Tank::Tank(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats)
    : Unit(position, radius, moveSpeed, faction, stats)
{
}

void Tank::setFacingAngleDegrees(float facingAngleDegrees)
{
    Unit::setFacingAngleDegrees(facingAngleDegrees);
    m_barrelAngleDegrees = facingAngleDegrees;
}

void Tank::renderBody(sf::RenderTarget& target) const
{
    const float tankLength = m_radius * 2.4f;
    const float tankHeight = m_radius * 1.6f;

    sf::RectangleShape body({ tankLength, tankHeight });
    body.setOrigin({ tankLength * 0.5f, tankHeight * 0.5f });
    body.setPosition(m_position);
    body.setRotation(sf::degrees(m_facingAngleDegrees));
    body.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

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


void Tank::updateFacingDirection(const sf::Vector2f& direction, float deltaTime)
{
    if ((direction.x * direction.x) + (direction.y * direction.y) <= 0.0001f)
    {
        return;
    }

    const float targetAngleDegrees = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
    const float angleDifference = std::remainder(targetAngleDegrees - m_facingAngleDegrees, 360.0f);
    const float maxStep = k_bodyTurnSpeedDegreesPerSecond * deltaTime;

    if (std::abs(angleDifference) <= maxStep)
    {
        m_facingAngleDegrees = targetAngleDegrees;
        return;
    }

    m_facingAngleDegrees += std::copysign(maxStep, angleDifference);
}

float Tank::getTerrainMovementFactor(TerrainType terrainType) const
{
    switch (terrainType)
    {
    case TerrainType::Road:
        return 1.5f;
    case TerrainType::Grass:
        return 1.0f;
    case TerrainType::Dirt:
        return 0.65f;
    case TerrainType::Water:
        return 0.25f;
    case TerrainType::Mountain:
        return 0.4f;
    }

    return 1.0f;
}

float Tank::calculateMovementSpeedFactor(const sf::Vector2f& direction, float distanceToTarget) const
{
    if ((direction.x * direction.x) + (direction.y * direction.y) <= 0.0001f)
    {
        return 1.0f;
    }

    const float targetAngleDegrees = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
    const float angleDifference = std::abs(std::remainder(targetAngleDegrees - m_facingAngleDegrees, 360.0f));

    if (angleDifference > k_reverseAngleThresholdDegrees && distanceToTarget <= k_reverseDistanceThreshold)
    {
        return k_reverseMoveSpeedFactor;
    }

    const float angleRad = angleDifference * 3.14159265f / 180.0f;
    float factor = std::cos(angleRad);

    if (factor < k_minMoveSpeedFactor)
    {
        factor = k_minMoveSpeedFactor;
    }

    return factor;
}

sf::Vector2f Tank::calculateFacingDirectionForMovement(const sf::Vector2f& direction, float distanceToTarget) const
{
    if ((direction.x * direction.x) + (direction.y * direction.y) <= 0.0001f)
    {
        return direction;
    }

    const float targetAngleDegrees = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
    const float angleDifference = std::abs(std::remainder(targetAngleDegrees - m_facingAngleDegrees, 360.0f));

    if (angleDifference > k_reverseAngleThresholdDegrees && distanceToTarget <= k_reverseDistanceThreshold)
    {
        return sf::Vector2f{ -direction.x, -direction.y };
    }

    return direction;
}

void Tank::updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime)
{
    const float targetAngleDegrees = calculateAngleTo(targetPosition);
    rotateBarrelTowards(targetAngleDegrees, deltaTime);
}

void Tank::resetWeaponDirectionToBody(float deltaTime)
{
    rotateBarrelTowards(m_facingAngleDegrees, deltaTime);
}

bool Tank::canAttackTarget(const Unit& target) const
{
    const float targetAngleDegrees = calculateAngleTo(target.getPosition());
    const float angleDifference = std::abs(std::remainder(targetAngleDegrees - m_barrelAngleDegrees, 360.0f));

    return angleDifference <= k_allowedAimErrorDegrees;
}

void Tank::rotateBarrelTowards(float targetAngleDegrees, float deltaTime)
{
    const float angleDifference = std::remainder(targetAngleDegrees - m_barrelAngleDegrees, 360.0f);
    const float maxStep = k_barrelTurnSpeed * deltaTime;

    if (std::abs(angleDifference) <= maxStep)
    {
        m_barrelAngleDegrees = targetAngleDegrees;
        return;
    }

    m_barrelAngleDegrees += std::copysign(maxStep, angleDifference);
}

float Tank::calculateAngleTo(const sf::Vector2f& targetPosition) const
{
    const sf::Vector2f toTarget = targetPosition - m_position;
    const float distanceSquared = (toTarget.x * toTarget.x) + (toTarget.y * toTarget.y);

    if (distanceSquared <= 0.0001f)
    {
        return m_barrelAngleDegrees;
    }

    return std::atan2(toTarget.y, toTarget.x) * 180.0f / 3.14159265f;
}