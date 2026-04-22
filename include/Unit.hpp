#pragma once

#include <optional>
#include <SFML/Graphics.hpp>
#include "Command.hpp"

enum class UnitFaction
{
    Player,
    Enemy
};

class Unit
{
public:
    Unit(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction);

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] UnitFaction getFaction() const;

    void setSelected(bool bSelected);
    [[nodiscard]] bool isSelected() const;
    [[nodiscard]] bool contains(const sf::Vector2f& worldPosition) const;

    void issueMoveCommand(const sf::Vector2f& targetPosition);
    void issueAttackCommand(Unit* pTargetUnit);
    void clearCommand();

    [[nodiscard]] bool hasActiveCommand() const;

private:
    sf::Vector2f m_position;
    float m_radius;
    float m_moveSpeed{ 120.0f };
    UnitFaction m_faction{ UnitFaction::Player };

    bool m_bSelected{ false };

    std::optional<Command> m_activeCommand;
    //float m_selectionRadius{ 18.0f };
};