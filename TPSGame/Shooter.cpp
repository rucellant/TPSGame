// Fill out your copyright notice in the Description page of Project Settings.


#include "Shooter.h"
#include "MeshPassProcessor.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AShooter::AShooter():
bAiming(false),
bFireButtonPressed(false),
bAimingButtonPressed(false)
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
	bFiringWeapon = true;
	
	// 애니메이션 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(FireWeaponMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		AnimInstance->Montage_JumpToSection(FireWeaponMontageSection,FireWeaponMontage);
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
		FVector Start = BarrelSocketTransform.GetLocation();
		FVector End = HitLocation;
		FVector ProjectileDirection = End - Start; ProjectileDirection.Normalize();
		FRotator ProjectileRotation = UKismetMathLibrary::MakeRotFromX(ProjectileDirection);
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass,Start,ProjectileRotation);
		
		// 총구 화염
		Projectile->SpawnMuzzleFlash(BarrelSocketTransform);
	}
	else
	{
		return;
	}
	
	// 타이머 호출
	GetWorldTimerManager().SetTimer(AutoFireDelayTimer,this,&AShooter::AutoFireWeapon,AutoFireDelayRate);
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
	FVector End = CrosshairWorldLocation + CrosshairWorldDirection * 50'000.f;
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
		OutHitLocation = InTarget;
	}
}

void AShooter::MoveForward(float Value)
{
	if(Value == 0.f) return;

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
	if(Value == 0.f) return;

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
	AddControllerYawInput(BaseLookRightRate * Value * GetWorld()->GetDeltaSeconds());
}

void AShooter::LookUp(float Value)
{
	AddControllerPitchInput(BaseLookUpRate * Value * GetWorld()->GetDeltaSeconds());
}

void AShooter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooter::AimingButtonPressed()
{
	bAimingButtonPressed = true;

	TransformByAiming(!bAiming);
}

void AShooter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
}

void AShooter::Jump()
{
	Super::Jump();
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
	
	PlayerInputComponent->BindAction(FName("Jump"),EInputEvent::IE_Pressed,this,&AShooter::Jump);
	PlayerInputComponent->BindAction(FName("Jump"),EInputEvent::IE_Released,this,&AShooter::StopJumping);
}

