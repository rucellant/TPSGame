// Fill out your copyright notice in the Description page of Project Settings.


#include "Shooter.h"
#include "MeshPassProcessor.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AShooter::AShooter():
bAiming(false),
bFireButtonPressed(false),
bAimingButtonPressed(false),
bUltButtonPressed(false),
bUltActivated(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 카메라붐 컴포넌트
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	
	// 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	// 파티클
	GunDashbardParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GunDashbardParticleSystem"));
	GunDashbardParticleSystemComponent->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,FName("GunDashboardSocket"));
	GunDashbardParticleSystemComponent->SetAutoActivate(false);
	
	BodyHologramParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BodyHologramParticleSystem"));
	BodyHologramParticleSystemComponent->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,FName("BodyHologramSocket"));
	BodyHologramParticleSystemComponent->SetAutoActivate(false);

	BodyHologramLeftoverParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BodyHologramLeftoverParticleSystem"));
	BodyHologramLeftoverParticleSystemComponent->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,FName("BodyHologramLeftoverSocket"));
	BodyHologramLeftoverParticleSystemComponent->SetAutoActivate(false);
}

// Called when the game starts or when spawned
void AShooter::BeginPlay()
{
	Super::BeginPlay();
	
	// 카메라붐
	CameraBoom->TargetArmLength = NonAimingTargetArmLength;
	CameraBoom->SocketOffset = NonAimingCameraBoomSocketOffset;

	// 에이밍 모드에 따른 변환을 초기화
	TransformByAiming(bAiming);

	// 체력 / 쉴드
	CurHealth = MaxHealth;
	CurShield = 0.f;
	TargetHealth = 0.f;
	TargetShield = 0.f;
	bHealthRecovery = false;
	bShieldRecovery = false;
	// 궁극기 게이지
	CurUltGauge = 0.f;
	// 컨트롤러
	ShooterController = Cast<AShooterController>(GetController());
	// 인벤토리
	for(int32 i=0;i<INVENTORY_CAPACITY;i++) Inventory.Add(nullptr);
	InventoryItemNum = 0;
}

void AShooter::TransformByAiming(bool bInput)
{
	// Aiming 토글
	bAiming = bInput;

	if(bAiming) // 조준으로 변경
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		
		bUseControllerRotationYaw = true;
		bUseControllerRotationRoll = false;
		bUseControllerRotationPitch = false;
		
		BaseTargetArmLength = AimingTargetArmLength;
		
		BaseCameraBoomSocketOffset = AimingCameraBoomSocketOffset;
		
		BaseLookUpRate = AimingLookUpRate;
		BaseLookRightRate = AimingLookRightRate;
	}
	else		// 비조준으로 변경
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
		bUseControllerRotationPitch = false;
		
		BaseTargetArmLength = NonAimingTargetArmLength;
		BaseCameraBoomSocketOffset = NonAimingCameraBoomSocketOffset;

		BaseLookUpRate = NonAimingLookUpRate;
		BaseLookRightRate = NonAimingLookRightRate;
	}
}

void AShooter::TickCameraBoom(float DeltaTime)
{
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength,BaseTargetArmLength,DeltaTime,6.f);
	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset,BaseCameraBoomSocketOffset,DeltaTime,6.f);
}

