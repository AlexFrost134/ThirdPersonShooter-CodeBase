// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Ammunition.h"
#include "AmmoDrop.h"
#include "Animation/AnimInstance.h"
#include "BulletHitInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "CustomLogs.h"
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Enemy.h"
#include "Explosiv.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "ShooterPlayerController.h"
#include "Spawner.h"
#include "Sound/SoundCue.h"
#include "StructHelper.h"
#include "StatsCalculator.h"
#include "Item.h"
#include "Weapon.h"	


// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Base rates for looking up / right
	BaseTurnRate = 50.f;
	BaseLookUpRate = 50.f;

	// True when aiming the weapon
	bAiming = false;
	bAimingButtonPressed = false;

	// Camera Field of View Values
	CameraDefaultFov = 0.f; // set in Begin Play
	ZoomedCameraFov = 40.f;
	ZoomInterpSpeed = 20.f;
	CurrentCameraFov = 0.f;
	ViewPitchMax = 30.f;
	ViewPitchMin = -50.f;
	// Camera Sensitivity for Controller Aiming / not Aiming
	
	ControllerAimingTurnRate = 20.f;
	ControllerAimingLookUpRate = 20.f;
	ControllerHipTurnRate = 90.f;
	ControllerHipLookUpRate = 90.f;

	// Caerma Sensitivity scale for Mouse Aiming / not Aiming
	MouseAimingTurnRate = 0.5f;
	MouseAimingLookUpRate = 0.5f;
	MouseHipLookUpRate = 1.f;
	MouseHipTurnRate = 1.f;

	// Crosshair Settings
	CrosshairSpreadMultiplier = 0.f;
	CrosshairBaseSpreadFactor = 1.f;
	CrosshairShootingFactor = 0.f;
	CrosshairAimFactor = 0.f;
	CrosshairInAirFactor = 0.f;
	CrosshairVelocitySpreadFactor = 0.f;
	CurrentCrosshairCrouchFactor = 0.f;
	//CrosshairCrouchBaseTargetValue = -1.f;
	CrosshairSpreadMax = 16.f;

	// Bullet fire timer variables
	bFiringBullet = false;
	ShootTimeDuration = 0.05f;

	// Automatic Fire Variables
	bFireButtonPressed = false;
	bAllowFire = true;

	// Jumping
	bCanJump = true;
	bAllowJumpInput = true;

	// Item
	bShouldTraceForItems = false;
	bInventoryIsFull = false;
	AllowExchangeWhileEquipping = true;
	AllowExchangeWhileAiming = true;
	QuotientOfChangeWhenSellingWeapon = 4.f;
	XPforNextLevelUp = 100.f;
	XPForNextLevelUpBase = XPforNextLevelUp;
	XpIncreasementForNextLevelUp = 10.f;
	//UI
	HighlightedSlot = -1;
	
	// Camera Interp Locations
	CameraInterpDistance = 250.f;
	CameraInterpElevation = 70.f;
	bShouldPlayPickupSound = true;
	bShouldPlayEquipSound = true;
	PickupSoundResetTime = 0.2;
	EquipSoundResetTime = 0.2;

	//Starting Ammunition
	StartingAmmount9mm = 0;
	StartingAmmountAssaultRifle = 0;
	StartingAmmountSMG = 0;

	// MontageSection
	HitReactionMontageSections = {	FName(TEXT("HitReactFront")),
									FName(TEXT("HitReactBack")),
									FName(TEXT("HitReactLeft")),
									FName(TEXT("HitReactRight"))};

	CharacterDeathMontageSections = {	FName(TEXT("Death_A")),
										FName(TEXT("Death_B")),
										FName(TEXT("Death_1")),
										FName(TEXT("Death_2")),
										FName(TEXT("Death_3"))};

	// Combat State
	CombatState = ECombatState::ECS_Unoccupied;

	// Reload Montage
	MagazineTransform = FTransform(ENoInit::NoInit);

	// Movement
	bAllowMovementInput = true;
	bCrouching = false;
	CrouchMovementSpeed = 400.f;
	TempCrouchMovementSpeed = CrouchMovementSpeed;
	BaseMovementSpeed = 700.f;
	TempBaseMovementSpeed = BaseMovementSpeed;
	CrouchJumpVelocity = 400.f;
	BaseJumpVelocity = 600.f;
	AimWalkMovementSpeed = 400.f;
	AimCrouchMovementSpeed = 233.f;
	StunMovementPenaltyFactor = 0.5f;

	// Capsule Height
	InterpCapsuleHalfHeight = 0.f;
	StandingCapsuleHalfHeight = 88.f;
	CrouchingCapsuleHalfHeight = 50.f;

	// Stats and HitPoints
	MaxHitPoints = 100.f;	
	StunResistance = 10.f;
	ArmorValuePercentage = 5.f;
	bCharacterIsDead = false;
	StartWeaponRarity = EItemRarity::EIR_Damaged;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a Camera SpringArm 
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f; //Distance between Camera und Character
	SpringArmComp->bUsePawnControlRotation = true; // Character Rotation is used by the Camera
	SpringArmComp->SocketOffset = FVector(0.f, 50.f, 50.f); // Create Camera Offset

	// Create a follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; //Camera does not rotate relaive to arm
	
	// Disable Character Rotation when the Controler rotates. Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true; //if this is true the character turns with camera movement
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f); //... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.8f;

	// Create HandScene Component
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

	// Create Interpolation Components 
	CreateInterpolationComponents();

	// Initialize PlayerStat with Default Start Stats
	PlayerStats = FPlayerStats(true);
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set Max and Min Pitch as well Camera FOV
	SetCameraProperties();
	
	// TODO:Sync with PlayerStats!!
	CurrentHitPoints = MaxHitPoints;
	
	// Spawn default Weapon and equipp it
	EquipWeapon(SpawnDefaultWeapon());

	// Add the 0. Element to the array
	AddElementToInventory(EquippedWeapon);

	// Assign the 0 Inventoryslot for the Default Weapon!
	EquippedWeapon->SetInventorySlot(0);
	
	// Initialize Default AmmoAmount
	InitializeAmmoMap();

	// Set Movement Properties
	// Save the BaseValue for later Calculation
	TempCrouchMovementSpeed = CrouchMovementSpeed;
	TempBaseMovementSpeed = BaseMovementSpeed;

	// Update MovementComponent
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	GetCharacterMovement()->JumpZVelocity = BaseJumpVelocity;
	GetCapsuleComponent()->SetCapsuleHalfHeight(StandingCapsuleHalfHeight);
	InterpCapsuleHalfHeight = StandingCapsuleHalfHeight;

	// Initialize Interpoplation Locations 
	InitializeInterpLocations();

#if EDITOR 
	GetMesh()->OnComponentHit.AddDynamic(this, &AShooterCharacter::OnSkeletalMeshHit);

	// Just renaming the DisplayName
	SetActorLabel(TEXT("MainCharacter"));
#endif	
}


void AShooterCharacter::OnSkeletalMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
#if EDITOR 
	UE_LOG(CombatLog, Warning, TEXT("HitComponent: %s || OtherActor:%s ||OtherComp:%s|| HitResult: %s"), *HitComponent->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), *Hit.ToString());

	DrawDebugBox(GetWorld(), OtherComp->GetComponentLocation(), FVector(10.f), FColor(FColor::Red), false, 3.f, 0, 5.f);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetBloodParticles(), OtherComp->GetComponentTransform());
#endif	
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Return Early if the Character is already Dead
	if (bCharacterIsDead)
	{
		return -1.f;
	}

	// STATSNOTIFY
	// Clamp Doge between 0 and 60. So a doge Value greater than 30 is useless
	float PlayerDogeClamped = FMath::Clamp(PlayerStats.Doge * 2.f, 0.f, 60.f);
	float RandomNumber = FMath::RandRange(0.f, 100.f);
	if (PlayerDogeClamped > RandomNumber)
	{
		// No Damage
	}
	else
	{
		CurrentHitPoints = CurrentHitPoints - DamageAmount;
	}
	
	// If Damage is greater than CurrentHitPoints then make CurrentHitPoints 0
	if (CurrentHitPoints <= 0.f)
	{
		CurrentHitPoints = 0.f;
		CharacterDeath();
	}

	// If the HitPoint should ever exceed the MaxHitPoints, CurrentHitPoints is set to MaxHitPoints
	if (CurrentHitPoints >= MaxHitPoints)
	{
		CurrentHitPoints = MaxHitPoints;
	}

	return DamageAmount;	
}

