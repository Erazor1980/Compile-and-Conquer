#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>

#include "World.hpp"

World::World()
{
    if (!m_debugFont.openFromFile("assets/arial.ttf"))
    {
        throw std::runtime_error("Failed to load debug font: assets/arial.ttf");
    }

    // adding some player test units
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 100.0f, 100.0f }, 12.0f, 120.0f, UnitFaction::Player));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 200.0f, 180.0f }, 12.0f, 80.0f, UnitFaction::Player));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 320.0f, 260.0f }, 12.0f, 40.0f, UnitFaction::Player));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 500.0f, 700.0f }, 12.0f, 120.0f, UnitFaction::Player));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 600.0f, 400.0f }, 12.0f, 120.0f, UnitFaction::Player));

    // adding some enemy test units
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 800.0f, 300.0f }, 12.0f, 120.0f, UnitFaction::Enemy));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 800.0f, 600.0f }, 12.0f, 120.0f, UnitFaction::Enemy));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 800.0f, 740.0f }, 12.0f, 120.0f, UnitFaction::Enemy));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 840.0f, 360.0f }, 12.0f, 120.0f, UnitFaction::Enemy));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 1000.0f, 460.0f }, 12.0f, 120.0f, UnitFaction::Enemy));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 1000.0f, 760.0f }, 12.0f, 120.0f, UnitFaction::Enemy));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 1000.0f, 260.0f }, 12.0f, 120.0f, UnitFaction::Enemy));
}

void World::update(float deltaTime)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }

    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        pUnit->update(deltaTime);
    }

    // removing dead units
    m_vUnits.erase(
        std::remove_if(
            m_vUnits.begin(),
            m_vUnits.end(),
            [](const std::unique_ptr<Unit>& pUnit)
            {
                return !pUnit->isAlive();
            }),
        m_vUnits.end());
}

void World::render(sf::RenderTarget& target) const
{
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        pUnit->render(target);

        sf::Text hitPointText(m_debugFont);
        hitPointText.setCharacterSize(12);
        hitPointText.setFillColor(sf::Color::White);
        hitPointText.setString(std::to_string(static_cast<int>(pUnit->getHitPoints())));
        hitPointText.setPosition(pUnit->getPosition() + sf::Vector2f{ 16.0f, -18.0f });


        target.draw(hitPointText);
    }
}

void World::clearSelection()
{
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        pUnit->setSelected(false);
    }
}

void World::selectUnitAt(const sf::Vector2f& worldPosition)
{
    clearSelection();

    Unit* pUnit = findPlayerUnitAt(worldPosition);
    if (pUnit != nullptr)
    {
        pUnit->setSelected(true);
    }
}

void World::toggleUnitAt(const sf::Vector2f& worldPosition)
{
    Unit* pUnit = findPlayerUnitAt(worldPosition);
    if (pUnit != nullptr)
    {
        pUnit->setSelected(!pUnit->isSelected());
    }
}

void World::selectUnitsInRect(const sf::FloatRect& rect)
{
    clearSelection();

    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (pUnit->getFaction() == UnitFaction::Player && rect.contains(pUnit->getPosition()))
        {
            pUnit->setSelected(true);
        }
    }
}

void World::toggleUnitsInRect(const sf::FloatRect& rect)
{
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (pUnit->getFaction() == UnitFaction::Player && rect.contains(pUnit->getPosition()))
        {
            pUnit->setSelected(!pUnit->isSelected());
        }
    }
}

void World::handleRightClick(const sf::Vector2f& worldPosition)
{
    Unit* pTargetUnit = findEnemyUnitAt(worldPosition);

    if (pTargetUnit != nullptr)
    {
        for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
        {
            if (pUnit->isSelected() && pUnit->getFaction() == UnitFaction::Player)
            {
                pUnit->issueAttackCommand(pTargetUnit);
            }
        }

        return;
    }

    moveSelectedUnitsTo(worldPosition);
}

void World::moveSelectedUnitsTo(const sf::Vector2f& targetPosition)
{
    std::size_t selectedCount = 0;

    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (pUnit->isSelected())
        {
            ++selectedCount;
        }
    }

    if (selectedCount == 0)
    {
        return;
    }

    const float spacing = 20.0f;
    const std::size_t columns = static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<float>(selectedCount))));

    std::size_t selectedIndex = 0;

    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (pUnit->isSelected())
        {
            const std::size_t row = selectedIndex / columns;
            const std::size_t column = selectedIndex % columns;

            const float offsetX = (static_cast<float>(column) - (static_cast<float>(columns) - 1.0f) * 0.5f) * spacing;
            const float offsetY = static_cast<float>(row) * spacing;

            pUnit->issueMoveCommand(targetPosition + sf::Vector2f{offsetX, offsetY});
            ++selectedIndex;
        }
    }    
}


void World::stopSelectedUnits()
{
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (pUnit->isSelected())
        {
            pUnit->clearCommand();
        }
    }
}

Unit* World::findUnitAt(const sf::Vector2f& worldPosition)
{
    for (auto it = m_vUnits.rbegin(); it != m_vUnits.rend(); ++it)
    {
        if ((*it)->contains(worldPosition))
        {
            return it->get();
        }
    }

    return nullptr;
}

const Unit* World::findUnitAt(const sf::Vector2f& worldPosition) const
{
    for (auto it = m_vUnits.rbegin(); it != m_vUnits.rend(); ++it)
    {
        if ((*it)->contains(worldPosition))
        {
            return it->get();
        }
    }

    return nullptr;
}

Unit* World::findEnemyUnitAt(const sf::Vector2f& worldPosition)
{
    Unit* pUnit = findUnitAt(worldPosition);
    if (pUnit == nullptr)
    {
        return nullptr;
    }

    if (pUnit->getFaction() != UnitFaction::Enemy)
    {
        return nullptr;
    }

    return pUnit;
}

const Unit* World::findEnemyUnitAt(const sf::Vector2f& worldPosition) const
{
    const Unit* pUnit = findUnitAt(worldPosition);
    if (pUnit == nullptr)
    {
        return nullptr;
    }

    if (pUnit->getFaction() != UnitFaction::Enemy)
    {
        return nullptr;
    }

    return pUnit;
}

Unit* World::findPlayerUnitAt(const sf::Vector2f& worldPosition)
{
    Unit* pUnit = findUnitAt(worldPosition);
    if (pUnit == nullptr)
    {
        return nullptr;
    }

    if (pUnit->getFaction() != UnitFaction::Player)
    {
        return nullptr;
    }

    return pUnit;
}

const Unit* World::findPlayerUnitAt(const sf::Vector2f& worldPosition) const
{
    const Unit* pUnit = findUnitAt(worldPosition);
    if (pUnit == nullptr)
    {
        return nullptr;
    }

    if (pUnit->getFaction() != UnitFaction::Player)
    {
        return nullptr;
    }

    return pUnit;
}
