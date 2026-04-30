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
    // --- Core ---
    sf::RenderWindow m_window;                // main render window
    World m_world;                            // game world containing all units

    // --- Camera / View ---
    sf::View m_worldView;                     // camera view used for world rendering and navigation

    // --- Selection Input State ---
    bool m_bIsSelecting{ false };             // true while left mouse button is held down
    bool m_bHasDragged{ false };              // true if mouse moved enough to count as drag selection

    sf::Vector2f m_selectionStart;            // world position where selection started (mouse down)
    sf::Vector2f m_selectionCurrent;          // current world position of mouse during selection
};
