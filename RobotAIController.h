#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RobotAIController.generated.h"

class UStateTreeAIComponent;

/** AI controller for robot NPCs. Runs the robot's State Tree, which is started only after possessing an ARobotNPC. */
UCLASS()
class ASOULINTHREE_API ARobotAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARobotAIController();

protected:
	/** Starts the State Tree logic when the possessed pawn is an ARobotNPC; logs a warning otherwise. */
	virtual void OnPossess(APawn* InPawn) override;

	/** Stops the State Tree logic before releasing the pawn. */
	virtual void OnUnPossess() override;

private:
	/** Runs the robot's State Tree. Does not start automatically; started in OnPossess. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeComponent = nullptr;
};
