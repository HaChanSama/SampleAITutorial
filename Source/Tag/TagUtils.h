// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Runtime/Engine/Classes/Engine/Engine.h"

#define print(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(x));}
#define printC(x, colour) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, colour, TEXT(x));}
#define printD(x, duration) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Yellow, TEXT(x));}
#define printCD(x, colour, duration) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, duration, colour, TEXT(x));}

#define printS(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, x);}
#define printSC(x, colour) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, colour, x);}
#define printSD(x, duration) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, duration, FColor::Yellow, x);}
#define printSCD(x, colour, duration) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, duration, colour, x);}

#else

#define print(x) {}
#define printC(x, colour) {}
#define printCD(x, colour, duration) {}

#define printS(x) {}
#define printSC(x, colour) {}
#define printSCD(x, colour, duration) {}

#endif