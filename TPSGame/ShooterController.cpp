// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterController.h"

#include "Blueprint/UserWidget.h"

AShooterController::AShooterController()
{
}

void AShooterController::BeginPlay()
{
	Super::BeginPlay();

	if(StaticHUDOverlayClass)
	{
		StaticHUDOverlay = CreateWidget(this,StaticHUDOverlayClass);
		if(StaticHUDOverlay)
		{
			StaticHUDOverlay->AddToViewport();
			StaticHUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if(DynamicHUDOverlayClass)
	{
		DynamicHUDOverlay = CreateWidget(this,DynamicHUDOverlayClass);
		if(DynamicHUDOverlay)
		{
			DynamicHUDOverlay->AddToViewport();
			DynamicHUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
