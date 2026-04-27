#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Command.hpp"

enum class UnitFaction
{
    Player,
    Enemy
};

enum class UnitType
{
    Soldier,
    Tank,
    Aircraft
};

struct UnitStats
{
    float maxHitPoints{ 200.0f };
    float attackDamagePerSecond{ 40.0f };
    float attackRange{ 60.0f };
    float attackInterval{ 0.5f };
};

class Unit
{
public:
    Unit(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, UnitType type, const UnitStats& stats = {});

    void update(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits);
    void render(sf::RenderTarget& target) const;

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] float getRadius() const;
    [[nodiscard]] UnitFaction getFaction() const;
    [[nodiscard]] float getHitPoints() const;
    [[nodiscard]] float getAttackRange() const;
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
    void renderSoldier(sf::RenderTarget& target) const;
    void renderTank(sf::RenderTarget& target) const;
    void renderAircraft(sf::RenderTarget& target) const;
    void updateFacingDirection(const sf::Vector2f& direction);

    sf::Vector2f m_position;
    float m_radius;
    float m_moveSpeed{ 120.0f };
    UnitType m_type{ UnitType::Soldier };
    UnitFaction m_faction{ UnitFaction::Player };
    float m_facingAngleDegrees{ 0.0f };

    UnitStats m_stats{};
    float m_hitPoints{ 200.0f };
    float m_timeSinceLastAttack{ 0.0f };    // time since last attack in seconds

    float m_hitEffectDuration{ 0.12f };
    float m_hitEffectTimeRemaining{ 0.0f };

    bool m_bSelected{ false };

    std::optional<Command> m_activeCommand;
    //float m_selectionRadius{ 18.0f };
};