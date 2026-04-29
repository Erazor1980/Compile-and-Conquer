#pragma once

#include "Unit.hpp"

class Soldier : public Unit
{
public:
    Soldier(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats = {});

    //HERE new code:
protected:
    void renderBody(sf::RenderTarget& target) const override;
    void updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime) override;
    void resetWeaponDirectionToBody(float deltaTime) override;

private:
    float m_gunAngleDegrees{ 0.0f };
};