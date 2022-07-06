// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SphereComponent
	SphereBox = CreateDefaultSubobject<USphereComponent>(TEXT("SphereBox"));
	SetRootComponent(SphereBox);

	// 무브먼트 컴포넌트
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(GetRootComponent());

	// 총알
	BulletParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletParticle"));
	BulletParticleSystemComponent->AttachToComponent(SphereBox,FAttachmentTransformRules::KeepRelativeTransform);
	BulletParticleSystemComponent->bAutoActivate = true;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if(ProjectileDataTable)
	{
		FProjectileDataTable* ProjectileDataTableRow = ProjectileDataTable->FindRow<FProjectileDataTable>(DataTableRowName, TEXT(""));

		if(ProjectileDataTableRow)
		{
			ProjectileType = ProjectileDataTableRow->ProjectileType;

			ProjectileSpeed = ProjectileDataTableRow->ProjectileSpeed;
			
			SphereBoxRadius = ProjectileDataTableRow->SphereBoxRadius;
			
			MaxLifeTime = ProjectileDataTableRow->MaxLifeTime;

			Damage = ProjectileDataTableRow->Damage;

			Impacts = ProjectileDataTableRow->Impacts;

			MuzzleFlashes = ProjectileDataTableRow->MuzzleFlashes;

			BulletParticleSystemAsset = ProjectileDataTableRow->BulletParticleSystemAsset;
		}
	}
	
	// SphereBox
	SphereBox->InitSphereRadius(SphereBoxRadius);
	// ProjectileMovement
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	FVector Velocity = ProjectileMovement->Velocity;
	Velocity.Normalize();
	ProjectileMovement->Velocity = Velocity * ProjectileSpeed;
	// BulletParticle
	BulletParticleSystemComponent->SetTemplate(BulletParticleSystemAsset);
	// 타이머 함수 호출
	GetWorldTimerManager().SetTimer(TimerHandle,this,&AProjectile::Suicide,MaxLifeTime);
}

void AProjectile::Suicide()
{
	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::SpawnMuzzleFlash(FTransform BarrelSocketTransform)
{
	for(auto& MuzzleFlash : MuzzleFlashes)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),MuzzleFlash,BarrelSocketTransform);	
	}
	
}

