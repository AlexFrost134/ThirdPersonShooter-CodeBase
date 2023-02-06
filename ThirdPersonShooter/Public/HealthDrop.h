// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroppableItem.h"
#include "HealthDrop.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AHealthDrop : public ADroppableItem
{
	GENERATED_BODY()

public:
	AHealthDrop();

	virtual void BeginPlay() override;

protected:
	void DoItemRoutine() override;

	// Increase the HealthAmount by a Defined Amount.
	void IncreaseHealthAmountByChance(float Chance = 35.f);

	void ChangeMeshSizeByCHealthAmount(int32 InHealthAmount);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDrop", meta = (AllowPrivateAccess = "true"))
	float MinHealthAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthDrop", meta = (AllowPrivateAccess = "true"))
	float HealthAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDrop", meta = (AllowPrivateAccess = "true"))
	float MaxHealthAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthDrop", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.f))
	float ChanceToIncreaseHealAmount;

	// The Radius of the PickupSphere
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float PickUpRange;	
};
