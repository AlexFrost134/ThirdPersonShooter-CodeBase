#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubMaschineGun	UMETA(Displayname = "SMG"),
	EWT_AssaultRifle	UMETA(Displayname = "AR"),
	EWT_Pistol			UMETA(Displayname = "Pistol"),
	EWT_HeavyPistol		UMETA(Displayname = "HeavyPistol"),
	EWT_Default			UMETA(Displayname = "Default")
};