void AShooter::FireWeapon()
{
	if(!bAiming) return;
	
	bFiringWeapon = true;
	
	// 애니메이션 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(FireWeaponMontage && AnimInstance)
	{
		if(bUltActivated)
		{
			AnimInstance->Montage_Play(FireWeaponMontage);
			AnimInstance->Montage_JumpToSection(FireWeaponMontageUltSection,FireWeaponMontage);
		}
		else
		{
			AnimInstance->Montage_Play(FireWeaponMontage);
			AnimInstance->Montage_JumpToSection(FireWeaponMontageBaseSection,FireWeaponMontage);
		}
	}

	// 레이트레이싱
	FHitResult HitResult;
	FVector HitLocation;
	if(LineTraceFromScreen(HitResult,HitLocation))
	{
		const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName(BarrelSocketName);
		FTransform BarrelSocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		FVector BarrelLocation = BarrelSocketTransform.GetLocation();
		LineTraceFromWorld(BarrelLocation,HitLocation,HitResult,HitLocation);

		// 총알 사출
		FVector Start = BarrelLocation;
		FVector End = HitLocation;
		FVector ProjectileDirection = End - Start; ProjectileDirection.Normalize();
		FRotator ProjectileRotation = UKismetMathLibrary::MakeRotFromX(ProjectileDirection);
		if(bUltActivated)
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(UltProjectileClass,Start,ProjectileRotation);
			// 총구 화염
			Projectile->SpawnMuzzleFlash(BarrelSocketTransform);
		}
		else
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BaseProjectileClass,Start,ProjectileRotation);
			// 총구 화염
			Projectile->SpawnMuzzleFlash(BarrelSocketTransform);
		}
	}
	else
	{
		return;
	}
	
	// 타이머 호출
	if(bUltActivated)
	{
		GetWorldTimerManager().SetTimer(AutoFireDelayTimer,this,&AShooter::AutoFireWeapon,AutoFireUltDelayRate);	
	}
	else
	{
		GetWorldTimerManager().SetTimer(AutoFireDelayTimer,this,&AShooter::AutoFireWeapon,AutoFireBaseDelayRate);
	}
}

void AShooter::AutoFireWeapon()
{
	if(!bFireButtonPressed)
	{
		bFiringWeapon = false;
		return;
	}
	FireWeapon();
}

void AShooter::CalculateCrosshairSpread(float DeltaTime)
{
	// 발사 + 이동 + 점프 + 기본

	// 이동
	const FVector2D WalkSpeedRange = FVector2D(0.f,GetCharacterMovement()->MaxWalkSpeed);
	const FVector2D VelocityMultiflierRange = FVector2D(0.f,CrosshairMaxVelocityFactor);
	FVector Velocity = GetVelocity(); Velocity.Z = 0.f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange,VelocityMultiflierRange,Velocity.Size());

	// 점프
	if(GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,CrosshairMaxInAirFactor,DeltaTime,2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor,0,DeltaTime,30.f);
	}

	// 발사
	if(bFiringWeapon)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor,CrosshairMaxShootingFactor,DeltaTime,60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor,0,DeltaTime,60.f);
	}

	CrosshairSpreadMultiplier = CrosshairBaseFactor +
		CrosshairVelocityFactor +
			CrosshairInAirFactor +
				CrosshairShootingFactor;
}

bool AShooter::LineTraceFromScreen(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	FVector2D ViewportSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation = FVector2D(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f) + CrosshairOffset;
	FVector CrosshairWorldLocation, CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this,0),
		CrosshairLocation,
		CrosshairWorldLocation,
		CrosshairWorldDirection);
	
	if(!bScreenToWorld) return false;
	
	FVector Start = CrosshairWorldLocation;
	FVector End = Start + CrosshairWorldDirection * 50'000.f;
	GetWorld()->LineTraceSingleByChannel(OutHitResult,Start,End,ECollisionChannel::ECC_Visibility);

	if(OutHitResult.bBlockingHit)
	{
		OutHitLocation = OutHitResult.Location;
		return true;
	}
	else
	{
		OutHitLocation = End;
		return true;
	}
}

void AShooter::LineTraceFromWorld(FVector InStart, FVector InTarget, FHitResult& OutHitResult, FVector& OutHitLocation)
{
	FVector Start = InStart;
	FVector Direction = InTarget - Start;
	FVector End = Start + Direction * 1.25f; 
	GetWorld()->LineTraceSingleByChannel(OutHitResult,Start,End,ECollisionChannel::ECC_Visibility);

	if(OutHitResult.bBlockingHit)
	{
		OutHitLocation = OutHitResult.Location;
	}
	else
	{
		OutHitLocation = End;
	}
}

void AShooter::TickUltGauge(float DeltaTime)
{
	if(!bUltActivated)
	{
		if(CurUltGauge >= MaxUltGauge) return;

		CurUltGauge += (DeltaTime * (MaxUltGauge / UltGageFactor));
	}
	else
	{
		if(CurUltGauge <= 0.f)
		{
			bUltActivated = !bUltActivated;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if(AnimInstance && ActivateUltMontage)
			{
				AnimInstance->Montage_Play(ActivateUltMontage);
				AnimInstance->Montage_JumpToSection(DeactivateUltMontageSection,ActivateUltMontage);
			}

			GunDashbardParticleSystemComponent->Deactivate();
			BodyHologramParticleSystemComponent->Deactivate();
		}
		else
		{
			CurUltGauge -= (DeltaTime * (MaxUltGauge / UltGageFactor));
		}
	}
}

