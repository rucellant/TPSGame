// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterController.generated.h"

/**
 * 
 */
UCLASS()
class TPSGAME_API AShooterController : public APlayerController
{
	GENERATED_BODY()
public:
	AShooterController();

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	// 게임 포즈 토글
	void TogglePauseGame();
	// 인벤토리 위젯 보여주고 안 보여주고
	UFUNCTION(BlueprintNativeEvent)
	void ShowInventory();
	virtual void ShowInventory_Implementation();
protected:
	void InventoryButtonPressed();
	void InventoryButtonReleased();
private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> StaticHUDOverlayClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	UUserWidget* StaticHUDOverlay;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> DynamicHUDOverlayClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	UUserWidget* DynamicHUDOverlay;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> InventoryHUDOverlayClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	UUserWidget* InventoryHUDOverlay;
private:
	// 인벤토리 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Control",meta=(AllowPrivateAccess="true"))
	bool bInventoryActivated;
	bool bInventoryButtonPressed;
	// 게임 포즈
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	bool bPauseGame;
};
