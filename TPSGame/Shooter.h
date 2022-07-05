// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Shooter.generated.h"

UENUM(BlueprintType)
enum class EShooterState : uint8
{
	ESS_Idle UMETA(DisplayNAme = "Idle"),
	ESS_Jog UMETA(DisplayNAme = "Jog"),
	
	ESS_MAX UMETA(DisplayNAme = "DefaultMax")
};

UCLASS()
class TPSGAME_API AShooter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// bAiming에 따라 바뀌는 것들 설정
	void TransformByAiming(bool bInput);

	// 카메라붐
	void TickCameraBoom(float DeltaTime);

	// 총 발사
	void FireWeapon();
protected: // 입력
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRight(float Value);
	void LookUp(float Value);
	void FireButtonPressed();
	void FireButtonReleased();
	void AimingButtonPressed();
	void AimingButtonReleased();
	virtual void Jump() override;
	virtual void StopJumping() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 상태
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	EShooterState ShooterState;
	// 조준상태
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	bool bAiming;
	// 카메라붐
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	USpringArmComponent* CameraBoom;
	// 카메라
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UCameraComponent* FollowCamera;
	// 타겟암렝스
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float AimingTargetArmLength;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float NonAimingTargetArmLength;
	UPROPERTY(VisibleAnywhere,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float BaseTargetArmLength;
	// 타겟 소켓 오프셋
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FVector AimingCameraBoomSocketOffset;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FVector NonAimingCameraBoomSocketOffset;
	UPROPERTY(VisibleAnywhere,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FVector BaseCameraBoomSocketOffset;
	// 발사버튼이 눌렀으면 true, 떼어지면 false
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	bool bFireButtonPressed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	bool bAimingButtonPressed;
	// 마우스 회전 계수들
	UPROPERTY(VisibleAnywhere,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float BaseLookUpRate;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float AimingLookUpRate;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float NonAimingLookUpRate;
	UPROPERTY(VisibleAnywhere,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float BaseLookRightRate;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float AimingLookRightRate;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float NonAimingLookRightRate;
	// 몽타주들
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UAnimMontage* FireWeaponMontage;
public:
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE EShooterState GetShooterState() const { return ShooterState; }
};
