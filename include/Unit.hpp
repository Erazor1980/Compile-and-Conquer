#pragma once

#include <SFML/Graphics.hpp>

class Unit
{
public:
    Unit(sf::Vector2f position, float radius, float moveSpeed);

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    [[nodiscard]] const sf::Vector2f& getPosition() const;

    void setSelected(bool bSelected);
    [[nodiscard]] bool isSelected() const;
    [[nodiscard]] bool contains(const sf::Vector2f& worldPosition) const;

    void setMoveTarget(const sf::Vector2f& targetPosition);
    void stopMovement();
    [[nodiscard]] bool hasMoveTarget() const;

private:
    sf::Vector2f m_position;
    sf::Vector2f m_targetPosition;
    float m_radius;
    float m_moveSpeed{ 120.0f };

    bool m_bSelected{ false };
    bool m_bHasMoveTarget{ false };
    //float m_selectionRadius{ 18.0f };
};