#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>

#include "World.hpp"
#include "Tank.hpp"
#include "Soldier.hpp"
#include "Aircraft.hpp"
#include "helper.hpp"

World::World()
{
    if (!m_debugFont.openFromFile("assets/fonts/arial.ttf"))
    {
        throw std::runtime_error("Failed to load debug font: assets/arial.ttf");
    }

    m_terrainMap.loadFromImage("assets/levels/test_map.png");

    createTestUnits();
}

void World::createTestUnits()
{
    // Player units
    addSoldier(sf::Vector2f{ 100.0f, 100.0f }, UnitFaction::Player);
    addSoldier(sf::Vector2f{ 500.0f, 700.0f }, UnitFaction::Player);
    addSoldier(sf::Vector2f{ 600.0f, 400.0f }, UnitFaction::Player);

    addTank(sf::Vector2f{ 200.0f, 180.0f }, UnitFaction::Player);
    addTank(sf::Vector2f{ 400.0f, 280.0f }, UnitFaction::Player);

    addAircraft(sf::Vector2f{ 320.0f, 260.0f }, UnitFaction::Player);

    // Enemy units
    addSoldier(sf::Vector2f{ 800.0f, 300.0f }, UnitFaction::Enemy);
    addSoldier(sf::Vector2f{ 800.0f, 740.0f }, UnitFaction::Enemy);
    addSoldier(sf::Vector2f{ 1000.0f, 460.0f }, UnitFaction::Enemy);

    addTank(sf::Vector2f{ 800.0f, 600.0f }, UnitFaction::Enemy);
    addTank(sf::Vector2f{ 1000.0f, 760.0f }, UnitFaction::Enemy);

    addAircraft(sf::Vector2f{ 840.0f, 360.0f }, UnitFaction::Enemy);
    addAircraft(sf::Vector2f{ 1000.0f, 260.0f }, UnitFaction::Enemy);
}

void World::addSoldier(const sf::Vector2f& position, UnitFaction faction, float facingAngleDegrees)
{
    const UnitStats soldierStats{
        100.0f,
        25.0f,
        80.0f,
        0.6f
    };

    const float soldierRadius = 8.0f;
    const float soldierMoveSpeed = 120.0f;

    auto pUnit = std::make_unique<Soldier>(position, soldierRadius, soldierMoveSpeed, faction, soldierStats);

    const float angle = (facingAngleDegrees < 0.0f) ? getRandomAngle() : facingAngleDegrees;
    pUnit->setFacingAngleDegrees(angle);

    m_vUnits.emplace_back(std::move(pUnit));
}

void World::addTank(const sf::Vector2f& position, UnitFaction faction, float facingAngleDegrees)
{
    const UnitStats tankStats{
        220.0f,
        55.0f,
        160.0f,
        0.9f
    };

    const float tankRadius = 16.0f;
    const float tankMoveSpeed = 80.0f;

    auto pUnit = std::make_unique<Tank>(position, tankRadius, tankMoveSpeed, faction, tankStats);

    const float angle = (facingAngleDegrees < 0.0f) ? getRandomAngle() : facingAngleDegrees;
    pUnit->setFacingAngleDegrees(angle);

    m_vUnits.emplace_back(std::move(pUnit));
}

void World::addAircraft(const sf::Vector2f& position, UnitFaction faction, float facingAngleDegrees)
{
    const UnitStats aircraftStats{
        140.0f,
        35.0f,
        120.0f,
        0.45f
    };

    const float aircraftRadius = 11.0f;
    const float aircraftMoveSpeed = 140.0f;

    auto pUnit = std::make_unique<Aircraft>(position, aircraftRadius, aircraftMoveSpeed, faction, aircraftStats);

    const float angle = (facingAngleDegrees < 0.0f) ? getRandomAngle() : facingAngleDegrees;
    pUnit->setFacingAngleDegrees(angle);

    m_vUnits.emplace_back(std::move(pUnit));
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
        const TerrainType terrainType = m_terrainMap.getTerrainAt(pUnit->getPosition());
        const float terrainMovementFactor = pUnit->getTerrainMovementFactor(terrainType);

        pUnit->update(deltaTime, m_vUnits, terrainMovementFactor);
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
    m_terrainMap.render(target);

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

sf::FloatRect World::getBounds() const
{
    return m_terrainMap.getBounds();
}


void World::renderDebugInfo(sf::RenderTarget& target) const
{
    // draw per-unit debug information
    for (const std::unique_ptr<Unit>& pUnit : m_vUnits)
    {
        renderUnitDebug(target, *pUnit);
    }
}

void World::renderDebugInfoBox(sf::RenderTarget& target, float zoomFactor, const sf::Vector2u& windowSize, bool bIsMouseGrabbed) const
{
    if (!m_bDebugInfo)
    {
        return;
    }

    const auto enemyCount = std::count_if(
        m_vUnits.begin(),
        m_vUnits.end(),
        [](const std::unique_ptr<Unit>& pUnit)
        {
            return pUnit->getFaction() == UnitFaction::Enemy;
        }
    );

    const std::vector<std::string> vLines{
        "Number units: " +
            std::to_string(static_cast<int>(m_vUnits.size())) +
            " (enemies: " +
            std::to_string(static_cast<int>(enemyCount)) +
            ")",
        "Zoom: " + std::to_string(zoomFactor),
        std::string("F1: Mouse Grab ") + (bIsMouseGrabbed ? "ON" : "OFF")
    };

    const float padding = 10.0f;
    const float lineHeight = 16.0f;
    const float boxWidth = 260.0f;
    const float boxHeight = padding * 2.0f + lineHeight * static_cast<float>(vLines.size());

    const float posX = static_cast<float>(windowSize.x) - boxWidth - 12.0f;
    const float posY = 12.0f;

    sf::RectangleShape box;
    box.setPosition(sf::Vector2f{ posX, posY });
    box.setSize(sf::Vector2f{ boxWidth, boxHeight });
    box.setFillColor(sf::Color(0, 0, 0, 120));
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(1.0f);
    target.draw(box);

    sf::Text text(m_debugFont);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::White);

    for (std::size_t i = 0; i < vLines.size(); ++i)
    {
        text.setString(vLines[i]);
        text.setPosition(sf::Vector2f{
            posX + padding,
            posY + padding + lineHeight * static_cast<float>(i)
            });

        target.draw(text);
    }
}

