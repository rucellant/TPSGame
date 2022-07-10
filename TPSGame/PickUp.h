// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "PickUp.generated.h"

USTRUCT(BlueprintType)
struct FPickUpDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FLinearColor PointLightColor;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName PickUpName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture* ItemImage;
};
/**
 * 
 */
UCLASS()
class TPSGAME_API APickUp : public AItem
{
	GENERATED_BODY()
public:
	APickUp();

	virtual void TransformItemState(EItemState State) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void SphereBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	// 데이터 테이블
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	UDataTable* PickUpDataTable;
	// 데이터 테이블 Row이름
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	TArray<FName> DataTableRowNames;
	// 머티리얼 인스턴스
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	UMaterialInstance* MaterialInstance;
	// 메시
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PickUpMesh;
	// 충돌용 스피어
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	USphereComponent* SphereBox;
	// 라이트
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	UPointLightComponent* PointLight;
	// 라이트 컬러
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	FLinearColor PointLightColor;
	// 포션 이름
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	UWidgetComponent* PickUpNameWidget;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	FName PickUpName;
private:
	// 회복량
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="PickUp",meta=(AllowPrivateAccess="true"))
	float IncrementRate;
};
