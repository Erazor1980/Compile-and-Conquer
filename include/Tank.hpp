#pragma once

#include "Unit.hpp"

class Tank : public Unit
{
public:
    Tank(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats = {});

protected:
    void renderBody(sf::RenderTarget& target) const override;
    void updateWeaponDirectionTo(const sf::Vector2f& targetPosition) override;
    void resetWeaponDirectionToBody() override;

private:
    void updateBarrelDirection(const sf::Vector2f& direction);

    float m_barrelAngleDegrees{ 0.0f };
};