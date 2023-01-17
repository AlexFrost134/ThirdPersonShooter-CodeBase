// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroppableItem.h"
#include "CashDrop.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API ACashDrop : public ADroppableItem
{
	GENERATED_BODY()

public:

	ACashDrop();

	virtual void BeginPlay() override;

protected:
	 void DoItemRoutine() override;
	 
	 void ChangeMeshSizeByCashValue(int32 InCashValue);

	 // Increase CashMoney by a RandomChance, Chance between 0.f to 100.f. eg 35.f = 35%
	 void IncreaseCashValueByChance(float Chance = 35.f);

private:
	 // Amount of Cash this Object is Worth when picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f))
	int32 CashValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.f))
	float ChanceToIncreaseValue;

	// The Radius of the PickupSphere
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CashDrop", meta = (AllowPrivateAccess = "true"))
	float PickUpRange;
	
};
