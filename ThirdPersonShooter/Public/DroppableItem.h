// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "DroppableItem.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API ADroppableItem : public AItem
{
	GENERATED_BODY()

private:
	void InitializeTSet();
	

protected:
	// Called when overlapping AreaSphere
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Set a global Ref to the MainCharacter, to save on casting in OnSphereOverlap
	void SetMainCharacterRef();

	// Enable Custom Depth on StaticMesh
	void EnableCustomDepth();

	// Disable Custom Depth on StaticMesh
	void DisableCustomDepth();

	// Scales the Mesh random between a Range
	void SetRandomStaticMeshSize();

	FVector CalculateRandomThrowAngle();

	void ActivateAreaSphereCollision();

	virtual void DoItemRoutine();

	void CheckingReferenzes();

public:

	ADroppableItem();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	class UStaticMeshComponent* GetStaticMeshComponent() { return StaticMeshComponent; };

	// Deactivate the AreaSphere Collision and activates itself after a short duration
	// Do this to avoid instance Uppicking!
	void MakeItemUnPickableforShortTIme();

	void InitializeDynamicSpawnParameter(ADroppableItem* Item);

	// TEST FUNCTION
	void SpawingItems(FTransform Transform, AActor* SpawnParent);

	float GetMeshMaxScaleSize() { return MeshMaxScaleSize; };

	AShooterCharacter* GetMainCharacter() {	return MainCharacter; };
	

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	// Referenz to the PlayerCharacter
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* MainCharacter;

	// DownCastVersion of MainCharacter
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	AActor* PlayerDownCast;
		
	// The Min Scale Value of the Mesh when Spawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MeshRandomSpawnDeviationMin;

	// The Power the Items get throw away from the Enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float ForceSize;

	// The Max Scale Value of the Mesh when Spawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MeshRandomSpawnDeviationMax;

	// The Max Scale Amount the Mesh can by applyed
	// Used in CashDrop for example, scales the Mesh based on the AMount of Money itself contains
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float MeshMaxScaleSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Loot", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AAmmoDrop> AmmoDrop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Loot", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ACashDrop> CashDrop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Loot", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AHealthDrop> HealthDrop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.f))
	float ItemDropChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.f))
	float HealthDropChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.f))
	float CashDropChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.f))
	float AmmoDropChance;

	// Used in InitializeTSet, 
	TSet <TPair<float, TSoftClassPtr<ADroppableItem>>> ItemSet;

	float AbsoluteSpawnPercentageOfAllItems;




	
};