void AShooterCharacter::MoveForward(float Value)
{
	if(!bAllowMovementInput)
	{
		return;
	}

	if (Controller != nullptr && (Value != 0.0f))
	{
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, ControllerRotation.Yaw, 0.f);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, Value);
	}
}

void AShooterCharacter::MoveSideward(float Value)
{
	if (!bAllowMovementInput)
	{
		return;
	}

	if (Controller != nullptr && (Value != 0.0f))
	{
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, ControllerRotation.Yaw, 0.f);

		const FVector SidewardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(SidewardDirection, Value);
	}
}

//Controller and Keyboard Input
void AShooterCharacter::TurnAtRate(float Rate)
{
	//Calculate delta for this frame	
	AddControllerYawInput(Rate * GetWorld()->DeltaTimeSeconds * BaseTurnRate);
} 

//Controller and Keyboard Input
void AShooterCharacter::LookUpArRate(float Rate)
{
	AddControllerPitchInput(Rate * GetWorld()->DeltaTimeSeconds * BaseLookUpRate);
}

void AShooterCharacter::MouseTurn(float Value)
{
	float TurnScaleFactor = 0.f;

	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}

	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::MouseLookUp(float value)
{
	float LookUpScaleFactor = 0.f;

	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}

	AddControllerPitchInput(value * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	if (bCharacterIsDead)
	{
		return;
	}

	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}

	if (WeaponHasAmmo())
	{
		// Sound
		PlayFireSound();

		// Perform Raytrace
		SendBullet();

		// Animation
		PlayGunFireMontage();

		// Decrement a specifed Ammount of Bullets from the clip of the weapon
		EquippedWeapon->DecrementAmmunition();

		// Start Bullet fire timer for crosshairs
		StartCrosshairBulletFire();
		
		StartFireTimer();

		// Displace Slide when EquippedWeapon is a Pistol
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			EquippedWeapon->StartSlideTimer();
		}

		// Play Recoil Animation
		EquippedWeapon->StartRecoilRotationTimer();
 
	}
	else
	{		
		EquippedWeapon->PlayEmptyWeaponSound();				
	}
}

// TODO: Needs Refactoring
// Is not 100 % working as intendet.
// If an Object is infront of Camera and the WeaponMuzzle is behind this Object, we got Wrong behavior
bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	// Check for CrosshairTraceHit
	FHitResult CrossHairHitResult;
	FVector OutBeamLocation;

	bool bCrossHairHit = TraceUnderCrosshairForWeaponFire(CrossHairHitResult, OutBeamLocation);
	
	if (bCrossHairHit)
	{
		// Tentative beam location - still need to trace from the gunbarrel
		OutBeamLocation = CrossHairHitResult.Location;
	}
	else // no crosshair trace hit
	{
		// OutBeamLocation is the End Locaton for the line trace	
	}
	
	//Perform a second trace, this time from the gun barrel
	FVector WeaponTraceStart = MuzzleSocketLocation;
	FVector StartToEnd = OutBeamLocation - MuzzleSocketLocation;
	FVector WeaponTraceEnd = OutBeamLocation + StartToEnd * 1.25f ; // * 1.25f otherwise second LineTrace is to short
	
	// Allowing the return of Physics
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility, Params);

	if (!OutHitResult.bBlockingHit) //Object between barrel and BeamEndPoint?
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}	
	return true;
}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	// Character Movement go to Aiming()
	if (CombatState != ECombatState::ECS_Reloading &&
		CombatState != ECombatState::ECS_Equipping &&
		CombatState != ECombatState::ECS_Stunned)
	{
		Aiming();
	}
}

void AShooterCharacter::AmingButtonReleased()
{
	// Character Movement go to StopAiming()
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::SetAimingZoom(float DeltaTime)
{
	if (bAiming)
	{
		// Interpolate to ZoomLevel
		CurrentCameraFov = FMath::FInterpTo(CurrentCameraFov, ZoomedCameraFov, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		// Interpolate to DefaultLevel
		CurrentCameraFov = FMath::FInterpTo(CurrentCameraFov, CameraDefaultFov, DeltaTime, ZoomInterpSpeed);
	}

	FollowCamera->SetFieldOfView(CurrentCameraFov);
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle interpolation for zoom when aiming
	SetAimingZoom(DeltaTime);

	// Change Look Sensitivity based on Aiming
	SetLookRates();

	// Calculate Spread of the CrossHair multiplier
	CalculateCrosshairSpread(DeltaTime);
		
	InterpCapsulehalfHeight(DeltaTime);	
}

void AShooterCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = ControllerAimingTurnRate;
		BaseLookUpRate = ControllerAimingLookUpRate;
	}
	else
	{
		BaseTurnRate = ControllerHipTurnRate;
		BaseLookUpRate = ControllerHipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	if (bCharacterIsDead)
	{
		return;
	}
	
	float MaxWalkspeed = 0.f; 

	if (GetMovementComponent())
	{
		 MaxWalkspeed = GetMovementComponent()->GetMaxSpeed();
	}
	
	FVector2D WalkSpeedRange = { 0.f, MaxWalkspeed };
	FVector2D VelocityMultiplierRange = { 0.f, 1.f };

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f; // Get only lateral Movement

	// Calculate crosshair velocity factor via Mapping.
	CrosshairVelocitySpreadFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// Calculate crosshair in air factor
	if (GetCharacterMovement()->IsFalling())
	{
		float CrosshairInAirTargetValue = 2.f;
		CrosshairInAirTargetValue = CrosshairInAirTargetValue * (EquippedWeapon->GetWeaponInAirSpreadFactor() / 2); // Divided by 2, to get a Normalized Value for the DataTable
		//Spread the crosshairs slowly  while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, CrosshairInAirTargetValue, DeltaTime, 2.f);
	}
	else // Character is on the Ground
	{
		//Shrink the crosshair rapidly while on the ground
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// Calculate crosshair AimFactor
	if (bAiming)
	{
		float CrosshairAimDefaultTargetValue = -1.0f;
		CrosshairAimDefaultTargetValue = CrosshairAimDefaultTargetValue * EquippedWeapon->GetWeaponAimSpreadFactor();
		// Shrink crosshairs a small amount very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, CrosshairAimDefaultTargetValue, DeltaTime, 10.f);
									
	}
	else
	{
		// Spread Crosshairs back to normal very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 10.f);
	}

	if (EquippedWeapon)
	{
		const float ShootingFactorCoolDownSpeed = EquippedWeapon->GetShootingFactorCoolDownSpeed(); // Default: 8.f
		const float ShootingFactorIncreaseValue = EquippedWeapon->GetShootingFactorCrosshairIncreaseValue(); // Default: 0.25f
		const float MaxSpreadAmount = EquippedWeapon->GetShootingFactorMaxSpreadAmount(); // Default: 5.f


		float ShootingFactorCurrent = CrosshairShootingFactor;
		float ShootingFactorTarget = ShootingFactorCurrent + ShootingFactorIncreaseValue;
	
		// GEngine->AddOnScreenDebugMessage(2, -1, FColor::Blue, FString::Printf(TEXT("ShootingFactorCurrent: %f, ShootingFactorTarget: %f "), ShootingFactorCurrent, ShootingFactorTarget));
		
		// True after Amount of "ShootTimeDuration"
		if (bFiringBullet)
		{
			ShootingFactorTarget = FMath::Clamp<float>(ShootingFactorTarget, 0.f, MaxSpreadAmount);
			CrosshairShootingFactor = FMath::FInterpTo(ShootingFactorCurrent, ShootingFactorTarget, DeltaTime, 20.f);
		}
		else
		{
			CrosshairShootingFactor = FMath::FInterpTo(ShootingFactorCurrent, 0.f, DeltaTime, ShootingFactorCoolDownSpeed);
		}
	}

	// Calculate Crosshair Crouchfactor
	if (bCrouching)
	{	// Default Target Value is -1.f
		float CrosshairCrouchDefaultTargetValue = -1.f;

		// Multiply the Weapon Factor to the Default Value
		CrosshairCrouchDefaultTargetValue = CrosshairCrouchDefaultTargetValue * EquippedWeapon->GetWeaponCrouchSpreadFactor();

		CurrentCrosshairCrouchFactor = FMath::FInterpTo(CurrentCrosshairCrouchFactor, CrosshairCrouchDefaultTargetValue, DeltaTime, 10.f);

		// DEBUG
		/*
		FString Text = FString::Printf(TEXT("WeaponCrouchFactor: %f"), EquippedWeapon->GetWeaponCrouchSpreadFactor());
		FString Text2 = FString::Printf(TEXT("CrosshairCrouchBaseTargetValue: %f"), CrosshairCrouchDefaultTargetValue);
		FString Text3 = FString::Printf(TEXT("CurrentCrosshairCrouchFactor: %f"), CurrentCrosshairCrouchFactor);
		GEngine->AddOnScreenDebugMessage(1, -1.f, FColor::Blue, Text);
		GEngine->AddOnScreenDebugMessage(2, -1.f, FColor::Blue, Text2);
		GEngine->AddOnScreenDebugMessage(3, -1.f, FColor::Blue, Text3);
		*/
	}
	else
	{
		CurrentCrosshairCrouchFactor = FMath::FInterpTo(CurrentCrosshairCrouchFactor, 0.f, DeltaTime, 30.f);
	}

	// Calculate CrosshairBaseSpreadFactor
	CrosshairBaseSpreadFactor = 1.f;
	CrosshairBaseSpreadFactor = CrosshairBaseSpreadFactor * EquippedWeapon->GetWeaponBaseSpreadFactor();

	CrosshairSpreadMultiplier = CrosshairBaseSpreadFactor
								+ CrosshairVelocitySpreadFactor
								+ CrosshairInAirFactor
								+ CrosshairAimFactor
								+ CrosshairShootingFactor
								+ CurrentCrosshairCrouchFactor;

	// Make sure CrosshairSpreadMultiplier is never smaller than 0 and greater than 25.f!
	if (CrosshairSpreadMultiplier <= 0.f)
	{
		CrosshairSpreadMultiplier = 0.f;
	}
	if (CrosshairSpreadMultiplier >= 25.f)
	{ 
		CrosshairSpreadMultiplier = 25.f;
	}

	// GEngine->AddOnScreenDebugMessage(4, 0.5f, FColor::Black, FString::Printf(TEXT("CrosshairSpreadMultiplier: %f"), CrosshairSpreadMultiplier));


}

void AShooterCharacter::StartCrosshairBulletFire()
{
	if (EquippedWeapon)
	{
		bFiringBullet = true;
		GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::FinishCrosshairBulletFire, EquippedWeapon->GetAutoFireFrequency(), false);		
	}
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

UPhysicalMaterial* AShooterCharacter::GetPhysicalMaterialUnderFootsteps(FName BoneName, bool ShowDebug)
{	
	FHitResult HitResult;
	FVector Start = GetMesh()->GetBoneTransform(GetMesh()->GetBoneIndex(BoneName)).GetLocation();

	// Make Vector that points down
	float CheckingRange = -200.f;
	FVector End = Start + FVector(0.f, 0.f, CheckingRange);

	// Allow Returning PhysicalMaterial
	FCollisionQueryParams QueryParameters;
	QueryParameters.bReturnPhysicalMaterial = true;

	// Line Trace
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParameters);
			
	if (ShowDebug)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 10.f, 0, 5.f);
		DrawDebugLine(GetWorld(), Start, (Start + FVector(0.f,0.f, CheckingRange * -1.f)), FColor::Orange, false, 10.f, 0, 5.f);

		if (HitResult.bBlockingHit)
		{
			UE_LOG(MyLog, Warning, TEXT("TraceHit: %s"), *HitResult.GetActor()->GetActorNameOrLabel());
		}
	}

	if (HitResult.bBlockingHit)
	{		
		return HitResult.PhysMaterial.Get();
	}
	
	return nullptr;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();	
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (EquippedWeapon)
	{
		CombatState = ECombatState::ECS_FireTimerInProgress;
		// Sets Timer for next Fire
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, EquippedWeapon->GetAutoFireFrequency());
	}	
}

