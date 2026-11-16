#include "RobotNPC.h"
#include "PatrolRouteComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ARobotNPC::ARobotNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the robot's patrol route component.
	PatrolRoute = CreateDefaultSubobject<UPatrolRouteComponent>(TEXT("PatrolRoute"));

	// Assign an AI controller to placed and spawned robots.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Ignore physics bodies on the capsule.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	// Rotate towards the movement direction.
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Set the patrol movement speed.
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
}

UPatrolRouteComponent* ARobotNPC::GetPatrolRoute() const
{
	return PatrolRoute;
}