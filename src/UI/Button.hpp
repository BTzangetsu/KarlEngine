#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

#include "RoundedRectShape.hpp"

namespace KE {

    /**
     * @class Button
     * @brief A customizable button widget for SFML.
     *
     * This class provides a graphical button with customizable properties
     * such as size, color, border, label, and rounded corners.
     *
     * Features:
     * - Automatic layout based on text content and padding
     * - Rounded corners support
     * - Text styling (color, size, spacing, style)
     * - Transformations (position, rotation, scale)
     * - Efficient dirty-flag based updates
     *
     * @warning The button stores a pointer to the font, not a copy.
     *          Ensure the font remains valid for the lifetime of the button.
     */
    class Button : public sf::Drawable, public sf::Transformable {
    private:
        // Member variables with consistent naming (m_ prefix)
        KE::RoundedRectShape m_rect;    ///< The rectangular shape of the button.
        sf::Text m_text;                ///< The text displayed on the button.
        float m_padding;                ///< The padding around the text.
        mutable bool m_needsUpdate;     ///< Flag indicating if layout needs recalculation.

        /**
         * @brief Recalculates the position and size of components.
         *
         * This is called automatically when properties change.
         * Uses dirty flag pattern to avoid unnecessary recalculations.
         */
        void revalidate() const {
            // Get text bounds
            sf::FloatRect textBounds = m_text.getLocalBounds();

            // Calculate rectangle size (text + padding on all sides)
            sf::Vector2f rectSize(
                textBounds.width + 2.0f * m_padding,
                textBounds.height + 2.0f * m_padding
            );

            // Update rectangle size
            const_cast<RoundedRectShape&>(m_rect).setSize(rectSize);

            // Position text with padding offset
            // Account for text local bounds offset
            const_cast<sf::Text&>(m_text).setPosition(
                m_padding - textBounds.left,
                m_padding - textBounds.top
            );

            m_needsUpdate = false;
        }

        /**
         * @brief Marks the button as needing a layout update.
         */
        void markDirty() {
            m_needsUpdate = true;
        }

    protected:
        /**
         * @brief Draws the button on the specified render target.
         *
         * Automatically recalculates layout if needed before drawing.
         *
         * @param target The target to render the button on.
         * @param states The render states to use for drawing.
         */
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
            // Update layout if needed
            if (m_needsUpdate) {
                revalidate();
            }

            // Apply transformations
            states.transform *= getTransform();

            // Draw components
            target.draw(m_rect, states);
            target.draw(m_text, states);
        }

    public:
        // -------------------- CONSTRUCTORS --------------------

        /**
         * @brief Default constructor.
         *
         * Creates a button with empty label and default styling.
         *
         * @warning You must call setFont() before drawing the button.
         */
        Button()
            : m_padding(5.0f)
            , m_needsUpdate(true)
        {
            m_text.setString("");
            m_text.setCharacterSize(15);
            m_text.setFillColor(sf::Color::Black);
            m_text.setStyle(sf::Text::Regular);

            m_rect.setFillColor(sf::Color::White);
            m_rect.setOutlineColor(sf::Color::Black);
            m_rect.setOutlineThickness(1.0f);
        }

        /**
         * @brief Constructor with position, padding, and font.
         *
         * @param posX X position of the button.
         * @param posY Y position of the button.
         * @param padding The padding around the text.
         * @param font Reference to the font used for the button text.
         *
         * @warning The button does not copy the font, only stores a pointer.
         *          Ensure the font remains valid for the lifetime of the button.
         */
        Button(float posX, float posY, float padding, const sf::Font& font)
            : Button()
        {
            m_padding = padding;
            m_text.setFont(font);
            setPosition(posX, posY);
        }

        /**
         * @brief Constructor with label, position, padding, and font.
         *
         * @param label The text displayed on the button.
         * @param posX X position of the button.
         * @param posY Y position of the button.
         * @param padding The padding around the text.
         * @param font Reference to the font used for the button text.
         *
         * @warning The button does not copy the font, only stores a pointer.
         *          Ensure the font remains valid for the lifetime of the button.
         */
        Button(const std::string& label, float posX, float posY, float padding, const sf::Font& font)
            : Button(posX, posY, padding, font)
        {
            m_text.setString(label);
            markDirty();
        }

        /**
         * @brief Default copy constructor.
         *
         * Note: The font pointer is copied, not the font itself.
         */
        Button(const Button&) = default;

        /**
         * @brief Default copy assignment operator.
         *
         * Note: The font pointer is copied, not the font itself.
         */
        Button& operator=(const Button&) = default;

        /**
         * @brief Default move constructor.
         */
        Button(Button&&) = default;

        /**
         * @brief Default move assignment operator.
         */
        Button& operator=(Button&&) = default;

        /**
         * @brief Default destructor.
         */
        ~Button() = default;

        // -------------------- SETTERS --------------------

        /**
         * @brief Sets the corner radius of the button.
         *
         * @param radius The radius of the rounded corners.
         */
        void setRadius(float radius) {
            m_rect.setRadius(radius);
            // No need to mark dirty - RoundedRect handles its own updates
        }

        /**
         * @brief Sets the padding around the text.
         *
         * @param padding The new padding value.
         */
        void setPadding(float padding) {
            m_padding = padding;
            markDirty();
        }

        /**
         * @brief Sets the fill color of the button.
         *
         * @param color The new fill color.
         */
        void setFillColor(const sf::Color& color) {
            m_rect.setFillColor(color);
        }

        /**
         * @brief Sets the outline thickness of the button.
         *
         * @param thickness The new outline thickness.
         */
        void setOutlineThickness(float thickness) {
            m_rect.setOutlineThickness(thickness);
        }

