#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "PatrolPoint.generated.h"

class UAnimSequence;

/** Patrol route point with an optional animation that the robot plays after reaching it (Play Patrol Animation task). */
UCLASS()
class ASOULINTHREE_API APatrolPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	/** Returns the animation to play at this point, or nullptr if none is set. */
	UAnimSequence* GetPatrolAnimation() const;

private:
	/** Animation played once by the robot at this patrol point. Leave empty to continue patrolling without stopping. */
	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TObjectPtr<UAnimSequence> PatrolAnimation = nullptr;
};
