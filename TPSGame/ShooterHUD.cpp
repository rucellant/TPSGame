// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"

#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"

AShooterHUD::AShooterHUD()
{
}

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();
	
	Shooter = Cast<AShooter>(UGameplayStatics::GetPlayerPawn(this,0));
}

void AShooterHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if(Shooter == nullptr)
	{
		Shooter = Cast<AShooter>(UGameplayStatics::GetPlayerPawn(this,0));
	}

	if(Shooter != nullptr)
	{
		DrawCrosshairs();
	}
}

void AShooterHUD::DrawCrosshairs()
{
	if(Shooter->GetAiming() == false) return;
	
	CrosshairsLocation = FVector2D(Canvas->ClipX * 0.5f,Canvas->ClipY * 0.5f) + Shooter->GetCrosshairOffset() -  CrosshairsSize * 0.5f;
	// 위
	FVector2D TopCrosshairLocation = CrosshairsLocation - FVector2D(0.f,Shooter->GetCrosshairSpreadMultiplier() * CrosshairSpreadMax); 
	DrawTexture(CrosshairTextures[0],TopCrosshairLocation.X,TopCrosshairLocation.Y,CrosshairsSize.X,CrosshairsSize.Y,0.f,0.f,1.f,1.f);
	// 오른
	FVector2D RightCrosshairLocation = CrosshairsLocation + FVector2D(Shooter->GetCrosshairSpreadMultiplier() * CrosshairSpreadMax,0.f); 
	DrawTexture(CrosshairTextures[1],RightCrosshairLocation.X,RightCrosshairLocation.Y,CrosshairsSize.X,CrosshairsSize.Y,0.f,0.f,1.f,1.f);
	// 아래
	FVector2D BottomCrosshairLocation = CrosshairsLocation + FVector2D(0.f,Shooter->GetCrosshairSpreadMultiplier() * CrosshairSpreadMax); 
	DrawTexture(CrosshairTextures[2],BottomCrosshairLocation.X,BottomCrosshairLocation.Y,CrosshairsSize.X,CrosshairsSize.Y,0.f,0.f,1.f,1.f);
	// 왼
	FVector2D LeftCrosshairLocation = CrosshairsLocation - FVector2D(Shooter->GetCrosshairSpreadMultiplier() * CrosshairSpreadMax,0.f); 
	DrawTexture(CrosshairTextures[3],LeftCrosshairLocation.X,LeftCrosshairLocation.Y,CrosshairsSize.X,CrosshairsSize.Y,0.f,0.f,1.f,1.f);
	// 중앙
	FVector2D CenterCrosshairLocation = CrosshairsLocation;
	DrawTexture(CrosshairTextures[4],CenterCrosshairLocation.X,CenterCrosshairLocation.Y,CrosshairsSize.X,CrosshairsSize.Y,0.f,0.f,1.f,1.f);
}
