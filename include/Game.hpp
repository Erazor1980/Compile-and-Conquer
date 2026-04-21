#pragma once

#include <SFML/Graphics.hpp>
#include "World.hpp"

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    void handleSelection(const sf::FloatRect& selectionRect, bool bIsClick);

private:    
    sf::RenderWindow m_window;
    World m_world;

    bool m_bIsSelecting{ false };
    bool m_bHasDragged{ false };

    sf::Vector2f m_selectionStart;
    sf::Vector2f m_selectionCurrent;
};
