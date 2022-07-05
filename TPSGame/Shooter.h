// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Shooter.generated.h"

UENUM(BlueprintType)
enum class EShooterState : uint8
{
	ESS_Idle UMETA(DisplayNAme = "Idle"),
	ESS_Jog UMETA(DisplayNAme = "Jog"),
	
	ESS_MAX UMETA(DisplayNAme = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FShooterDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 데이터 테이블
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Shooter",meta=(AllowPrivateAccess="true"))
	UDataTable* ShooterDataTable;
	// 상태
	EShooterState ShooterState;
	// 조준상태
	bool bAiming;
};
