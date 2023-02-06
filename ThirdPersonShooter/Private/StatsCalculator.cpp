// Fill out your copyright notice in the Description page of Project Settings.

#include "StatsCalculator.h"
#include "Enemy.h"
#include "CustomLogs.h"
#include "ShooterCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Weapon.h"	

StatsCalculator::StatsCalculator()
{
}

StatsCalculator::~StatsCalculator()
{
}

float StatsCalculator::ApplyArmorBlocked(float InDamage, float InArmor)
{
	float Armor = 0.f;

	float MaxAllowedArmorValue = 100.f;

	if (InDamage <= 0.f)
	{
		// No Damage or Negativ InDamge, so no Damage was blocked
		return 0.f;
	}

	if (InArmor <= -0.000'001f)
	{
		// Negtaiv Armor Value
		UE_LOG(LogTemp, Error, TEXT("Error in StatsCalculator::ApplyArmorBlocked! Negativ Armor Value"));
		Armor = 0.f;
	}

	if (InArmor > MaxAllowedArmorValue)
	{
		Armor = MaxAllowedArmorValue;
	}

	// 100 Damage, 5 % Armor -> 5 Damage reducation. Divded by 100 to get %
	float ReducedDamageByArmor = InDamage * Armor / 100;

	if (ReducedDamageByArmor < 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("Error in StatsCalculator::ApplyArmorBlocked! ReducedDamageByArmor is less than 0!"));
		return 0.f;
	}

	return ReducedDamageByArmor;
}

float StatsCalculator::ApplyArmorPierced(float InDamage, float InArmor)
{
	float Damage = InDamage;
	float Armor = 0.f;
	float MaxAllowedArmorValue = 100.f;

	if (Damage <= 0.f)
	{
		// No Damage or Negativ InDamge, so no Damage was blocked
		return 0.f;
	}

	if (InArmor <= -0.000'001f)
	{
		// Negtaiv Armor Value
		Armor = 0.f;
	}

	if (InArmor > MaxAllowedArmorValue)
	{
		Armor = MaxAllowedArmorValue;
	}

	// 100 Damage, 5 % Armor -> 5 Damage reducation. Divded by 100 to get %
	float DamagePiercedThroughArmor = Damage - (Damage * InArmor / 100);

	if (DamagePiercedThroughArmor <= 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("Error in StatsCalculator::ApplyArmorPierced!"));
		// Damage should not be smaller tha 0.f
		return 0.f;
	}

	return DamagePiercedThroughArmor;
}

// Need to Move code, that only are used either by Enemy or Shooter, no need to have it here
float StatsCalculator::CalculateDamage(FHitZone& HitZone)
{
	float DamageAmount = 0.f;
	float AdversaryArmorValue = 0.f;
	bool bDamageFromEnemy = false;

	auto AdversaryEnemy = Cast<AEnemy>(HitZone.DamagedActor);
	auto AdversaryChar = Cast<AShooterCharacter>(HitZone.DamagedActor);

	auto InstigatorEnemy = Cast<AEnemy>(HitZone.InstigatorRef);
	auto InstigatorChar = Cast<AShooterCharacter>(HitZone.InstigatorRef);

	// Damage to Enemy
	if (AdversaryEnemy)
	{
		// Needs be done first to fill HitZone with Informations abour the Zone
		DamageCalculatebyZone(HitZone);
		AdversaryArmorValue = HitZone.EnemyArmor;
	}
	// Damage To Player
	if (AdversaryChar)
	{
		AdversaryArmorValue = AdversaryChar->GetArmorValue();
	}

	// Damage from Player
	if (InstigatorChar)
	{		
		DamageAmount = StatsCalculator::DamageFromCharacter(HitZone);		
	}

	// Damage from Enemy
	if (InstigatorEnemy) 
	{
		DamageAmount = StatsCalculator::DamageFromEnemy(InstigatorEnemy);			
	}
	
	/// Add Modifer
	// Take Armor into Account.
	DamageAmount -= StatsCalculator::ApplyArmorBlocked(DamageAmount, AdversaryArmorValue);

	// At Least One Damage
	if (DamageAmount < 1.f)
	{
		DamageAmount = 1.f;
	}

	return DamageAmount;
}

float StatsCalculator::DamageFromCharacter(FHitZone& HitZone)
{
	auto Character = Cast<AShooterCharacter>(HitZone.InstigatorRef);

	if (Character)
	{
		if (HitZone.DamageMultiplier <= .000'1f)
		{
#if EDITOR 
			UE_LOG(CombatLog, Error, TEXT("DamageMultiplier not set on %s"), *HitZone.DamagedActor->GetActorLabel());
#endif
			HitZone.DamageMultiplier = 1.f;
		}

		float Damage = 0.f;
		float DamageMultiplier = 0.f;
		float LuckyHitMultiplier = 1.f;

		// TODO: FIX below
		// Reseting for now, otherwise HitZone flag carries to the next HitZone over?? Architektur flaw
		HitZone.bLuckyHit = false;
		HitZone.bCriticalHitZone = false;

		DamageMultiplier = HitZone.DamageMultiplier;

		// GetBaseDamage
		if (Character->GetEquippedWeapon())
		{
			//STATSNOTIFY
			float AdditionalDamgeFactor = (1 + (Character->GetPlayerStats().AttackDamagePercentage / 100));
			Damage = Character->GetEquippedWeapon()->GetWeaponDamage() * AdditionalDamgeFactor;
		}
	
		// Random Chance of a CriticalHit
		if (Character->GetEquippedWeapon()->RollLuckyHit())
		{
			//STATSNOTIFY
			float AdditionalLuck = Character->GetPlayerStats().Luck;

			LuckyHitMultiplier = Character->GetEquippedWeapon()->GetLuckyHitMultiplier() + (AdditionalLuck / 10);
			HitZone.bLuckyHit = true;
		}

		// Hit a Critical HitZone?
		if (UPhysicalMaterial::DetermineSurfaceType(HitZone.PhysicalMaterial) == EPhysicalSurface::SurfaceType8)
		{
			Damage = Character->GetEquippedWeapon()->GetCriticalShotDamage();
			HitZone.bCriticalHitZone = true;
		}

		Damage = Damage * (DamageMultiplier * LuckyHitMultiplier);

		return Damage;
	}

	UE_LOG(CombatLog, Error, TEXT("StatsCalculator::DamageFromCharacter Cast failed!"));
	return 0.f;
}

void StatsCalculator::DamageCalculatebyZone(FHitZone& OutHitZoneRef)
{
	AEnemy* Enemy = Cast<AEnemy>(OutHitZoneRef.DamagedActor);

	EPhysicalSurface PhysicalSurface = UPhysicalMaterial::DetermineSurfaceType(OutHitZoneRef.PhysicalMaterial);

	if (Enemy->GetZoneDamagePtr())
	{
		FHitZone NewHitZone = **Enemy->GetZoneDamagePtr()->Find(PhysicalSurface);
		OutHitZoneRef.DamageMultiplier = NewHitZone.DamageMultiplier;
		OutHitZoneRef.EnemyArmor = NewHitZone.EnemyArmor;
	}

	if (bShowDebugMessages)
	{
		UE_LOG(CombatLog, Warning, TEXT("DamageMultiplier: %f, EnemyArmor: %f"), OutHitZoneRef.DamageMultiplier, OutHitZoneRef.EnemyArmor);
	}

	return;
}

float StatsCalculator::DamageFromEnemy(AEnemy* Enemy)
{
	// TODO:
	// Add random luckyHit chance
	return Enemy->GetBaseDamage();
}
