// Fill out your copyright notice in the Description page of Project Settings.

#include "TagCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "AIController.h"

// By default, the materials are null
UMaterialInterface* ATagCharacter::RedMaterial = nullptr;
UMaterialInterface* ATagCharacter::BlueMaterial = nullptr;

ATagCharacter::ATagCharacter()
{
	// We are not going to use on tick logic in our character if we can avoid it
	PrimaryActorTick.bCanEverTick = false;

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Volume"));
	InteractionVolume->SetupAttachment(RootComponent);

	FCollisionResponseContainer InteractionResponse{ ECR_Ignore };
	InteractionResponse.SetResponse(ECC_Pawn, ECR_Overlap);
	InteractionVolume->SetCollisionResponseToChannels(InteractionResponse);

	InteractionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATagCharacter::OnInteractionEnter);

	VisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Vision Volume"));
	VisionVolume->SetupAttachment(RootComponent);

	VisionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ATagCharacter::OnVisionEnter);

	// If the static material hasn't been loaded yet
	if (BlueMaterial == nullptr)
	{
		// Load the Material from our Content folder
		auto BlueMaterialLoader =
			ConstructorHelpers::FObjectFinderOptional<UMaterialInterface>(
				TEXT("/Game/TemplateContent/Mannequin/Character/Materials/MI_Body_Blue"));

		if (BlueMaterialLoader.Succeeded())
		{
			BlueMaterial = BlueMaterialLoader.Get();
		}
	}

	// If the static material hasn't been loaded yet
	if (RedMaterial == nullptr)
	{
		// Load the Material from our Content folder
		auto RedMaterialLoader =
			ConstructorHelpers::FObjectFinderOptional<UMaterialInterface>(
				TEXT("/Game/TemplateContent/Mannequin/Character/Materials/MI_Body_Red"));

		if (RedMaterialLoader.Succeeded())
		{
			RedMaterial = RedMaterialLoader.Get();
		}
	}
}

void ATagCharacter::ChangeTaggedStatus(bool NewStatus)
{
	Tagged = NewStatus;

	// Select the correct material using the Ternary operator
	auto Material = Tagged ? RedMaterial : BlueMaterial;

	// If we have a mesh
	auto Mesh = GetMesh();
	if (Mesh)
	{
		// Apply the material
		Mesh->SetMaterial(0, Material);
	}

	// Select the Walk Speed using the Ternary operator
	auto MaxWalkSpeed = Tagged ? 660.f : 600.f;

	// Apply the walk speed
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

	// If we're in-game, stop moving!
	OnTagged.Broadcast();
}

void ATagCharacter::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	ChangeTaggedStatus(Tagged);
}

const TArray<ATagCharacter*> ATagCharacter::GetVisibleEnemies() const
{
	static TArray<AActor*> VisibleCharacters;
	VisionVolume->GetOverlappingActors(VisibleCharacters, ATagCharacter::StaticClass());

	TArray<ATagCharacter*> VisibleEnemies;

	for (auto Character : VisibleCharacters)
	{
		if (Character == this)
			continue;

		auto TagCharacter = Cast<ATagCharacter>(Character);

		// If they don't have the same tag as me, and I can see them
		if (TagCharacter->Tagged != Tagged && CanSeeEnemy(TagCharacter))
		{
			VisibleEnemies.Add(TagCharacter);
		}
	}

	return VisibleEnemies;
}

bool ATagCharacter::CanSeeEnemy(ATagCharacter * Target) const
{
	// Raycast from my eyes
	FVector RayOrigin{ GetCapsuleComponent()->Bounds.Origin };
	RayOrigin.Z += BaseEyeHeight;

	// To the middle of their body
	FVector RayTarget{ Target->GetCapsuleComponent()->Bounds.Origin };

	static FHitResult HitResult(ForceInit);

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult, RayOrigin, RayTarget, ECC_Camera, params);

	return HitResult.Actor == Target;
}

void ATagCharacter::OnInteractionEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto Target = Cast<ATagCharacter>(OtherActor);

	if (Target)
	{
		if (Tagged && !Target->Tagged)
		{
			Target->ChangeTaggedStatus(true);
		}
	}
}

void ATagCharacter::OnVisionEnter(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	auto Target = Cast<ATagCharacter>(OtherActor);

	if (Target)
	{
		if (Tagged != Target->Tagged && CanSeeEnemy(Target))
		{
			OnNewEnemySeen.Broadcast();
		}
	}
}
