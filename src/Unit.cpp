#include "Unit.hpp"

Unit::Unit(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, UnitType type, const UnitStats& stats)
    : m_position(position)
    , m_radius(radius)
    , m_moveSpeed(moveSpeed)
    , m_faction(faction)
    , m_type(type)
    , m_stats(stats)
    , m_hitPoints(stats.maxHitPoints)
{
}

void Unit::update(float deltaTime, const std::vector<std::unique_ptr<Unit>>& vUnits)
{
    if (m_hitEffectTimeRemaining > 0.0f)
    {
        m_hitEffectTimeRemaining -= deltaTime;

        if (m_hitEffectTimeRemaining < 0.0f)
        {
            m_hitEffectTimeRemaining = 0.0f;
        }
    }

    m_timeSinceLastAttack += deltaTime;

    // auto attack target in range, if no active command
    if (!m_activeCommand.has_value())
    {
        const float attackRangeSquared = m_stats.attackRange * m_stats.attackRange;

        for (const std::unique_ptr<Unit>& pUnit : vUnits)
        {
            if (pUnit.get() == this)
            {
                continue;
            }

            if (!pUnit->isAlive())
            {
                continue;
            }

            if (pUnit->getFaction() == m_faction)
            {
                continue;
            }

            const sf::Vector2f toTarget = pUnit->getPosition() - m_position;
            const float distanceSquared = (toTarget.x * toTarget.x) + (toTarget.y * toTarget.y);

            if (distanceSquared <= attackRangeSquared)
            {
                if (m_timeSinceLastAttack >= m_stats.attackInterval)
                {
                    const float damage = m_stats.attackDamagePerSecond * m_stats.attackInterval;
                    pUnit->applyDamage(damage);

                    m_timeSinceLastAttack = 0.0f;
                }

                break;
            }
        }

        return;
    }

    // move command
    if (MoveCommand* pMoveCommand = std::get_if<MoveCommand>(&m_activeCommand.value()))
    {
        // attack unit(s) in range (but continue moving)
        const float attackRangeSquared = m_stats.attackRange * m_stats.attackRange;

        for (const std::unique_ptr<Unit>& pUnit : vUnits)
        {
            if (pUnit.get() == this)
            {
                continue;
            }

            if (!pUnit->isAlive())
            {
                continue;
            }

            if (pUnit->getFaction() == m_faction)
            {
                continue;
            }

            const sf::Vector2f toEnemy = pUnit->getPosition() - m_position;
            const float enemyDistanceSquared = (toEnemy.x * toEnemy.x) + (toEnemy.y * toEnemy.y);

            if (enemyDistanceSquared <= attackRangeSquared)
            {
                if (m_timeSinceLastAttack >= m_stats.attackInterval)
                {
                    const float damage = m_stats.attackDamagePerSecond * m_stats.attackInterval;
                    pUnit->applyDamage(damage);

                    m_timeSinceLastAttack = 0.0f;
                }

                break;
            }
        }

        const sf::Vector2f toTarget = pMoveCommand->targetPosition - m_position;
        const float distanceSquared = (toTarget.x * toTarget.x) + (toTarget.y * toTarget.y);

        if (distanceSquared <= 0.0001f)
        {
            m_position = pMoveCommand->targetPosition;
            m_activeCommand.reset();
            return;
        }

        const float distance = std::sqrt(distanceSquared);
        const float maxStep = m_moveSpeed * deltaTime;

        if (distance <= maxStep)
        {
            m_position = pMoveCommand->targetPosition;
            m_activeCommand.reset();
            return;
        }

        const sf::Vector2f direction = toTarget / distance;
        m_position += direction * maxStep;
        return;
    }

    // attack command
    if (AttackCommand* pAttackCommand = std::get_if<AttackCommand>(&m_activeCommand.value()))
    {
        if (pAttackCommand->m_pTargetUnit == nullptr)
        {
            m_activeCommand.reset();
            return;
        }

        if (!pAttackCommand->m_pTargetUnit->isAlive())
        {
            m_activeCommand.reset();
            return;
        }

        const sf::Vector2f targetPosition = pAttackCommand->m_pTargetUnit->getPosition();
        const sf::Vector2f toTarget = targetPosition - m_position;
        const float distanceSquared = (toTarget.x * toTarget.x) + (toTarget.y * toTarget.y);

        const float attackRangeSquared = m_stats.attackRange * m_stats.attackRange;

        if (distanceSquared <= attackRangeSquared)
        {
            if (m_timeSinceLastAttack >= m_stats.attackInterval)
            {
                const float damage = m_stats.attackDamagePerSecond * m_stats.attackInterval;
                pAttackCommand->m_pTargetUnit->applyDamage(damage);

                m_timeSinceLastAttack = 0.0f;

                if (!pAttackCommand->m_pTargetUnit->isAlive())
                {
                    m_activeCommand.reset();
                }
            }

            return;
        }

        const float distance = std::sqrt(distanceSquared);
        const float maxStep = m_moveSpeed * deltaTime;

        if (distance <= maxStep)
        {
            m_position = targetPosition;
            return;
        }

        const sf::Vector2f direction = toTarget / distance;
        m_position += direction * maxStep;
        return;
    }
}