        /**
         * @brief Sets the outline color of the button.
         *
         * @param color The new outline color.
         */
        void setOutlineColor(const sf::Color& color) {
            m_rect.setOutlineColor(color);
        }

        /**
         * @brief Sets the letter spacing of the text.
         *
         * @param spacingFactor The spacing factor (1.0 = normal spacing).
         */
        void setLetterSpacing(float spacingFactor) {
            m_text.setLetterSpacing(spacingFactor);
            markDirty();
        }

        /**
         * @brief Sets the line spacing of the text.
         *
         * @param lineSpaceFactor The line spacing factor (1.0 = normal spacing).
         */
        void setLineSpacing(float lineSpaceFactor) {
            m_text.setLineSpacing(lineSpaceFactor);
            markDirty();
        }

        /**
         * @brief Sets the style of the text.
         *
         * @param style The text style (e.g., sf::Text::Bold, sf::Text::Italic).
         */
        void setTextStyle(sf::Uint32 style) {
            m_text.setStyle(style);
            markDirty();
        }

        /**
         * @brief Sets the text label of the button.
         *
         * @param label The new text.
         */
        void setLabel(const std::string& label) {
            m_text.setString(label);
            markDirty();
        }

        /**
         * @brief Sets the character size of the button's text.
         *
         * @param characterSize The new character size.
         */
        void setCharacterSize(unsigned int characterSize) {
            m_text.setCharacterSize(characterSize);
            markDirty();
        }

        /**
         * @brief Sets the text color.
         *
         * @param color The new text color.
         */
        void setTextColor(const sf::Color& color) {
            m_text.setFillColor(color);
        }

        /**
         * @brief Sets the font of the button text.
         *
         * @param font Reference to the new font.
         *
         * @warning The button does not copy the font, only stores a pointer.
         *          Ensure the font remains valid for the lifetime of the button.
         */
        void setFont(const sf::Font& font) {
            m_text.setFont(font);
            markDirty();
        }

        // -------------------- GETTERS --------------------

        /**
         * @brief Gets the size of the button.
         *
         * @return A vector containing the width and height.
         */
        sf::Vector2f getSize() const {
            if (m_needsUpdate) {
                revalidate();
            }
            return m_rect.getSize();
        }

        /**
         * @brief Gets the text label of the button.
         *
         * @return The current text as a string.
         */
        std::string getLabel() const {
            return m_text.getString();
        }

        /**
         * @brief Gets the global bounds of the button.
         *
         * This includes transformations (position, rotation, scale).
         *
         * @return A FloatRect representing the button's bounding box.
         */
        sf::FloatRect getGlobalBounds() const {
            if (m_needsUpdate) {
                revalidate();
            }
            return getTransform().transformRect(m_rect.getGlobalBounds());
        }

        /**
         * @brief Gets the local bounds of the button.
         *
         * This is the untransformed bounding box.
         *
         * @return A FloatRect representing the button's local bounds.
         */
        sf::FloatRect getLocalBounds() const {
            if (m_needsUpdate) {
                revalidate();
            }
            return m_rect.getLocalBounds();
        }

        /**
         * @brief Gets the fill color of the button.
         *
         * @return The current fill color.
         */
        const sf::Color& getFillColor() const {
            return m_rect.getFillColor();
        }

        /**
         * @brief Gets the outline color of the button.
         *
         * @return The current outline color.
         */
        const sf::Color& getOutlineColor() const {
            return m_rect.getOutlineColor();
        }

        /**
         * @brief Gets the outline thickness of the button.
         *
         * @return The current outline thickness.
         */
        float getOutlineThickness() const {
            return m_rect.getOutlineThickness();
        }

        /**
         * @brief Gets the character size of the text.
         *
         * @return The current character size.
         */
        unsigned int getCharacterSize() const {
            return m_text.getCharacterSize();
        }

        /**
         * @brief Gets the font used by the button.
         *
         * @return Pointer to the font, or nullptr if no font is set.
         */
        const sf::Font* getFont() const {
            return m_text.getFont();
        }

        /**
         * @brief Gets the corner radius of the button.
         *
         * @return The current corner radius.
         */
        float getRadius() const {
            return m_rect.getRadius();
        }

        /**
         * @brief Gets the letter spacing factor.
         *
         * @return The current letter spacing factor.
         */
        float getLetterSpacing() const {
            return m_text.getLetterSpacing();
        }

        /**
         * @brief Gets the line spacing factor.
         *
         * @return The current line spacing factor.
         */
        float getLineSpacing() const {
            return m_text.getLineSpacing();
        }

        /**
         * @brief Gets the text style.
         *
         * @return The current text style flags.
         */
        sf::Uint32 getStyle() const {
            return m_text.getStyle();
        }

        /**
         * @brief Gets the padding around the text.
         *
         * @return The current padding value.
         */
        float getPadding() const {
            return m_padding;
        }

        /**
         * @brief Gets the text color.
         *
         * @return The current text color.
         */
        const sf::Color& getTextColor() const {
            return m_text.getFillColor();
        }

        // -------------------- UTILITY --------------------

        /**
         * @brief Checks if a point is inside the button.
         *
         * Useful for mouse click detection.
         *
         * @param point The point to test (in global coordinates).
         * @return true if the point is inside the button, false otherwise.
         */
        bool contains(const sf::Vector2f& point) const {
            return getGlobalBounds().contains(point);
        }

        /**
         * @brief Forces an immediate layout recalculation.
         *
         * This is rarely needed as updates happen automatically.
         */
        void forceUpdate() const {
            revalidate();
        }
    };

} // namespace KE