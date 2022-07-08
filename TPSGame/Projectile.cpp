// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Shooter.h"
#include "TPSGame.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

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
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,this,&AProjectile::Suicide,MaxLifeTime);
	// 충돌
	SphereBox->OnComponentBeginOverlap.AddDynamic(this,&AProjectile::SphereBoxBeginOverlap);
	SphereBox->OnComponentHit.AddDynamic(this,&AProjectile::SphereBoxHit);
	SphereBox->BodyInstance.SetCollisionProfileName(CollisionProfileName);
}

void AProjectile::Suicide()
{
	Destroy();
}

void AProjectile::SphereBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ProjectileType == EProjectileType::EPT_ShooterBase || ProjectileType == EProjectileType::EPT_ShooterUlt)
	{
		AShooter* Shooter = Cast<AShooter>(OtherActor);
		if(Shooter) return;

		Shooter = Cast<AShooter>(UGameplayStatics::GetPlayerPawn(this,0));
		if(Shooter == nullptr) return;
		
		FTransform Transform = FTransform(Shooter->GetBaseAimRotation(),GetActorLocation(),GetActorScale3D());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Impacts[0],Transform);

		Destroy();
	}
}

void AProjectile::SphereBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(ProjectileType == EProjectileType::EPT_ShooterBase || ProjectileType == EProjectileType::EPT_ShooterUlt)
	{
		AShooter* Shooter = Cast<AShooter>(UGameplayStatics::GetPlayerPawn(this,0));
		if(Shooter == nullptr) return;
		
		if(UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get()) == EPS_Water)
		{
			FTransform Transform = FTransform(Shooter->GetBaseAimRotation(),Hit.Location,GetActorScale3D());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Impacts[2],Transform);
		}
		else
		{
			FTransform Transform = FTransform(Shooter->GetBaseAimRotation(),Hit.Location,GetActorScale3D());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Impacts[1],Transform);	
		}

		Destroy();
	}
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

