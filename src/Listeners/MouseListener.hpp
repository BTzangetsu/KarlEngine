#pragma once
#include<SFML/Graphics.hpp>

namespace KE {

	class MouseListener {

	public:
		virtual void onMouseButtonPressed(sf::Mouse::Button button, int x, int y) = 0;

		virtual void onMouseWheelScrolled(sf::Mouse::Wheel wheel, float delta, int x, int y) = 0;

		virtual void onMouseButtonReleased(sf::Mouse::Button button, int x, int y) = 0;

		virtual void onMouseMoved(int x, int y) = 0;

	};

}

