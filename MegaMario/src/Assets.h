#pragma once

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include <string>
#include "Animation.h"

class Assets
{
	std::map<std::string, sf::Texture> m_textures;
	std::map<std::string, Animation> m_animations;
	std::map<std::string, sf::Sound> m_sounds;
	std::map < std::string, sf::Font > m_fonts;

public:

	Assets();

	void AddTexture(std::string name, std::string path);
	void AddAnimation(std::string name, Animation animation);
	void AddSound(std::string name, std::string path);
	void AddFont(std::string name, std::string path);

	const sf::Texture& GetTexture(std::string name) const;
	const Animation& GetAnimation(std::string name) const;
	const sf::Sound& GetSound(std::string name) const;
	const sf::Font& GetFont(std::string name) const;
};
