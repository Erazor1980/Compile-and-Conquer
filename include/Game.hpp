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
    void clampWorldViewToBounds();
    void renderWorldBounds();

private:
    // --- Core ---
    sf::RenderWindow m_window;                // main render window
    World m_world;                            // game world containing all units

    // --- Camera / View ---
    sf::View m_worldView;                     // camera view used for world rendering and navigation
    sf::FloatRect m_worldBounds{ sf::Vector2f{ 0.0f, 0.0f }, sf::Vector2f{ 3000.0f, 2000.0f } }; // playable world area
    float m_zoomFactor{ 1.0f };               // current zoom level (1.0 = default)
    float m_minZoom{ 0.5f };                  // minimum zoom factor (closer view)
    float m_maxZoom{ 2.5f };                  // maximum zoom factor (wider view)

    // --- Camera Input State ---
    bool m_bIsRightMousePressed{ false };     // true while right mouse button is held down
    bool m_bHasRightMouseDragged{ false };    // true if right mouse movement becomes camera panning
    bool m_bIsMouseGrabbed{ false };          // true if mouse cursor is confined to the window

    sf::Vector2i m_rightMouseStartPixel;      // screen position where right mouse press started
    sf::Vector2i m_rightMouseCurrentPixel;    // current screen position during right mouse press

    // --- Selection Input State ---
    bool m_bIsSelecting{ false };             // true while left mouse button is held down
    bool m_bHasDragged{ false };              // true if mouse moved enough to count as drag selection

    sf::Vector2f m_selectionStart;            // world position where selection started
    sf::Vector2f m_selectionCurrent;          // current world position of mouse during selection
};
