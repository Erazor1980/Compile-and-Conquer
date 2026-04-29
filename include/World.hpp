#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics/Font.hpp>
#include "Unit.hpp"

class World
{
public:
    World();

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    void clearSelection();
    void selectUnitAt(const sf::Vector2f& worldPosition);
    void toggleUnitAt(const sf::Vector2f& worldPosition);
    void selectUnitsInRect(const sf::FloatRect& rect);
    void toggleUnitsInRect(const sf::FloatRect& rect);
    void handleRightClick(const sf::Vector2f& worldPosition);

    void stopSelectedUnits();
    void setMouseWorldPosition(const sf::Vector2f& worldPosition);
private:
    void createTestUnits();
    void addSoldier(const sf::Vector2f& position, UnitFaction faction);
    void addTank(const sf::Vector2f& position, UnitFaction faction);
    void addAircraft(const sf::Vector2f& position, UnitFaction faction);
    void moveSelectedUnitsTo(const sf::Vector2f& targetPosition);

    Unit* findUnitAt(const sf::Vector2f& worldPosition);
    const Unit* findUnitAt(const sf::Vector2f& worldPosition) const;

    Unit* findEnemyUnitAt(const sf::Vector2f& worldPosition);
    const Unit* findEnemyUnitAt(const sf::Vector2f& worldPosition) const;

    Unit* findPlayerUnitAt(const sf::Vector2f& worldPosition);
    const Unit* findPlayerUnitAt(const sf::Vector2f& worldPosition) const;

    // hover & selection
    enum class CommandPreviewType
    {
        Move,
        Attack
    };
    enum class HoverPreviewType
    {
        None,
        PlayerUnit,
        Move,
        Attack
    };
    void renderCommandPreviewMarker(sf::RenderTarget& target, const sf::Vector2f& position, sf::Color color, CommandPreviewType type) const;
    void renderSelectionMarkers(sf::RenderTarget& target) const;
    void renderCornerMarker(sf::RenderTarget& target, const Unit& unit, sf::Color color, float scale) const;
    bool hasSelectedPlayerUnits() const;
    void drawThickLine(sf::RenderTarget& target, const sf::Vector2f& from, const sf::Vector2f& to, sf::Color color, float thickness) const;
    HoverPreviewType getHoverPreviewType(const Unit* pHoveredUnit) const;

private:
    std::vector<std::unique_ptr<Unit>> m_vUnits;

    // hover & selection
    sf::Vector2f m_mouseWorldPosition{};
    const Unit* m_pHoveredUnit{ nullptr };
    float m_markerPulseTime{ 0.0f };
    HoverPreviewType m_hoverPreviewType{ HoverPreviewType::None };

    // debug stuff
public:
    void toggleDebugMode();
private:
    void renderDebugInfo(sf::RenderTarget& target) const;
    void renderUnitDebug(sf::RenderTarget& target, const Unit& unit, sf::Text& text) const;    
    bool m_bDebugInfo{ true };
    sf::Font m_debugFont;
};