#include "Physics.h"
#include "Components.h"

Physics::Physics() {};

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	const auto& aBB = a->GetComponent<CBoundingBox>();
	const auto& aPos = a->GetComponent<CTransform>().pos;
	const auto& bBB = b->GetComponent<CBoundingBox>();
	const auto& bPos = b->GetComponent<CTransform>().pos;

	Vec2 delta = Vec2(fabs(aPos.x - bPos.x), fabs(aPos.y - bPos.y));
	float overlapX = aBB.halfSize.x + bBB.halfSize.x - delta.x;
	float overlapY = aBB.halfSize.y + bBB.halfSize.y - delta.y;

	return Vec2(overlapX, overlapY);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return previous overlap rectangle size of the bounding boxes of entity a and b
	//		 previous overlap uses the entity's previous position

	const auto& aBB = a->GetComponent<CBoundingBox>();
	const auto& aPos = a->GetComponent<CTransform>().prevPos;
	const auto& bBB = b->GetComponent<CBoundingBox>();
	const auto& bPos = b->GetComponent<CTransform>().prevPos;

	Vec2 delta = Vec2(fabs(aPos.x - bPos.x), fabs(aPos.y - bPos.y));
	float overlapX = aBB.halfSize.x + bBB.halfSize.x - delta.x;
	float overlapY = aBB.halfSize.y + bBB.halfSize.y - delta.y;

	return Vec2(overlapX, overlapY);
}
