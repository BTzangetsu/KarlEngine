#pragma once
#include "AudioManager.hpp"
#include "FontManager.hpp"
#include "TextureManager.hpp"


namespace KE {

	class GameRessources{

	private:
		GameRessources() {}

	public:

		static GameRessources& getInstance(void) {
			if (!RMan) RMan = new GameRessources();
			return *RMan;
		}

		~GameRessources() {
			if (RMan) {
				delete RMan;
				RMan = nullptr;
			}
		}

		KE::AudioManager AudMan;
		KE::FontManager FontMan;
		KE::TextureManager TexMManager;
	};

	KE::GameRessources* RMan = nullptr;
}

