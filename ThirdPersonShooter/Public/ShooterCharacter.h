// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ExplosionHitInterface.h"
#include "StructHelper.h"
#include "Item.h" // need that for the EItemRarity enum
#include "WeaponType.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied			UMETA(Displayname = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(Displayname ="FireTimerInProgress"),
	ECS_Reloading			UMETA(Displayname = "Reloading"),
	ECS_Equipping			UMETA(Displayname = "Equipping"),
	ECS_Stunned				UMETA(Displayname = "Stunned"),
	ECS_Default				UMETA(Displayname = "Default")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	//Default Constructor
	FInterpLocation() :
	SceneComponent(nullptr),
	ItemCount(0)
	{}

	// Set Scene and Itemcount as MemberVariables
	FInterpLocation(USceneComponent* Scene, int32 Itemcount) :
		SceneComponent(Scene),
		ItemCount(Itemcount)
	{}

	// Scene Component's location that is used for Interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent = nullptr;

	// Number of items interping to/at this scene comp location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;

};

// Declare Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightItemDelegate, int32, SlotIndex, bool, bStartAnimation);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterIsDeadMultiDelegate, bool, bState);

UCLASS()
class THIRDPERSONSHOOTER_API AShooterCharacter : public ACharacter, public IExplosionHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	// Call this function to deal damage to this character
	// @return Returns DamageAmount , return -1.f if the Character is already Dead
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called for Forward/backward Movement
	void MoveForward(float Value);

	// Called for Siedward Movement
	void MoveSideward(float Value);

	//Called via input to turn at a given rate.
	void TurnAtRate(float Rate);

	//Called via input to look up/down at a given rate.
	void LookUpArRate(float Rate);

	// Rotate Controller Component  based on rate  mouse X movement
	void MouseTurn(float Value);

	// Rotate Controller Component based on rate  mouse Y movement
	void MouseLookUp(float value);

	// Called when FireButton is pressed
	void FireWeapon();

	// Call inherited Jump function and set bCanJump
	void Jump() override;

	// Call inherited Stop Jump function and reset MovementSpeed Variables
	void StopJumping() override;

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	// Set bAiming to true
	void AimingButtonPressed();

	// Set bAiming to false
	void AmingButtonReleased();

	// Set FoV while Aiming and releaseing Aiming
	void SetAimingZoom(float DeltaTime);

	// Set BaseTurnRate and BaseLookUpRate based on aiming State 
	void SetLookRates();

	// Compute CrosshairSpread
	void CalculateCrosshairSpread(float DeltaTime);

	// Start Timer for Bullet Spread Timer
	void StartCrosshairBulletFire();

	// Callback function for Bullet Spread Timer
	UFUNCTION() // Needed due to FTimerHandle Object
	void FinishCrosshairBulletFire();

	UFUNCTION(BlueprintCallable)
	UPhysicalMaterial* GetPhysicalMaterialUnderFootsteps(FName BoneName, bool ShowDebug);
	
	// Gets Called when Fire Button is Pressed
	void FireButtonPressed();

	// Gets Called when Fire Button is Released
	void FireButtonReleased();

	// Fires and Start Automatic Fire Delay Timer
	void StartFireTimer();

	// Call back function for StartFireTimer
	UFUNCTION()// Needed due to FTimerHandle Object
	void AutoFireReset();

	// Line trace for items under the crosshair
	bool TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation);

	// Use this for calculating WeaponFireEndTrace, CrosshairSpreadMultiplier is taken into account,
	bool TraceUnderCrosshairForWeaponFire(FHitResult& OutHitResult, FVector& OutHitLocation);

	// Trace for items if OverlappedItemCount is greater than 0
	/*
	void TraceForItems();
	*/
	// Spawn Default Weapon 
	class AWeapon* SpawnDefaultWeapon();

	// Takes a weapon and attached it to the mesh
	void EquipWeapon( AWeapon* WeaponToEquip, bool bSwapping = false);

	// Detach Weapon an let it fall to the ground
	void DropWeapon();

	void SelectButtonPressed();

	void SelectButtonReleased();

	// Drops currently equipped Weapon and Equips TraceHitItem
	void SwapWeapon(AWeapon* WeaponToSwap);

	// Initialize the Ammo Map with ammo values
	void InitializeAmmoMap();

	// Check to make sure the current Weapon has Ammunition
	bool WeaponHasAmmo();

	void PlayFireSound();

	void SendBullet();

	void PlayGunFireMontage();

	void PlayGunReloadMontage();

	void PlayEquippingMontage();

	// Reload Buttion got pressed
	void ReloadButtonPressed();

	// Reloads Weapon if Ammo is left and plays ReloadMontage.
	void ReloadWeapon();

	// get called when Reloading is finished via AnimNotifier
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	// gets called when Equipping Montage finished via AnimNotifier
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
		
	// Checks to see if we have ammo of the equipped Weapon' ammo Type
	bool EnoughSpareAmmo();

	// Called from ReloadMontage with GrapClip notify
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	// Called from ReloadMontage with ReplaceClip notify
	UFUNCTION(BlueprintCallable)
	void ReplaceClip();

	// Called form Blueprint; Ends the stunned State and set it to ECS_Unoccupied
	// If AmingButton is pressed, return back to aiming
	UFUNCTION(BlueprintCallable)
	void EndStun();

	void Aiming();

	void StopAiming();

	void PickUpAmmunition(class AAmmunition* Ammo);

	// Alternative Function when Pickup Ammo that dropped from an Enemy
	void PickUpAmmunitionDrop(class AAmmoDrop* AmmoDrop);

	void CrouchButtonPressed();

	// Interps capsule Height when crouching / standing
	void InterpCapsulehalfHeight(float DeltaTime);

	// Check if the the Character has enough space to stand up
	bool EnoughSpaceforStanding();

	// Call in BeginPlay Initialize Interpolation Scene Components
	void InitializeInterpLocations();

	// Reset bShouldPlayPickupSound;
	void ResetPickSoundTimer();

	// Reset bShouldPlayEquipSound;
	void ResetEquipSoundTimer();

	// Is Called in EquippWeapon(); Disables the GlowEffect on the SpawnWeapon
	void DisableGlowEffectOnDefaultWeapon();

	// Helper function to get a better Overiew when Element is being added to the Array
	void AddElementToInventory(AWeapon* WeaponToAdd);

	// Callback Function when Key is Pressed
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();
	void SixKeyPressed();

	// Swap Weapons with the CurrentItemIndex with the NewItemIndex
	// And Equips new Weapon
	void ExchangeInventoryItem(int32 CurrentItemIndex, int32 NewItemIndex);

	// return the Slot of the next free Inventory Slot. If Inventory is full, return -1
	int32 GetEmptyInventorySlot();

	void HighlightInventorySlot();

	// Set the CombatState to Stunned, also reduces Walkspeed
	void SetCombatStateToStunned();
	
	// Reset MovementSpeed Based on different Stances
	void ResetMovementSpeedBasedOnStance();

	// Takes care of the Character Death
	void CharacterDeath();

	// Stops the Animation, gets called via a Timer in PlayDeathMontage()
	void StopAnimation();

	// DisableInputCommands when Dead
	void DisableCustomInput();

	// Execute the binded functions of the Delegate
	void CharacterIsDeadDelegateBroadcast();

	// Calculate Damage after Hitting an Enemy
	float CalculateDamage(FHitZone& FHitZone);

	// Spawn a Muzzel Flash on the BarrelSocket Transform
	void SpawnMuzzleFlashAtLocation(const FTransform& Transform);

	// Spawn a Trail effect from "Start" to Destination
	void SpawnTrailParticles(const FVector& Start, const FVector& Destination);

	// Return the Transform of the Socket on the Weapon named: "weapon_Barrel_Socket"
	FTransform GetWeaponBarrelTransform();

	// Called when Pause Button is pressed
	void GamePauseButtonPressed();

	// Get the Rarity one above of the curren Rarity
	EItemRarity RarityAbove(EItemRarity InItemRarity);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Struct that contains all PlayerStats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"))
	FPlayerStats PlayerStats;

	// SpringArmComp positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComp;

	// Camera that follows character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	// Base TurnRate of the Camera 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	
	// Base Look up/down Rate of the Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	// Sets the Camera MaxPitch
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), DisplayName ="Camera Pitch Max")
	float ViewPitchMax;

	// Sets the Camera MinPitch
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), DisplayName = "Camera Pitch Max")
	float ViewPitchMin;
	

	/// Aiming Sensitivity

	// Controller Turn Rate while Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraController, meta = (AllowPrivateAccess = "true"))
	float ControllerAimingTurnRate;

	// Controller Look UpRate while Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraController, meta = (AllowPrivateAccess = "true"))
	float ControllerAimingLookUpRate;
	
	// Controller Turn Rate while not Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraController, meta = (AllowPrivateAccess = "true"))
	float ControllerHipTurnRate;

	// Controller Look Up Sensivity while not Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraController, meta = (AllowPrivateAccess = "true"))
	float ControllerHipLookUpRate;
		

	// Mouse Turn Rate while Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraMouse, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;

	// Mouse Look UpRate while Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraMouse, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate;

	// Mouse Turn Rate while not Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraMouse, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	// Mouse Look Up Sensivity while not Aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CameraMouse, meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	// Handle Jumping
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bCanJump;

	///Crosshair

	// Determines the spread of the crosshair
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	// Velocity component of the crosshairspread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocitySpreadFactor;

	// InAir component of the crosshairspread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	// Aim component of the crosshairspread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	// Shooting component of the crosshairspread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	// Base Crosshair Spread Factor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairBaseSpreadFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	// Crosshair Crouch Spread Factor
	float CurrentCrosshairCrouchFactor;
		
	// Increase the effect of CrosshairBaseSpreadFactor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMax;
		
	// Amount of Time until Shoot Multiplyer Resets
	// Must be smaller than AutomaticFireRate 
	//Weapon->AutoFireFrequency !needs to be greater than ShootTImeDuration, that the crosshair has enough time to shrink back.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ShootTimeDuration;

	// Bullet fired? Default = false
	bool bFiringBullet;
	
	// Left mouse button or right console trigger pressed
	bool bFireButtonPressed;

	// True when we can fire, false when waiting for the timer
	bool bAllowFire;

	// TimerHandle for Crosshair effect
	FTimerHandle CrosshairShootTimer;
		
	// Sets a timer between gunshoots
	FTimerHandle AutoFireTimer;

	FTimerHandle PickUpSoundTimer;

	FTimerHandle EquipSoundTimer;

	///Particles and Sound
			
	// Animation Montage that is used when fired
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// Animation Montage when equipping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	// Animation Montage when Character is Stunned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactMontage;

	// Animation Montage when Character is dying
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CharacterDeathMontage;

	// Particle spawned upon bullet Impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	// SmokeTrail for Bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* TrailParticles;

	// ParticleSystem for Spawing blood when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticles;

	// Array with the names of the HitReaction Montage Sections, initialized in the Constructor
	TArray<FName> HitReactionMontageSections;

	// True when Aiming
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// Default FoV
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFov;

	// Zommed in FoV
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomedCameraFov;

	// Current field of view this frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CurrentCameraFov;

	// Interpolation Speed for Zooming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// True if we should trace every frame for items
	bool bShouldTraceForItems;

	// Number of overlapped Items
	int8 OverlappedItemCount;

	// The Aitem we hit last Frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AItem* ItemLastFrame;

	// equipped Weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedWeapon;

	// Can be choosen in the editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// Weapon Rarity of the StartWeapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EItemRarity StartWeaponRarity;

	// WeaponType of the StartWeapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EWeaponType StartWeaponType;

	// The item currently hit by our trace in TraceForItems (could be null)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	// Distance outward form the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	// Distance Upward from the camera
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	// Map to keep track of ammo of the different ammo typ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	// Startig Ammount of 9mm Ammunition
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 StartingAmmount9mm;

	// Startig Ammount of AR Ammunition
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 StartingAmmountAssaultRifle;

	// Starting Ammount of the SMG Amunition
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 StartingAmmountSMG;

	// Combat State, can only Reload or fire when unoccupied
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	// Animation Montage that is used when fired
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	// Initial Transform of the clip during the Reload Montage when we grab it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform MagazineTransform;

	// Scene Component to attach to the Character's hand during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	// Determines if Movement Input should be executed
	bool bAllowMovementInput;

	// True when aiming button is pressed
	bool bAimingButtonPressed;

	// Change this bool to Allow Changing Weapons while the Equipping Animation; Default = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool AllowExchangeWhileEquipping;

	// Change this bool to Allow Changing Weapons while Aiming; Default = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool AllowExchangeWhileAiming;

	// True when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	// Regular Movement Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	// Crouch Movement Speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;

	// Regular Jump Height
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseJumpVelocity;

	// Crouching Jumo Height
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchJumpVelocity;

	// Current Half Height of the Collision Capsule
	float InterpCapsuleHalfHeight;

	// Half Height of the Collision Capsule when not crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	// Half Height of the Collision Capsule when crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	// Amount of speed when Aiming and Walking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AimWalkMovementSpeed;

	// Amount of speed when Aiming and Crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AimCrouchMovementSpeed;

	/// Item Interpolation

	// Array of InterpLocation Structs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocationArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

		
	// MultiDelegate that holds functions of every Enemy on what should happen on MainCharacter Death
	UPROPERTY()
	FCharacterIsDeadMultiDelegate CharacterIsDeadMultiDelegate;

	// True when PickupSound is allowed to Play
	bool bShouldPlayPickupSound;

	// True when EquipSound is allowed to Play
	UPROPERTY()
	bool bShouldPlayEquipSound;

	// Time until PickupSound can be played again
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;
	
	// Time until EquipSound can be played again
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	// Container that holds the Item in the Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	// Maxium Amount of Item slots in the inventory
	// Never go Higher than 6!!
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 IVENTORY_CAPACITY = 5;

	// Delegate for sending slot information to InventoryUI when equipping
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	// Delegate for Play UI Animation to show free slots in the inventory
	UPROPERTY(BlueprintAssignable, Category = "Delegates", meta = (AllowPrivateAccess = "true"))
	FHighlightItemDelegate HighlightItemDelegate;

	// Index for the currentyl hightedSlot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot;

	// Is True when Inventory is Full
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true")) 
	bool bInventoryIsFull;

	// True if Character is Dead, false as long the Character is Alive
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	bool bCharacterIsDead;

	// if true, the abillity to Jump is disabled
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	bool bAllowJumpInput;

	// Contains the sections names of the CharacterDeathMontage
	TArray<FName> CharacterDeathMontageSections;

	// The Amount of the current Hit Points
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true", DisplayName = "Current Health"))
	float CurrentHitPoints;

	// The Amount of the Maxium Hit Points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true", DisplayName = "Max Health"))
	float MaxHitPoints;

	// Chance of Resisting a Stun. 0.f eq 0 stun resistance. 100.f eq. 100 % Stund resistance. Should not be higer than 100.f
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float StunResistance;

	// The Amount of Armor, reduces the Amount of Taken Damage. 100.f eq. 100% negates all DamageIncome, 1.f eq. to 1% negation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float ArmorValuePercentage;

	// Sound that is played when the Player is Hit in Melee
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* MeleeImpactSound;

	// Sound that is played when the Player pickups Cash
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundCue* CashPickupSound;

	// Sound that is played when the Player pickup Healing Item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundCue* HealthPickupSound;

	// PickupSound for Ammunition
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundCue* AmmoPickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	float RetunOfMoneyWhenSalingWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector StartPosition;
	
	// Pointer to a HitZone Struct that contains Information about the DamageCalculation, 
	// Information should be set on the Way
	UPROPERTY()
	FHitZone DetectedHitZone;
		
