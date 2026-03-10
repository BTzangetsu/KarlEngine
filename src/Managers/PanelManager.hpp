#pragma once
#include "Panel.hpp"

namespace KE {
	class PanelManager{

	private:
		std::vector<Panel*> panels;
		sf::RenderWindow* window;

	public:

		PanelManager(sf::RenderWindow* window):window(window){}

		virtual void addPanel(Panel* panel) {
			if (!panel) {
				std::cout << "Can't add a null pointer" << std::endl;
				return;
			}
			size_t index = panel->getZIndex();
			auto it = panels.begin();
			while (it != panels.end() && (*it)->getZIndex() <= index) {
				++it;
			}
			panels.insert(it, panel);
		}

		virtual int removePanel(Panel* panel) {
			auto it = std::find(panels.begin(), panels.end(), panel);
			if (it == panels.end()) {
				std::cout << "Panel not found" << std::endl;
				return 0;
			}
			panels.erase(it);
			return 1;
		}


	};
}

