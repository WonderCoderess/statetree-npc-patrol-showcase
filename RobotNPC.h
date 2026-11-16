#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RobotNPC.generated.h"

class UPatrolRouteComponent;

UCLASS()
class ASOULINTHREE_API ARobotNPC : public ACharacter
{
	GENERATED_BODY()

public:
	ARobotNPC();

	/** Returns this robot's patrol route component. */
	UFUNCTION(BlueprintPure, Category = "Patrol")
	UPatrolRouteComponent* GetPatrolRoute() const;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPatrolRouteComponent> PatrolRoute = nullptr;
};