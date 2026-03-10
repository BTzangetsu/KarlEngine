#pragma once
#include<SFML/Graphics.hpp>

namespace KE {

	class WindowListener {

	public:
		virtual void onMouseEntered() = 0;

		virtual void onMouseLeft() = 0;

		virtual void onClosed() = 0;

		virtual void onResized(unsigned int width, unsigned int height) = 0;

		virtual void onLostFocus() = 0;

		virtual void onGainedFocus() = 0;

	};

}

