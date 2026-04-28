#pragma once

#include "Unit.hpp"

class Tank : public Unit
{
public:
    Tank(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats = {});

protected:
    void renderBody(sf::RenderTarget& target) const override;
    void updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime) override;
    void resetWeaponDirectionToBody(float deltaTime) override;

private:
    void rotateBarrelTowards(float targetAngleDegrees, float deltaTime);
    float calculateAngleTo(const sf::Vector2f& targetPosition) const;

    float m_barrelAngleDegrees{ 0.0f };
    float m_barrelTurnSpeedDegreesPerSecond{ 120.0f };
};