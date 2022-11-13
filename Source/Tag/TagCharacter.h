// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ShapeComponent.h"
#include "TagCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewEnemySeen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTagged);

UCLASS()
class TAG_API ATagCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATagCharacter();

	// Denotes the area that an "it" bot can "tag"
	UPROPERTY(VisibleAnywhere)
		UShapeComponent* InteractionVolume;

	// Denotes the area a bot can see
	UPROPERTY(VisibleAnywhere)
		UShapeComponent* VisionVolume;

	// Whether the bot is "it" or "not-it"
	// Do not edit directly in any code (use ChangeTaggedStatus)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool Tagged;

	// Sets the correct material and speed when changing Tag status
	UFUNCTION(BlueprintCallable)
		void ChangeTaggedStatus(bool NewStatus);

	UFUNCTION(BlueprintCallable)
		const TArray<ATagCharacter*> GetVisibleEnemies() const;

	UFUNCTION(BlueprintPure)
		bool CanSeeEnemy(ATagCharacter* Target) const;

	UFUNCTION(BlueprintCallable)
		void OnInteractionEnter(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void OnVisionEnter(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FNewEnemySeen OnNewEnemySeen;

	FTagged OnTagged;

	// Call ChangeTaggedStatus if required
	void OnConstruction(const FTransform& Transform) override;

private:
	// Using static material interfaces, 
	// we can ensure we only load each material once
	static UMaterialInterface* RedMaterial;
	static UMaterialInterface* BlueMaterial;
};
