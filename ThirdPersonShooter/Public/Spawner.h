// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"


USTRUCT(BlueprintType)
struct FEnemyAmount
{
	GENERATED_BODY()
	FEnemyAmount() {}

	FEnemyAmount(int32 Mini, int32 Standard, int32 Big)
	{
		AmountOfMiniEnemy =  Mini ;
		AmountOfStandardEnemy = Standard;
		AmountOfBigEnemy = Big;
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountOfMiniEnemy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountOfStandardEnemy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmountOfBigEnemy = 0;
};


UCLASS()
class THIRDPERSONSHOOTER_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = ( AllowPrivateAccess = "true"))
	class UBoxComponent* MaxSpawnArea;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DebugMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,  meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

	// Easy Enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BlueprintBaseOnly = "true"))
	TSubclassOf<class AEnemy> SmallEnemy;

	// Medium Enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BlueprintBaseOnly = "true"))
	TSubclassOf<AEnemy> StandardEnemy;

	// Though Enemy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true", BlueprintBaseOnly = "true"))
	TSubclassOf<AEnemy> BigEnemy;

	UPROPERTY()
	TArray<FEnemyAmount> EnemyArray;

	bool WaveTimerStarted;
	// List Of Enemy to Spawn
	TArray<UClass*> ListOfEnemyToSpawn;

	// True if a Entiy to Spawn is Selected in the Editor
	bool bSpawnEntityIsValid;

	// If an Actual Enemy should Spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	bool bAllowSpawn;

	// Default SpawnTime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float SpawnTime;

	// The Radius around the Character no Enemy can Spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float SpawnSafeRadius;

	// Show Debug Primitives
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	bool bShowDebug;

	// Shows more Debug Log
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	bool bShowCylceTries;

	// Timer Handle that callback to SpawnControll(). Fires first Time Starting a Wave
	FTimerHandle SpawnTimerHandle;
	
	// Spawn Location if no Location could be determined, For now only X and Y Coordinates will be used. z is taken from tis Actor Location
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true", DisplayPriority = "1", Delta = 10))
	FVector FallBackSpawnLocation;

	// Amount of Enemy to Spawn for A Level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int32 EnemySpawnAmount;

	// Amount of Enemy to Spawn for A Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int32 AmountOfMiniEnemy;

	// Amount of Enemy to Spawn for A Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int32 AmountOfStandardEnemy;

	// Amount of Enemy to Spawn for A Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int32 AmountOfBigEnemy;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int32 StartLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float SpawnDelayAfterPreSpawnEffect;

	// Spawn Parameter set in Construction
	FActorSpawnParameters Parameters;

	// True if all Enemy are Killed in this Wave
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	bool bWaveFinished;

	bool FirstWave;

	// Is true when Mainchar dies
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	bool bMainCharacterIsDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PreSpawnEffectEnemyEasy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PreSpawnLightFlashEnemyEasy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PreSpawnEffectEnemyDangerous;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PreSpawnLightFlashEnemyDangerous;

	// Ref to the Player that is set in the Begin of the Level for faster Access
	class AShooterCharacter* PlayerCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Return a Valid SpawnLocation
	FVector CalculateSpawnCordinates();

	FVector2D RandomPointWithinBorder();

	bool PointIsInsideBorder(FVector2D RandomPoint2D);

	FVector2D GetPointInsideBorder();

	float SquareDistance(FVector2D RandomPoint2D, FVector2D PlayerPosition2D);

	// Decrement EnemySpawnAmount and return left SpawnAmount
	int32 DecrementEnemySpawnAmount();

	// Stores a Ref to the Player
	void StoreRefToPlayer();

	void BeginStartLevelPreCountDown();
	
	//Register on the CharacterIsDeadDlegate on the Player class
	void RegisterCharacterIsDeadDelegate();

	void RegisterSelfOnPlayerController();

	UFUNCTION()
	void Callback_SpawnMiniEnemy(FVector SpawnLocation, UClass* ClassToSpawn);

	// Make sure to destroy smoke Effect before Spawing
	UFUNCTION()
	void Callback_DestroySmoke(UParticleSystemComponent* Effect);

	void SetupSpawnMiniEnemy(FVector& SpawnLocation, UClass* ClassToSpawn);

	void SmokeEffect(FVector& SpawnLocation, UClass* ClassToSpawn);

	void PrepareSpawn(FVector& SpawnLocation, UClass* ClassToSpawn);

	UClass* ChooseEnemyToSpawn();

	// Fill Array
	void InitzializeEnemyArray();	

	void AddEnemyToList(int32 MiniEnemyCount, int32 BigEnemyCount, int32 AmountOfStandardEnemy);

	void SpawnControll();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function get called when CharacterIsDeadDelegate broadcasts
	UFUNCTION()
	void MainCharacterIsDead(bool State);

	// Spawn an Actor
	UFUNCTION()
	void StartWave();

	int32 GetCurrentLevel() const {	return CurrentLevel; };

	void IncrementLevel() { CurrentLevel++; };

	void SetUpLevelSpawn(int32 CurrenLevel);

	void StopSpawnTimerHandle();

	void SetbWaveFinished(bool State) { bWaveFinished = State; };

	void SetWaveTimerStarted(bool State) { WaveTimerStarted = State; };

	void SetFirstWave(bool State) {	FirstWave = State; };
};
