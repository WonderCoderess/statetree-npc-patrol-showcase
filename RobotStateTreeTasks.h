#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "RobotStateTreeTasks.generated.h"

class ARobotNPC;
class UAnimSequence;

/** Instance data for the Get Robot Patrol Point task. */
USTRUCT()
struct ASOULINTHREE_API FRobotGetPatrolPointInstanceData
{
	GENERATED_BODY()

	/** Robot whose patrol route is read, bound from the State Tree context. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<ARobotNPC> Robot = nullptr;

	/** World location of the robot's current patrol point, for binding to a move task. */
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector TargetLocation = FVector::ZeroVector;
};

/**
 * State Tree task that reads the robot's current patrol point location into TargetLocation.
 * Stays Running for the whole state so that TargetLocation remains available to sibling tasks.
 */
USTRUCT(meta = (DisplayName = "Get Robot Patrol Point", Category = "Robot"))
struct ASOULINTHREE_API FRobotGetPatrolPointTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRobotGetPatrolPointInstanceData;

	FRobotGetPatrolPointTask();

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

/** Instance data for the Advance Patrol task. */
USTRUCT()
struct ASOULINTHREE_API FRobotAdvancePatrolInstanceData
{
	GENERATED_BODY()

	/** Robot whose patrol route is advanced, bound from the State Tree context. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<ARobotNPC> Robot = nullptr;
};

/** State Tree task that advances the robot's patrol route to the next point and succeeds immediately. */
USTRUCT(meta = (DisplayName = "Advance Patrol", Category = "Robot"))
struct ASOULINTHREE_API FRobotAdvancePatrolTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRobotAdvancePatrolInstanceData;

	FRobotAdvancePatrolTask();

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

/** Instance data for the Play Patrol Animation task. */
USTRUCT()
struct ASOULINTHREE_API FRobotPlayPatrolAnimationInstanceData
{
	GENERATED_BODY()

	/** Robot that plays the animation, bound from the State Tree context. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<ARobotNPC> Robot = nullptr;

	/** Looping animation restored after the patrol point's animation finishes. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UAnimSequence> IdleAnimation = nullptr;

	/** Time left until the patrol point's animation finishes, in seconds. */
	UPROPERTY()
	float RemainingTime = 0.f;

	/** True if EnterState started a patrol point animation, so ExitState has to restore idle. */
	UPROPERTY()
	bool bPlayedAnimation = false;
};

/** State Tree task that plays the current patrol point's animation, waits for it to finish, then restores idle. */
USTRUCT(meta = (DisplayName = "Play Patrol Animation", Category = "Robot"))
struct ASOULINTHREE_API FRobotPlayPatrolAnimationTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FRobotPlayPatrolAnimationInstanceData;

	FRobotPlayPatrolAnimationTask();

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
