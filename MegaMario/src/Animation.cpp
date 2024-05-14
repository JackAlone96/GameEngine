#include "Animation.h"
#include <cmath>

Animation::Animation() { }

Animation::Animation(const std::string& name, const sf::Texture& t)
	: Animation(name, t, 1, 0) { }

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
: m_name(name), m_sprite(t), m_frameCount(frameCount), m_currentFrame(0), m_speed(speed) 
{ 
	m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(m_currentFrame * m_size.x, 0, m_size.x, m_size.y));
}

// updates the animation to show the next frame, depending on its speed
// animation loops when it reches the end
void Animation::update()
{
	// add the speed variable to the current frame
	m_currentFrame = (m_gameFrame / m_speed) % m_frameCount;
	m_sprite.setTextureRect(sf::IntRect(m_currentFrame * m_size.x, 0, m_size.x, m_size.y));

	// TODO: 1) calculate the correct frame of animation to play based on currentFrame and speed
	//		 2) set the texture rectangle properly (see constructor for sample)

	
	m_gameFrame++;
}

bool Animation::hasEnded() const
{
	if (m_currentFrame >= m_frameCount - 1) return true;

	return false;
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}
