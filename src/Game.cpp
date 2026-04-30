#include <algorithm>

#include "Game.hpp"

Game::Game()
    : m_window(sf::VideoMode({ 1280u, 1024u }), "Compile and Conquer")
    , m_worldView(m_window.getDefaultView())
{
    m_window.setFramerateLimit(60);
}

void Game::run()
{
    sf::Clock clock;

    while (m_window.isOpen())
    {
        const float deltaTime = clock.restart().asSeconds();

        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        // close game
        if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
            m_window.close();
        }
        // stop units
        else if (const auto* pKeyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (pKeyPressed->code == sf::Keyboard::Key::S)
            {
                m_world.stopSelectedUnits();
            }
            if (pKeyPressed->code == sf::Keyboard::Key::D)
            {
                m_world.toggleDebugMode();
            }
        }
        // unit selection (including box)
        else if (const auto* pMouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (pMouseButtonPressed->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2i mousePixelPosition{ pMouseButtonPressed->position.x, pMouseButtonPressed->position.y };
                m_selectionStart = m_window.mapPixelToCoords(mousePixelPosition, m_worldView);
                m_selectionCurrent = m_selectionStart;
                m_bIsSelecting = true;
                m_bHasDragged = false;
            }
        }
        else if (const auto* pMouseMoved = event->getIf<sf::Event::MouseMoved>())
        {
            const sf::Vector2i mousePixelPosition{ pMouseMoved->position.x, pMouseMoved->position.y };
            const sf::Vector2f worldPosition = m_window.mapPixelToCoords(mousePixelPosition, m_worldView);
            m_world.setMouseWorldPosition(worldPosition);

            if (m_bIsSelecting)
            {
                m_selectionCurrent = m_window.mapPixelToCoords(mousePixelPosition, m_worldView);

                const float dragThreshold = 5.0f;

                if (std::abs(m_selectionStart.x - m_selectionCurrent.x) > dragThreshold ||
                    std::abs(m_selectionStart.y - m_selectionCurrent.y) > dragThreshold)
                {
                    m_bHasDragged = true;
                }
            }
        }
        else if (const auto* pMouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (pMouseButtonReleased->button == sf::Mouse::Button::Left && m_bIsSelecting)
            {
                m_bIsSelecting = false;

                const sf::Vector2f min{
                    std::min(m_selectionStart.x, m_selectionCurrent.x),
                    std::min(m_selectionStart.y, m_selectionCurrent.y)
                };

                const sf::Vector2f max{
                    std::max(m_selectionStart.x, m_selectionCurrent.x),
                    std::max(m_selectionStart.y, m_selectionCurrent.y)
                };

                const sf::FloatRect selectionRect(min, max - min);


                handleSelection(selectionRect, !m_bHasDragged);
            }
            else if (pMouseButtonReleased->button == sf::Mouse::Button::Right)
            {
                const sf::Vector2i mousePixelPosition{ pMouseButtonReleased->position.x, pMouseButtonReleased->position.y };
                const sf::Vector2f worldPosition = m_window.mapPixelToCoords(mousePixelPosition, m_worldView);
                m_world.handleRightClick(worldPosition);
            }
        }
    }
}

void Game::update(float deltaTime)
{
    updateCamera(deltaTime);
    m_world.update(deltaTime);
}

void Game::updateCamera(float deltaTime)
{
    const float cameraSpeed = 500.0f;
    sf::Vector2f movement{ 0.0f, 0.0f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        movement.x -= cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        movement.x += cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        movement.y -= cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        movement.y += cameraSpeed * deltaTime;
    }

    m_worldView.move(movement);
    clampWorldViewToBounds();
}

void Game::clampWorldViewToBounds()
{
    const sf::Vector2f viewSize = m_worldView.getSize();
    const sf::Vector2f halfViewSize{ viewSize.x * 0.5f, viewSize.y * 0.5f };

    const float minCenterX = m_worldBounds.position.x + halfViewSize.x;
    const float maxCenterX = m_worldBounds.position.x + m_worldBounds.size.x - halfViewSize.x;
    const float minCenterY = m_worldBounds.position.y + halfViewSize.y;
    const float maxCenterY = m_worldBounds.position.y + m_worldBounds.size.y - halfViewSize.y;

    sf::Vector2f center = m_worldView.getCenter();

    center.x = std::clamp(center.x, minCenterX, maxCenterX);
    center.y = std::clamp(center.y, minCenterY, maxCenterY);

    m_worldView.setCenter(center);
}

void Game::renderWorldBounds()
{
    const float thickness = 6.0f;

    sf::RectangleShape boundsRect;
    boundsRect.setPosition(m_worldBounds.position + sf::Vector2f{ thickness * 0.5f, thickness * 0.5f });
    boundsRect.setSize(m_worldBounds.size - sf::Vector2f{ thickness, thickness });
    boundsRect.setFillColor(sf::Color::Transparent);
    boundsRect.setOutlineColor(sf::Color::Green);
    boundsRect.setOutlineThickness(thickness);

    m_window.draw(boundsRect);
}

void Game::render()
{
    m_window.clear(sf::Color(30, 30, 30));

    m_window.setView(m_worldView);
    m_world.render(m_window);
    
    m_window.setView(m_worldView);
    renderWorldBounds();

    if (m_bIsSelecting)
    {
        const sf::Vector2f min{
            std::min(m_selectionStart.x, m_selectionCurrent.x),
            std::min(m_selectionStart.y, m_selectionCurrent.y)
        };

        const sf::Vector2f size{
            std::abs(m_selectionStart.x - m_selectionCurrent.x),
            std::abs(m_selectionStart.y - m_selectionCurrent.y)
        };

        sf::RectangleShape rect;
        rect.setPosition(min);
        rect.setSize(size);
        rect.setFillColor(sf::Color(0, 0, 255, 50));
        rect.setOutlineColor(sf::Color::Blue);
        rect.setOutlineThickness(1.0f);

        m_window.draw(rect);
    }

    m_window.setView(m_window.getDefaultView());
    m_world.renderDebugInfoBox(m_window);

    m_window.display();
}

void Game::handleSelection(const sf::FloatRect& selectionRect, bool bIsClick)
{
    const bool bCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

    if (bIsClick)
    {
        if (bCtrlPressed)
        {
            m_world.toggleUnitAt(m_selectionStart);
        }
        else
        {
            m_world.selectUnitAt(m_selectionStart);
        }
    }
    else
    {
        if (bCtrlPressed)
        {
            m_world.toggleUnitsInRect(selectionRect);
        }
        else
        {
            m_world.selectUnitsInRect(selectionRect);
        }
    }
}