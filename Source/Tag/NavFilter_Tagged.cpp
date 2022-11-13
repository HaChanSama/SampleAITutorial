// Fill out your copyright notice in the Description page of Project Settings.

#include "NavFilter_Tagged.h"
#include "NavArea_Launchpad.h"



UNavFilter_Tagged::UNavFilter_Tagged()
{
	FNavigationFilterArea FilterArea { };
	FilterArea.AreaClass = UNavArea_Launchpad::StaticClass();
	FilterArea.bIsExcluded = true;
	Areas.Add(FilterArea);
}
