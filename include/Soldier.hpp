#pragma once

#include "Unit.hpp"

class Soldier : public Unit
{
public:
    Soldier(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats = {});

protected:
    void renderBody(sf::RenderTarget& target) const override;
};