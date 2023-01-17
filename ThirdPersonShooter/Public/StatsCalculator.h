// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class THIRDPERSONSHOOTER_API StatsCalculator
{
public:
	StatsCalculator();

	// Return the Amount of Damage that was blocked by the armor
	static float ApplyArmorBlocked(float InDamage, float InArmor);

	// Return the Amount of Damage that got through the Armor
	static float ApplyArmorPierced(float InDamage, float InArmor);

	static float CalculateDamage(struct FHitZone& HitZone);

private:
	static float DamageFromCharacter(FHitZone& HitZone);

	static float DamageFromEnemy(class AEnemy* Enemy);

	static void DamageCalculatebyZone(FHitZone& OutHitZoneRef);

	inline static bool bShowDebugMessages = false;
		
	~StatsCalculator();
};
