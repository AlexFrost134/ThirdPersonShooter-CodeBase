// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammunition.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AAmmunition : public AItem
{
	GENERATED_BODY()

public:
	AAmmunition();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	// override of SetItemProperties to set Mesh Properties
	virtual void SetItemProperties(EItemState state) override;

	// Called when overlapping AreaSphere
	UFUNCTION()
	void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Creates and set the Dynamic Material for a specific Index
	virtual void CreateDynamicMaterial() override;

private:
	// Mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	// Ammo Type for the Ammunition
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	// Texture for the Ammo Icon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	// Collision Sphere for automatic PickUp
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AmmoCollisionSphere;

	// Radius of the Sphere for AutoPickUp
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	float AutoPickUpRange;

public:

	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; };

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };

	virtual void EnableCustomDepth() override;

	virtual void DisableCustomDepth() override;
};
