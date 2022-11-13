// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavLinkComponent.h"
#include "BlueprintableNavLinkComponent.generated.h"

/**
 *
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class TAG_API UBlueprintableNavLinkComponent : public UNavLinkComponent
{
	GENERATED_BODY()


		UFUNCTION(BlueprintCallable)
		void SetupLink(FVector Left, FVector Right, ENavLinkDirection::Type Direction);

};
