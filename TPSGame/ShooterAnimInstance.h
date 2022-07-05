// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shooter.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TPSGAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UShooterAnimInstance();
	// NativeInitializeAnimation먼저 수행되고 NativeBeginPlay수행됨.
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	void UpdateAnimationProperties(float DeltaTime);

	void Lean(float DeltaTime);
	void MovementYaw();
private:
	// 애님인스턴스 소유 객체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	AShooter* Shooter;
	// 슈터 스테이트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	EShooterState ShooterState;
	// 조준
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	bool bAiming;
	// 속력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	float Speed;
	// 점프
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;
	// 가속력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	// Lean
	FRotator CurCharacterRotation;
	FRotator PreCharacterRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	float LeaningRate;
	// MovementYaw
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	float CurMovementOffsetYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shooter", meta = (AllowPrivateAccess = "true"))
	float PreMovementOffsetYaw;
};
