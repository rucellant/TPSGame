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

void APickUp::BeginPlay()
{
	Super::BeginPlay();

	if(PickUpDataTable)
	{
		FPickUpDataTable* PickUpDataTableRow = PickUpDataTable->FindRow<FPickUpDataTable>(DataTableRowName, TEXT(""));

		if(PickUpDataTableRow)
		{
			PickUpType = PickUpDataTableRow->PickUpType;

			MaterialInstance = PickUpDataTableRow->MaterialInstance;

			PointLightColor = PickUpDataTableRow->PointLightColor;

			PickUpName = PickUpDataTableRow->PickUpName;
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

	switch (PickUpType)
	{
	case EPickUpType::EPT_HealthPotion:
		Shooter->IncrementHealthStepByStep(IncrementRate);
		break;
	case EPickUpType::EPT_ShieldPotion:
		Shooter->IncrementShieldStepByStep(IncrementRate);
		break;
	case EPickUpType::EPT_BothPotion:
		Shooter->IncrementHealth(IncrementRate);
		Shooter->IncrementShield(IncrementRate);
		break;
	default:
		break;
	}
	
	Destroy();
}

void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
