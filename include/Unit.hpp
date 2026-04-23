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
    [[nodiscard]] float getRadius() const;
    [[nodiscard]] UnitFaction getFaction() const;
    [[nodiscard]] float getHitPoints() const;
    [[nodiscard]] bool isAlive() const;

    void setSelected(bool bSelected);
    [[nodiscard]] bool isSelected() const;
    [[nodiscard]] bool contains(const sf::Vector2f& worldPosition) const;

    void applyDamage(float damage);

    void issueMoveCommand(const sf::Vector2f& targetPosition);
    void issueAttackCommand(Unit* pTargetUnit);
    void clearCommand();
    void clearAttackCommandIfTarget(const Unit* pTargetUnit);

    [[nodiscard]] bool hasActiveCommand() const;

private:
    sf::Vector2f m_position;
    float m_radius;
    float m_moveSpeed{ 120.0f };
    UnitFaction m_faction{ UnitFaction::Player };

    float m_hitPoints{ 100.0f };
    float m_attackDamagePerSecond{ 40.0f }; // dps
    float m_attackRange{ 60.0f };           // in pixels
    float m_attackInterval{ 0.5f };         // time between 2 attacks in seconds
    float m_timeSinceLastAttack{ 0.0f };    // time since last attack in seconds

    float m_hitEffectDuration{ 0.12f };
    float m_hitEffectTimeRemaining{ 0.0f };

    bool m_bSelected{ false };

    std::optional<Command> m_activeCommand;
    //float m_selectionRadius{ 18.0f };
};