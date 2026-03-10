#pragma once
#include"KeyboardListener.hpp"
#include"JoystickListener.hpp"
#include"WindowListener.hpp"
#include"MouseListener.hpp"
#include<vector>

namespace KE {

    class LWindow : public sf::RenderWindow {
    private:
        // Stockage de pointeurs bruts (Non-owning Pointers)
        // C'est la responsabilité de l'utilisateur de garantir que l'objet pointé
        // ne soit PAS détruit tant qu'il est dans la liste.
        std::vector<KE::KeyboardListener*> KeyboardListeners;
        std::vector<KE::MouseListener*> MouseListeners;
        std::vector<KE::WindowListener*> WindowListeners;
        std::vector<KE::JoystickListener*> JoystickListeners;

        void ListenToKey(const sf::Event& event) {
            if (event.type == sf::Event::KeyPressed) {
                for (auto listener : KeyboardListeners) {
                    listener->onKeyPressed(
                        event.key.code,
                        event.key.alt,
                        event.key.control,
                        event.key.shift,
                        event.key.system
                    );
                }
            }
            if (event.type == sf::Event::KeyReleased) {
                for (auto listener : KeyboardListeners) {
                    listener->onKeyReleased(
                        event.key.code,
                        event.key.alt,
                        event.key.control,
                        event.key.shift,
                        event.key.system
                    );
                }
            }

            if (event.type == sf::Event::TextEntered) {
                for (auto listener : KeyboardListeners) listener->onTextEntered(event.text.unicode);
            }
        }

        void ListenToMouse(const sf::Event& event) {
            if (event.type == sf::Event::MouseButtonPressed) {
                for (auto listener : MouseListeners) {
                    listener->onMouseButtonPressed(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                for (auto listener : MouseListeners) {
                    listener->onMouseButtonReleased(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                for (auto listener : MouseListeners) {
                    listener->onMouseMoved(event.mouseMove.x, event.mouseMove.y);
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                for (auto listener : MouseListeners) {
                    listener->onMouseWheelScrolled(event.mouseWheelScroll.wheel, event.mouseWheelScroll.delta, event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                }
            }
        }

        void ListenToJoystick(const sf::Event& event) {
            if (event.type == sf::Event::JoystickButtonPressed) {
                for (auto listener : JoystickListeners) {
                    listener->onJoystickButtonPressed(event.joystickButton.joystickId, event.joystickButton.button);
                }
            }
            if (event.type == sf::Event::JoystickMoved) {
                for (auto listener : JoystickListeners) {
                    listener->onJoystickMoved(event.joystickMove.joystickId, event.joystickMove.axis, event.joystickMove.position);
                }
            }
            if (event.type == sf::Event::JoystickConnected) {
                for (auto listener : JoystickListeners) {
                    listener->onJoystickConnected(event.joystickConnect.joystickId);
                }
            }
        }

        void ListenToWindow(const sf::Event& event) {
            if (event.type == sf::Event::Closed) {
                for (auto listener : WindowListeners) listener->onClosed();
            }
            if (event.type == sf::Event::Resized) {
                for (auto listener : WindowListeners) listener->onResized(event.size.width, event.size.height);
            }
            if (event.type == sf::Event::LostFocus) {
                for (auto listener : WindowListeners) listener->onLostFocus();
            }
            if (event.type == sf::Event::GainedFocus) {
                for (auto listener : WindowListeners) listener->onGainedFocus();
            }
            if (event.type == sf::Event::MouseEntered) {
                for (auto listener : WindowListeners) listener->onMouseEntered();
            }
            if (event.type == sf::Event::MouseLeft) {
                for (auto listener : WindowListeners) listener->onMouseLeft();
            }
        }

    public:
        // Constructeur
        LWindow(sf::VideoMode mode, const std::string& title)
            : sf::RenderWindow(mode, title) {}

        // Méthodes d'ajout : acceptent une référence et stockent l'adresse (&listener)
        void addKeyboardListener(KE::KeyboardListener& listener) {
            this->KeyboardListeners.push_back(&listener);
        }

        void addMouseListener(KE::MouseListener &listener) {
            this->MouseListeners.push_back(&listener);
        }

        void addWindowListener(KE::WindowListener& listener) {
            this->WindowListeners.push_back(&listener);
        }

        void addJoystickListener(KE::JoystickListener& listener) {
            this->JoystickListeners.push_back(&listener);
        }

        void HandleEvent() {
            sf::Event event;
            while (this->pollEvent(event)) {
                ListenToKey(event);
                ListenToMouse(event);
                ListenToJoystick(event);
                ListenToWindow(event);
            }
        }
    };

}