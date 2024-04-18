#include "Assets.h"
#include <iostream>

Assets::Assets() {};

void Assets::AddTexture(std::string name, std::string path)
{
	sf::Texture texture;
	if (!texture.loadFromFile(path))
	{
		std::cout << "Texture: " << name << " was not found at " << path << std::endl;
		return;
	}

	m_textures[name] = texture;
}

void Assets::AddSound(std::string name, std::string path)
{
	sf::SoundBuffer soundBuffer;
	if (!soundBuffer.loadFromFile(path))
	{
		std::cout << "Sound: " << name << " was not found at " << path << std::endl;
		return;
	}

	sf::Sound sound;
	sound.setBuffer(soundBuffer);
	m_sounds[name] = sound;
}

void Assets::AddFont(std::string name, std::string path)
{
	sf::Font font;
	if (!font.loadFromFile(path))
	{
		std::cout << "Font: " << name << " was not found at " << path << std::endl;
		return;
	}

	m_fonts[name] = font;
}

void Assets::AddAnimation(std::string name, Animation animation)
{
	m_animations[name] = animation;
}

const sf::Texture& Assets::GetTexture(std::string name) const
{
	return m_textures.at(name);
}

const sf::Sound& Assets::GetSound(std::string name) const
{
	return m_sounds.at(name);
}

const sf::Font& Assets::GetFont(std::string name) const
{
	return m_fonts.at(name);
}

const Animation& Assets::GetAnimation(std::string name) const
{
	return m_animations.at(name);
}
