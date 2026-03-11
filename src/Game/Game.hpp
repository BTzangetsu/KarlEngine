#pragma once
#include "GameRenderer.hpp"
#include "GameController.hpp"
#include "GameModel.hpp"
#include "GameRessources.hpp"

class Game {
protected:
    sf::RenderWindow window;
    KE::GameRenderer* renderer;
    KE::GameController* controller;
    KE::GameModel* model;
    KE::GameRessources* resourceManager;
    sf::Clock clock;

public:
    Game(const std::string& title, int width, int height)
        : window(sf::VideoMode(width, height), title),
        resourceManager(&KE::GameRessources::getInstance()) {}

    virtual ~Game() = default;

    
    virtual void initGame() = 0;

    
    virtual bool loadResources() = 0;

    
    void run() {
        initGame();

        if (!loadResources()) {
            return;
        }

        while (window.isOpen() && model->isRunning()) {
            float deltaTime = clock.restart().asSeconds();

            controller->processEvents();
            if (!model->isPaused()) {
                model->update(deltaTime);
            }

            if (model->isGameOver()) {
                onGameOver();
            }

            renderer->clear();
            renderer->render();
            renderer->display();
        }

    }

    virtual void onGameOver() {}


    sf::RenderWindow* getWindow() { return &window; }
};

