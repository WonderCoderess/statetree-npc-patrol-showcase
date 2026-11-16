#include "PatrolPoint.h"

// Returns the animation assigned to this point in the level, or nullptr if none is set.
UAnimSequence* APatrolPoint::GetPatrolAnimation() const
{
	return PatrolAnimation;
}
