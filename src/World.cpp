#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>

#include "World.hpp"
#include "Tank.hpp"

World::World()
{
    if (!m_debugFont.openFromFile("assets/arial.ttf"))
    {
        throw std::runtime_error("Failed to load debug font: assets/arial.ttf");
    }

    const UnitStats defaultStats{};

    const UnitStats heavyStats{
        180.0f,
        30.0f,
        170.0f,
        0.8f
    };

    const UnitStats fastAttackStats{
        80.0f,
        60.0f,
        55.0f,
        0.25f
    };

    const UnitStats soldierStats{
       100.0f, // max hit points
       25.0f,  // damage per second
       80.0f,  // attack range
       0.6f    // attack interval
    };

    const UnitStats tankStats{
        220.0f, // max hit points
        55.0f,  // damage per second
        160.0f, // attack range
        0.9f    // attack interval
    };

    const UnitStats aircraftStats{
        140.0f, // max hit points
        35.0f,  // damage per second
        120.0f, // attack range
        0.45f   // attack interval
    };

    const float soldierRadius = 8.0f;
    const float tankRadius = 16.0f;
    const float aircraftRadius = 11.0f;

    // adding some player test units
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 100.0f, 100.0f }, soldierRadius, 120.0f, UnitFaction::Player, UnitType::Soldier, soldierStats));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 500.0f, 700.0f }, soldierRadius, 120.0f, UnitFaction::Player, UnitType::Soldier, soldierStats));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 600.0f, 400.0f }, soldierRadius, 120.0f, UnitFaction::Player, UnitType::Soldier, soldierStats));
    m_vUnits.emplace_back(std::make_unique<Tank>(sf::Vector2f{ 200.0f, 180.0f }, tankRadius, 80.0f, UnitFaction::Player, tankStats));
    m_vUnits.emplace_back(std::make_unique<Tank>(sf::Vector2f{ 400.0f, 280.0f }, tankRadius, 80.0f, UnitFaction::Player, tankStats));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 320.0f, 260.0f }, aircraftRadius, 140.0f, UnitFaction::Player, UnitType::Aircraft, aircraftStats));

    // adding some enemy test units
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 800.0f, 300.0f }, soldierRadius, 120.0f, UnitFaction::Enemy, UnitType::Soldier, soldierStats));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 800.0f, 740.0f }, soldierRadius, 120.0f, UnitFaction::Enemy, UnitType::Soldier, soldierStats));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 1000.0f, 460.0f }, soldierRadius, 120.0f, UnitFaction::Enemy, UnitType::Soldier, soldierStats));
    m_vUnits.emplace_back(std::make_unique<Tank>(sf::Vector2f{ 800.0f, 600.0f }, tankRadius, 80.0f, UnitFaction::Enemy, tankStats));
    m_vUnits.emplace_back(std::make_unique<Tank>(sf::Vector2f{ 1000.0f, 760.0f }, tankRadius, 80.0f, UnitFaction::Enemy, tankStats));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 840.0f, 360.0f }, aircraftRadius, 140.0f, UnitFaction::Enemy, UnitType::Aircraft, aircraftStats));
    m_vUnits.emplace_back(std::make_unique<Unit>(sf::Vector2f{ 1000.0f, 260.0f }, aircraftRadius, 140.0f, UnitFaction::Enemy, UnitType::Aircraft, aircraftStats));
}

void World::update(float deltaTime)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }

    m_markerPulseTime += deltaTime;

    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        pUnit->update(deltaTime, m_vUnits);
    }

    // cleanup attack commands that reference units which will be removed (avoid dangling pointers)
    std::vector<const Unit*> vDeadUnits;
    vDeadUnits.reserve(m_vUnits.size());

    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (!pUnit->isAlive())
        {
            vDeadUnits.push_back(pUnit.get());
        }
    }

    for (const Unit* pDeadUnit : vDeadUnits)
    {
        for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
        {
            pUnit->clearAttackCommandIfTarget(pDeadUnit);
        }
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

    m_pHoveredUnit = findUnitAt(m_mouseWorldPosition);
}

void World::render(sf::RenderTarget& target) const
{
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        pUnit->render(target);
    }

    renderSelectionMarkers(target);

    if (m_bDebugInfo)
    {
        renderDebugInfo(target);
    }
}
void World::renderDebugInfo(sf::RenderTarget& target) const
{
    sf::Text text(m_debugFont);
    text.setCharacterSize(12);

    // draw per-unit debug information
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        renderUnitDebug(target, *pUnit, text);
    }

    // count enemy units
    const auto enemyCount = std::count_if(
        m_vUnits.begin(),
        m_vUnits.end(),
        [](const std::unique_ptr<Unit>& pUnit)
        {
            return pUnit->getFaction() == UnitFaction::Enemy;
        }
    );

    // draw global unit count info
    text.setString(
        "Number units: " +
        std::to_string(static_cast<int>(m_vUnits.size())) +
        " (enemies: " +
        std::to_string(enemyCount) +
        ")"
    );

    text.setPosition(sf::Vector2f{ 16.0f, 18.0f });
    text.setFillColor(sf::Color::White);
    target.draw(text);
}


