#include "World.hpp"

World::World()
{
    // adding some test units
    m_vUnits.emplace_back(sf::Vector2f{ 100.0f, 100.0f }, 12.0f, 120.0f);
    m_vUnits.emplace_back(sf::Vector2f{ 200.0f, 180.0f }, 12.0f, 80.0f);
    m_vUnits.emplace_back(sf::Vector2f{ 320.0f, 260.0f }, 12.0f, 40.0f);
    m_vUnits.emplace_back(sf::Vector2f{ 500.0f, 700.0f }, 12.0f, 120.0f);
    m_vUnits.emplace_back(sf::Vector2f{ 600.0f, 400.0f }, 12.0f, 120.0f);
    m_vUnits.emplace_back(sf::Vector2f{ 800.0f, 300.0f }, 12.0f, 120.0f);
}

void World::update(float deltaTime)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }

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
            return;
        }
    }
}

void World::toggleUnitAt(const sf::Vector2f& worldPosition)
{
    for (auto it = m_vUnits.rbegin(); it != m_vUnits.rend(); ++it)
    {
        if (it->contains(worldPosition))
        {
            it->setSelected(!it->isSelected());
            return;
        }
    }
}

void World::selectUnitsInRect(const sf::FloatRect& rect)
{
    clearSelection();

    for (Unit& unit : m_vUnits)
    {
        if (rect.contains(unit.getPosition()))
        {
            unit.setSelected(true);
        }
    }
}

void World::toggleUnitsInRect(const sf::FloatRect& rect)
{
    for (Unit& unit : m_vUnits)
    {
        if (rect.contains(unit.getPosition()))
        {
            unit.setSelected(!unit.isSelected());
        }
    }
}

void World::moveSelectedUnitsTo(const sf::Vector2f& targetPosition)
{
    std::size_t selectedIndex = 0;

    for (Unit& unit : m_vUnits)
    {
        if (unit.isSelected())
        {
            const std::size_t row = selectedIndex / 3;
            const std::size_t column = selectedIndex % 3;

            const float offsetX = (static_cast<float>(column) - 1.0f) * 20.0f;
            const float offsetY = static_cast<float>(row) * 20.0f;

            unit.setMoveTarget(targetPosition + sf::Vector2f{ offsetX, offsetY });
            ++selectedIndex;
        }
    }
}