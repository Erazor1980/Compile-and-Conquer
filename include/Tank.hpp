#pragma once

#include "Unit.hpp"

class Tank : public Unit
{
public:
    Tank(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats = {});

protected:
    void renderBody(sf::RenderTarget& target) const override;
    void updateFacingDirection(const sf::Vector2f& direction, float deltaTime) override;
    void updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime) override;
    void resetWeaponDirectionToBody(float deltaTime) override;
    bool canAttackTarget(const Unit& target) const override;

private:
    void rotateBarrelTowards(float targetAngleDegrees, float deltaTime);
    float calculateAngleTo(const sf::Vector2f& targetPosition) const;

    float m_barrelAngleDegrees{ 0.0f };

    // constants
    static constexpr float k_allowedAimErrorDegrees{ 10.0f };
    static constexpr float k_barrelTurnSpeed{ 130.0f };
    static constexpr float k_bodyTurnSpeedDegreesPerSecond{ 100.0f };
};