void World::renderUnitDebug(sf::RenderTarget& target, const Unit& unit, sf::Text& text) const
{
    // hitpoints
    text.setString(std::to_string(static_cast<int>(unit.getHitPoints())));
    text.setPosition(unit.getPosition() + sf::Vector2f{ 16.0f, -18.0f });
    text.setFillColor(sf::Color::Green);
    target.draw(text);

    // speed
    const float speed = unit.getCurrentSpeed();
    text.setString(std::to_string(static_cast<int>(speed)));
    text.setPosition(unit.getPosition() + sf::Vector2f{ -10.0f, -30.0f });
    text.setFillColor(sf::Color::White);
    target.draw(text);

    // range
    const float attackRange = unit.getAttackRange();
    sf::CircleShape rangeCircle(attackRange);
    rangeCircle.setOrigin({ attackRange, attackRange });
    rangeCircle.setPosition(unit.getPosition());
    rangeCircle.setFillColor(sf::Color::Transparent);
    rangeCircle.setOutlineColor(sf::Color::Blue);
    rangeCircle.setOutlineThickness(1.5f);
    target.draw(rangeCircle);
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

void World::setMouseWorldPosition(const sf::Vector2f& worldPosition)
{
    m_mouseWorldPosition = worldPosition;

    const Unit* pNewHoveredUnit = findUnitAt(worldPosition);
    if (pNewHoveredUnit != m_pHoveredUnit)
    {
        m_pHoveredUnit = pNewHoveredUnit;
        m_markerPulseTime = 0.0f;
    }
}

bool World::hasSelectedPlayerUnits() const
{
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (pUnit->isSelected() && pUnit->getFaction() == UnitFaction::Player)
        {
            return true;
        }
    }

    return false;
}

void World::renderSelectionMarkers(sf::RenderTarget& target) const
{
    const bool bHasSelectedPlayerUnits = hasSelectedPlayerUnits();

    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        if (pUnit->isSelected() && pUnit->getFaction() == UnitFaction::Player)
        {
            renderCornerMarker(target, *pUnit, sf::Color::White, 1.0f);
        }
    }

    if (m_pHoveredUnit == nullptr)
    {
        return;
    }

    const float minPulseScale = 0.70f;
    const float maxPulseScale = 1.00f;
    const float pulseSpeed = 9.0f;

    const float pulseT = 0.5f + 0.5f * std::cos(m_markerPulseTime * pulseSpeed);
    const float pulse = minPulseScale + (maxPulseScale - minPulseScale) * pulseT;

    if (m_pHoveredUnit->getFaction() == UnitFaction::Player)
    {
        if (!m_pHoveredUnit->isSelected())
        {
            renderCornerMarker(target, *m_pHoveredUnit, sf::Color::White, pulse);
        }
        return;
    }

    if (bHasSelectedPlayerUnits && m_pHoveredUnit->getFaction() == UnitFaction::Enemy)
    {
        renderCornerMarker(target, *m_pHoveredUnit, sf::Color::Red, pulse);
        renderAttackMarker(target, *m_pHoveredUnit);
    }
}

void World::renderCornerMarker(sf::RenderTarget& target, const Unit& unit, sf::Color color, float scale) const
{
    const float baseSize = unit.getSelectionRadius() * 2.0f;
    const float size = baseSize * scale;
    const float halfSize = size * 0.5f;
    const float cornerLength = size * 0.22f;
    const sf::Vector2f center = unit.getPosition();

    const sf::Vector2f topLeft{ center.x - halfSize, center.y - halfSize };
    const sf::Vector2f topRight{ center.x + halfSize, center.y - halfSize };
    const sf::Vector2f bottomLeft{ center.x - halfSize, center.y + halfSize };
    const sf::Vector2f bottomRight{ center.x + halfSize, center.y + halfSize };

    const sf::Vertex lines[] =
    {
        sf::Vertex{ topLeft, color },
        sf::Vertex{ topLeft + sf::Vector2f{ cornerLength, 0.0f }, color },
        sf::Vertex{ topLeft, color },
        sf::Vertex{ topLeft + sf::Vector2f{ 0.0f, cornerLength }, color },

        sf::Vertex{ topRight, color },
        sf::Vertex{ topRight + sf::Vector2f{ -cornerLength, 0.0f }, color },
        sf::Vertex{ topRight, color },
        sf::Vertex{ topRight + sf::Vector2f{ 0.0f, cornerLength }, color },

        sf::Vertex{ bottomLeft, color },
        sf::Vertex{ bottomLeft + sf::Vector2f{ cornerLength, 0.0f }, color },
        sf::Vertex{ bottomLeft, color },
        sf::Vertex{ bottomLeft + sf::Vector2f{ 0.0f, -cornerLength }, color },

        sf::Vertex{ bottomRight, color },
        sf::Vertex{ bottomRight + sf::Vector2f{ -cornerLength, 0.0f }, color },
        sf::Vertex{ bottomRight, color },
        sf::Vertex{ bottomRight + sf::Vector2f{ 0.0f, -cornerLength }, color }
    };

    target.draw(lines, 16, sf::PrimitiveType::Lines);
}

void World::renderAttackMarker(sf::RenderTarget& target, const Unit& unit) const
{
    const sf::Vector2f center = unit.getPosition();
    const float size = unit.getSelectionRadius() * 0.7f;
    const sf::Color color{ 120, 0, 0 };

    const sf::Vertex lines[] =
    {
        sf::Vertex{ center + sf::Vector2f{ -size, -size }, color },
        sf::Vertex{ center + sf::Vector2f{ size, size }, color },
        sf::Vertex{ center + sf::Vector2f{ size, -size }, color },
        sf::Vertex{ center + sf::Vector2f{ -size, size }, color }
    };

    target.draw(lines, 4, sf::PrimitiveType::Lines);
}

void World::toggleDebugMode()
{
    m_bDebugInfo = !m_bDebugInfo;
}