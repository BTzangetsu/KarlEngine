#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath> 
#include "RoundedRectShape.hpp"

namespace KE {

    class TextZone : public sf::Drawable, public sf::Transformable {

    private:
        // graphic components
        KE::RoundedRectShape my_bg;       // the backGround rectangle
        sf::RenderTexture    my_texture;  // the target texture (for clipping)
        sf::Sprite           my_sprite;   // the sprite to draw the texture
        sf::Text             my_text;     // the text object to display
        sf::View             my_text_view;// the internal view (for the scrolling)
        sf::RectangleShape cursor_line;
        sf::Clock cursor_clock;
        bool cursor_visible = true;

        // Data
        std::string BGText;    // Placeholder
        bool        HasFocus;
        size_t      CursorPos;
        float       padding;
        sf::Vector2f size;

        // --- Internal methods ---

        //Convert the mouse position to local coordinates
        sf::Vector2f getLocalMousePos(const sf::RenderWindow& window) const {
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);
            // Transform the world coordinates to local coordinates
            return getInverseTransform().transformPoint(mouseWorld);
        }

        bool MouseIsInside(const sf::RenderWindow& window) const {
            sf::Vector2f localPos = getLocalMousePos(window);
            //We check if the local position is inside the size of the TextZone
            sf::FloatRect localBounds(0.f, 0.f, size.x, size.y);
            return localBounds.contains(localPos);
        }


    protected:

        virtual void drawCursor(sf::RenderTarget& target) const {
            if (!HasFocus) return;

            // appear every 500 ms
            if (cursor_clock.getElapsedTime().asMilliseconds() > 500) {
                const_cast<bool&>(cursor_visible) = !cursor_visible;
                const_cast<sf::Clock&>(cursor_clock).restart();
            }

            if (cursor_visible) {
                sf::Vector2f pos = my_text.findCharacterPos(CursorPos);

                float height = static_cast<float>(my_text.getCharacterSize());
                const_cast<sf::RectangleShape&>(cursor_line).setSize(sf::Vector2f(2.f, height));
                const_cast<sf::RectangleShape&>(cursor_line).setFillColor(my_text.getFillColor());
                const_cast<sf::RectangleShape&>(cursor_line).setPosition(pos.x, pos.y);

                target.draw(cursor_line);
            }
        }

        virtual void addChar(char CharToAdd) {
            sf::String str = my_text.getString();

            //jump line handling (to be improved later)
            if (CharToAdd == 13) {
                CharToAdd = '\n';
            }

            str.insert(CursorPos, CharToAdd);
            my_text.setString(str);
            CursorPos++;
            revalidate();
        }

        virtual void removeChar() {
            sf::String str = my_text.getString();
            if (CursorPos > 0 && str.getSize() > 0) {
                str.erase(CursorPos - 1, 1);
                my_text.setString(str);
                CursorPos--;
                revalidate();
            }
        }

        // Logique principale du scrolling
        virtual void revalidate() {
            if (my_text.getString().isEmpty()) return;

            // 1. cursorPos in the text world
            sf::Vector2f cursorPos = my_text.findCharacterPos(CursorPos);

            // estimated line height
            float lineHeight = static_cast<float>(my_text.getCharacterSize());

            // 2. current view information
            sf::Vector2f viewSize = my_text_view.getSize();
            sf::Vector2f viewCenter = my_text_view.getCenter();

            float viewLeft = viewCenter.x - viewSize.x / 2.f;
            float viewRight = viewCenter.x + viewSize.x / 2.f;
            float viewTop = viewCenter.y - viewSize.y / 2.f;
            float viewBottom = viewCenter.y + viewSize.y / 2.f;

            // 3. offset computing (Scrolling)

            // Horizontal
            float textPadding = padding * 2.f; // padding
            if (cursorPos.x + textPadding > viewRight) {
                viewCenter.x += (cursorPos.x + textPadding - viewRight);
            }
            else if (cursorPos.x - textPadding < viewLeft) {
                viewCenter.x -= (viewLeft - (cursorPos.x + textPadding));
            }

            // Vertical
            if (cursorPos.y + lineHeight + padding > viewBottom) {
                viewCenter.y += ((cursorPos.y + lineHeight + padding) - viewBottom);
            }
            else if (cursorPos.y < viewTop) {
                viewCenter.y -= (viewTop - cursorPos.y);
            }

            // preventing view from going out of text bounds (optional)
            if (viewCenter.x < viewSize.x / 2.f) viewCenter.x = viewSize.x / 2.f;
            if (viewCenter.y < viewSize.y / 2.f) viewCenter.y = viewSize.y / 2.f;

            my_text_view.setCenter(viewCenter);
        }


        virtual void MouseListener(sf::RenderWindow& window, sf::Event& event) {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (MouseIsInside(window)) {
                        HasFocus = true;

                        sf::Vector2f localMouse = getLocalMousePos(window);

                        sf::Vector2f textWorldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), my_text_view);

                        sf::Vector2f clickInText = localMouse - (my_text_view.getSize() / 2.f) + my_text_view.getCenter();
                        //we look for the closest character position
                        float minDiff = 99999.f;
                        size_t bestIndex = my_text.getString().getSize();

