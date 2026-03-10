#pragma once
#include<SFML/Graphics.hpp>
namespace KE {

	class JoystickListener {

	public:
		virtual void onJoystickButtonPressed(unsigned int joystickId, unsigned int button) = 0;

		virtual void onJoystickMoved(unsigned int joystickId, sf::Joystick::Axis axis, float position) = 0;

		virtual void onJoystickConnected(unsigned int joystickId) = 0;
	};
}