void AShooterCharacter::AutoFireReset()
{
	if (CombatState == ECombatState::ECS_Stunned)
	{
		return;
	}
			
	if ( EquippedWeapon == nullptr)
	{
		return;
	}

	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo())
	{
		// Continue Firing when Firebutton is Pressed
		if (bFireButtonPressed && EquippedWeapon->IsAutomaticGun())
		{
			FireWeapon();
		}
	}
	else
	{
			ReloadWeapon();		
	}
	if (!WeaponHasAmmo() && !EnoughSpareAmmo() && bFireButtonPressed)
	{
		EquippedWeapon->PlayEmptyWeaponSound();
		StartFireTimer();		
	}
}

// Refactor with TraceUnderCrosshairForWeaponFire()
bool AShooterCharacter::TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	if (bCharacterIsDead)
	{
		return false;
	}

	// GetViewPort 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Calculate middle
	FVector2D CrosshairPosition = { ViewportSize.X / 2.f, ViewportSize.Y / 2.f };

	FVector CrosshairWorldPosition;
	FVector CrosshairOrientation;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), CrosshairPosition, CrosshairWorldPosition, CrosshairOrientation);

	if (bScreenToWorld)
	{
		const FVector Start = CrosshairWorldPosition;
		const FVector End = CrosshairWorldPosition + CrosshairOrientation * 50'000.f;
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		/*
		GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Black, FString::Printf(TEXT("CrosshairPosition %s"), *CrosshairPosition.ToString()));
		GEngine->AddOnScreenDebugMessage(2, 0.5f, FColor::Black, FString::Printf(TEXT("CrosshairWorldPosition %s"), *CrosshairWorldPosition.ToString()));
		GEngine->AddOnScreenDebugMessage(3, 0.5f, FColor::Black, FString::Printf(TEXT("CrosshairOrientation %s"), *CrosshairOrientation.ToString()));
		*/

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}				
	}
	return false;
}

// Use this for calculating WeaponFireEndTrace CrosshairSpreadMultiplier is taken into account,
// Refactor with TraceUnderCrosshair()
bool AShooterCharacter::TraceUnderCrosshairForWeaponFire(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// GetViewPort 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairPosition = { ViewportSize.X / 2.f, ViewportSize.Y / 2.f };

	// Weapon Spread taken into Account
	float SpreadAmount = CrosshairSpreadMax * CrosshairSpreadMultiplier;
	CrosshairPosition.X += FMath::FRandRange(-SpreadAmount, SpreadAmount);
	CrosshairPosition.Y += FMath::FRandRange(-SpreadAmount, SpreadAmount);

	FVector CrosshairWorldPosition;
	FVector CrosshairOrientation;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), CrosshairPosition, CrosshairWorldPosition, CrosshairOrientation);

	if (bScreenToWorld)
	{
		const FVector Start = CrosshairWorldPosition;
		const FVector End = CrosshairWorldPosition + CrosshairOrientation * 50'000.f;
		OutHitLocation = End;

		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	// Check the TsubClass 
	if (DefaultWeaponClass)
	{
		// Spawn Weapon
		AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		if (DefaultWeapon)
		{
			// Set the Start ItemRarity
			DefaultWeapon->PostBeginPlayerIntialisierung(StartWeaponRarity, StartWeaponType);
			
			// Set the custom DepthStencil Value to default Value
			DefaultWeapon->GetItemMesh()->SetCustomDepthStencilValue(250);

			// UE_LOG(LogTemp, Warning, TEXT("SpawnDefaultWeapon gets executed"));
			return DefaultWeapon;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("SpawnDefaultWeapon did  NOT Spawn"));
	return nullptr;	
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{	
		// Get Hand Socket Ref
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(TEXT("RightHandSocket"));
		
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("HandSocket is NOT Valid"));
		}

		// No Weapon currently equipped; Enters when called for the first time
		if (EquippedWeapon == nullptr)
		{
			// -1 == No EquippedWeapon yet. No need to reverse the icon animation
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetInventorySlot());
		}
		else if (!bSwapping)
		{
			// Blueprints Event is bind to the Delegate that performs an Animation based on the Slotsindexes
			EquipItemDelegate.Broadcast(EquippedWeapon->GetInventorySlot(), WeaponToEquip->GetInventorySlot());
			
		}
		// Set Weapon Reference
		EquippedWeapon = WeaponToEquip;

		// Set Owner to get later a Reference to Character form the Weapon
		EquippedWeapon->SetNewOwner(this);

		// Reset the Local Rotation of the Skeletal Mesh
		WeaponToEquip->ResetLocalMeshRotation();
		EquippedWeapon->SetItemState(EItemState::EIS_Equppied);
				
		// Disable MaterialGlowEffect
		DisableGlowEffectOnDefaultWeapon();
	}	
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);

		EquippedWeapon->DetachFromActor(Rules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->EnableGlowMaterial();		
		EquippedWeapon->ThrowWeapon();
		
	}
}