void World::renderUnitDebug(sf::RenderTarget& target, const Unit& unit) const
{
    sf::Text text(m_debugFont);
    text.setCharacterSize(12);

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
    const HoverPreviewType newHoverPreviewType = getHoverPreviewType(pNewHoveredUnit);

    if (pNewHoveredUnit != m_pHoveredUnit || newHoverPreviewType != m_hoverPreviewType)
    {
        m_pHoveredUnit = pNewHoveredUnit;
        m_hoverPreviewType = newHoverPreviewType;
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

World::HoverPreviewType World::getHoverPreviewType(const Unit* pHoveredUnit) const
{
    const bool bHasSelectedPlayerUnits = hasSelectedPlayerUnits();

    if (pHoveredUnit == nullptr)
    {
        if (bHasSelectedPlayerUnits)
        {
            return HoverPreviewType::Move;
        }

        return HoverPreviewType::None;
    }

    if (pHoveredUnit->getFaction() == UnitFaction::Enemy && bHasSelectedPlayerUnits)
    {
        return HoverPreviewType::Attack;
    }

    if (pHoveredUnit->getFaction() == UnitFaction::Player)
    {
        return HoverPreviewType::PlayerUnit;
    }

    return HoverPreviewType::None;
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
        if (bHasSelectedPlayerUnits)
        {
            renderCommandPreviewMarker(target, m_mouseWorldPosition, sf::Color{ 0, 200, 0 }, CommandPreviewType::Move);
        }
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
        renderCommandPreviewMarker(target, m_mouseWorldPosition, sf::Color{ 255, 80, 40 }, CommandPreviewType::Attack);
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

void World::renderCommandPreviewMarker(sf::RenderTarget& target, const sf::Vector2f& position, sf::Color color, CommandPreviewType type) const
{
    const float minScale = 0.80f;
    const float maxScale = 1.20f;
    const float pulseSpeed = 6.0f;

    const float pulseT = 0.5f + 0.5f * std::cos(m_markerPulseTime * pulseSpeed);
    const float scale = minScale + (maxScale - minScale) * pulseT;

    const float outerDistance = 20.0f * scale;
    const float innerDistance = 8.0f * scale;
    const float arrowHalfWidth = 5.0f * scale;

    const sf::Vertex lines[] =
    {
        sf::Vertex{ position + sf::Vector2f{ -outerDistance, -outerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance - arrowHalfWidth, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, -innerDistance - arrowHalfWidth }, color },

        sf::Vertex{ position + sf::Vector2f{ outerDistance, -outerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance + arrowHalfWidth, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, -innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, -innerDistance - arrowHalfWidth }, color },

        sf::Vertex{ position + sf::Vector2f{ -outerDistance, outerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance - arrowHalfWidth, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ -innerDistance, innerDistance + arrowHalfWidth }, color },

        sf::Vertex{ position + sf::Vector2f{ outerDistance, outerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance + arrowHalfWidth, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, innerDistance }, color },
        sf::Vertex{ position + sf::Vector2f{ innerDistance, innerDistance + arrowHalfWidth }, color }
    };


    if (type == CommandPreviewType::Move)
    {
        target.draw(lines, 24, sf::PrimitiveType::Lines);

        const float centerRadius = 3.5f * scale;

        sf::CircleShape centerDot;
        centerDot.setRadius(centerRadius);
        centerDot.setOrigin(sf::Vector2f{ centerRadius, centerRadius });
        centerDot.setPosition(position);
        centerDot.setFillColor(color);

        target.draw(centerDot);
    }
    else if (type == CommandPreviewType::Attack)
    {
        const float size = 5.0f * scale;

        const sf::Vertex cross[] =
        {
            sf::Vertex{ position + sf::Vector2f{ -size, -size }, color },
            sf::Vertex{ position + sf::Vector2f{ size, size }, color },

            sf::Vertex{ position + sf::Vector2f{ size, -size }, color },
            sf::Vertex{ position + sf::Vector2f{ -size, size }, color }
        };

        target.draw(cross, 4, sf::PrimitiveType::Lines);

        const float thickness = 2.0f;

        for (std::size_t i = 0; i < 24; i += 2)
        {
            drawThickLine(target, lines[i].position, lines[i + 1].position, color, thickness);
        }
    }
}

void World::drawThickLine(sf::RenderTarget& target, const sf::Vector2f& from, const sf::Vector2f& to, sf::Color color, float thickness) const
{
    const sf::Vector2f direction = to - from;
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length <= 0.0f)
    {
        return;
    }

    sf::RectangleShape line;
    line.setSize(sf::Vector2f{ length, thickness });
    line.setOrigin(sf::Vector2f{ 0.0f, thickness * 0.5f });
    line.setPosition(from);
    line.setRotation(sf::degrees(std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f));
    line.setFillColor(color);

    target.draw(line);
}

void World::toggleDebugMode()
{
    m_bDebugInfo = !m_bDebugInfo;
}