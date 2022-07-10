// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_HealthPotion UMETA(DisplayNAme = "HealthPotion"),
	EIT_ShieldPotion UMETA(DisplayNAme = "ShieldPotion"),
	EIT_BothPotion UMETA(DisplayNAme = "BothPotion"),
	
	EIT_MAX UMETA(DisplayNAme = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_PickUp UMETA(DisplayName = "PickUp"),
	EIS_Inventory UMETA(DisplayName = "Inventory"),
	EIS_Used UMETA(DisplayName = "Used"),
	
	EIS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class TPSGAME_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// 아이템 상태 변환 함수
	virtual void TransformItemState(EItemState State);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:
	// 아이템 타입
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item",meta=(AllowPrivateAccess="true"))
	EItemType ItemType;
	// 아이템 상태
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item",meta=(AllowPrivateAccess="true"))
	EItemState ItemState;
	// 아이템 개수
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item",meta=(AllowPrivateAccess="true"))
	int32 ItemCount;
	// 아이템 인벤토리 텍스쳐
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Item",meta=(AllowPrivateAccess="true"))
	UTexture* ItemImage;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	FORCEINLINE void IncrementItemCount(int32 Count) { ItemCount += Count; }
	FORCEINLINE void DecrementItemCount(int32 Count) { ItemCount -= Count; }
	FORCEINLINE EItemType GetItemType() const { return ItemType; }
	FORCEINLINE UTexture* GetItemImage() const { return ItemImage; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
};
