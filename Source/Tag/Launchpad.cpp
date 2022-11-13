// Fill out your copyright notice in the Description page of Project Settings.

#include "Launchpad.h"
#include "TagUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavArea_Launchpad.h"
#include "NavigationSystem.h"
#include "NavLinkComponent.h"
#include "AI/NavigationSystemBase.h"
#include "TagCharacter.h"

ALaunchpad::ALaunchpad()
{
	// We don't need our Launchpad to Tick
	PrimaryActorTick.bCanEverTick = false;

	// Create our Components and setup default values
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Launchpad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Launchpad"));
	Launchpad->SetupAttachment(Root);
	Launchpad->SetRelativeLocation(FVector::ZeroVector);

	// Load the Launchpad Mesh from our Content folder
	static auto LaunchpadMesh = ConstructorHelpers::FObjectFinderOptional<UStaticMesh>(TEXT("/Game/Meshes/SM_Launchpad"));
	if (LaunchpadMesh.Succeeded())
	{
		Launchpad->SetStaticMesh(LaunchpadMesh.Get());
	}

	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Volume"));
	TriggerVolume->SetupAttachment(Launchpad);
	TriggerVolume->SetSphereRadius(25.f, true);

	NavLink = CreateDefaultSubobject<UNavLinkComponent>(TEXT("Nav Link"));
	NavLink->SetupAttachment(Root);

	// Set a default target to be slightly outside our mesh
	Target = { 100.f, 0.f, 0.f };

	// Bind so that when the trigger gets touched we get notified
	TriggerVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALaunchpad::OnTriggerBeginOverlap);
}

FVector ALaunchpad::CalculateLaunchVelocity(ACharacter* LaunchedCharacter)
{
	// Launch from the "feet" of the Character
	auto Start = LaunchedCharacter->GetActorLocation();
	Start.Z -= LaunchedCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// Launch to the Target in WS
	auto End = GetActorTransform().TransformPosition(Target);

	FVector Result;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, Result, Start, End);

	return Result;
}

void ALaunchpad::LaunchCharacter(ACharacter* Character)
{
	// Set the Velocity and Mode manually
	auto MovementComponent = Character->GetCharacterMovement();
	MovementComponent->Velocity = CalculateLaunchVelocity(Character);
	MovementComponent->SetMovementMode(EMovementMode::MOVE_Falling);
}

void ALaunchpad::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateNavLinks();
}

void ALaunchpad::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto TagCharacter = Cast<ATagCharacter>(Other);

	if (TagCharacter && OtherComp == TagCharacter->GetRootComponent() && !TagCharacter->Tagged)
	{
		LaunchCharacter(TagCharacter);
	}
}

void ALaunchpad::UpdateNavLinks()
{
	// We should never manually edit this actor's links.
	NavLink->SetRelativeLocation(FVector::ZeroVector);
	check(NavLink->Links.Num() == 1);

	// Setup link properties
	auto& Link = NavLink->Links[0];
	Link.Left = FVector::ZeroVector;
	Link.Right = Target;
	Link.Direction = ENavLinkDirection::LeftToRight;
	Link.SetAreaClass(UNavArea_Launchpad::StaticClass());

	// Force rebuild of local NavMesh
	auto World = GetWorld();
	if (World)
	{
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSystem)
		{
			NavSystem->UpdateComponentInNavOctree(*NavLink);
		}
	}
}
