#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "PanelManager.hpp"

namespace KE {

	class Panel:  public sf::Drawable{

	protected:

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
			sf::View oldView = target.getView();

			target.setView(this->view);

			for (auto drawable : drawables) {
				target.draw(*drawable, states);
			}

			target.setView(oldView);
		}

		size_t z_index;
		sf::View view;
		sf::RectangleShape bounds;
		std::vector<sf::Drawable*> drawables;
		friend class KE::PanelManager;

	public:

		Panel(float x = 0.f, float y = 0.f, float width = 100.f, float height = 100.f, size_t z_index = 0) {
			this->view.reset(sf::FloatRect(x, y, width, height));
			this->bounds.setPosition(x, y);
			this->bounds.setSize(sf::Vector2f(width, height));
			this->z_index = z_index;
		}

		const size_t getZIndex()const {
			return this->z_index;
		}

		const sf::FloatRect& getGlobalBounds()const {
			return bounds.getGlobalBounds();
		}

		const sf::Vector2f& getPosition()const {
			return getGlobalBounds().getPosition();
		}

		const sf::Vector2f& getSize()const {
			getGlobalBounds().getSize();
		}

		void setZIndex(size_t z_index) {
			this->z_index = z_index;
		}

		void setSize(const float width, const float height) {
			setSize(sf::Vector2f(width, height));
		}

		void setSize(const sf::Vector2f& size) {
			bounds.setSize(size);
			view.reset(getGlobalBounds());    
		}

		//The elements drawn in this panel will be relative to the panel's position.
		//it's first added, first drawn.
		void addDrawable(sf::Drawable* drawable) {
			this->drawables.push_back(drawable);
		}

		void insertDrawableAt(sf::Drawable* drawable, size_t index) {
			if (index >= drawables.size()) {
				this->drawables.push_back(drawable);
			}
			else {
				this->drawables.insert(this->drawables.begin() + index, drawable);
			}
		}

		void deleteDrawableAt(size_t index) {
			if (index < drawables.size()) {
				this->drawables.erase(this->drawables.begin() + index);
			}
		}

		void clearDrawables() {
			this->drawables.clear();
		}
	};
}

