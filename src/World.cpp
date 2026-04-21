#include "World.hpp"

World::World()
{
    // adding some test units
    m_vUnits.emplace_back(sf::Vector2f{ 100.0f, 100.0f }, sf::Vector2f{ 30.0f, 0.0f }, 12.0f);
    m_vUnits.emplace_back(sf::Vector2f{ 200.0f, 180.0f }, sf::Vector2f{ 0.0f, 20.0f }, 12.0f);
    m_vUnits.emplace_back(sf::Vector2f{ 320.0f, 260.0f }, sf::Vector2f{ 15.0f, 10.0f }, 12.0f);
}

void World::update(float deltaTime)
{
    for (Unit& unit : m_vUnits)
    {
        unit.update(deltaTime);
    }
}

void World::render(sf::RenderTarget& target) const
{
    for (const Unit& unit : m_vUnits)
    {
        unit.render(target);
    }
}

void World::clearSelection()
{
    for (Unit& unit : m_vUnits)
    {
        unit.setSelected(false);
    }
}

void World::selectUnitAt(const sf::Vector2f& worldPosition)
{
    clearSelection();

    for (auto it = m_vUnits.rbegin(); it != m_vUnits.rend(); ++it)
    {
        if (it->contains(worldPosition))
        {
            it->setSelected(true);
            break;
        }
    }
}