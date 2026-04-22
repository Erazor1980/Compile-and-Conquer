#pragma once

#include <variant>
#include <SFML/System/Vector2.hpp>

class Unit;

struct MoveCommand
{
    sf::Vector2f targetPosition;
};

struct AttackCommand
{
    Unit* m_pTargetUnit{ nullptr };
};

using Command = std::variant<MoveCommand, AttackCommand>;