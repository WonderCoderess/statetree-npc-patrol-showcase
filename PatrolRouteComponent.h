#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrolRouteComponent.generated.h"

UENUM(BlueprintType)
enum class EPatrolMode : uint8
{
	/** After the last point, continue from the first point. */
	Loop     UMETA(DisplayName = "Loop"),
	/** After the last point, walk the route backwards, reversing at each end. */
	PingPong UMETA(DisplayName = "Ping Pong")
};

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class ASOULINTHREE_API UPatrolRouteComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPatrolRouteComponent();

	/** Gets the location of the current patrol point. Returns false if the index is invalid or the point actor is not valid. */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	bool GetCurrentPointLocation(FVector& OutLocation) const;

	/** Advances to the next patrol point according to the patrol mode (Loop or Ping Pong). Does nothing with fewer than two points. */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void AdvanceToNextPoint();

	/** Returns the current patrol point actor, or nullptr if the index is invalid or the actor is not valid. */
	UFUNCTION(BlueprintPure, Category = "Patrol")
	AActor* GetCurrentPoint() const;

protected:
	virtual void BeginPlay() override;

private:
	/** Ordered patrol points for this instance. Invalid entries are removed on BeginPlay. */
	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	/** How the route continues after reaching its last point. */
	UPROPERTY(EditAnywhere, Category = "Patrol")
	EPatrolMode PatrolMode = EPatrolMode::Loop;

	/** Index of the current patrol point (read-only, for debugging). */
	UPROPERTY(VisibleInstanceOnly, Category = "Patrol|Debug")
	int32 CurrentIndex = 0;

	/** Ping Pong travel direction: +1 forward, -1 backward. */
	int32 Direction = 1;
};