void AShooterCharacter::SelectButtonPressed()
{	
	if (bCharacterIsDead)
	{
		return;
	}

	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}

	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this);
		// Prevent Spawming when the Item is just picked up
		TraceHitItem = nullptr;
	}
}

void AShooterCharacter::SelectButtonReleased()
{
	// Called when SelectButton is relased
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (bCharacterIsDead)
	{
		return;
	}
		// Make sure the Slot of the Equipped Weapon is Valid
	if (Inventory.IsValidIndex(EquippedWeapon->GetInventorySlot()))
	{
		// Replace the current Slot with the Weapon to swap.
		Inventory[EquippedWeapon->GetInventorySlot()] = WeaponToSwap;
		// Gives the WeaponToSwap the correct InventorySlot
		WeaponToSwap->SetInventorySlot(EquippedWeapon->GetInventorySlot());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap, true);

	TraceHitItem = nullptr;
	ItemLastFrame = nullptr;
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, StartingAmmount9mm);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingAmmountAssaultRifle);
	AmmoMap.Add(EAmmoType::EAT_SMG, StartingAmmountSMG);
	AmmoMap.Add(EAmmoType::EAT_INFINITE, INT_MAX);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (!EquippedWeapon)
	{
		return false;
	}
	return (EquippedWeapon->GetAmmunitionInMagazin() > 0);
}

void AShooterCharacter::PlayFireSound()
{
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SpawnMuzzleFlashAtLocation(const FTransform& Transform)
{
	// Spawn Emitter at SocketLocation
	if (EquippedWeapon->GetMuzzleFlash())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), Transform);
	}
}

void AShooterCharacter::SpawnTrailParticles(const FVector& Start, const FVector& Destination)
{
	// Particle Effect
	if (TrailParticles)
	{
		UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailParticles, Start);

		if (Trail)
		{
			Trail->SetVectorParameter(TEXT("Target"), Destination);
		}
	}
}

FTransform AShooterCharacter::GetWeaponBarrelTransform()
{
	if (EquippedWeapon)
	{
		const USkeletalMeshSocket* Weapon_Barrel_Socket = EquippedWeapon->GetItemMesh()->GetSocketByName(TEXT("weapon_Barrel_Socket"));
		if (Weapon_Barrel_Socket)
		{
			// Get WeaponBarrelLocation
			FTransform StartTransform = Weapon_Barrel_Socket->GetSocketTransform(EquippedWeapon->GetItemMesh());
			return  StartTransform;
		}
	}
	// Return NUll transform;
	UE_LOG(MyLog, Error, TEXT("AShooterCharacter::GetWeaponBarrelTransform(), Return Transform = NULL"));
	return FTransform(FRotator3d(0.f), FVector(0.f), FVector(0.f));
	
}

void AShooterCharacter::SendBullet()
{					
	// Send RayTrace
	FHitResult BeamHitResult;
	bool bBeamEnd = GetBeamEndLocation(GetWeaponBarrelTransform().GetLocation(), BeamHitResult);

	// Is there a Hit
	if (bBeamEnd)
	{
		// Spawn Effect on WeaponBarrelTip
		SpawnMuzzleFlashAtLocation(GetWeaponBarrelTransform());
		// Spawns a Trail Effect
		SpawnTrailParticles(GetWeaponBarrelTransform().GetLocation(), BeamHitResult.Location);
			
		if (BeamHitResult.GetActor())
		{
			// How to check if an Object has a Interface??
			// There are Two ways:
				// First, try via a cast to check if the interface is implemented
					// --
					// IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());
					// BulletHitInterface->BulletHit_Implementation;
					// --
				// Second, use the ImplementsInterface() Check
					// --
					// ImplementsInterface(UBulletHitInterface::StaticClass()
					// --
			// Does hit Actor implement BulletHitInterface ?
			// Call Funtion Body an the actor that has the implementation.
			
			if (BeamHitResult.GetActor()->GetClass()->ImplementsInterface(UBulletHitInterface::StaticClass()))
			{
				DetectedHitZone.InstigatorRef = this;
				DetectedHitZone.DamagedActor = BeamHitResult.GetActor();

				////STATSNOTIFY
				// Take LifeSteal Stat into account
				LifeSteal();

				IBulletHitInterface::Execute_BulletHit(BeamHitResult.GetActor(), BeamHitResult, DetectedHitZone);
			}
			// SpawnDefault ImpactParicles
			else 
			{
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamHitResult.Location);
				}
			}
		}			
	}	
}

void AShooterCharacter::LifeSteal()
{
	if (PlayerStats.LifeSteal > 0)
	{
		float RandomNumber = FMath::RandRange(0.f, 100.f);
		// Get 2 life for every successful Lifesteal
		// Change can't get Higher than 70 % Percent
		float LifeSteal = FMath::Clamp(PlayerStats.LifeSteal * 2.5f, 0.f, 70.f);

		if (LifeSteal > RandomNumber)
		{
			// 2 Base Heal Amount
			AddPlayerHealth(2.f);
		}
	}	
}

void AShooterCharacter::PlayGunFireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HipFireMontage)
	{
		// Play Animation form Code
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(TEXT("Start_Fire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ExplosionHit_Implementation(float Damage, float StunChance)
{
	// Return Early if the Character is already Dead
	if (bCharacterIsDead)
	{
		return;
	}

	CurrentHitPoints = CurrentHitPoints - Damage;	

	// If Damage is greater than CurrentHitPoints than set CurrentHitPoints 0
	if (CurrentHitPoints <= 0.f)
	{
		CurrentHitPoints = 0.f;
		CharacterDeath();
	}
	else
	{
		RollForStunResistance(StunChance);
	}

	// If the HitPoint should ever exceed the MaxHitPoints, CurrentHitPoints is set to MaxHitPoints
	if (CurrentHitPoints >= MaxHitPoints)
	{
		CurrentHitPoints = MaxHitPoints;
	}

#if EDITOR 
	UE_LOG(CombatLog, Warning, TEXT("%s Got Hit for %f by ExplosionDamage"), *this->GetActorLabel(), Damage);
#endif
}

void AShooterCharacter::ReloadWeapon()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	if (bCharacterIsDead)
	{
		return;
	}

	if (EquippedWeapon->GunHasFullMagazine())
	{
		return;
	}

	if (CombatState != ECombatState::ECS_Unoccupied)
	{
		return;
	}

	if (CombatState == ECombatState::ECS_Stunned)
	{
		return;
	}

	else
	{
		//Do we have the correct spare Ammo ?
		if (EnoughSpareAmmo())
		{
			if (bAiming) // Make sure while we are Reloading we dont Aim
			{
				StopAiming();
			}

			// Update CombatState
			CombatState = ECombatState::ECS_Reloading;
			
			PlayGunReloadMontage();						
		}
	}
}

void AShooterCharacter::PlayGunReloadMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (ReloadMontage && AnimInstance)
	{
		FName MontageSection = EquippedWeapon->GetReloadMontageSectionName();

		AnimInstance->Montage_Play(ReloadMontage);
		AnimInstance->Montage_JumpToSection(MontageSection);
	}
}

void AShooterCharacter::PlayEquippingMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(TEXT("Equip"));
	}
}

