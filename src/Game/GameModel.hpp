#pragma once

namespace KE {

    class GameModel {

    protected:
        bool gameRunning;
        bool gamePaused;

    public:
        GameModel() : gameRunning(true), gamePaused(false) {}
        virtual ~GameModel() = default;

        virtual void update(float deltaTime) = 0;
        virtual bool isGameOver() const = 0;
        virtual void reset() = 0;

        bool isRunning() const { return gameRunning; }
        bool isPaused() const { return gamePaused; }
        void togglePause() { gamePaused = !gamePaused; }
        void stop() { gameRunning = false; }
    };
}

