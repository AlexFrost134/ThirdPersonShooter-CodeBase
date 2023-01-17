#pragma once


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm		UMETA(Displayname = "9mm"),
	EAT_AR		UMETA(Displayname = "AssaultRifle"),
	EAT_SMG		UMETA(Displayname = "SMG"),
	EAT_INFINITE UMETA(Displayname = "Infinite"),
	EAT_Default	UMETA(Displayname = "Default")

};