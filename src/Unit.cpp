#include "Unit.hpp"

Unit::Unit(sf::Vector2f position, float radius, float moveSpeed, UnitFaction faction, const UnitStats& stats)
    : m_position(position)
    , m_radius(radius)
    , m_moveSpeed(moveSpeed)
    , m_faction(faction)
    , m_stats(stats)
    , m_hitPoints(stats.maxHitPoints)
{
}

void Unit::update(float deltaTime)
{
    if (m_hitEffectTimeRemaining > 0.0f)
    {
        m_hitEffectTimeRemaining -= deltaTime;

        if (m_hitEffectTimeRemaining < 0.0f)
        {
            m_hitEffectTimeRemaining = 0.0f;
        }
    }

    if (!m_activeCommand.has_value())
    {
        return;
    }

    m_timeSinceLastAttack += deltaTime;

    // move command
    if (MoveCommand* pMoveCommand = std::get_if<MoveCommand>(&m_activeCommand.value()))
    {
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
    sf::CircleShape shape(m_radius);
    if (m_faction == UnitFaction::Player)
    {
        shape.setFillColor(sf::Color::Green);
    }
    else
    {
        shape.setFillColor(sf::Color::Red);
    }

    shape.setOrigin({ m_radius, m_radius });
    shape.setPosition(m_position);

    if (m_bSelected)
    {
        shape.setOutlineThickness(2.0f);
        shape.setOutlineColor(sf::Color::Yellow);
    }
    else
    {
        shape.setOutlineThickness(0.0f);
    }

    target.draw(shape);

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