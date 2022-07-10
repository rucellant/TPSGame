// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"

#include "Shooter.h"

APickUp::APickUp()
{
	// SphereBox
	SphereBox = CreateDefaultSubobject<USphereComponent>(TEXT("SphereBox"));
	SetRootComponent(SphereBox);
	// PickUpMesh 
	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	PickUpMesh->SetupAttachment(GetRootComponent());
	// 포인트라이트
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(PickUpMesh);
	// 위젯
	PickUpNameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpNameWidget"));
	PickUpNameWidget->SetupAttachment(GetRootComponent());
}

void APickUp::TransformItemState(EItemState State)
{
	Super::TransformItemState(State);

	if(State == EItemState::EIS_Inventory)
	{
		SphereBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PickUpMesh->SetVisibility(false);
		PointLight->SetVisibility(false);
		PickUpNameWidget->SetVisibility(false);
	}

	else if(State == EItemState::EIS_Used)
	{
		
	}

	else if(State == EItemState::EIS_PickUp)
	{
		
	}
}

void APickUp::BeginPlay()
{
	Super::BeginPlay();

	if(PickUpDataTable)
	{
		int32 DataTableRowIndex = FMath::RandRange(0,DataTableRowNames.Num() - 1);
		FPickUpDataTable* PickUpDataTableRow = PickUpDataTable->FindRow<FPickUpDataTable>(DataTableRowNames[DataTableRowIndex], TEXT(""));

		if(PickUpDataTableRow)
		{
			ItemType = PickUpDataTableRow->ItemType;

			MaterialInstance = PickUpDataTableRow->MaterialInstance;

			PointLightColor = PickUpDataTableRow->PointLightColor;

			PickUpName = PickUpDataTableRow->PickUpName;

			ItemImage = PickUpDataTableRow->ItemImage;
		}
	}

	// 델리게이트
	SphereBox->OnComponentBeginOverlap.AddDynamic(this,&APickUp::SphereBoxBeginOverlap);

	// 타입에 따른 머티리얼
	PickUpMesh->SetMaterial(0,MaterialInstance);

	// 포인트라이트 색
	PointLight->SetLightColor(PointLightColor);
	
	// 픽업 위젯
	PickUpNameWidget->SetVisibility(true);
}

void APickUp::SphereBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooter* Shooter = Cast<AShooter>(OtherActor);
	if(Shooter == nullptr) return;

	bool bShouldDestroy;
	if(Shooter->EquipItem(this,bShouldDestroy))
	{
		if(bShouldDestroy)
		{
			Destroy();
		}
		else
		{
			TransformItemState(EItemState::EIS_Inventory);	
		}
	}
}

void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