public:	
		
	// Delegate Binding with an Enemy
	void CharacterIsDeadDelegateBinding(class AEnemy* Source);
	// Delegate Binding with a Spawner
	void CharacterIsDeadDelegateBinding(class ASpawner* Source);

	virtual void ExplosionHit_Implementation(float Damage, float StunChance = 0.f) override;

	// Add Item to the Inventory, if full Swap Items
	void GetPickupItem(AItem* Item);

	// Called from the Shop, when Weapon bought
	// Spawn a Weapon in the next slot with given Rarity and Type
	// Return Weapon that got bought
	UFUNCTION(BlueprintCallable)
	AWeapon* BoughtWeaponInShop(EItemRarity WeaponRarity, EWeaponType WeaponType);

	// Tries to Upgrade Weapon
	UFUNCTION(BlueprintCallable)
	void UpgradeWeapon(int32 SlotNumber);

	// Add/subtracts to/from OverlappedItemCount
	void IncrementOverlappedItemCount(int8 Amount);

	// Returns the SpringArmComp
	USpringArmComponent* GetSpringArmComp() const { return SpringArmComp; };

	// Returns the FollowCameraComp
	UCameraComponent* GetCameraComp() const { return FollowCamera; };

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier();

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMax() const;
		
	FORCEINLINE bool GetAimingState() const { return bAiming; }

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; };
	
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; };
	
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; };
	
	FORCEINLINE bool GetCrouching() const {	return bCrouching; };

	FVector GetStartPosition() { return StartPosition; };

	// Calls this to determine if the Character gets stuned or not
	// @param WeaponStunChance The Chance that the Weapon has to stun the Character
	// @return Retuns true if the Character gets stuned, return false if the Character resist the stun or is already Stunned
	// Player Cannot be stunned if Jumping or Crouching
	bool RollForStunResistance(float EnemyStunChance);
	
	FInterpLocation GetInterpLocationArray(int32 index);

	// This Value is the Divisor of MaxWalkSpeed when the Character is in StunnedState. Default Value 2.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float StunMovementPenaltyFactor;

	// Return the Index from the InterpLocationArray with the smalest Amount of Item interping to
	int32 GetInterpLocationIndex();

	void IncrementInterpLocationItemCount(int32 Index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; };
		
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; };

	void StartPickUpSoundTimer();

	void StartEquipSoundTimer();

	FORCEINLINE void SetInventoryIsFull(bool bFull) { bInventoryIsFull = bFull; };

	void UnHighlightInventorySlot();

	FORCEINLINE USoundCue* GetCashPickupSound() { return CashPickupSound; };

	FORCEINLINE USoundCue* GetHealthPickupSound() { return HealthPickupSound; };

	FORCEINLINE USoundCue* GetMeleeImpactSound() { return MeleeImpactSound; };

	FORCEINLINE USoundCue* GetAmmoPickupSound() const { return AmmoPickupSound; };
	
	FORCEINLINE UParticleSystem* GetBloodParticles() const { return BloodParticles; };

	UFUNCTION()
	void OnSkeletalMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Play the HitReactMontage with a given PlaybackSpeed
	void PlayHitReactMontage(float PlayRate = 1.f);

	// Play the CharacterDeathMontage with a given PlaybackSpeed
	void PlayCharacterDeathMontage(float Playrate = 1.f);

	float GetArmorValue() const { return ArmorValuePercentage; };

	// Return Players Stats Struct
	FPlayerStats GetPlayerStats() { return PlayerStats; };

	// Return True if the Inventory has no space
	// Return False if the Inventory has space
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInventoryFull();

	// Return True if the Inventory is Empty
	// Return False if the Inventory has a Weapon in it
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInventoryEmpty();

	// Returns the Amount of Money of the Player // Reading from PlayerStatsStruct
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPlayerMoney();

	// Return the Amount of Expierence of the Player // Reading from PlayerStatsStruct
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPlayerExperience();

	// Return the PlayerLevel // Reading from PlayerStatsStruct
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPlayerLevel();

	// Resets the Amount of Expierence of the Player // Writing to PlayerStatsStruct
	UFUNCTION(BlueprintCallable)
	void ResetPlayerExperience();

	// Add an Amount of Experience to the Player // Writing to PlayerStatsStruct
	// @return Returns the Amount of Experience
	int32 AddPlayerExperience(int32 Experience);

	// Subtracts the Amount from PlayerMoney
	// @Retrun the Amount after change.
	UFUNCTION(BlueprintCallable)
	int32 AlterPlayerMoney(int32 Value);
	
	// Add the Amount to the PlayerMoney
	// @Retrun the Amount after change.
	UFUNCTION(BlueprintCallable)
	int32 AddPlayerMoney(int32 Value);

	// Add the Amount to the PlayerHealth
	// Make Sure not to over Heal
	// @Retrun the Amount after heal.
	UFUNCTION(BlueprintCallable)
	float AddPlayerHealth(float HealAmount);

	// Advances one Level, Reseting PlayerExperience // Writing to Player StatsStruct
	// Return current Level
	// Resets all Experience Points
	UFUNCTION(BlueprintCallable)
	int32 AdvanceLevel(int32 AmountOfLevels);

	// Return true if the Player Has Enough Money for the Cost, else false
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasEnoughMoney(int32 Cost);

	void AddWeaponToInventory(AWeapon* Weapon);

	// Sell Weapon in this slot Number
	UFUNCTION(BlueprintCallable)
	void SellInventorySlot(int32 SlotNumber);

	// Spawn a Pistol Weapon if the Player has sold all his weapons
	UFUNCTION(BlueprintCallable)
	void SpawnBackupWeapon();

	// TEST FUNCTION
	UFUNCTION()
	void TimerDelegateTestFunction(int32 Number);

	bool GetCharacterIsDead() { return bCharacterIsDead; };

	// Fill the Ammo storage a bit
	void LevelAmmoFillUp();

	// not a good way to update Playerstats
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerStats();

	void LifeSteal(); //STATSNOTIFY

};


