// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Camera/CameraComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Shooter.generated.h"

// UENUM(BlueprintType)
// enum class EShooterState : uint8
// {
// 	ESS_Idle UMETA(DisplayNAme = "Idle"),
// 	ESS_Jog UMETA(DisplayNAme = "Jog"),
// 	ESS_FireWeapon UMETA(DisplayNAme = "FireWeapon"),
// 	
// 	ESS_MAX UMETA(DisplayNAme = "DefaultMax")
// };

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
	// 마우스 누르고 있으면 계속 발사
	void AutoFireWeapon();
	// 크로스헤어 벌어지는 정도 계산
	void CalculateCrosshairSpread(float DeltaTime);
	// 라인트레이스. 스크린에서 한번. 총구에서 한번. 이거는 일단 OutHitLocation를 구하면 성공
	bool LineTraceFromScreen(FHitResult& OutHitResult, FVector& OutHitLocation);
	void LineTraceFromWorld(FVector InStart, FVector InTarget, FHitResult& OutHitResult, FVector& OutHitLocation);
	void TickUltGauge(float DeltaTime);
	// 궁극기 활성화
	void ActivateUlt();
	// 타이머로 호출할 궁극기 이펙트 관련 함수들
	void BodyHologramLeftoverActiavte();
	void BodyHologramLeftoverDeactiavte();
protected: // 입력
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRight(float Value);
	void LookUp(float Value);
	void FireButtonPressed();
	void FireButtonReleased();
	void AimingButtonPressed();
	void AimingButtonReleased();
	void UltButtonPressed();
	void UltButtonReleased();
	virtual void Jump() override;
	virtual void StopJumping() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SpawnVentEmitter();
private:
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
	bool bFiringWeapon;
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
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FName FireWeaponMontageBaseSection;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FName FireWeaponMontageUltSection;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UAnimMontage* ActivateUltMontage;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FName ActivateUltMontageSection;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FName DeactivateUltMontageSection;
	// 타이머들
	FTimerHandle AutoFireDelayTimer;
	// 자동 발사 딜레이
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float AutoFireBaseDelayRate;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float AutoFireUltDelayRate;
	// 크로스헤어 기본 + 총쏠 때 + 이동할 때 + 점프할 때
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CrosshairSpreadMultiplier;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CrosshairBaseFactor;
	float CrosshairVelocityFactor;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CrosshairMaxVelocityFactor;
	float CrosshairInAirFactor;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CrosshairMaxInAirFactor;
	float CrosshairShootingFactor;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CrosshairMaxShootingFactor;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FVector2D CrosshairOffset;
	// 총구 소켓 이름
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	FName BarrelSocketName;
	// 총알
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	TSubclassOf<AProjectile> BaseProjectileClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Shooter",meta=(AllowPrivateAccess="true"))
	TSubclassOf<AProjectile> UltProjectileClass;
	// 체력
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CurHealth;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float MaxHealth;
	// 쉴드
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CurShield;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float MaxShield;
	// 궁극기
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float CurUltGauge;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float MaxUltGauge;
	// 궁극기 게이지 다 차는 시간, 초단위임 5 -> 5초만에 100프로
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	float UltGageFactor;
	bool bUltButtonPressed;
	bool bUltActivated;
	// 파티클들
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UParticleSystemComponent* GunDashbardParticleSystemComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UParticleSystemComponent* BodyHologramParticleSystemComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UParticleSystemComponent* BodyHologramLeftoverParticleSystemComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UParticleSystem* VentParticleSystem;
public:
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE FVector2D GetCrosshairOffset() const { return CrosshairOffset; }
	FORCEINLINE float GetCrosshairSpreadMultiplier() const { return CrosshairSpreadMultiplier; }
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Value) { CurHealth += Value; }
	UFUNCTION(BlueprintCallable)
	void DecrementHealth(float Value) { CurHealth -= Value; }
	UFUNCTION(BlueprintCallable)
	void IncrementShield(float Value) { CurShield += Value; }
	UFUNCTION(BlueprintCallable)
	void DecrementShield(float Value) { CurShield -= Value; }
	UFUNCTION(BlueprintCallable)
	float GetUltGauge() const { return CurUltGauge; }
	bool GetUltActivated() const { return bUltActivated; }
};
