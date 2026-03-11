#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>


namespace KE {

	class TextureManager{

	private:
		std::unordered_map<std::string, sf::Texture*> textures;
	public:

		TextureManager() = default;

		~TextureManager() {
			for (auto& pair : textures)
				delete pair.second;
		}

		bool loadTexture(const std::string& name, const std::string& filePath) {
			sf::Texture* texture = new sf::Texture();
			if (!texture->loadFromFile(filePath)) {
				delete texture;
				sf::err() << "Failed to load texture from file: " << filePath << std::endl;
				return 0;
			}
			if (getTexture(name) != nullptr) {
				delete texture;
				sf::err() << "Texture with name already exist: " << name << std::endl;
				return 0;
			}
			textures[name] = texture;
			return 1;
		}

		sf::Texture* getTexture(const std::string & name) {
			auto it = textures.find(name);
			if (it == textures.end()) {
				sf::err() << "Texture not found: " << name << std::endl;
				return nullptr;
			}
			else {
				return it->second;
			}
		}

		bool deleteTexture(const std::string& name) {
			auto it = textures.find(name);
			if (it != textures.end()) {
				delete it->second;
				textures.erase(it);
				return 1;
			}
			return 0;
		}

		void clear() {
			for (auto& pair : textures)
				delete pair.second;
			textures.clear();
		}

	};
}

