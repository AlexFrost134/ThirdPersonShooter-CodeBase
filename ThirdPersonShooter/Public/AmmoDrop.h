// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroppableItem.h"
#include "AmmoType.h"
#include "AmmoDrop.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AAmmoDrop : public ADroppableItem
{
	GENERATED_BODY()

public:
		AAmmoDrop();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	void DoItemRoutine() override;

	virtual void SetItemProperties(EItemState state);

	void RollAmmoType();

	void SetRandomAmmunitionAmount();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PistolAmmoStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* ARAmmoStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* SMGAmmoStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true"))
	 EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	int32 AmmunitionAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	int32 RandomDeviationMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	int32 RandomDeviationMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	float PickUpRange;

	// NotImplementet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	float IncreaseAmmoCountByChance;

public:

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };

	int32 GetAmmunitionAmount() const { return AmmunitionAmount; };

	
};
