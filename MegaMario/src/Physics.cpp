#include "Physics.h"
#include "Components.h"

Physics::Physics() {};

Math::Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the overlap rectangle size of the bounding boxes of entity a and b

	return Math::Vec2(0,0);
}

Math::Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return previous overlap rectangle size of the bounding boxes of entity a and b
	//		 previous overlap uses the entity's previous position

	return Math::Vec2(0, 0);
}
