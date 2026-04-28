#pragma once

#include "Unit.hpp"

class Tank : public Unit
{
public:
    Tank(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats = {});

protected:
    void renderBody(sf::RenderTarget& target) const override;
    void updateFacingDirection(const sf::Vector2f& direction, float deltaTime) override;
    float calculateMovementSpeedFactor(const sf::Vector2f& direction) const;
    void updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime) override;
    void resetWeaponDirectionToBody(float deltaTime) override;
    bool canAttackTarget(const Unit& target) const override;

private:
    void rotateBarrelTowards(float targetAngleDegrees, float deltaTime);
    float calculateAngleTo(const sf::Vector2f& targetPosition) const;

    float m_barrelAngleDegrees{ 0.0f };

    // constants
    static constexpr float k_barrelTurnSpeed{ 180.0f };                  // barrel rotation speed in degrees per second
    static constexpr float k_bodyTurnSpeedDegreesPerSecond{ 100.0f };    // body rotation speed in degrees per second
    static constexpr float k_allowedAimErrorDegrees{ 10.0f };            // max angle difference to allow shooting
    static constexpr float k_minMoveSpeedFactor{ 0.2f };                 // minimum movement speed factor when misaligned
};