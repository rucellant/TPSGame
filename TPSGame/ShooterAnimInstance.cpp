// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance()
{
}

void UShooterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Shooter = Cast<AShooter>(TryGetPawnOwner());
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateAnimationProperties(DeltaSeconds);
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if(Shooter == nullptr)
	{
		Shooter = Cast<AShooter>(TryGetPawnOwner());
	}

	if(Shooter != nullptr)
	{
		// 조준
		bAiming = Shooter->GetAiming();

		// 피치
		Pitch = Shooter->GetBaseAimRotation().Pitch;
		
		// 속도
		FVector Velocity = Shooter->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		// 점프
		bIsInAir = Shooter->GetCharacterMovement()->IsFalling();

		// 가속도
		bIsAccelerating = Shooter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

		// Lean
		Lean(DeltaTime);

		// MovementYaw
		MovementYaw();

		// TurnInPlace
		TurnInPlace();
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	PreCharacterRotation = CurCharacterRotation;
	CurCharacterRotation = Shooter->GetActorRotation();

	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurCharacterRotation,PreCharacterRotation);

	const float Target = DeltaRotation.Yaw / DeltaTime;

	LeaningRate = FMath::Clamp(FMath::FInterpTo(LeaningRate,Target,DeltaTime,6.f),-90.f,90.f);
}

void UShooterAnimInstance::MovementYaw()
{
	const FRotator AimRotation = Shooter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Shooter->GetVelocity());

	CurMovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation).Yaw;

	if(Shooter->GetVelocity().Size() > 0.f)
	{
		PreMovementOffsetYaw = CurMovementOffsetYaw;
	}
}

void UShooterAnimInstance::TurnInPlace()
{
	if(Speed > 0.f || bIsInAir || !bAiming)
	{
		RootYawOffset = 0.f;
		PreRotationCurve = 0.f;
		CurRotationCurve = 0.f;
		PreTIPCharacterRotation = CurTIPCharacterRotation;
		CurTIPCharacterRotation = Shooter->GetActorRotation();
	}
	else
	{
		PreTIPCharacterRotation = CurTIPCharacterRotation;
		CurTIPCharacterRotation = Shooter->GetActorRotation();
		const FRotator RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(CurTIPCharacterRotation,PreTIPCharacterRotation);

		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - RotationDelta.Yaw);
		
		const float Turning = GetCurveValue(FName("Turning"));
		if(Turning > 0.f)
		{
			bTurnInPlace = true;
			
			PreRotationCurve = CurRotationCurve;
			CurRotationCurve = GetCurveValue(FName("DistanceCurve"));
			const float RotationCurveDelta = CurRotationCurve - PreRotationCurve;

			RootYawOffset > 0.f ?  RootYawOffset -= RotationCurveDelta : RootYawOffset += RotationCurveDelta;

			const float ABSRootYawOffset = FMath::Abs(RootYawOffset);
			if(ABSRootYawOffset > 90.f)
			{
				const float YawExcess = ABSRootYawOffset - 90.f;
				RootYawOffset > 0.f ?  RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else
		{
			bTurnInPlace = false;
		}
	}
}

