#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <SFML/Graphics.hpp>

#include "Command.hpp"
#include "TerrainType.hpp"

enum class UnitFaction
{
    Player,
    Enemy
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
    Unit(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats = {});
    virtual ~Unit() = default;

    void update(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits, float terrainMovementFactor);
    virtual void render(sf::RenderTarget& target) const;

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] float getFacingAngleDegrees() const;
    [[nodiscard]] float getRadius() const;
    [[nodiscard]] UnitFaction getFaction() const;
    [[nodiscard]] float getHitPoints() const;
    [[nodiscard]] float getCurrentSpeed() const;
    [[nodiscard]] float getAttackRange() const;
    [[nodiscard]] float getSelectionRadius() const;
    [[nodiscard]] bool isAlive() const;
    [[nodiscard]] bool isSelected() const;
    [[nodiscard]] bool contains(const sf::Vector2f& worldPosition) const;
    [[nodiscard]] virtual float getTerrainMovementFactor(TerrainType terrainType) const;

    virtual void setFacingAngleDegrees(float facingAngleDegrees);
    void setSelected(bool bSelected);    

    void applyDamage(float damage);

    void issueMoveCommand(const sf::Vector2f& targetPosition);
    void issueAttackCommand(Unit* pTargetUnit);
    void clearCommand();
    void clearAttackCommandIfTarget(const Unit* pTargetUnit);

    [[nodiscard]] bool hasActiveCommand() const;

protected:
    virtual void renderBody(sf::RenderTarget& target) const = 0;
    virtual void updateFacingDirection(const sf::Vector2f& direction, float deltaTime);

    void updateHitEffect(float deltaTime);
    void updateAutoAttack(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits);
    void updateMoveCommand(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits, MoveCommand& command, float terrainMovementFactor);
    void updateAttackCommand(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits, AttackCommand& command, float terrainMovementFactor);

    Unit* findEnemyInRange(const std::vector<std::unique_ptr<Unit>>& vUnits) const;
    void attack(Unit& target);
    virtual void updateWeaponDirectionTo(const sf::Vector2f& targetPosition, float deltaTime);
    virtual void resetWeaponDirectionToBody(float deltaTime);
    virtual bool canAttackTarget(const Unit& target) const;
    virtual float calculateMovementSpeedFactor(const sf::Vector2f& direction, float distanceToTarget) const;
    virtual sf::Vector2f calculateFacingDirectionForMovement(const sf::Vector2f& direction, float distanceToTarget) const;

    // --- Transform / Spatial ---
    sf::Vector2f m_position;                  // world position of the unit
    float m_radius;                           // collision and selection radius
    float m_facingAngleDegrees{ 0.0f };       // current facing direction in degrees

    // --- Movement ---
    float m_moveSpeed{ 120.0f };              // base movement speed
    float m_currentSpeed{ 0.0f };             // current movement speed (can be reduced)

    // --- Ownership / State ---
    UnitFaction m_faction{ UnitFaction::Player }; // player or enemy affiliation
    bool m_bSelected{ false };                // selection state

    // --- Combat ---
    UnitStats m_stats{};                      // combat parameters (hp, damage, range, etc.)
    float m_hitPoints{ 200.0f };              // current hit points
    float m_timeSinceLastAttack{ 0.0f };      // time since last attack in seconds

    // --- Effects ---
    float m_hitEffectDuration{ 0.12f };       // total duration of hit flash effect
    float m_hitEffectTimeRemaining{ 0.0f };   // remaining time of hit effect

    // --- Command System ---
    std::optional<Command> m_activeCommand;   // current active command (move, attack, etc.)

    // --- Rendering / UI ---
    float m_selectionFactor{ 1.8f };          // scale factor for selection visualization
};