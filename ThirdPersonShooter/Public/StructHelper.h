#pragma once

 #include "StructHelper.generated.h"

USTRUCT(BlueprintType)
struct FHitZone
{
	GENERATED_BODY()

		// Constructor
	FHitZone()
	{
		DamageMultiplier = 0.f;
		EnemyArmor = 0.f;
		InstigatorRef = nullptr;
		PhysicalMaterial = nullptr;
		bLuckyHit = false;
		bCriticalHitZone = false;
	};

	// The DamageMultiplier of the Enemy of this Zone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageMultiplier;

	// The Enemy Armor Value of the this HitZone
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EnemyArmor;

	// Damage Causer
	AActor* InstigatorRef;

	// Damage Receiver
	AActor* DamagedActor;

	// Hold the PhysicalMaterial of the HitZone
	UPROPERTY()
		UPhysicalMaterial* PhysicalMaterial;

	// Flags
	// This Hit is a Lucky Hit
	UPROPERTY()
		bool bLuckyHit;

	// This Hit is on a CriticalZone
	UPROPERTY()
		bool bCriticalHitZone;

};


USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	// Constructor for Blueprint Instance Creation
	FPlayerStats()
	{
		PlayerMoney = 0;
		PlayerExperience = 0;
		PlayerLevel = 0;
		Health = 0;
		LifeSteal = 0;
		Athletic = 0;
		Endurace = 0;
		Wisdome = 0;
		Doge = 0;
		Attitude = 0;
		Luck = 0;
		AttackDamagePercentage = 0;
		Unkown_2 = 0;
	};

	// Use this at the Start of the Game
	FPlayerStats(bool PlayerStartDefault)
	{
		PlayerMoney = 15;
		PlayerExperience = 0;
		PlayerLevel = 1;
		Health = 10;
		LifeSteal = 0;
		Athletic = 0;
		Endurace = 0;
		Wisdome = 0;
		Doge = 0;
		Attitude = 100;
		Luck = 0;
		AttackDamagePercentage = 0;
		Unkown_2 = 99;
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerMoney;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerExperience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LifeSteal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Athletic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Endurace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Wisdome;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Doge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Luck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackDamagePercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Unkown_2;

	// Armor
	// StunResistance
	// Speed

};

UENUM(BlueprintType)
enum class EUIItemType : uint8
{
	EUIIT_BuyableItem		UMETA(Displayname = "BuyableItem"),
	EUIIT_BuyableWeapon		UMETA(Displayname = "BuyableWeapon"),
	EUIIT_Default			UMETA(Displayname = "None")

};
