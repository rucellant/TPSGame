// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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

	if(InventoryHUDOverlayClass)
	{
		InventoryHUDOverlay = CreateWidget(this,InventoryHUDOverlayClass);
		if(InventoryHUDOverlay)
		{
			InventoryHUDOverlay->AddToViewport();
			InventoryHUDOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// 게임포즈
	bPauseGame = false;
}

void AShooterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 포즈일 때도 입력 가능하게 하기 -> .bExecuteWhenPaused = true;
	InputComponent->BindAction(FName("InventoryButton"),EInputEvent::IE_Pressed,this,&AShooterController::InventoryButtonPressed);
	InputComponent->BindAction(FName("InventoryButton"),EInputEvent::IE_Released,this,&AShooterController::InventoryButtonReleased);
}

void AShooterController::TogglePauseGame()
{
	bPauseGame = !bPauseGame;
	
	ShowInventory();
}

void AShooterController::ShowInventory_Implementation()
{
	if(bPauseGame)
	{
		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);
		
		SetShowMouseCursor(true);
	}
	else
	{
		FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		
		SetShowMouseCursor(false);
	}
}

void AShooterController::InventoryButtonPressed()
{
	bInventoryButtonPressed = true;

	TogglePauseGame();
}

void AShooterController::InventoryButtonReleased()
{
	bInventoryButtonPressed = false;
}
