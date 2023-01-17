// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHitInterface.h"
#include "ExplosionHitInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Explosiv.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API AExplosiv : public AActor, public IBulletHitInterface, public IExplosionHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiv();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// A Pointer to the Object itself // set in the Constructor
	AExplosiv* PointerToSelf;

	// Static Mesh of the Explosive
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAcess = "true"))
	class UStaticMeshComponent* StaticMesh;

	// Actor with in these Reach will get hurt if the explosiv explode
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	class USphereComponent* ExplosionRange;

	// Sound that is played when Explosiv gets hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (AllowPrivateAcess = "true"))
	class USoundCue* ImpactSound;

	// Sound that is played when Explosiv gets hit melee
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (AllowPrivateAcess = "true"))
	USoundCue* MeleeImpactSound;

	// Sound of the Explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (AllowPrivateAcess = "true"))
	USoundCue* ExplosionSound;

	// Particle Effect that gets spawned when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAcess = "true"))
	class UParticleSystem* HitParticles;

	// Particle Effect that gets spawned when destroyed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAcess = "true"))
	UParticleSystem* ExplosionParticles;

	// Current Amount of Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAcess = "true"))
	float CurrentHealth;

	// Maximal Amount of Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAcess = "true"))
	float MaxHealth;

	// if True, Actor gets destroyed next Tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAcess = "true"))
	bool bShouldDestroyNextTick;

	// Radius of the ExplosionRange Sphere
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	float ExplosionRadius;

	// Damage of the Explosion when close to detonation 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	float ExplosionDamageClose;

	// Damage of the Explosion when Far to detonation 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	float ExplosionDamageFar;

	// StunChance of the Explosion when close to detonation 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	float ExplosionStunChanceClose;

	// StunChance of the Explosion when Far to detonation 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	float ExplosionStunChanceFar;

	// Time it takes after destring to explode
	// Affectet by Deviation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	float ExplosionDelay;

	// Exploding Time Deviation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAcess = "true"))
	float ExplosionDeviationDelay;


protected:
	// Destroys the Actor next tick if true
	void DestroyNextTick();

	void PlayDestroySound();

	void EmmitDestroyEffect();

	// Checks for Actor within in the ExplosionRange, also call destory() after deal Damage to all within Range
	void CheckforActorWithinExplosionRange();

	void HandleDamage(float Damage);

	void HandleDestroy();

	// When detonate, calculate Damage
	void DealExplosivDamage(IExplosionHitInterface* Interface, AActor* Target);

	// Takes care of the Timer when do explode and deal Damage
	void ExplodeTimer();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, FHitZone& HitZone) override;

	// return DamageAmount
	// @param DamageAmount How much damage to apply
	// @param FDamageEvent Data package that fully describes the damage received
	// @param EventInstigator The Controller responsible for the damage
	// @param The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventIstigator, AActor* DamageCauser) override;

	virtual void ExplosionHit_Implementation(float Damage, float StunChance = 0.f) override;

	USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; };

	USoundCue* GetImpactSound() const { return ImpactSound; };
};
