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
    virtual ~Unit() = default;

    void update(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits);
    virtual void render(sf::RenderTarget& target) const;

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] float getRadius() const;
    [[nodiscard]] UnitFaction getFaction() const;
    [[nodiscard]] float getHitPoints() const;
    [[nodiscard]] float getCurrentSpeed() const;
    [[nodiscard]] float getAttackRange() const;
    [[nodiscard]] float getSelectionRadius() const;
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

protected:
    virtual void renderBody(sf::RenderTarget& target) const;
    void renderSoldier(sf::RenderTarget& target) const;
    void renderAircraft(sf::RenderTarget& target) const;
    virtual void updateFacingDirection(const sf::Vector2f& direction, float deltaTime);

    void updateHitEffect(float deltaTime);
    void updateAutoAttack(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits);
    void updateMoveCommand(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits, MoveCommand& command);
    void updateAttackCommand(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits, AttackCommand& command);

    Unit* findEnemyInRange(const std::vector<std::unique_ptr<Unit>>& vUnits) const;
    void attack(Unit& target);
    virtual void updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime);
    virtual void resetWeaponDirectionToBody(float deltaTime);
    virtual bool canAttackTarget(const Unit& target) const;
    virtual float calculateMovementSpeedFactor(const sf::Vector2f& direction, float distanceToTarget) const;
    virtual sf::Vector2f calculateFacingDirectionForMovement(const sf::Vector2f& direction, float distanceToTarget) const;

    sf::Vector2f m_position;
    float m_radius;
    float m_moveSpeed{ 120.0f };
    float m_currentSpeed{ 0.0f };
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
    float m_selectionFactor{ 1.8f };
};