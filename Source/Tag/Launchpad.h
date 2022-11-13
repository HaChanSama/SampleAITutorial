// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Launchpad.generated.h"

UCLASS()
class TAG_API ALaunchpad : public AActor
{
	GENERATED_BODY()

public:
	ALaunchpad();

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* Launchpad;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* TriggerVolume;

	// This component's details are automatically set in the Construction Script
	UPROPERTY(VisibleAnywhere)
		class UNavLinkComponent* NavLink;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget = ""))
		FVector Target;

	UFUNCTION(BlueprintPure)
		FVector CalculateLaunchVelocity(ACharacter* LaunchedCharacter);

	// When our Trigger Volume overlaps with something, this function gets called
	UFUNCTION()
		void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UFUNCTION()
		void LaunchCharacter(ACharacter* Character);

	// Whenever we edit the Actor Transform or any properties via the Details panel, this will trigger.
	void OnConstruction(const FTransform& Transform) override;

	// Update the Nav Link Component to link from the Mesh to the Target
	void UpdateNavLinks();
};