void Unit::render(sf::RenderTarget& target) const
{
    if (m_type == UnitType::Soldier)
    {
        renderSoldier(target);
    }
    else if (m_type == UnitType::Tank)
    {
        renderTank(target);
    }
    else if (m_type == UnitType::Aircraft)
    {
        renderAircraft(target);
    }


    if (m_hitEffectTimeRemaining > 0.0f)
    {
        const float normalizedTime = m_hitEffectTimeRemaining / m_hitEffectDuration;

        sf::CircleShape hitEffect(m_radius + 5.0f);
        hitEffect.setOrigin({ m_radius + 5.0f, m_radius + 5.0f });
        hitEffect.setPosition(m_position);
        hitEffect.setFillColor(sf::Color::Transparent);
        hitEffect.setOutlineThickness(2.0f);
        hitEffect.setOutlineColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(180.0f * normalizedTime)));

        target.draw(hitEffect);
    }

    if (m_bSelected && m_activeCommand.has_value())
    {
        if (const MoveCommand* pMoveCommand = std::get_if<MoveCommand>(&m_activeCommand.value()))
        {
            sf::Vertex line[] =
            {
                sf::Vertex{ m_position, sf::Color::Red },
                sf::Vertex{ pMoveCommand->targetPosition, sf::Color::Red }
            };

            sf::CircleShape targetMarker(4.0f);
            targetMarker.setFillColor(sf::Color::Red);
            targetMarker.setOrigin({ 4.0f, 4.0f });
            targetMarker.setPosition(pMoveCommand->targetPosition);

            target.draw(line, 2, sf::PrimitiveType::Lines);
            target.draw(targetMarker);
        }
        else if (const AttackCommand* pAttackCommand = std::get_if<AttackCommand>(&m_activeCommand.value()))
        {
            if (pAttackCommand->m_pTargetUnit != nullptr)
            {
                sf::Vertex line[] =
                {
                    sf::Vertex{ m_position, sf::Color::Yellow },
                    sf::Vertex{ pAttackCommand->m_pTargetUnit->getPosition(), sf::Color::Yellow }
                };

                target.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }
    }
}


void Unit::renderSoldier(sf::RenderTarget& target) const
{
    sf::CircleShape shape(m_radius);
    shape.setOrigin({ m_radius, m_radius });
    shape.setPosition(m_position);
    shape.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

    if (m_bSelected)
    {
        shape.setOutlineThickness(2.0f);
        shape.setOutlineColor(sf::Color::Yellow);
    }

    target.draw(shape);
}

