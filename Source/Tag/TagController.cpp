// Fill out your copyright notice in the Description page of Project Settings.

#include "TagController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavFilter_Tagged.h"

void ATagController::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), Waypoints);

	TagCharacter = Cast<ATagCharacter>(GetCharacter());

	if (TagCharacter)
	{
		TagCharacter->LandedDelegate.AddUniqueDynamic(this, &ATagController::OnLanded);
		TagCharacter->MovementModeChangedDelegate.AddUniqueDynamic(this, &ATagController::OnMovementModeChanged);
		TagCharacter->OnNewEnemySeen.AddUniqueDynamic(this, &ATagController::RestartMovement);
		TagCharacter->OnTagged.AddUniqueDynamic(this, &ATagController::RestartMovement);
		MovementBehaviour();
	}
}

void ATagController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ATagController::MovementBehaviour, 1.0f, false);
}

ATargetPoint* ATagController::GetRandomWaypoint() const
{
	auto index = FMath::RandRange(0, Waypoints.Num() - 1);
	return Cast<ATargetPoint>(Waypoints[index]);
}

void ATagController::GoToRandomWaypoint()
{
	TaggedMove(GetRandomWaypoint());
}

void ATagController::OnMovementModeChanged(ACharacter* MovedCharacter, EMovementMode PrevMovementMode, uint8 PreviousCustomMode /*= 0*/)
{
	// If the new movement mode is Falling
	if (MovedCharacter->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		GetPathFollowingComponent()->Deactivate();
	}
}

void ATagController::OnLanded(const FHitResult & Hit)
{
	GetPathFollowingComponent()->Activate();
}

void ATagController::RestartMovement()
{
	MovementBehaviour();
}

void ATagController::MovementBehaviour()
{
	const auto VisibleEnemies = TagCharacter->GetVisibleEnemies();

	if (VisibleEnemies.Num() == 0)
	{
		GoToRandomWaypoint();
		return;
	}

	if (TagCharacter->Tagged)
	{
		TaggedMove(GetClosestEnemy(VisibleEnemies));
	}
	else
	{
		TaggedMove(FindFleeTarget(VisibleEnemies));
	}
}

void ATagController::TaggedMove(AActor * Goal)
{
	TSubclassOf<UNavigationQueryFilter> FilterClass = nullptr;

	if (TagCharacter->Tagged)
	{
		FilterClass = UNavFilter_Tagged::StaticClass();
	}

	MoveToActor(Goal, -1.0f, true, true, true, FilterClass, true);
}

AActor* ATagController::FindFleeTarget(const TArray<ATagCharacter*> Enemies) const
{
	const FVector EnemyAverageLocation = UGameplayStatics::GetActorArrayAverageLocation(static_cast<TArray<AActor*>>(Enemies));

	TArray<AActor*> FleePoints = { GetRandomWaypoint(), GetRandomWaypoint(), GetRandomWaypoint() };

	// Sort by the greatest distance to the enemy average location
	Algo::Sort(FleePoints, [&](const AActor* A, const AActor* B) {
		return FVector::DistSquared(A->GetActorLocation(), EnemyAverageLocation)
			> FVector::DistSquared(B->GetActorLocation(), EnemyAverageLocation);
	});

	return FleePoints[0];
}

AActor* ATagController::GetClosestEnemy(TArray<ATagCharacter*> VisibleEnemies) const
{
	auto TagCharacterLocation = TagCharacter->GetActorLocation();

	// Sort by the nearest enemy
	Algo::Sort(VisibleEnemies, [&](const AActor* A, const AActor* B) {
		return FVector::DistSquared(A->GetActorLocation(), TagCharacterLocation)
			< FVector::DistSquared(B->GetActorLocation(), TagCharacterLocation);
	});

	return VisibleEnemies[0];
}
