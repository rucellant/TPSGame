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
private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> StaticHUDOverlayClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	UUserWidget* StaticHUDOverlay;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> DynamicHUDOverlayClass;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Widget",meta=(AllowPrivateAccess="true"))
	UUserWidget* DynamicHUDOverlay;
};
