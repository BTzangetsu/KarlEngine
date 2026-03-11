#pragma once
#include<SFML/Audio.hpp>
#include<unordered_map>

namespace KE {

	class AudioManager {
	private:

		std::unordered_map<std::string, sf::Music*> Music_map;
		std::unordered_map<sf::Sound*, sf::SoundBuffer*> Sound_Buffer_map;
		std::unordered_map<std::string, sf::Sound*> Sound_map;

	public:

		AudioManager() {}
		virtual ~AudioManager() {
			clearAudioManager();
		}

		//return 1 if the sound has been succefully added and 0 if there is an error while loadind or if the sound name already exist
		int addSound(const std::string& name, const std::string& path) {
			for (auto sound : Sound_map) {
				if (sound.first == name) {
					sf::err() << "Another sound named: " << sound.first << " already exist!";
					return 0;
				}
			}

			sf::SoundBuffer* buffer = new sf::SoundBuffer();
			if (!buffer->loadFromFile(path)) {
				sf::err() << "Error while loading the sound buffer from: " << path << "!";
				return 0;
			}

			sf::Sound* sound = new sf::Sound(*buffer);

			Sound_map.insert(std::pair<std::string, sf::Sound*>(name, sound));
			Sound_Buffer_map.insert(std::pair<sf::Sound*, sf::SoundBuffer*>(sound, buffer));

			return 1;
		}

		int addMusic(const std::string& name, const std::string& path) {
			for (auto music : Music_map) {
				if (music.first == name) {
					sf::err() << "Another Music named: " << music.first << " already exist!";
					return 0;
				}
			}

			sf::Music* music = new sf::Music();
			if (!music->openFromFile(path)) {
				sf::err() << "Error while opening the music from: " << path << "!";
				return 0;
			}

			Music_map.insert(std::pair<std::string, sf::Music*>(name, music));

			return 1;
		}

		void removeMusic(const std::string& name) {
			sf::Music* oldMusic = getMusic(name);
			Music_map.erase(name);
			delete oldMusic;
		}

		void removeSound(const std::string& name) {
			sf::Sound* oldSound = getSound(name);
			if (!oldSound) return;
			sf::SoundBuffer* oldSoundBuffer = Sound_Buffer_map[oldSound];

			Sound_map.erase(name);
			Sound_Buffer_map.erase(oldSound);
			delete oldSound;
			delete oldSoundBuffer;
		}

		void clearMusicList() {
			for (auto music : Music_map) {
				removeMusic(music.first);
			}
		}

		void clearSoundList() {
			for (auto sound : Sound_map) {
				removeSound(sound.first);
			}
		}

		void clearAudioManager() {
			clearMusicList();
			clearSoundList();
		}

		sf::Music* getMusic(const std::string& name) {
			auto it = Music_map.find(name);
			if (it != Music_map.end()) {
			}
			sf::err() << "No music named: " << name << " found!";
			return nullptr;
		}

		sf::Sound* getSound(const std::string& name) {
			auto it = Sound_map.find(name);
			if (it != Sound_map.end()) {
			}
			sf::err() << "No sound named: " << name << " found!";
			return nullptr;
		}

		void stopSound(const std::string& name) {
			sf::Sound* sound = getSound(name);
			if (sound) {
				sound->stop();
			}
		}

		void stopMusic(const std::string& name) {
			sf::Music* music = getMusic(name);
			if (music) {
				music->stop();
			}
		}

		void playSound(const std::string& name) {
			sf::Sound* sound = getSound(name);
			if (sound) {
				sound->play();
			}
		}

		void playMusic(const std::string& name) {
			sf::Music* music = getMusic(name);
			if (music) {
				music->stop();
			}
		}
	};

}

