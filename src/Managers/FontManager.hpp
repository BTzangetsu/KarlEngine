#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

namespace KE{
	class FontManager{

private:
	std::unordered_map<std::string, sf::Font*> fonts;

public:

	FontManager() = default;

	~FontManager() {
		for (auto& pair : fonts)
			delete pair.second;
	}

	bool loadFont(const std::string& name, const std::string& filePath) {
		sf::Font* font = new sf::Font();
		if (!font->loadFromFile(filePath)) {
			delete font;
			sf::err() << "Failed to load font from file: " << filePath << std::endl;
			return 0;
		}
		if (getFont(name) != nullptr) {
			delete font;
			sf::err() << "Font with name already exist: " << name << std::endl;
			return 0;
		}
		fonts[name] = font;
		return 1;
	}

	sf::Font* getFont(const std::string& name) {
		auto it = fonts.find(name);
		if (it == fonts.end()) {
			sf::err() << "Font not found: " << name << std::endl;
			return nullptr;
		}
		else {
			return it->second;
		}
	}

	bool deleteFont(const std::string& name) {
		auto it = fonts.find(name);
		if (it != fonts.end()) {
			delete it->second;
			fonts.erase(it);
			return 1;
		}
		return 0;
	}

	void clear() {
		for (auto& pair : fonts)
			delete pair.second;
		fonts.clear();
	}
};


}