                        for (size_t i = 0; i <= my_text.getString().getSize(); i++) {
                            sf::Vector2f charPos = my_text.findCharacterPos(i);
                            float diff = std::abs(charPos.x - clickInText.x) + std::abs(charPos.y - clickInText.y);
                            if (diff < minDiff) {
                                minDiff = diff;
                                bestIndex = i;
                            }
                        }
                        CursorPos = bestIndex;
                        revalidate();
                    }
                    else {
                        HasFocus = false;
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    if (MouseIsInside(window)) {
                        HasFocus = true;
                        CursorPos = my_text.getString().getSize();
                        revalidate();
                    }
                }
            }
        }

        virtual void KeyboardListener(sf::Event& event) {
            if (!HasFocus) return;

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Left:
                    if (CursorPos > 0) {
                        CursorPos--;
                        revalidate();
                    }
                    break;
                case sf::Keyboard::Right:
                    if (CursorPos < my_text.getString().getSize()) {
                        CursorPos++;
                        revalidate();
                    }
                    break;
                case sf::Keyboard::BackSpace:
                    removeChar();
                    break;
                default:
                    break;
                }
            }
            else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    addChar(static_cast<char>(event.text.unicode));
                }
            }
        }

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
            //  we first apply the transform
            states.transform *= getTransform();

            target.draw(my_bg, states);

            auto& texture = const_cast<sf::RenderTexture&>(my_texture);

            texture.clear(sf::Color::Transparent);
            texture.setView(my_text_view);

            if (!my_text.getString().isEmpty()) {
                texture.draw(my_text);
            }
            else if (!HasFocus && !BGText.empty()) {
                // dispaly placeholder
                sf::Text placeholder = my_text;
                placeholder.setString(BGText);
                placeholder.setFillColor(sf::Color(150, 150, 150)); // gray
                texture.draw(placeholder);
            }

            drawCursor(texture);
            texture.display();
            target.draw(my_sprite, states);
        }

    public:
        TextZone(int width, int height, sf::Font* font = nullptr) : size(static_cast<float>(width), static_cast<float>(height)) {
            my_bg.setSize(size);
            // Couleur par défaut, à changer via setters
            my_bg.setFillColor(sf::Color(240, 240, 240));
            my_bg.setOutlineThickness(1);
            my_bg.setOutlineColor(sf::Color::Black);

            // Init RenderTexture
            if (!my_texture.create(width, height)) {
                std::cerr << "Erreur: Impossible de creer RenderTexture pour TextZone" << std::endl;
            }

            // Init Sprite 
            my_sprite.setTexture(my_texture.getTexture());

            // Init text view
            my_text_view.setSize(size);
            my_text_view.setCenter(size.x / 2.f, size.y / 2.f);

            // Init Text
            if (font) my_text.setFont(*font);
            my_text.setFillColor(sf::Color::Black);
            my_text.setCharacterSize(20); // default size

            padding = 5.f;
            BGText = "...";
            HasFocus = false;
            CursorPos = 0;
        }


        TextZone(const TextZone& other) : TextZone(other.size.x, other.size.y) {
            *this = other;
        }

        TextZone& operator=(const TextZone& other) {
            if (this != &other) {
                this->setPosition(other.getPosition());
                this->setRotation(other.getRotation());
                this->setScale(other.getScale());

                this->size = other.size;
                this->my_bg = other.my_bg;
                this->my_text = other.my_text;
                this->BGText = other.BGText;
                this->HasFocus = other.HasFocus;
                this->CursorPos = other.CursorPos;
                this->padding = other.padding;


                if (my_texture.getSize().x != static_cast<unsigned int>(size.x) ||
                    my_texture.getSize().y != static_cast<unsigned int>(size.y)) {
                    my_texture.create(size.x, size.y);
                    my_sprite.setTexture(my_texture.getTexture(), true);
                }
                this->my_text_view = other.my_text_view;

                revalidate();
            }
            return *this;
        }

        virtual ~TextZone() {}

        // --- publics methodes---

        void Listen(sf::RenderWindow& window, sf::Event& event) {
            MouseListener(window, event);
            KeyboardListener(event);
        }


        bool hasFocus() const { return HasFocus; }
        void takeFocus() { HasFocus = true; }
        void loseFocus() { HasFocus = false; }

        std::string getText() const { return my_text.getString(); }

        void setText(const std::string& str) {
            my_text.setString(str);
            CursorPos = str.size();
            revalidate();
        }

        //use this only if you understand the consequences
        sf::Text& getTextObject() { return my_text; }

        void setBackgroundColor(sf::Color color) { my_bg.setFillColor(color); }
        void setTextColor(sf::Color color) { my_text.setFillColor(color); }

        void setPlaceholderText(const std::string& str) { BGText = str; }
        void setPadding(float pad) { padding = pad; revalidate(); }

        void setRadius(float radius) {
            my_bg.setRadius(radius);
        }

        float getRadius() const {
            return my_bg.getRadius();
        }
    };
}