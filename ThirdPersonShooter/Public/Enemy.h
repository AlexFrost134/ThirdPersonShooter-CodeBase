// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "ExplosionHitInterface.h"
#include "StructHelper.h"
// Used because of the struct FHItZone
#include "ShooterCharacter.h"
#include "Enemy.generated.h"


UCLASS()
class THIRDPERSONSHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface, public IExplosionHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	AEnemy* PointerToSelf;

	// Particle that spawn when hit by bullets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* DefaultImpactParticles;

	// Scale Vector of the DefaultImpactParticles. Default Value 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true", ClampMin = "0.01", UIMin = "0.01"))
	FVector ImpactParticlesSize;

	// Sound to play when hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	// Current Amount of Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth;

	// Maximal Amount of Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 ExperiencePointsOnKill;

	// The Speed of which the Enemy moves
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float DefaultMovementSpeed;
	
	// Name of the HeadBone
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	FName HeadBoneName;
	
	int32 AmountofLoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 MinAmountofLoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 MaxAmountofLoot;

	// Distance of the AcceptanceRadius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float AIAcceptableRadius;

	// Collision Volume for the right weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* RightWeaponCollisionBox;

	// Collision Volume for the left weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftWeaponCollisionBox;

	//FTimerHandle ShowHealthBarTimer;

	FTimerHandle ShowHealthBarDisplayTimer;

	FTimerHandle ShowHealthAmountTimer;

	FTimerHandle HitMontageReactTimer;

	FTimerHandle AttackCooldownHandle;

	// Could be set in local scope in ForceAgressionOnPlayer()
	// Set in global space for debuggin Purpose
	FTimerHandle ForceAggressionTimerHandle;

	//  Time for Displaying HealthAmount 
	// Should has the same Amount as ShowHealthbarforTimeX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float ShowHealthAmountForTimeX;

	//  Time for Displaying Healthbar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float ShowHealthbarForTimeX;

	float HalfMontageSectionTime;

	// If true, UI shows HealthBar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	bool bShowHealthBar;

	// If true, UI shows HealthAmount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	bool bShowHealthAmount;

	// If true, allows to play Hitmontage, Prevents playspamming HitMontage
	bool bCanPlayHitMontage;

	// Montage containing Hit and Death Animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	// Montage containing Attack Animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	// Death Montage containing die Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	// Blend InTime of the AttackMontage; Default = 0.25f
	// Can be lowered if the Animation plays to fast, if Animiation play fine no need to lower the Value
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
	float AttackMontageBlendInTime;

	// Blend OutTime of the AttackMontage; Default = 0.25f
	// Can be lowered if the Animation plays to fast, if Animiation play fine no need to lower the Value
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0"))
	float AttackMontageBlendOutTime;

	// Array that hold the Animation sections names 
	TArray<FName> AttackMontageSections;

	// Array that hold the Animation sections names
	TArray<FName> DeathMontageSections;

	// Montages Sections Names in the AttackMontage
	FName PrimaryAttack_LA;
	FName PrimaryAttack_LAF;
	FName PrimaryAttack_RA;
	FName PrimaryAttack_RAF;

	// Map to stroe HitNumber widgets an their hit locations
	//UPROPERTY(VisibleAnywhere, Category = "UI", meta = (AllowPrivateAcess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	// Time after the HitNumberWidget gets destroyed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

	// BehaviorTree for the AI Character
	UPROPERTY(EditDefaultsOnly, Category = "BehaviorTree", meta = (AllowPrivateAcess = "true"))
	class UBehaviorTree* BehaviorTree;

	// 1st Point for the Enemy to Move to
	// "MakeEditWidget" makes the Vector to localSpace of the Character. Further it creates a Widget in the Editor that can be moved
	// Only Edditable when bActivatePatrolPointsFunctionality == true
	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true", EditCondition = "bActivatePatrolPointsFunctionality"))
	FVector PatrolPoint;

	// 2. Point for the Enemy to Move to
	// Only Edditable when bActivatePatrolPointsFunctionality == true
	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true", EditCondition = "bActivatePatrolPointsFunctionality"))
	FVector PatrolPoint2;

	// Stores a Reference to the EnemyAIController, is set in BeginPlay.
	class AEnemyAIController* EnemyAIController;

	// if the Player moves within this Sphere, the enemy becomes aggresive.
	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgressionSphere2;

	// MelleRange sphere, within these sphere the character can attack other actors
	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		USphereComponent* MeleeRangeSphere;

	// Array with overlapping Actor when Enemy gets spawned
	TArray<AActor*> SpawingOverlappingActor;

	// true if the Enemy is stunned, if true Enemy should not move
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bStunned;

	// Target that is beeing Attacked
	AActor* AttackTarget = nullptr;

	// true when in attack range
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange;

	// Percentage to resist a stun. Should not be higher than 100.f eq. 100%
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float StunResistance;

	// The Chance of this Enemy to Stun the Character when it lands a Hit. Default = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float StunChance;

	// Speed of the AnimationAttack PlaybackSpeed, Defaul 1.f eq. normal Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "5.0", UIMin = "0.1", UIMax = "5.0"))
	float AttackAnimationSpeed;

	// If true, the character turns to the Target while attacking.
	UPROPERTY(VisibleInstanceOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAllowTurntoTargetWhileAttacking;

	// IF true the Character can turn itself;
	UPROPERTY(VisibleInstanceOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAllowToTurn;

	// is True when the Enemy is on AttackCooldown, False if the cooldown is over
	UPROPERTY(VisibleInstanceOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsOnAttackCooldown;

	// Time untill the next attack is Allowed.
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackCooldownTime;

	// AttackCooldownTime will be adjusted by the Deviation, a Random deviation is determined before each attack
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true", ClampMin = "0.0", UIMin = "0.0", UIMax = "10.0"))
	float AttackCooldownDeviationMax;

	// AttackCooldownTime will be adjusted by the Deviation, a Random deviation is determined before each attack
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true", Clampax = "0.0", UIMin = "-10.0", UIMax = "0.0"))
	float AttackCooldownDeviationMin;

	// The Calculatet AttackCooldown Deviation
	float AttackCooldownDeviation;

	// True if AttackAnimation is finished, false if attackanimation is playing. Set in PlayAttackMontage Unset via BlueprintNotify
	UPROPERTY(VisibleInstanceOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAttackAnimationIsPlaying;

	// The Base Damage Amount that the Enemy deals
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	// Amount of time, Enemy follows the Character and trys to attack him, ignoring AgressionSphereOverlapping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float FollowTimeAfterShoot;

	// The Enemy follows the Character despite not beeing within the agressionSphere
	UPROPERTY(VisibleInstanceOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bForceAggressionOnPlayer;

	// If this is true, this Character is allowed to Damage the Player, set to true as soon as the Player receives Damage
	// Is set to false, when the attack Animation finishs
	bool bDealtDamageInThisAnimation;

	// True if the Enemy is dead, False if alive
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bDead;

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	FName LeftWeaponSocket;

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	FName RightWeaponSocket;

	// DestoryHandle gets called of Amount of Time, if the Enemy dies.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DestroyDelayAfterDeath;

	// True if Main Character is Dead
	bool bMainCharacterIsDead;

	// A Ref to the Main PlayerCharacter
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* MainCharacter;

	// Base FHitZones
	FHitZone CriticalHitZone;
	FHitZone HighHitZone;
	FHitZone DefaultHitZone;
	FHitZone LowHitZone;
	FHitZone MinorHitZone;

	// Sounds for HitZones
	USoundCue* CritImpactSound;
	USoundCue* HighImpactSound;
	USoundCue* DefaultImpactSound;
	USoundCue* LowImpactSound;
	USoundCue* MinorImpactSound;

	// Effects for HitZones
	UParticleSystem* CritImpactEffect;
	UParticleSystem* HighImpactEffect;
	UParticleSystem* DefaultImpactEffect;
	UParticleSystem* LowImpactEffect;
	UParticleSystem* MinorImpactEffect;
		
	// TODO:
	// Would love not to expose it to blueprint and create in in pure c++ 
	// Array that contains the different UPhysicalMaterial of the HitZones
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitZones", meta = (AllowPrivateAccess = "true", EditFixedOrder = "true"))
	TArray<UPhysicalMaterial*> HitZoneMaterials;
		
	// From Crit to Minor Zone, override if you desire to play another Sound on the HitZone than the default ImpactSound
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true", EditFixedOrder = "true"))
	TArray<USoundCue*> HitZoneImpactSound;

	// Array that contains Particle effects for each HitZone, override if you desire to emmit another Effect on the HitZone than the default ImpactEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects", meta = (AllowPrivateAccess = "true", EditFixedOrder = "true"))
	TArray<UParticleSystem*> HitZoneImpactEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitZones", meta = (AllowPrivateAccess = "true"))
	FHitZone CriticalHitZoneOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitZones", meta = (AllowPrivateAccess = "true"))
	FHitZone HighHitZoneOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitZones", meta = (AllowPrivateAccess = "true"))
	FHitZone DefaultHitZoneOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitZones", meta = (AllowPrivateAccess = "true"))
	FHitZone LowHitZoneOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HitZones", meta = (AllowPrivateAccess = "true"))
	FHitZone MinorHitZoneOverride;
	
	// Map that contains all HitZones structs assosiate with a PhysicalSurface
	TMap<EPhysicalSurface, FHitZone*> ZoneDamage;

	// Pointer that holds the adress to the ZoneDamageMap
	TMap<EPhysicalSurface, FHitZone*>* ZoneDamagePtr;

	// This is set to false by default, if true the enmy has a Sphere that can detect MainPlayer and set it as AttackTarget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Functionalty", meta = (AllowPrivateAcess = "true"))
	bool bActivateAgressionSphereFunctionality;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Functionalty", meta = (AllowPrivateAcess = "true"))
	bool bActivatePatrolPointsFunctionality;

protected:
		
	// Calling this Function will run the Blueprintcode that has been set up
	// Desingned to be overridden in Blueprint
	// See for Makro https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/GameplayArchitecture/Functions/Specifiers/
	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthAmount();

	// This is the C++ Function Variant
	void ShowHealthAmount_Implementation();

	// Custom Event for Blueprint
	// Call this to call a Blueprint Event
	// Set bShowHealthAmount to false
	// See for Makro https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/GameplayArchitecture/Functions/Specifiers/
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthAmount();

	// Calls this Event to Disable All Collision of some Channels of the root and of the Skeletal Mesh
	// Look at the BP Implementation for exact definition
	UFUNCTION(BlueprintImplementableEvent)
	void DisableCollisionOnDeath();

	// Allows HealtBar to be visible, and starts a Timer to unhide Healthbar
	void ShowHealthBar();

	// HidesHealthBar by setting bShowHealthBar to false
	void HideHealthBar();

	// Setup the Enemy AIController and run Enemy BehaviorTree
	void SetupEnemyBehaviorTree();

	// Takes Care of setting the AttackTarget of the Enemy
	void HandleAttackTarget();

	// call when Enemy has Zero Health, takes care of Hiding HealthHUD, Play DeathMontage, start DestroyHandle
	void EnemyDie();
	
	void PlayHitMontage(FName MontageSectionName, float PlayRate = 1.f);

	// Stores the Widgets a TMap with a Location
	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location );

	// Function is used by FTimerDelegate
	UFUNCTION()
	void DestoryHitNumberWidget(UUserWidget* HitNumber);

	// Call this to deal Damage to the PlayerCharacter
	void DealDamageToPlayerCharacter(class AShooterCharacter* PlayerCharacter);

	// Update the HitNumber Position on the Screen
	void UpdateHitNumbersScreenPosition();

	// A Vector in Local Space gets converted to WorldSpace
	// @param &LocalVector This vector gets converted to Worldspace
	void TranformLocalVectorToWorldVector(FVector& InLocalVector);

	// Updates the PatrolPoints of the BlackBoard
	void UpdatePatrolPointsLocations();

	// Draw Debugboxes the each PatorlPoint
	void DrawDebugBoxOnPatrolPoints();

	// Stores a Ref to the Player
	void StoreRefToPlayer();

	// Called when something overlaps with the AgressionSPhere
	UFUNCTION()
	void AgressionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when ending overlaps with the AgressionSPhere
	UFUNCTION()
	void AgressionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Overridding BeginOverlap function
	UFUNCTION()
	void MeleeRangeSpherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Overridding EndOverlap function
	UFUNCTION()
	void MeleeRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void RightWeaponCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	UFUNCTION()
	void LeftWeaponCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Activation and Deactivation  for the Weapon Collision Box
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateRightWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeaponCollision();

	// Assure that Current Health is never greater than MaxHealth
	void EnsureMaxHealthisCurrentHealth();

	// Add Tag to the Enemy
	void SetEnemyTag();

	// Send ExpierencePoints To Player on Enemy Death
	// @return returns Amount of Experience Points Send.
	int32 SendExperienceToPlayer();

	// Check if the Character is within the agression sphere
	// @param SetAsTarget True if the EnemyAIController should set the Player as the Target when he IS FOUND
	// @return true if the Player is inside the Sphere, false if not.
	bool IsPlayerInsideAggressionSphere(bool SetAsTarget = false);

	// Calls this to determine if a enemy gets stuned of not
	// @param WeaponStunChance The Chance that the Weapon has to stun the Enemy
	// @return Retuns true if the enemy gets stuned, return false if the enemy resist the stun or is already Stunned
	bool RollForStunResistance(float WeaponStunChance = 15.f);
	
	// Set the PlayerCharacter as the AggressionTarget 
	void ForceAgressionOnPlayer();

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	// Set bAllowTurntoTargetWhileAttacking to the State
	void SetAllowTurntoTargetWhileAttacking(bool State);

	// This set the bool bDealtDamageInThisAnimation to false
	void ResetDealtDamageInThisAnimation();

	// Callbackfunction for timer AttackAnimationHandle
	void AttackAnimationCallback();

	// Callback for ForceAggressionTimerHandle
	void ForceAggressionCallback();

	// Callback function for the AttackCooldownTimerHandle, Set bIsOnAttackCooldown to false;
	// Attack CoolDowntime is at least the length of the Animation. Additonaly the AttackCooldownTime is added.
	void AttackCooldownCallback();

	// Return a AttackCooldownDeviation based on a Random float between AttackCooldownDeviationMin and AttackCooldownDeviationMax
	// if the Deviation is less than the AttackColldownTime, the Deveation is set to the negativ CoolDownTime
	// Therefore the Colldowntime is at least 0.f
	float CalculateAttackCooldownDeviation();

	// Start Timer an resets after finishing the AttackCooldown State
	void StartAttackCooldownTimer(float Length);

	// Set bIsOnAttackCooldown to State
	void SetIsOnAttackCooldownState(bool State);

	// Spawn a Emitter at a Location with an optional Scale
	void SpawnBloodEmitter(UParticleSystem* BloodParticle, UPrimitiveComponent* OtherComp, double EmitterScale = 1.f);

	// Set the bAttackAnimation and the Variable on the BlackBoard on the AIController
	void SetAttackAnimationIsPlaying(bool State);

	// Deletes the Enemy from Memory
	void HandleDestroy();

	// Play Death Animation
	void PlayDeathAnimation(float Playrate = 1.f);

	// Pauses All Animation before Death, Call DestroyHandle after a set Time;
	void PauseAnimationBeforeDeath();

	// Gets called via Anim Notifier in GruxDeath_Montage
	UFUNCTION(BlueprintCallable)
	void FinishedDeathAnimation();

	// Register the AEnemy::MainCharacterIsDead function on the Delegate
	UFUNCTION()
	void RegisterCharacterIsDeadDelegate();

	// Initializes The DamageZone Map
	void InitializeDamageZoneValues();

	// Initializes FHit struct to the Default Values, need to be updatet when FHit Struck gets new Member
	void InitializeDefaultFHitZoneValues();

	// Initializes the Sounds of the HitZones
	void InitializeHitZoneImpactSound();

	void InitializeHitZoneImpactEffects();
	
	// Updates the FHitZone structs, uses Values from the Blueprint
	void UpdateFHitZoneValues();

	// Internal Check if HitZoneImpactSound Array has A Sound set, otherwise Default Sound is played
	// Called in PlayHitSoundForHitZone()
	void CheckSoundForHitZone(uint32 Index);

	// Internal Check if HitZoneImpactEffects Array has A Effect set, otherwise Default Effect is played
	// Called in PlayEffectForHitZone()
	void CheckEffectForHitZone(const FHitResult& HitResult, uint32 Index);

	// Binds and Call Fuction that are need for the AgressionSphere to Work
	void SetupAgressionSphereFunctionality();

	void SetUpPatrolPointsFunctionanlity();

	// Set Collision Settings for the Weapon Boxes in here
	void SetupWeaponCollisionBoxes();

	// Spawn Cash Objects around Dead Enemy
	void SpawnCashItems();

	// Set bMainCharacetIsDead on the EnemyBehaviorTree depeinding if the Main Character is Alive or Dead
	void SetMainCharacterIsDead_DepedingOnCharacterState();
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ADroppableItem> ItemLootBP;

public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// _Implementation is needed to set up the definition of the fuction
	// Doesn't need the Macro UFunction, since we are overriding the parent Function that already has the UFUNCTION Makro
	virtual void BulletHit_Implementation(FHitResult HitResult, FHitZone& HitZone) override;

	virtual void ExplosionHit_Implementation(float Damage, float StunChance = 0.f) override;

	// Play a specif Montage Section
	// @param MontageSectionName The Name of the MontageSection to play
	// @param PlayRate PlaybackSpeed of the Animation; Default 1.f
	// @return return True if Animation is set to play, return false if Animation is not set to play sucessfully
	UFUNCTION(BlueprintCallable)
	bool PlayAttackMontage(FName MontageSectionName, float PlayRate = 1.f, bool bAllowAttackTurning = true);
	
	// @return Returns a random MontageSection that is stored in the AttackMontageSections Array
	UFUNCTION(BlueprintPure)
	FName GetRandomAttackSectionName();

	// Makes the Character turn to its target.
	void TurnToTarget();

	FORCEINLINE float GetAttackAnimationSpeed() const { return AttackAnimationSpeed; };

	// Call this function to deal Damage
	// @param DamageAmount How much damage to apply
	// @param FDamageEvent Data package that fully describes the damage received
	// @param EventInstigator The Controller responsible for the damage
	// @param The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
	// @return Return the Amount of Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventIstigator, AActor* DamageCauser) override;

	FName GetHeadBoneName() const { return HeadBoneName; };

	float GetHealth() const { return CurrentHealth; };

	// Called in ShooterCharacter.cpp; Create Widget and takes care of drawing the Widget in ScreenSpace and Play Animation
	UFUNCTION(BlueprintImplementableEvent)
	void ShotHitNumbers(int32 Damage, FVector HitLocation, bool bCriticalHitZone = false, bool bLuckyHit = false);

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; };

	FORCEINLINE bool IsStunned() const { return bStunned; };

	FORCEINLINE	float GetStunChance() const { return StunChance; };

	// Broadcast function of the RegisterCharacterIsDead Delegate
	UFUNCTION()
	void MainCharacterIsDead(bool State);
	
	float GetAIAcceptableRadius() const { return AIAcceptableRadius; };
	
	TMap<EPhysicalSurface, FHitZone*>* GetZoneDamagePtr() { return ZoneDamagePtr; };

	float GetBaseDamage() const { return BaseDamage; };
	
	void PlayHitSoundForHitZone(FHitZone* HitZone);

	void PlayEffectForHitZone(const FHitResult& HitResult, FHitZone* HitZone);

	// Called from Spawner.cpp
	void SetPlayerCharacterAsTarget(AShooterCharacter* Target);

};