void Unit::renderTank(sf::RenderTarget& target) const
{
    const float tankLength = m_radius * 2.4f;
    const float tankHeight = m_radius * 1.6f;

    sf::RectangleShape body({ tankLength, tankHeight });
    body.setOrigin({ tankLength * 0.5f, tankHeight * 0.5f });
    body.setPosition(m_position);
    body.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

    if (m_bSelected)
    {
        body.setOutlineThickness(2.0f);
        body.setOutlineColor(sf::Color::Yellow);
    }

    target.draw(body);

    // barrel settings
    const float barrelLength = tankLength * 0.75f;
    const float barrelThickness = m_radius * 0.3f;

    sf::Color barrelColor;
    if (m_faction == UnitFaction::Player)
    {
        barrelColor = sf::Color(0, 120, 0); // dark green
    }
    else
    {
        barrelColor = sf::Color(120, 0, 0); // dark red
    }

    sf::RectangleShape barrel({ barrelLength, barrelThickness });
    barrel.setOrigin({ 0.0f, barrelThickness * 0.5f });
    barrel.setPosition(m_position);
    barrel.setFillColor(barrelColor);

    target.draw(barrel);

    // pivot circle (turret base)
    const float pivotRadius = m_radius * 0.45f;

    sf::CircleShape pivot(pivotRadius);
    pivot.setOrigin({ pivotRadius, pivotRadius });
    pivot.setPosition(m_position);
    pivot.setFillColor(sf::Color(100, 100, 100));

    target.draw(pivot);
}

void Unit::renderAircraft(sf::RenderTarget& target) const
{
    sf::ConvexShape shape;
    shape.setPointCount(3);
    shape.setPoint(0, { m_radius * 1.4f, 0.0f });
    shape.setPoint(1, { 0.0f, m_radius * 2.4f });
    shape.setPoint(2, { m_radius * 2.8f, m_radius * 2.4f });
    shape.setOrigin({ m_radius * 1.4f, m_radius * 1.2f });
    shape.setPosition(m_position);
    shape.setFillColor(m_faction == UnitFaction::Player ? sf::Color::Green : sf::Color::Red);

    if (m_bSelected)
    {
        shape.setOutlineThickness(2.0f);
        shape.setOutlineColor(sf::Color::Yellow);
    }

    target.draw(shape);
}

const sf::Vector2f& Unit::getPosition() const
{
    return m_position;
}

float Unit::getRadius() const
{
    return m_radius;
}

UnitFaction Unit::getFaction() const
{
    return m_faction;
}

float Unit::getHitPoints() const
{
    return m_hitPoints;
}

float Unit::getAttackRange() const
{
    return m_stats.attackRange;
}

bool Unit::isAlive() const
{
    return m_hitPoints > 0.0f;
}

void Unit::setSelected(bool bSelected)
{
    m_bSelected = bSelected;
}

bool Unit::isSelected() const
{
    return m_bSelected;
}

bool Unit::contains(const sf::Vector2f& worldPosition) const
{
    const sf::Vector2f delta = worldPosition - m_position;
    const float distanceSquared = (delta.x * delta.x) + (delta.y * delta.y);
    //const float radiusSquared = m_selectionRadius * m_selectionRadius;
    const float radiusSquared = m_radius * m_radius;

    return distanceSquared <= radiusSquared;
}

void Unit::applyDamage(float damage)
{
    if (damage <= 0.0f)
    {
        return;
    }

    if (!isAlive())
    {
        return;
    }

    m_hitEffectTimeRemaining = m_hitEffectDuration;

    m_hitPoints -= damage;

    if (m_hitPoints < 0.0f)
    {
        m_hitPoints = 0.0f;
    }
}

void Unit::issueMoveCommand(const sf::Vector2f& targetPosition)
{
    m_activeCommand = MoveCommand{ targetPosition };
}

void Unit::issueAttackCommand(Unit* pTargetUnit)
{
    if (pTargetUnit == nullptr)
    {
        return;
    }

    m_activeCommand = AttackCommand{ pTargetUnit };
}

void Unit::clearCommand()
{
    m_activeCommand.reset();
}

void Unit::clearAttackCommandIfTarget(const Unit* pTargetUnit)
{
    if (pTargetUnit == nullptr)
    {
        return;
    }

    if (!m_activeCommand.has_value())
    {
        return;
    }

    AttackCommand* pAttackCommand = std::get_if<AttackCommand>(&m_activeCommand.value());
    if (pAttackCommand == nullptr)
    {
        return;
    }

    if (pAttackCommand->m_pTargetUnit == pTargetUnit)
    {
        m_activeCommand.reset();
    }
}

bool Unit::hasActiveCommand() const
{
    return m_activeCommand.has_value();
}