void AShooterCharacter::FinishReloading()
{
	if (bCharacterIsDead)
	{
		return;
	}

	if (CombatState == ECombatState::ECS_Stunned)
	{
		UE_LOG(MyLog, Log, TEXT("Reloading interrupted"));
		return;
	}

	if (EquippedWeapon == nullptr)
	{
		return;
	}

	EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		// Amount of Ammuniton that the character has of the specific AmmoType
		int32 CarriedAmmunition = AmmoMap[AmmoType];

		// Missing Bullets in the Magazin
		int32 MagEmptySpace = EquippedWeapon->GetMaxMagazineCapacity() - EquippedWeapon->GetAmmunitionInMagazin();

		if (CarriedAmmunition >= MagEmptySpace) //Enough Spare Ammunition
		{
			// Reloads the weapon up to full
			EquippedWeapon->SetAmmunitionInMagazin(MagEmptySpace);
			
			// Only subtract Ammuniton if the AmmoType is not EAT_INFINTE
			if (AmmoType != EAmmoType::EAT_INFINITE)
			{
				// Calculate New CarriedAmmuniton Amount
				CarriedAmmunition = CarriedAmmunition - MagEmptySpace;

				// Set new CarriedAmmuniton Amount in Map
				AmmoMap.Add(AmmoType, CarriedAmmunition);
			}

		}
		else // To few spare Ammunition to reload completely
		{ 
			// Reload only with bullets we have left on the Character
			EquippedWeapon->SetAmmunitionInMagazin(CarriedAmmunition);

			// Set new CarriedAmmuniton Amount in Map
			AmmoMap.Add(AmmoType, 0);
		}
	}
	CombatState = ECombatState::ECS_Unoccupied;

	// When Aiming Buttons is pressed immediately return back to aiming
	if (bAimingButtonPressed)
	{
		Aiming();
	}
}

void AShooterCharacter::FinishEquipping()
{
	if (CombatState == ECombatState::ECS_Stunned)
	{
		return;
	}

	CombatState = ECombatState::ECS_Unoccupied;

	// if Player Aiming while the Equipping Animation finished, the character automaticly starts to aim again
	if (bAimingButtonPressed)
	{
		Aiming();
	}
}

bool AShooterCharacter::EnoughSpareAmmo()
{
	if (EquippedWeapon == nullptr)
	{
		return false;
	}

	EAmmoType AmmoType =  EquippedWeapon->GetAmmoType();

	// Check if we have that Ammo Type in our AmmoMap
	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	
	return false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr || HandSceneComponent == nullptr)
	{
		return;
	}

	// Get Transform of MagazinBone
	FName MagazineBoneName = EquippedWeapon->GetMagazineBoneName();
	int32 BoneIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(MagazineBoneName);
	MagazineTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(BoneIndex);

	// Attach Scene Component to Hand form Character Mesh
	FAttachmentTransformRules Ruels = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), Ruels, TEXT("hand_l"));
	HandSceneComponent->SetWorldTransform(MagazineTransform);
	EquippedWeapon->SetbMovingClip(true);
}

void AShooterCharacter::ReplaceClip()
{
	EquippedWeapon->SetbMovingClip(false);
}

void AShooterCharacter::Jump()
{
	if (!bAllowJumpInput)
	{
		return;
	}

	if (CombatState == ECombatState::ECS_Stunned)
	{
		return;
	}

	if (bCanJump)
	{
		ACharacter::Jump();
		bCanJump = false;
	}
}

