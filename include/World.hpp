#pragma once

#include <vector>
#include "Unit.hpp"

class World
{
public:
    World();

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

private:
    std::vector<Unit> m_vUnits;
};
