// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "WeaponType.h"
#include "Weapon.generated.h"


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

		FWeaponDataTable() :
		AmmoType(EAmmoType::EAT_Default),
		AmmunitionInMagazin(0),
		MagazingCapacity(0),
		ItemName(""),
		MagazineBoneName(EName::None),
		ReloadMontageSectionName(EName::None),
		MaterialIndex(0),
		AutoFireFrequency(0.f),
		CrosshairIncreaseValue(0.f),
		CrosshairCoolDownSpeed(0.f),
		CrosshairMaxSpreadAmount(0.f),
		WeaponBaseSpreadFactor(0.f),
		WeaponInAirSpreadFactor(0.f),
		WeaponAimSpreadFactor(0.f),
		WeaponCrouchSpreadFactor(0.f),
		BoneToHide(EName::None),
		bAutomaticFire(false),
		WeaponDamage(0),
		CriticalShotDamage(0),
		LuckyHitChance(0.f),
		LuckyHitMultiplier(0.f)
		{ }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmunitionInMagazin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazingCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* EquipSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* PickUpSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EmptyWeaponSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MagazineBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSectionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmunitonIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimationBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsCenter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsLeft = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsRight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsTop = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairBottom = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFlash = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound = nullptr;

	// Amount that the crosshair spreads with each shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrosshairIncreaseValue;

	// InterpValue, determines how fast the Crosshair shrinks, when stopped shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrosshairCoolDownSpeed;

	// Maximal Amount the crosshair can spread while firingweapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrosshairMaxSpreadAmount;

	// Base Amount of Crosshair SpreadHair of this Weapon; Greater than 1 increase Value, lower than 1 decreases Value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponBaseSpreadFactor;

	// Factor Amount, that influcences the crosshair while jumping; Greater than 1 increase Value, lower than 1 decreases Value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponInAirSpreadFactor;

	// Factor Amount, that influcences the crosshair while aiming; Greater than 1 increase Value, lower than 1 decreases Value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponAimSpreadFactor;

	// Factor Amount, that influcences the crosshair while crouching; Greater than 1 increase Value, lower than 1 decreases Value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponCrouchSpreadFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomaticFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CriticalShotDamage;

	// The CritChance in %, to land a Critical Hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LuckyHitChance;

	// The Factor the Damage Amount gets Multiplied
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LuckyHitMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* WeaponTierDataTable = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponTierDataTable : public FTableRowBase
{
	GENERATED_BODY()

		FWeaponTierDataTable() :
		MagazingCapacity(0),
		AutoFireFrequency(0),
		WeaponDamage(0),
		CriticalShotDamage(0),
		LuckyHitChance(0),
		LuckyHitMultiplier(0),
		CrosshairIncreaseValue(0),
		CrosshairCoolDownSpeed(0),
		CrosshairMaxSpreadAmount(0),
		ItemTier(0),
		WeaponType(EWeaponType::EWT_Default),
		WeaponName(""),
		WeaponCost(0){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MagazingCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AutoFireFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CriticalShotDamage;

	// The CritChance in %, to land a Critical Hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LuckyHitChance;

	// The Factor the Damage Amount gets Multiplied
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LuckyHitMultiplier;

	// Amount that the crosshair spreads with each shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CrosshairIncreaseValue;

	// InterpValue, determines how fast the Crosshair shrinks, when stopped shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CrosshairCoolDownSpeed;

	// Maximal Amount the crosshair can spread while firingweapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CrosshairMaxSpreadAmount;

	// ItemTier of this Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemTier;

	// WeaponType of this Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	// Name of this Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;

	// Cost to buy this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponCost;

};


UCLASS()
class THIRDPERSONSHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
		AWeapon();

	virtual void Tick(float DeltaTime) override;
protected:
	
	virtual void BeginPlay() override;

	void StopFalling();

	virtual void CreateDynamicMaterial() override;

	// Deprecated
	// void SetGlowMaterialProperties() override;
	
	// Get Called Before BeginPlay and EveryTime the Object Transform changed
	virtual void OnConstruction(const FTransform& Transform) override;

	void FinishMovingSlide();

	void FinishRecoilRotation();

	// UpdateSlide Displacement every Tick
	void UpdateSildeDisplacement();

	void UpdateWeaponRecoilRotation(float DeltaTime);

	// Hide Bones, gets called On Construction
	virtual void HideBones() override;

	// Checks if the Member BonetoHide is a Valid Name, if not return false
	bool BoneNameIsValid(FName BoneName);


	// Weapon Initialization
	// Call this direct after Construction to setup Values of the Weapon
	void SetWeaponTierProperties(EItemRarity ItemTier);

	void SetupWeaponProperties(EWeaponType WeaponType);

	void ReadFromTierDataTable(); // Should this be public??

	void ReadFromWeaponValuesDataTable();

	void ReadFromItemPropertiesDataTable();

	
private:

	// Target Recoil Value; Should be initilaize with 0.f;
	float RecoilTargetX;
	float RecoilTargetY;
	float RecoilTargetZ;

	FTimerHandle ThrowWeaponTimer;
	// Amount of time the Physics Engine Takes over the Object
	float ThrowWeaponTime;
	// Is Weapon in bFalling State? Default false
	bool bFalling;
	// Reference to Character that hold the weapon
	class AShooterCharacter* Owner;
	// Reference to Mesh of the Weapon
	class USkeletalMeshComponent* Mesh;

	// Type of Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	// Ammunition Ammout in Clip 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 0, UIMax = 999, ClampMin = 0, ClampMax = 999, Delta = 5, EditCondition = bOverrideWeaponProperties))
	int32 AmmunitionInMagazin;

	// Maxium Capacity of the Clip of this gun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 1, UIMax = 999, ClampMin = 1, ClampMax = 999, Delta = 5, EditCondition = bOverrideWeaponProperties))
	int32 MagazineCapacity;

	// Ammount of Bullets the clip loses when fired
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 0, UIMax = 10, ClampMin = 0, ClampMax = 10, EditCondition = bOverrideWeaponProperties))
	int32 AmmunitionFireCost;

	// Cost to buy this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 WeaponCost;
		
	// Ammo Type that is used by this weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;
	
	// FName for the Reload Section in the Reload Montage
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSectionName;

	// Boolean for ReloadMontage, determines if clip is moving
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bMovingClip;

	// Name of the Magazine Bone
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName MagazineBoneName;

	// Data Table for Weapon Propterties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponDataTable", meta = (AllowPrivateAccess))
	UDataTable* WeaponDataTable;

	// Set form WeaponDataTable during Construction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponDataTable", meta = (AllowPrivateAccess))
	UDataTable* WeaponTierDataTable = nullptr;

	// if true a new Value can be set to AmmunitionInMagazin 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bOverrideWeaponProperties;

	// Last MaterialIndex that was used
	int32 PreviousMaterialIndex;

	// Crosshair Middle Element
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairsCenter;

	// Crosshair Left Element
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairsLeft;

	// Crosshair Right Element
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairsRight;

	// Crosshair Top Element
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairsTop;

	// Crosshair Bottom Element
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairBottom;

	// Time it takes until next shoot can happen
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 0, UIMax = 999, ClampMin = 0, ClampMax = 999, Delta = 1, EditCondition = bOverrideWeaponProperties))
	float AutoFireFrequency;

	//DEPCRICATED
	// Amount of Time so that the crosshair has enough time to shrink back. // DefaultValue  = 0.05f
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 0, UIMax = 999, ClampMin = 0, ClampMax = 999, Delta = 1, EditCondition = bOverrideWeaponProperties))
	// float CrosshairResetTime;

	// Amount that the crosshair spreads with every gunfire; Default Value = 0.25f
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 0.05, UIMax = 10, ClampMin = 0.001, ClampMax = 100, Delta = 0.05, EditCondition = bOverrideWeaponProperties))
	float CrosshairIncreaseValue;
		
	// Speed how fast the Crosshairs shrinks back to the default Position; Default = 10.f
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 0.1, UIMax = 50, ClampMin = 0.1, ClampMax = 100, Delta = 0.05, EditCondition = bOverrideWeaponProperties))
	float CrosshairCoolDownSpeed;

	// Max Amount that the Crosshair can spread by firing the gun; Default = 5.f
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", UIMin = 0.1, UIMax = 50, ClampMin = 0.1, ClampMax = 100, Delta = 0.1, EditCondition = bOverrideWeaponProperties))
	float CrosshairMaxSpreadAmount;

	// Base Amount of Crosshair SpreadHair of this Weapon; A Value greater than 1 increase the Spread Amount, A value less than 1 decreases the Spread Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WeaponBaseSpreadFactor;

	// Factor Amount, that influcences the crosshair while jumping; A Value greater than 1 increase the Spread Amount, A value less than 1 decreases the Spread Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WeaponInAirSpreadFactor;

	// Factor Amount, that influcences the crosshair while aiming; Default Value 1, A Value of 1 means crouching has Zero Effect, A Value of 2 means the CrouchingCrosshair Bonus is doubled on the CrossHairSpread, Negativ Value will reverse the effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WeaponAimSpreadFactor;

	// Factor Amount, that influcences the crosshair while crouching; A Value greater than 1 decreases  the Spread Amount, A value less than 1 increase the Spread Amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float WeaponCrouchSpreadFactor;

	// true for automatic GunFire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", EditCondition = bOverrideWeaponProperties))
	bool bAutomaticGunFire;

	// Flash thats spawn on the Muzzle
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	// Sound play when fired
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	// Sound play when gun has no bullets in Magazine / Clip
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EmptyWeaponSound;

	// The Default Empty Weapon Sound 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* DefaultEmptyWeaponSound;

	// Bone to hide on the WeaponMesh
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName BoneToHide;

	// Amount that the Pistol slide is pushed back during pistol fire
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacement;

	// Curve for the slide Displacement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SlideDisplacementCurve;
	
	// Timmer for updating Pistol SlideDisplacement
	FTimerHandle SlideTimerHandle;

	// Time for displacing the slide during pistol fire
	float SlideDisplacementTime;

	// true if slide is moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	bool bMovingSlide;

	// Scale Factor that the slides Moves at Maximum
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacementMaxValue;

	// Curve for the Gun Recoil Behavior
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GunRecoilRotation;

	// Base Rotation for Pistol Recoil; Scaling Value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float MaxRecoilRotation;

	// Base X-Axis Rotation for Pistol Recoil;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float BaseRecoilRotationX;
	
	// Base Y-Axis Rotation for Pistol Recoil;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float BaseRecoilRotationY;

	// Base Z-Axis Rotation for Pistol Recoil;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float BaseRecoilRotationZ;

	// Current Recoil Rotation of the weapon; Rotation along the X-Axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float RecoilRotationX;

	// Current Recoil Rotation of the weapon; Rotation along the Y-Axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float RecoilRotationY;

	// Current Recoil Rotation of the weapon; Rotation along the Z-Axis
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float RecoilRotationZ;
	
	// true if Gun has recoil Animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bGunHasRecoil;

	// reduces the Recoil effect on the weapon when the player crouches; Scaling factor of 2.f means the effect is halfed!
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	float RecoilCrouchScalingAmount;

	bool bCalculatedRecoilRandomAmount;

	FTimerHandle RecoilTimerHandel;

	// Time until the recoil animation is finished
	float RecoilRotationTime;

	// Amount of Damaged caused by a bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", EditCondition = bOverrideWeaponProperties))
	float WeaponDamage;

	// Amount of Damaged when a bullet hits the head
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", EditCondition = bOverrideWeaponProperties))
	float CriticalShotDamage;

	// The CritChance in %, to land a Critical Hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", EditCondition = bOverrideWeaponProperties))
	float LuckyHitChance;

	// The Factor the Damage Amount gets Multiplied
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true", EditCondition = bOverrideWeaponProperties))
	float LuckyHitMultiplier;	

