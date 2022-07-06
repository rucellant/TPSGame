// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	EPT_ShooterBase UMETA(DisplayNAme = "ShooterBase"),
	EPT_ShooterUlt UMETA(DisplayNAme = "ShooterUlt"),
	
	EMT_MAX UMETA(DisplayNAme = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FProjectileDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EProjectileType ProjectileType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ProjectileSpeed;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float SphereBoxRadius;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxLifeTime;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UParticleSystem* BulletParticleSystemAsset;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UParticleSystem*> Impacts;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UParticleSystem*> MuzzleFlashes;
};

UCLASS()
class TPSGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Suicide();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnMuzzleFlash(FTransform BarrelSocketTransform);
private:
	// 총알 종류
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	EProjectileType ProjectileType;
	// 데이터 테이블
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	UDataTable* ProjectileDataTable;
	// 데이터 테이블 Row이름
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	FName DataTableRowName;
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	USphereComponent* SphereBox;
	// 무브먼트 컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	UProjectileMovementComponent* ProjectileMovement;
	// 총알 파티클 에셋
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	UParticleSystem* BulletParticleSystemAsset;
	// 총알 파티클 컴포넌트
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	UParticleSystemComponent* BulletParticleSystemComponent;
	// 어디 부딪혔을 때 생성하는 파티클
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Projectile",meta=(AllowPrivateAccess="true"))
	TArray<UParticleSystem*> Impacts;
	// 머즐 플래쉬
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	TArray<UParticleSystem*> MuzzleFlashes;
	// 총알 속도
	float ProjectileSpeed;
	// 충돌체 크기
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	float SphereBoxRadius;
	// 생명 주기
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	float MaxLifeTime;
	// 데미지
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Projectile",meta=(AllowPrivateAccess="true"))
	float Damage;
	// 타이머핸들
	FTimerHandle TimerHandle;
};
