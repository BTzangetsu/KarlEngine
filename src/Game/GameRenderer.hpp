#pragma once
#include <SFML/Graphics.hpp>

namespace KE {

    class GameRenderer {
    protected:

        sf::RenderWindow* window;

    public:

        GameRenderer(sf::RenderWindow* win) : window(win) {}
        virtual ~GameRenderer() = default;

        virtual void render() = 0;

        void clear(sf::Color color = sf::Color::Black) {
            if (window) window->clear(color);
        }

        void display() {
            if (window) window->display();
        }

        sf::RenderWindow* getWindow() { return window; }
    };
}