public:// GetSet Function

	void PostBeginPlayerIntialisierung(EItemRarity ItemTier, EWeaponType InWeaponType);

	// Sets Owner of the Weapon
	void SetNewOwner(AShooterCharacter* NewOwner);

	// Adds an Impusle to the Weapon
	// Only Allow Throwing when PlayerCharacter is Owner of the Weapon
	void ThrowWeapon();

	// By Calling this function the Rotation of the SkeletalMesh will be resetet to 0.
	void ResetLocalMeshRotation();

	UFUNCTION(BlueprintCallable)
	int32 GetAmmunitionInMagazin() const { return AmmunitionInMagazin; };

	void SetAmmunitionInMagazin(int32 Amount);
	
	// Called from Character class when firing Weapon
	void DecrementAmmunition();

	// Call this Function to Check if the Hit was a CricialHit by Chance, Return true if it is, returns false if not.
	bool RollLuckyHit();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };

	void SetWeaponType(EWeaponType InWeaponType) { WeaponType = InWeaponType; };

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };

	FORCEINLINE FName GetReloadMontageSectionName() const { return ReloadMontageSectionName; };

	FORCEINLINE int32 GetMaxMagazineCapacity() const {	return MagazineCapacity; };

	FORCEINLINE FName GetMagazineBoneName() const {	return MagazineBoneName; };

	void SetbMovingClip(bool State) { bMovingClip = State; };

	// Return true if the Magazin is full
	bool GunHasFullMagazine();

	void StartSlideTimer();

	// Plays the Empty Weapon Sound, if there is no selected the DefaulEmptyWeaponSound will be played
	void PlayEmptyWeaponSound();

	// Start the recoil AnimationTimer
	void StartRecoilRotationTimer();

	FORCEINLINE float GetAutoFireFrequency() const { return AutoFireFrequency; };

	// DEPCRICATED
	// FORCEINLINE float GetCrosshairResetTime() const { return CrosshairResetTime; };

	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; };

	FORCEINLINE USoundCue* GetFireSound() const { return FireSound; } ;

	FORCEINLINE float GetShootingFactorCrosshairIncreaseValue() const {	return CrosshairIncreaseValue;} ;

	FORCEINLINE float GetShootingFactorCoolDownSpeed() const { return CrosshairCoolDownSpeed; };

	FORCEINLINE float GetShootingFactorMaxSpreadAmount() const { return CrosshairMaxSpreadAmount; };
		
	FORCEINLINE float GetWeaponBaseSpreadFactor() const { return WeaponBaseSpreadFactor; };

	FORCEINLINE float GetWeaponInAirSpreadFactor() const { return WeaponInAirSpreadFactor; };

	FORCEINLINE float GetWeaponAimSpreadFactor() const { return WeaponAimSpreadFactor; };

	FORCEINLINE float GetWeaponCrouchSpreadFactor() const { return WeaponCrouchSpreadFactor; };

	FORCEINLINE bool IsAutomaticGun() const { return bAutomaticGunFire ? true : false ; };

	FORCEINLINE float GetWeaponDamage() const { return WeaponDamage; };

	FORCEINLINE float GetCriticalShotDamage() const { return CriticalShotDamage; };

	FORCEINLINE float GetLuckyHitChance() const { return LuckyHitChance; };

	FORCEINLINE float GetLuckyHitMultiplier() const { return LuckyHitMultiplier; };

	FORCEINLINE int32 GetWeaponCost() const { return WeaponCost; };

	// Converts ItemRarity to Int32, EPic = 5.
	UFUNCTION(BlueprintCallable)
	int32 RarityToInt32(EItemRarity InItemRarity);

};

