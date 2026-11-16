#include "RobotStateTreeTasks.h"

#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "PatrolPoint.h"
#include "PatrolRouteComponent.h"
#include "RobotNPC.h"
#include "StateTreeExecutionContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogRobotTasks, Log, All);

// ---------------------------------------------------------------------------
// Get Robot Patrol Point
// ---------------------------------------------------------------------------

// Instant task: no tick needed, it only writes TargetLocation on enter.
FRobotGetPatrolPointTask::FRobotGetPatrolPointTask()
{
	bShouldCallTick = false;
}

// Writes the current patrol point location to TargetLocation.
// Returns Running (not Succeeded) on purpose: the task must stay active for the whole state,
// so the move task bound to TargetLocation keeps a valid target. The state is completed by that move task.
EStateTreeRunStatus FRobotGetPatrolPointTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (!IsValid(InstanceData.Robot))
	{
		return EStateTreeRunStatus::Failed;
	}

	const UPatrolRouteComponent* PatrolRoute = InstanceData.Robot->GetPatrolRoute();
	if (!IsValid(PatrolRoute))
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!PatrolRoute->GetCurrentPointLocation(InstanceData.TargetLocation))
	{
		return EStateTreeRunStatus::Failed;
	}

	// Enable with cmd "log LogRobotTasks Verbose"
	UE_LOG(LogRobotTasks, Verbose, TEXT("%s: patrol target %s, location %s"),
		*InstanceData.Robot->GetActorNameOrLabel(),
		*GetNameSafe(PatrolRoute->GetCurrentPoint()),
		*InstanceData.TargetLocation.ToString());

	return EStateTreeRunStatus::Running;
}

// ---------------------------------------------------------------------------
// Advance Patrol
// ---------------------------------------------------------------------------

// Instant task: no tick needed, it advances the route on enter.
FRobotAdvancePatrolTask::FRobotAdvancePatrolTask()
{
	bShouldCallTick = false;
}

// Moves the robot's patrol route to the next point and succeeds immediately.
EStateTreeRunStatus FRobotAdvancePatrolTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (!IsValid(InstanceData.Robot))
	{
		return EStateTreeRunStatus::Failed;
	}

	UPatrolRouteComponent* PatrolRoute = InstanceData.Robot->GetPatrolRoute();
	if (!IsValid(PatrolRoute))
	{
		return EStateTreeRunStatus::Failed;
	}

	PatrolRoute->AdvanceToNextPoint();
	return EStateTreeRunStatus::Succeeded;
}

// ---------------------------------------------------------------------------
// Play Patrol Animation
// ---------------------------------------------------------------------------

// Ticks to count down the remaining animation time.
FRobotPlayPatrolAnimationTask::FRobotPlayPatrolAnimationTask()
{
	bShouldCallTick = true;
}

// Starts the current patrol point's animation and stores its duration in RemainingTime.
// Succeeds immediately if the point is not an APatrolPoint or has no animation assigned.
EStateTreeRunStatus FRobotPlayPatrolAnimationTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	InstanceData.RemainingTime = 0.f;
	InstanceData.bPlayedAnimation = false;

	if (!IsValid(InstanceData.Robot))
	{
		return EStateTreeRunStatus::Failed;
	}

	const UPatrolRouteComponent* PatrolRoute = InstanceData.Robot->GetPatrolRoute();
	if (!IsValid(PatrolRoute))
	{
		return EStateTreeRunStatus::Failed;
	}

	const APatrolPoint* Point = Cast<APatrolPoint>(PatrolRoute->GetCurrentPoint());
	if (!IsValid(Point))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	UAnimSequence* Animation = Point->GetPatrolAnimation();
	if (!IsValid(Animation))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	USkeletalMeshComponent* Mesh = InstanceData.Robot->GetMesh();
	if (!IsValid(Mesh))
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!IsValid(InstanceData.IdleAnimation))
	{
		UE_LOG(LogRobotTasks, Warning, TEXT("%s: Play Patrol Animation has no IdleAnimation set, the robot will freeze on the last frame of %s."),
			*InstanceData.Robot->GetActorNameOrLabel(), *Animation->GetName());
	}

	Mesh->PlayAnimation(Animation, false);
	InstanceData.bPlayedAnimation = true;

	// Duration in real time, taking the sequence's own play rate into account.
	const float RateScale = FMath::Abs(Animation->RateScale);
	InstanceData.RemainingTime = RateScale > UE_KINDA_SMALL_NUMBER
		? Animation->GetPlayLength() / RateScale
		: Animation->GetPlayLength();

	return EStateTreeRunStatus::Running;
}

// Counts down RemainingTime and succeeds once the patrol point's animation has finished.
EStateTreeRunStatus FRobotPlayPatrolAnimationTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (!IsValid(InstanceData.Robot))
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.RemainingTime -= DeltaTime;
	return InstanceData.RemainingTime <= 0.f ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Running;
}

// Restores the looping idle animation, but only if EnterState actually played a patrol point animation,
// so idle is not restarted from the first frame at points without an animation.
void FRobotPlayPatrolAnimationTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (!InstanceData.bPlayedAnimation)
	{
		return;
	}
	InstanceData.bPlayedAnimation = false;

	if (!IsValid(InstanceData.Robot) || !IsValid(InstanceData.IdleAnimation))
	{
		return;
	}

	USkeletalMeshComponent* Mesh = InstanceData.Robot->GetMesh();
	if (IsValid(Mesh))
	{
		Mesh->PlayAnimation(InstanceData.IdleAnimation, true);
	}
}
