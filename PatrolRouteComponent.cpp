#include "PatrolRouteComponent.h"

#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogPatrol, Log, All);

// The component only holds route state and is driven by AI tasks, so it never needs to tick.
UPatrolRouteComponent::UPatrolRouteComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Removes empty or invalid patrol point entries and resets the route to the first point, moving forward.
void UPatrolRouteComponent::BeginPlay()
{
	Super::BeginPlay();

	PatrolPoints.RemoveAll([](const AActor* Point)
		{
			return !IsValid(Point);
		});
	CurrentIndex = 0;
	Direction = 1;
}

// Writes the world location of the current patrol point to OutLocation. Returns false (leaving OutLocation unchanged) if there is no valid current point.
bool UPatrolRouteComponent::GetCurrentPointLocation(FVector& OutLocation) const
{
	const AActor* CurrentPoint = GetCurrentPoint();
	if (!CurrentPoint)
	{
		return false;
	}

	OutLocation = CurrentPoint->GetActorLocation();
	return true;
}

// Moves CurrentIndex to the next patrol point.
// Loop: wraps from the last point back to the first.
// PingPong: walks in the current Direction and, at either end of the route, flips Direction and steps back.
// Routes with fewer than two points are left unchanged.
void UPatrolRouteComponent::AdvanceToNextPoint()
{
	const int32 NumPoints = PatrolPoints.Num();
	if (NumPoints <= 1)
	{
		return;
	}

	switch (PatrolMode)
	{
	case EPatrolMode::Loop:
		CurrentIndex = (CurrentIndex + 1) % NumPoints;
		break;

	case EPatrolMode::PingPong:
	{
		const int32 NextIndex = CurrentIndex + Direction;
		if (NextIndex < 0 || NextIndex >= NumPoints)
		{
			Direction = -Direction;
			CurrentIndex += Direction;
		}
		else
		{
			CurrentIndex = NextIndex;
		}
	}
	break;
	}

	// Verbose logging for debugging patrol point advancement (cmd "log LogPatrol Verbose")
	UE_LOG(LogPatrol, Verbose, TEXT("%s: index %d, direction %d"), *GetOwner()->GetActorNameOrLabel(), CurrentIndex, Direction);
}

// Returns the actor at CurrentIndex, or nullptr if the index is out of range or the actor has been destroyed.
AActor* UPatrolRouteComponent::GetCurrentPoint() const
{
	if (!PatrolPoints.IsValidIndex(CurrentIndex))
	{
		return nullptr;
	}

	AActor* Point = PatrolPoints[CurrentIndex];

	return IsValid(Point) ? Point : nullptr;
}
