#pragma once

#include <SFML/Graphics.hpp>

class Unit
{
public:
    Unit(sf::Vector2f position, sf::Vector2f velocity, float radius);

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    [[nodiscard]] const sf::Vector2f& getPosition() const;

    void setSelected(bool bSelected);
    [[nodiscard]] bool isSelected() const;
    [[nodiscard]] bool contains(const sf::Vector2f& worldPosition) const;

private:
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    float m_radius;

    bool m_bSelected{ false };
    float m_selectionRadius{ 18.0f };
};