void AShooter::ActivateUlt()
{
	if(CurUltGauge < MaxUltGauge) return;
	
	bUltActivated = true;

	// 애니메이션 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(ActivateUltMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(ActivateUltMontage);
		AnimInstance->Montage_JumpToSection(ActivateUltMontageSection,ActivateUltMontage);
	}
	
	GunDashbardParticleSystemComponent->Activate();
	BodyHologramParticleSystemComponent->Activate();
	FTimerHandle BodyHologramLeftoverTimer;
	GetWorldTimerManager().SetTimer(BodyHologramLeftoverTimer,this,&AShooter::BodyHologramLeftoverActiavte,1.0);
}

void AShooter::BodyHologramLeftoverActiavte()
{
	BodyHologramLeftoverParticleSystemComponent->Activate();
	FTimerHandle BodyHologramLeftoverTimer;
	GetWorldTimerManager().SetTimer(BodyHologramLeftoverTimer,this,&AShooter::BodyHologramLeftoverDeactiavte,1.0);
}

void AShooter::BodyHologramLeftoverDeactiavte()
{
	BodyHologramLeftoverParticleSystemComponent->Deactivate();
}

void AShooter::TickHealthRecovery(float DeltaTime)
{
	if(!bHealthRecovery) return;

	CurHealth = FMath::FInterpTo(CurHealth,TargetHealth,DeltaTime,2.f);

	if(CurHealth >= TargetHealth)
	{
		TargetHealth = 0.f;
		bHealthRecovery = false;
	}
}

void AShooter::TickShieldRecovery(float DeltaTime)
{
	if(!bShieldRecovery) return;

	CurShield = FMath::FInterpTo(CurShield,TargetShield,DeltaTime,2.f);

	if(CurShield >= TargetShield)
	{
		TargetShield = 0.f;
		bShieldRecovery = false;
	}
}

bool AShooter::CanWork()
{
	if(ShooterController->GetPause())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void AShooter::MoveForward(float Value)
{
	if(Value == 0.f || !CanWork()) return;

	if(bAiming)
	{
		const FRotator Rotation = FRotator(0.f,GetActorRotation().Yaw,0.f);
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
			
		AddMovementInput(Direction,Value);
	}
	else
	{
		const FRotator Rotation = FRotator(0.f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction,Value);
	}
}

void AShooter::MoveRight(float Value)
{
	if(Value == 0.f || !CanWork()) return;

	if(bAiming)
	{
		const FRotator Rotation = FRotator(0.f,GetActorRotation().Yaw,0.f);
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
			
		AddMovementInput(Direction,Value);
	}
	else
	{
		const FRotator Rotation = FRotator(0.f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction,Value);
	}
}

void AShooter::LookRight(float Value)
{
	if(Value == 0.f || !CanWork()) return;
	
	AddControllerYawInput(BaseLookRightRate * Value * GetWorld()->GetDeltaSeconds());
}

void AShooter::LookUp(float Value)
{
	if(Value == 0.f || !CanWork()) return;
	
	AddControllerPitchInput(BaseLookUpRate * Value * GetWorld()->GetDeltaSeconds());
}

void AShooter::FireButtonPressed()
{
	if(!CanWork()) return;
	
	bFireButtonPressed = true;

	if(bAiming)
	{
		FireWeapon();	
	}
}

void AShooter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooter::AimingButtonPressed()
{
	if(!CanWork()) return;
	
	bAimingButtonPressed = true;

	TransformByAiming(!bAiming);
}

void AShooter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
}

void AShooter::UltButtonPressed()
{
	if(!CanWork()) return;
	
	bUltButtonPressed = true;

	ActivateUlt();
}

void AShooter::UltButtonReleased()
{
	bUltButtonPressed = false;
}

