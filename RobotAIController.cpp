#include "RobotAIController.h"

#include "Components/StateTreeAIComponent.h"
#include "RobotNPC.h"

DEFINE_LOG_CATEGORY_STATIC(LogRobotAI, Log, All);

// Creates the State Tree component with automatic start disabled, so the logic runs only once a robot is possessed.
ARobotAIController::ARobotAIController()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeComponent"));
	StateTreeComponent->SetStartLogicAutomatically(false);
}

// Starts the State Tree after possessing a robot. Any other pawn type is ignored with a warning.
void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn || !InPawn->IsA<ARobotNPC>())
	{
		UE_LOG(LogRobotAI, Warning, TEXT("%s: possessed pawn %s is not an ARobotNPC, State Tree not started."), *GetName(), *GetNameSafe(InPawn));
		return;
	}

	if (IsValid(StateTreeComponent))
	{
		StateTreeComponent->StartLogic();
	}
}

// Stops the State Tree before releasing the pawn, so no task keeps running without a pawn.
void ARobotAIController::OnUnPossess()
{
	if (IsValid(StateTreeComponent))
	{
		StateTreeComponent->StopLogic(TEXT("Unpossess"));
	}

	Super::OnUnPossess();
}