void AShooterCharacter::StopJumping()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->JumpZVelocity = BaseJumpVelocity;
	}
	ACharacter::StopJumping();
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Holds exceution if the PlayerInputComponent is not Valid
	checkSlow(PlayerInputComponent);

	//Binds Input to Functions
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveSideward);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpArRate);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AShooterCharacter::MouseTurn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AShooterCharacter::MouseLookUp);

	// Jumping
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &AShooterCharacter::StopJumping);

	// Fire
	PlayerInputComponent->BindAction(TEXT("FireButton"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(TEXT("FireButton"), EInputEvent::IE_Released, this, &AShooterCharacter::FireButtonReleased);

	// Aiming
	PlayerInputComponent->BindAction(TEXT("AimingButton"), EInputEvent::IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction(TEXT("AimingButton"), EInputEvent::IE_Released, this, &AShooterCharacter::AmingButtonReleased);

	// Select
	PlayerInputComponent->BindAction(TEXT("Select"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Select"), EInputEvent::IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	//Reload
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

	// Crouching
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	// Weapon Swap
	PlayerInputComponent->BindAction(TEXT("1Key"), EInputEvent::IE_Pressed, this, &AShooterCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction(TEXT("2Key"), EInputEvent::IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction(TEXT("3Key"), EInputEvent::IE_Pressed, this, &AShooterCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction(TEXT("4Key"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction(TEXT("5Key"), EInputEvent::IE_Pressed, this, &AShooterCharacter::FiveKeyPressed);
	PlayerInputComponent->BindAction(TEXT("6Key"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SixKeyPressed);

	// GamePauseButton
	PlayerInputComponent->BindAction(TEXT("GamePauseButton"), EInputEvent::IE_Pressed, this, &AShooterCharacter::GamePauseButtonPressed);

}

void AShooterCharacter::GamePauseButtonPressed()
{	
	if (!(GetController<AShooterPlayerController>()->GetShowPauseMenu()))
	{
		GetController<AShooterPlayerController>()->ShowPauseMenu();
	}
	/* Does not work because Input is blocked via Keyboard while UI is opened
 	else
	{
		GetController<AShooterPlayerController>()->HidePauseMenu();
	}
	*/		
}

void AShooterCharacter::Aiming()
{
	if (bCharacterIsDead)
	{
		return;
	}

	bAiming = true;
	ResetMovementSpeedBasedOnStance();
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	ResetMovementSpeedBasedOnStance();
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (bCharacterIsDead)
	{
		return;
	}

	// Check for enough Space above Head if crouching
	if (!EnoughSpaceforStanding() && bCrouching)
	{
		return;
	}
	if (!GetCharacterMovement()->IsFalling())
	{
		// Toggle
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		// Lower Movement und JumpVelocitySpeed
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->JumpZVelocity = CrouchJumpVelocity;
	}
	else
	{
		// Reset Movement und JumpVelocitySpeed
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->JumpZVelocity = BaseJumpVelocity;
	}
}

void AShooterCharacter::InterpCapsulehalfHeight(float DeltaTime)
{
	float TargetHeight = 0;
	float CurrentCapsuleHalfHeigt = 0;

	if (GetCapsuleComponent())
	{
		 CurrentCapsuleHalfHeigt = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	if (bCrouching)
	{
		TargetHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetHeight = StandingCapsuleHalfHeight;
	}
		
	InterpCapsuleHalfHeight = FMath::FInterpTo(InterpCapsuleHalfHeight, TargetHeight, DeltaTime, 10.f);

	// Negtativ Vaule if Player starts crouching, positiv Value if Player stands up
	float DeltaCapsuleHalfHeigt = InterpCapsuleHalfHeight - CurrentCapsuleHalfHeigt;
	// Invert MeshOffset to get the correct Direction
	FVector MeshOffset = FVector(0.f, 0.f, -DeltaCapsuleHalfHeigt);

	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpCapsuleHalfHeight);
}

bool AShooterCharacter::EnoughSpaceforStanding()
{
	FHitResult HitResult;
	FVector ActorPosition = GetTransform().GetLocation();
	FVector StartOffset = ActorPosition + FVector(0.f, 0.f, 40.f);

	// Amount of Units the area above is checked
	float AmountofHeighttoCheck = 85.f;

	FVector End = StartOffset + GetActorUpVector() * AmountofHeighttoCheck;
	
	//DrawDebugSphere(GetWorld(), StartOffset, 10.f, 10, FColor::Red, false, -1.f);
	//DrawDebugSphere(GetWorld(), End, 10.f, 10, FColor::Green, false, -1.f);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartOffset, End, ECollisionChannel::ECC_Visibility))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void AShooterCharacter::InitializeInterpLocations()
{
	// Create FInterpLocation Struct and initialize it with a Scene Component and an Amount
	FInterpLocation WeaponLocation{ WeaponInterpComp , 0 };
	// Then Add the struct the InterpLocation Struct Array
	InterpLocationArray.Add(WeaponLocation);

	FInterpLocation InterpLocation1{ InterpComp1, 0 };
	InterpLocationArray.Add(InterpLocation1);

	FInterpLocation InterpLocation2{ InterpComp2, 0 };
	InterpLocationArray.Add(InterpLocation2);

	FInterpLocation InterpLocation3{ InterpComp3, 0 };
	InterpLocationArray.Add(InterpLocation3);

	FInterpLocation InterpLocation4{ InterpComp4, 0 };
	InterpLocationArray.Add(InterpLocation4);

	FInterpLocation InterpLocation5{ InterpComp5, 0 };
	InterpLocationArray.Add(InterpLocation5);

	FInterpLocation InterpLocation6{ InterpComp6, 0 };
	InterpLocationArray.Add(InterpLocation6);

}

void AShooterCharacter::ResetPickSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

void AShooterCharacter::DisableGlowEffectOnDefaultWeapon()
{
	if (EquippedWeapon)
	{ 
		EquippedWeapon->DisableGlowMaterial();
	}	
}

void AShooterCharacter::AddElementToInventory(AWeapon* WeaponToAdd)
{
	Inventory.Add(WeaponToAdd);
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	// start with 1 because 0. Position is reserved for Weapons
	for (int32 i = 1; i < InterpLocationArray.Num(); i++)
	{
		if (InterpLocationArray[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocationArray[i].ItemCount;
		}
	}

	return LowestIndex;
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() 
{
	return CrosshairSpreadMultiplier;
}

float AShooterCharacter::GetCrosshairSpreadMax() const
{
	return CrosshairSpreadMax;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;	
		bShouldTraceForItems = true;
	}
}

void AShooterCharacter::AddWeaponToInventory(AWeapon* Weapon)
{
	// here find next free inventory slot
	Weapon->SetInventorySlot(GetEmptyInventorySlot()); 
	// Set Item Attributes
	Weapon->SetItemState(EItemState::EIS_Carrying);
	// Add to Inventory Array
	AddElementToInventory(Weapon);
}

AWeapon* AShooterCharacter::BoughtWeaponInShop(EItemRarity WeaponRarity, EWeaponType WeaponType)
{
	// Spawn Weapon
	AWeapon* AnotherWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	// Set Type and Rarity
	AnotherWeapon->PostBeginPlayerIntialisierung(WeaponRarity, WeaponType);
	// Handle Inventory Management
	AddWeaponToInventory(AnotherWeapon);

	return AnotherWeapon;
}


void AShooterCharacter::SellInventorySlot(int32 SlotNumber)
{	
	if(Inventory[SlotNumber])
	{
		AWeapon* WeaponToSale = Cast<AWeapon>(Inventory[SlotNumber]);

		if (WeaponToSale)
		{
			int32 MoneyCost = FMath::Floor((WeaponToSale->GetWeaponCost()/ QuotientOfChangeWhenSellingWeapon));

			AddPlayerMoney(MoneyCost);
		}

		Inventory.RemoveAt(SlotNumber);
		WeaponToSale->Destroy();

		// Equip Default Weapon
		if(Inventory.Num() == 0)
		{
			SpawnBackupWeapon();
		}
		else
		{
			EquipWeapon(Cast<AWeapon>(Inventory[0]));
		}		
	}
}

// TODO: Need Refactoring, porblems with Unreal Array, of Size of the Array Changes every Time an Item is Added or removed, 
// solution switching to other Container for Inventory?
void AShooterCharacter::UpgradeWeapon(int32 SlotNumber)
{
	// Weapon To Upgrade
	if (Inventory[SlotNumber])
	{
		AWeapon* WeaponToUpgrade = Cast<AWeapon>(Inventory[SlotNumber]);	

		// Looking for the Same Weapon Type in the Inventory
		for (int32 i = 0; i < Inventory.Num(); i++)
		{
			// Check if it has the same Inventory Slot, if so skip this Item
			if (Inventory[i] && WeaponToUpgrade->GetInventorySlot() != Inventory[i]->GetInventorySlot())
			{
				AWeapon* WeaponToCompare = Cast<AWeapon>(Inventory[i]);
				// Compare WeaponRarity AND WeaponType
				// If True allow Upgrading
				if (WeaponToCompare->GetItemRarity() == WeaponToUpgrade->GetItemRarity()
					&& WeaponToCompare->GetWeaponType() == WeaponToUpgrade->GetWeaponType())
				{
					TArray<AItem*> TempInventory = Inventory;

					
					if (WeaponToCompare->GetInventorySlot() > WeaponToUpgrade->GetInventorySlot())
					{
						// The OtherWeapon
						TempInventory.RemoveAt(WeaponToCompare->GetInventorySlot());
						WeaponToCompare->Destroy();

						// The Weapon To Upgrade
						TempInventory.RemoveAt(WeaponToUpgrade->GetInventorySlot());
						WeaponToUpgrade->Destroy();
					}
					else
					{
						// The Weapon To Upgrade
						TempInventory.RemoveAt(WeaponToUpgrade->GetInventorySlot());
						WeaponToUpgrade->Destroy();
						// The OtherWeapon
						TempInventory.RemoveAt(WeaponToCompare->GetInventorySlot());
						WeaponToCompare->Destroy();
					}
					
					// Reorder Inventory
					Inventory = TempInventory;
					int32 x = 0;
					for (int32 z = 0; z < Inventory.Num(); z++)
					{
						if(Inventory[z])
						{
							Inventory[z]->SetInventorySlot(x);
							x++;
						}
					}

					AWeapon* NewWeapon = BoughtWeaponInShop(RarityAbove(WeaponToUpgrade->GetItemRarity()), WeaponToUpgrade->GetWeaponType());
					EquipWeapon(NewWeapon);
				}				
			}
		}
	}
}

void AShooterCharacter::SpawnBackupWeapon()
{
	AWeapon* BackupWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);

	BackupWeapon->PostBeginPlayerIntialisierung(EItemRarity::EIR_Damaged, EWeaponType::EWT_Pistol);

	AddWeaponToInventory(BackupWeapon);
	EquipWeapon(BackupWeapon);
}


EItemRarity AShooterCharacter::RarityAbove(EItemRarity InItemRarity)
{
	switch (InItemRarity)
	{
		case EItemRarity::EIR_Damaged:
			return EItemRarity::EIR_Common;
			break;
		case EItemRarity::EIR_Common:
			return EItemRarity::EIR_Uncommon;
			break;
		case EItemRarity::EIR_Uncommon:
			return EItemRarity::EIR_Rare;
			break;
		case EItemRarity::EIR_Rare:
			return EItemRarity::EIR_Epic;
			break;
		case EItemRarity::EIR_Epic:
			return EItemRarity::EIR_Epic;
			break;
		case EItemRarity::EIR_Default:
			return EItemRarity::EIR_Damaged;
			break;
		default:
			return EItemRarity::EIR_Damaged;
			break;
	}
}




void AShooterCharacter::GetPickupItem(AItem* Item)
{	
	Item->PlayEquipSound();

	AWeapon* Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		//	Slot	INV.NUM()	INVCAP
		//	0		1			6
		//	1		2			6
		//	2		3			6
		//  ...		...			6
		//	5		6			6
		if (Inventory.Num() < IVENTORY_CAPACITY)
		{
			// Set WeaponSlot to the current Size of the Array
			Weapon->SetInventorySlot(Inventory.Num());
			AddElementToInventory(Weapon);
			
			// Set State
			Weapon->SetItemState(EItemState::EIS_Carrying);
		}
		else // Inventory is full, swap with equipped weapon
		{
			SwapWeapon(Weapon);
			UE_LOG(LogTemp, Warning, TEXT("SwapWeapon is called!"));
		}
	}

	// Check if it is A Ammunition?
	AAmmunition* Ammo = Cast<AAmmunition>(Item);
	if(Ammo)
	{
		PickUpAmmunition(Ammo);
	}

	// Check if it is a droppable
	if (Item->GetItemType() == EItemType::EIT_Droppable)
	{
		AAmmoDrop* AmmoDrop = Cast<AAmmoDrop>(Item);

		if (AmmoDrop)
		{
			PickUpAmmunitionDrop(AmmoDrop);
		}
	}		
}

void AShooterCharacter::PickUpAmmunitionDrop(AAmmoDrop* AmmoDrop)
{
	if (AmmoMap.Find(AmmoDrop->GetAmmoType()))
	{
		// Get the current Amount of that AmmoType
		int32 CurrentAmmoCount = AmmoMap[AmmoDrop->GetAmmoType()];

		int32 NewAmmoCount = CurrentAmmoCount + AmmoDrop->GetAmmunitionAmount();
		// Refresh Amount of Ammo in the map of that specific AmmoType
		AmmoMap[AmmoDrop->GetAmmoType()] = NewAmmoCount;

		UE_LOG(ItemLog, Log, TEXT("Added %d Ammo to the Player"), NewAmmoCount);
	}

	if (EquippedWeapon->GetAmmoType() == AmmoDrop->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmunitionInMagazin() == 0)
		{
			ReloadWeapon();
		}
	}

	AmmoDrop->Destroy();
}

void AShooterCharacter::LevelAmmoFillUp()
{
	if (AmmoMap[EAmmoType::EAT_AR] <= 35)
		AmmoMap[EAmmoType::EAT_AR] = 50;
	if (AmmoMap[EAmmoType::EAT_SMG] <= 45)
		AmmoMap[EAmmoType::EAT_SMG] = 60;
	if (AmmoMap[EAmmoType::EAT_9mm] <= 20)
		AmmoMap[EAmmoType::EAT_9mm] = 20;	
}

void AShooterCharacter::PickUpAmmunition(AAmmunition* Ammo)
{
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// Get the current Amount of that AmmoType
		int32 CurrentAmmoCount = AmmoMap[Ammo->GetAmmoType()];

		int32 NewAmmoCount = CurrentAmmoCount + Ammo->GetItemCount();
		// Refresh Amount of Ammo in the map of that specific AmmoType
		AmmoMap[Ammo->GetAmmoType()] = NewAmmoCount;

		UE_LOG(ItemLog, Log, TEXT("Added %d Ammo to the Player"), NewAmmoCount);
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmunitionInMagazin() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

FInterpLocation AShooterCharacter::GetInterpLocationArray(int32 index)
{
	if (index <= InterpLocationArray.Num())
	{
		return InterpLocationArray[index];
	}
	else
	{   // return an empty FInterpLocation Struct.
		return  FInterpLocation();
	}
}

void AShooterCharacter::IncrementInterpLocationItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1)
	{
		return;
	}
	if (InterpLocationArray.Num() >= Index)
	{
		InterpLocationArray[Index].ItemCount += Amount;
	}	
}

void AShooterCharacter::StartPickUpSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickUpSoundTimer, this, &AShooterCharacter::ResetPickSoundTimer, PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void AShooterCharacter::OneKeyPressed()
{
	// Weapon is already equipped
	if (EquippedWeapon->GetInventorySlot() == 0)
	{
		return;
	}

	ExchangeInventoryItem(EquippedWeapon->GetInventorySlot(), 0);
}

void AShooterCharacter::TwoKeyPressed()
{
	// Weapon is already equipped
	if (EquippedWeapon->GetInventorySlot() == 1)
	{
		return;
	}

	ExchangeInventoryItem(EquippedWeapon->GetInventorySlot(), 1);
}

void AShooterCharacter::ThreeKeyPressed()
{
	// Weapon is already equipped
	if (EquippedWeapon->GetInventorySlot() == 2)
	{
		return;
	}

	ExchangeInventoryItem(EquippedWeapon->GetInventorySlot(), 2);
}

void AShooterCharacter::FourKeyPressed()
{
	// Weapon is already equipped
	if (EquippedWeapon->GetInventorySlot() == 3)
	{
		return;
	}

	ExchangeInventoryItem(EquippedWeapon->GetInventorySlot(), 3);
}

void AShooterCharacter::FiveKeyPressed()
{
	// Weapon is already equipped
	if (EquippedWeapon->GetInventorySlot() == 4)
	{
		return;
	}

	ExchangeInventoryItem(EquippedWeapon->GetInventorySlot(), 4);
}

void AShooterCharacter::SixKeyPressed()
{
	// Weapon is already equipped
	if (EquippedWeapon->GetInventorySlot() == 5)
	{
		return;
	}

	ExchangeInventoryItem(EquippedWeapon->GetInventorySlot(), 5);
}

void AShooterCharacter::ExchangeInventoryItem(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if (bCharacterIsDead)
	{
		return;
	}
	// Do Nothing if the weapon to swap is the equipped Weapon AND
	// if the Weapon hat a greater or equaul Index than the Inventory Array Size
	// NOTE; Size is always 1 greater than index, due to the default weapon given index 0
	// AND we are not Unoccupied
		
	bool bAllowExchange;

	if (AllowExchangeWhileEquipping)
	{
		  bAllowExchange = (CurrentItemIndex != NewItemIndex) &&
									(NewItemIndex < Inventory.Num()) &&
									((CombatState == ECombatState::ECS_Unoccupied) ||
									(CombatState == ECombatState::ECS_Equipping)) &&
									!bAiming;

		  if (AllowExchangeWhileAiming)
		  {
			  bAllowExchange = (CurrentItemIndex != NewItemIndex) &&
				  (NewItemIndex < Inventory.Num()) &&
				  ((CombatState == ECombatState::ECS_Unoccupied) ||
					  (CombatState == ECombatState::ECS_Equipping));
		  }
	}
	else
	{
		  bAllowExchange = (CurrentItemIndex != NewItemIndex &&
									NewItemIndex < Inventory.Num() && 
									CombatState == ECombatState::ECS_Unoccupied &&
									!bAiming);

		  if (AllowExchangeWhileAiming)
		  {
			  bAllowExchange = (CurrentItemIndex != NewItemIndex &&
				  NewItemIndex < Inventory.Num() &&
				  CombatState == ECombatState::ECS_Unoccupied);
		  }

	}
	if (bAllowExchange)
	{
		// Stop Aiming if the player is aiming while changing weapons
		if (bAiming)
		{
			StopAiming();
		}

		AWeapon* CurrentWeapon = EquippedWeapon;
		AWeapon* NewWeaponToEquipp = Cast<AWeapon>(Inventory[NewItemIndex]);
			
		EquipWeapon(NewWeaponToEquipp);

		CurrentWeapon->SetItemState(EItemState::EIS_Carrying);
		NewWeaponToEquipp->SetItemState(EItemState::EIS_Equppied);

		CombatState = ECombatState::ECS_Equipping;

		PlayEquippingMontage();
	}
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	// Looking for empty slots in the Inventory
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	// Next Free Slot in the Inventory
	if (Inventory.Num() < IVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}

	// Inventory is full
	return -1;
}

bool AShooterCharacter::IsInventoryFull()
{
	if (Inventory.Num() < IVENTORY_CAPACITY)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool AShooterCharacter::IsInventoryEmpty()
{
	if (Inventory.Num() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HighlightItemDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightItemDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}

void AShooterCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;

	// Reset MovementSpeed
	ResetMovementSpeedBasedOnStance();	
	
	// Continue Aming
	if (bAimingButtonPressed)
	{
		Aiming();
	}
	// Continue Firing
	if (bFireButtonPressed)
	{
		AutoFireReset();
	}
}

void AShooterCharacter::ResetMovementSpeedBasedOnStance()
{
	if (bCrouching && bAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = AimCrouchMovementSpeed;
	}
	if (!bCrouching && bAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = AimWalkMovementSpeed;
	}
	if (bCrouching && !bAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
	}
	if (!bCrouching && !bAiming)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::PlayCharacterDeathMontage(float Playrate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && CharacterDeathMontage)
	{
		int64 Counter = FMath::RandRange(0, CharacterDeathMontageSections.Num() - 1);
		
		AnimInstance->Montage_Play(CharacterDeathMontage, Playrate);
		AnimInstance->Montage_JumpToSection(CharacterDeathMontageSections[Counter]);

		// Stop Animation after MontageSection has reached the End
		float DeathSectionLength = CharacterDeathMontage->GetSectionLength(CharacterDeathMontage->GetSectionIndex(CharacterDeathMontageSections[Counter]));
		FTimerHandle StopAnimationTimerHandle;

		// Add a small Buffer to make sure the Montage never finshes before the Timer
		GetWorldTimerManager().SetTimer(StopAnimationTimerHandle, this, &AShooterCharacter::StopAnimation, DeathSectionLength - 0.3f, false);
	}
}

void AShooterCharacter::PlayHitReactMontage(float PlayRate)
{
	if (bCharacterIsDead)
	{
		return;
	}

	//Set CombatState and Reduce Speed
	SetCombatStateToStunned();
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// Determine Random MontageSection
	// TODO: Figure Attack Angle, to determine the correct HitAnimation
	int64 SectionNumber = FMath::RandRange(0, HitReactionMontageSections.Num()-1);
	FName SectionName = HitReactionMontageSections[SectionNumber];
	
	if (HitReactMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(HitReactMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AShooterCharacter::SetCombatStateToStunned()
{
	// Set State
	CombatState = ECombatState::ECS_Stunned;

	// Reduce Speed, By DefaultHalf it
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * StunMovementPenaltyFactor;
}

bool AShooterCharacter::RollForStunResistance(float EnemyStunChance)
{
	if (bCrouching)
	{
		return false;
	}

	// Indirect indicator that the is / was jumping
	// Disallow Stunning when Character jumps
	if (!bCanJump)
	{
		return false;
	}

	// Allow Check only if Character is not Stunned
	if (CombatState != ECombatState::ECS_Stunned)
	{
		EnemyStunChance -= StunResistance;
		// UE_LOG(CombatLog, Warning, TEXT("INPUTWeaponStunChance: %f"), EnemyStunChance);

		// Check Limits
		if (EnemyStunChance < 1.f)
		{
			EnemyStunChance = 1.f; // 1% Chance
		}
		if (EnemyStunChance > 99.f)
		{
			EnemyStunChance = 99.f; // 99% Chance
		}

		float RandomRoll = FMath::FRandRange(0.f, 100.f);

		if (EnemyStunChance >= RandomRoll)
		{
			// Stun sucessful
			// UE_LOG(CombatLog, Warning, TEXT("Stun sucessful: Rolled: %f, EnemyStunChance: %f"), RandomRoll, EnemyStunChance);
			return true;
		}
		else
		{	
			// Stun failed
			// UE_LOG(CombatLog, Warning, TEXT("Stun failed: Rolled: %f, EnemyStunChance: %f"), RandomRoll, EnemyStunChance);
			return false;						
		}
	}

	// UE_LOG(CombatLog, Warning, TEXT("Stunned failed, Character already stunned!"));
	return false;
}

void AShooterCharacter::CharacterDeath()
{
	if (!bCharacterIsDead)
	{
		PlayCharacterDeathMontage();
		bCharacterIsDead = true;
		bAimingButtonPressed = false;
		// BroadCastDelegate
		CharacterIsDeadDelegateBroadcast();
		// Disable Certain Inputs
		DisableCustomInput();

		//Call to Controller to show EndScreen UI
		AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->ShowEndScreen_Lost();
		}
	}	
}

void AShooterCharacter::StopAnimation()
{
	GetMesh()->bPauseAnims = true;
}

void AShooterCharacter::DisableCustomInput()
{
	// Allows Rotating the Camera without moving the Character
	bUseControllerRotationYaw = false;
	// DisableForward and SidewardMovement
	bAllowMovementInput = false;
	// Disable Jumping
	bAllowJumpInput = false;
}

void AShooterCharacter::CharacterIsDeadDelegateBinding(AEnemy* Source)
{
	if (Source)
	{		
		CharacterIsDeadMultiDelegate.AddDynamic(Source, &AEnemy::MainCharacterIsDead);

#if EDITOR 
		// Check if DelegeeBind did work
		bool Success = CharacterIsDeadMultiDelegate.IsAlreadyBound(Source, &AEnemy::MainCharacterIsDead);
		UE_LOG(DelegateLog, Log, TEXT("MultiDelegateBound did work on %s !: %s"), *Source->GetActorLabel(), (Success ? TEXT("True") : TEXT("False")));
#endif
	}
}

void AShooterCharacter::CharacterIsDeadDelegateBinding(ASpawner* Source)
{
	if (Source)
	{
		CharacterIsDeadMultiDelegate.AddDynamic(Source, &ASpawner::MainCharacterIsDead);
#if EDITOR 
		// Check if DelegeeBind did work
		bool Success = CharacterIsDeadMultiDelegate.IsAlreadyBound(Source, &ASpawner::MainCharacterIsDead);
		UE_LOG(DelegateLog, Log, TEXT("MultiDelegateBound did work on %s !: %s"), *Source->GetActorLabel(), (Success ? TEXT("True") : TEXT("False")));
#endif
	}
}
void AShooterCharacter::CharacterIsDeadDelegateBroadcast()
{
	// Call all binded functions with the parameter true;
	CharacterIsDeadMultiDelegate.Broadcast(true);
}

int32 AShooterCharacter::GetPlayerMoney()
{
	return PlayerStats.PlayerMoney;
}

int32 AShooterCharacter::AlterPlayerMoney(int32 Value)
{
	PlayerStats.PlayerMoney -= Value;

	return PlayerStats.PlayerMoney;
}

int32 AShooterCharacter::AddPlayerMoney(int32 Value)
{	//STATSNOTIFY
	// Give Player more Money for each Wisdom he has
	if (PlayerStats.Wisdom > 0)
	{
		PlayerStats.PlayerMoney += Value + FMath::FloorToInt(PlayerStats.Wisdom * 1.2f);
	}
	else
	{
		PlayerStats.PlayerMoney += Value;
	}

	return PlayerStats.PlayerMoney;
}

float AShooterCharacter::AddPlayerHealth(float HealAmount)
{	
	//STATSNOTIFY
	CurrentHitPoints += HealAmount;

	CurrentHitPoints = FMath::Clamp<float>(CurrentHitPoints, 0.f, PlayerStats.Health);

	return CurrentHitPoints;
}


bool AShooterCharacter::HasEnoughMoney(int32 Cost)
{
	int32 PlayerMoney = PlayerStats.PlayerMoney;
	int32 AmountAfterCast = PlayerMoney - Cost;

	if (AmountAfterCast < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int32 AShooterCharacter::GetPlayerExperience()
{
	return PlayerStats.PlayerExperience;
}

int32 AShooterCharacter::GetPlayerLevel()
{
	return PlayerStats.PlayerLevel;
}

void AShooterCharacter::ResetPlayerExperience()
{
	PlayerStats.PlayerExperience = 0;
}

int32 AShooterCharacter::AdvanceLevel(int32 AmountOfLevels)
{
	PlayerStats.PlayerLevel = PlayerStats.PlayerLevel + AmountOfLevels;

	// Subtract needed Xp, but keep Xp that is more than the required XP
	PlayerStats.PlayerExperience -= XPforNextLevelUp;

	// Calculate next XP needed for Level up // Subtract 1 to get the wanted Behavior: Level 1 -> 100 XP, Level 2 ->110XP,...
	XPforNextLevelUp = (XpIncreasementForNextLevelUp * (PlayerStats.PlayerLevel - 1)) + XPForNextLevelUpBase;

	return GetPlayerLevel();
}

int32 AShooterCharacter::AddPlayerExperience(int32 Experience)
{
	PlayerStats.PlayerExperience += Experience;

	if (PlayerStats.PlayerExperience >= XPforNextLevelUp)
	{
		AdvanceLevel(1);
	}

	return PlayerStats.PlayerExperience;
}

void AShooterCharacter::UpdatePlayerStats()
{
	//STATSNOTIFY
	//Athletic
	float MovementBonusWalking = 3.5f * PlayerStats.Athletic;
	BaseMovementSpeed = TempBaseMovementSpeed + MovementBonusWalking;

	float MovementBonusCrouching = 2.5f * PlayerStats.Athletic;
	CrouchMovementSpeed = TempCrouchMovementSpeed + MovementBonusCrouching;

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed;

	// Armor
	ArmorValuePercentage = PlayerStats.Athletic * 2.5f + 10; // 10 = BaseArmor

	// Health
	MaxHitPoints = PlayerStats.Health;
}

void AShooterCharacter::CreateInterpolationComponents()
{
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(FollowCamera);

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component1"));
	InterpComp1->SetupAttachment(GetCameraComp());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component2"));
	InterpComp2->SetupAttachment(GetCameraComp());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component3"));
	InterpComp3->SetupAttachment(GetCameraComp());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component4"));
	InterpComp4->SetupAttachment(GetCameraComp());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component5"));
	InterpComp5->SetupAttachment(GetCameraComp());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component6"));
	InterpComp6->SetupAttachment(GetCameraComp());
}

void AShooterCharacter::SetCameraProperties()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	if (PlayerController)
	{
		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->ViewPitchMax = ViewPitchMax;
			PlayerController->PlayerCameraManager->ViewPitchMin = ViewPitchMin;
		}
	}
	if (FollowCamera)
	{
		CameraDefaultFov = FollowCamera->FieldOfView;
		CurrentCameraFov = CameraDefaultFov;
	}
}