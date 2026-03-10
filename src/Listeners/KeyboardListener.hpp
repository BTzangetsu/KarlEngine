#pragma once
#include<SFML/Graphics.hpp>

namespace KE {

	class KeyboardListener {

	public:
		virtual void onKeyPressed(sf::Keyboard::Key key, bool alt, bool control, bool shift, bool system) = 0;

		virtual void onKeyReleased(sf::Keyboard::Key key, bool alt, bool control, bool shift, bool system) = 0;

		virtual void onTextEntered(uint32_t unicode) = 0;

	};

}

