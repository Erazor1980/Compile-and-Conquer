#pragma once

#include "World.hpp"
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

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
    void updateCamera(float deltaTime);

private:    
    sf::RenderWindow m_window;
    World m_world;
    sf::View m_worldView;

    bool m_bIsSelecting{ false };
    bool m_bHasDragged{ false };

    sf::Vector2f m_selectionStart;
    sf::Vector2f m_selectionCurrent;
};
