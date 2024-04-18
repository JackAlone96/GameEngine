#pragma once

#include "Vec2.h"
#include <memory>
#include "Entity.h"

class Physics
{
public:

	Physics();

	Math::Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	Math::Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
};