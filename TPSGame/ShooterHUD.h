// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shooter.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

/**
 * 
 */
UCLASS()
class TPSGAME_API AShooterHUD : public AHUD
{
	GENERATED_BODY()
public:
	AShooterHUD();
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void DrawHUD() override;

	void DrawCrosshairs();
private:
	AShooter* Shooter;
private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Crosshairs",meta=(AllowPrivateAccess="true"))
	FVector2D CrosshairsSize;
	FVector2D CrosshairsLocation;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Crosshairs",meta=(AllowPrivateAccess="true"))
	TArray<UTexture*> CrosshairTextures;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Crosshairs",meta=(AllowPrivateAccess="true"))
	float CrosshairSpreadMax;
};