void AShooter::Jump()
{
	if(!CanWork()) return;
	
	Super::Jump();

	TransformByAiming(false);
}

void AShooter::StopJumping()
{
	Super::StopJumping();
}

// Called every frame
void AShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 카메라붐
	TickCameraBoom(DeltaTime);
	// 크로스헤어
	CalculateCrosshairSpread(DeltaTime);
	// 궁극기 게이지
	TickUltGauge(DeltaTime);
	// 체력 회복
	TickHealthRecovery(DeltaTime);
	// 쉴드 회복
	TickShieldRecovery(DeltaTime);
}

// Called to bind functionality to input
void AShooter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"),this,&AShooter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"),this,&AShooter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"),this,&AShooter::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRight"),this,&AShooter::LookRight);

	PlayerInputComponent->BindAction(FName("FireButton"),EInputEvent::IE_Pressed,this,&AShooter::FireButtonPressed);
	PlayerInputComponent->BindAction(FName("FireButton"),EInputEvent::IE_Released,this,&AShooter::FireButtonReleased);

	PlayerInputComponent->BindAction(FName("AimingButton"),EInputEvent::IE_Pressed,this,&AShooter::AimingButtonPressed);
	PlayerInputComponent->BindAction(FName("AimingButton"),EInputEvent::IE_Released,this,&AShooter::AimingButtonReleased);

	PlayerInputComponent->BindAction(FName("UltButton"),EInputEvent::IE_Pressed,this,&AShooter::UltButtonPressed);
	PlayerInputComponent->BindAction(FName("UltButton"),EInputEvent::IE_Released,this,&AShooter::UltButtonReleased);
	
	PlayerInputComponent->BindAction(FName("Jump"),EInputEvent::IE_Pressed,this,&AShooter::Jump);
	PlayerInputComponent->BindAction(FName("Jump"),EInputEvent::IE_Released,this,&AShooter::StopJumping);
}

void AShooter::SpawnVentEmitter()
{
	const USkeletalMeshSocket* RightVentEmitterSocket = GetMesh()->GetSocketByName(FName("RightVentEmitterSocket"));

	FTransform RightVentEmitterTransform = RightVentEmitterSocket->GetSocketTransform(GetMesh());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),VentParticleSystem,RightVentEmitterTransform);

	const USkeletalMeshSocket* LeftVentEmitterSocket = GetMesh()->GetSocketByName(FName("LeftVentEmitterSocket"));

	FTransform LeftVentEmitterTransform = LeftVentEmitterSocket->GetSocketTransform(GetMesh());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),VentParticleSystem,LeftVentEmitterTransform);	
}

bool AShooter::EquipItem(AItem* Item, bool& OutShouldDestroy)
{
	if((InventoryItemNum >= INVENTORY_CAPACITY) || (Item == nullptr)) return false;

	// 1. 먼저 동일한 아이템이 인벤토리에 있는 지 확인
	for(int32 i=0;i<INVENTORY_CAPACITY;i++)
	{
		if((Inventory[i] != nullptr) && (Inventory[i]->GetItemType() == Item->GetItemType()))
		{
			Inventory[i]->IncrementItemCount(1);
			
			OutShouldDestroy = true;
			return true;
		}
	}
	
	for(int32 i=0;i<INVENTORY_CAPACITY;i++)
	{
		if(Inventory[i] == nullptr)
		{
			Inventory[i] = Item;
			InventoryItemNum += 1;
			
			// 여기서 위젯과 상호작용하는 함수 호출
			ShooterController->InventoryWidgetUpdate(i,Inventory[i]->GetItemImage(),Inventory[i]->GetItemCount());
			
			OutShouldDestroy = false;
			return true;
		}
	}

	return false;
}

void AShooter::IncrementHealthStepByStep(float Value)
{
	bHealthRecovery = true;
	TargetHealth = CurHealth + Value;
	if(TargetHealth >= MaxHealth)
	{
		TargetHealth = MaxHealth;
	}
}

void AShooter::IncrementShieldStepByStep(float Value)
{
	bShieldRecovery = true;
	TargetShield = CurShield + Value;
	if(TargetShield >= MaxShield)
	{
		TargetShield = MaxShield;
	}
}

