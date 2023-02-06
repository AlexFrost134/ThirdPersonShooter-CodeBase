// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "CustomLogs.h"
#include "ShooterPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/Canvas.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASpawner::ASpawner()
{
	bSpawnEntityIsValid = false;
	bAllowSpawn = true;
	bWaveFinished = false;
	bMainCharacterIsDead = false;

	FirstWave = true;
	WaveTimerStarted = false;

	// Debug
	bShowCylceTries = false;
	bShowDebug = false;

	SpawnTime = 5.f;
	SpawnSafeRadius = 300.f;
	EnemySpawnAmount = 10;
	SpawnDelayAfterPreSpawnEffect = 3.f;
	CurrentLevel = 1;
	StartLevel = 1;
		
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxSpawnArea = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("SpawnArea")));
	MaxSpawnArea->SetupAttachment(RootComponent);
	SetRootComponent(MaxSpawnArea);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("DebugMesh2")));
	StaticMesh->SetupAttachment(MaxSpawnArea);	
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	// Spawn Parameters
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	StoreRefToPlayer();

	// Register Delegate
	RegisterCharacterIsDeadDelegate();

	RegisterSelfOnPlayerController();

	InitzializeEnemyArray();
	SetUpLevelSpawn(StartLevel);		
}

void ASpawner::RegisterSelfOnPlayerController()
{
	AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->SetLevelSpawner(this);
	}
}

void ASpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Create a DebugBox with in the MaxSpawnArea
	DrawDebugSolidBox(GetWorld(), GetActorLocation(), MaxSpawnArea->GetScaledBoxExtent(), FColor::Blue);		
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

UClass* ASpawner::ChooseEnemyToSpawn()
{
	int32 RandomNumber = FMath::RandRange(0, ListOfEnemyToSpawn.Num()-1);

	if (ListOfEnemyToSpawn.IsValidIndex(RandomNumber))
	{
		UClass* TempReturn = ListOfEnemyToSpawn[RandomNumber];
		ListOfEnemyToSpawn.RemoveAt(RandomNumber);
		return TempReturn;
	}	
	
	return nullptr;
}

void ASpawner::SetUpLevelSpawn(int32 CurrenLevel)
{			
	EnemySpawnAmount = EnemyArray[CurrenLevel - 1].AmountOfMiniEnemy + EnemyArray[CurrenLevel - 1].AmountOfStandardEnemy + EnemyArray[CurrenLevel - 1].AmountOfBigEnemy;

	AddEnemyToList(EnemyArray[CurrenLevel - 1].AmountOfMiniEnemy, EnemyArray[CurrenLevel - 1].AmountOfStandardEnemy, EnemyArray[CurrenLevel - 1].AmountOfBigEnemy);		
}

void ASpawner::InitzializeEnemyArray()
{
	EnemyArray.Empty();
	EnemyArray.Add(FEnemyAmount(7, 0, 0));	// 1
	EnemyArray.Add(FEnemyAmount(10, 2, 0));	// 2
	EnemyArray.Add(FEnemyAmount(15, 6, 0));	// 3
	EnemyArray.Add(FEnemyAmount(7, 10, 0));	// 4
	EnemyArray.Add(FEnemyAmount(30, 0, 1));	// 5
	EnemyArray.Add(FEnemyAmount(10, 10, 0));// 6
	EnemyArray.Add(FEnemyAmount(0, 20, 3));	// 7
	EnemyArray.Add(FEnemyAmount(20, 15, 2));// 8
	EnemyArray.Add(FEnemyAmount(30, 30, 5));// 9
	EnemyArray.Add(FEnemyAmount(33, 33, 10));// 10
}


void ASpawner::AddEnemyToList(int32 MiniEnemyCount, int32 StandardEnemyCount, int32 BigEnemyCount)
{
	if(SmallEnemy.Get())
	{
		for (size_t i = 0; i < MiniEnemyCount; i++)
		{
			ListOfEnemyToSpawn.Add(SmallEnemy.Get());
		}
	}

	if (StandardEnemy.Get())
	{
		for (size_t i = 0; i < StandardEnemyCount; i++)
		{
			ListOfEnemyToSpawn.Add(StandardEnemy.Get());
		}
	}

	if (BigEnemy.Get())
	{
		for (size_t i = 0; i < BigEnemyCount; i++)
		{
			ListOfEnemyToSpawn.Add(BigEnemy.Get());
		}
	}
}

void ASpawner::StartWave()
{
	// Based on Level start timer with different length
	// ListOfEnemyToSpawn should updated by now

	if (!bMainCharacterIsDead && !bWaveFinished)
	{
		float SpawnDeviation;
		if (FirstWave)
		{
			SpawnDeviation = 0.6f;
			FirstWave = false;
		}
		else
		{
			// Timer that takes care of Spawing
			SpawnDeviation = FMath::FRandRange(2.f, 4.f) + SpawnTime;
			SpawnDeviation = FMath::Clamp(SpawnDeviation, 2.f, 5.f);
		}			
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawner::SpawnControll, SpawnDeviation, false);				
	}
	
}

void ASpawner::SpawnControll()
{
	if (bAllowSpawn && !bWaveFinished)
	{
		// Count AMount of Enemy on the fields // Never Spawn more than 15
		TArray<AActor*> FoundActors;
		FoundActors.Empty();
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), FoundActors);
		if (FoundActors.Num() >= 15)
		{
			// skip spawing
			
		}
		else // spawn
		{
			int32 AmountOfsimultaneouslySpawned = FMath::RandRange(1, 4);
			if (AmountOfsimultaneouslySpawned > EnemySpawnAmount)
			{
				AmountOfsimultaneouslySpawned = EnemySpawnAmount;
			}

			for (size_t i = 0; i < AmountOfsimultaneouslySpawned && !bWaveFinished; i++)
			{
				FVector SpawnLocation = CalculateSpawnCordinates();
				UClass* EnemyToSpawnClass = ChooseEnemyToSpawn();
				SetupSpawnMiniEnemy(SpawnLocation, EnemyToSpawnClass);
				// Decrement EnemySpawnAmount
				DecrementEnemySpawnAmount();

				// All Enemy Spawned
				if (EnemySpawnAmount == 0)
				{
					bWaveFinished = true;
				}
			}
		}		
	}
	if (bWaveFinished)
	{
		// NO more Spawning
		StopSpawnTimerHandle();				
	}
	else
	{
		StartWave();
	}
}

void ASpawner::StopSpawnTimerHandle()
{
	if (GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

// TODO: Change FunctionName
void ASpawner::SetupSpawnMiniEnemy(FVector& SpawnLocation, UClass* ClassToSpawn)
{
	SmokeEffect(SpawnLocation, ClassToSpawn);
	PrepareSpawn(SpawnLocation, ClassToSpawn);
			
}

void ASpawner::PrepareSpawn(FVector& SpawnLocation, UClass* ClassToSpawn)
{
	//  AEnemy Spawn
	FTimerDelegate SpawnTimerDelegate;
	// Timer that call the Spawn Enemy Function after finishing
	FTimerHandle SpawnDelayHandle;

	// TODO: Change FunctionName
	SpawnTimerDelegate.BindUFunction<ASpawner, FVector>(this, FName(TEXT("Callback_SpawnMiniEnemy")), SpawnLocation, ClassToSpawn);
	GetWorldTimerManager().SetTimer(SpawnDelayHandle, SpawnTimerDelegate, SpawnDelayAfterPreSpawnEffect, false);

}

void ASpawner::SmokeEffect(FVector& SpawnLocation, UClass* ClassToSpawn)
{
	UParticleSystemComponent* Effect = nullptr;

	// Effect Emits slightly above Enemy
	FVector EffectSpawnLocation = SpawnLocation;
	EffectSpawnLocation.Z += 100.f;

	if (ClassToSpawn == SmallEnemy.Get())
	{
		Effect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PreSpawnEffectEnemyEasy, FTransform(FRotator(0.f), EffectSpawnLocation, FVector(2.f)), false);
	}

	if (ClassToSpawn == StandardEnemy.Get())
	{
		Effect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PreSpawnEffectEnemyEasy, FTransform(FRotator(0.f), EffectSpawnLocation, FVector(2.f)), false);
	}

	if (ClassToSpawn == BigEnemy.Get())
	{
		Effect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PreSpawnEffectEnemyDangerous, FTransform(FRotator(0.f), EffectSpawnLocation, FVector(2.f)), false);
	}

	if (Effect)
	{		
		// Smoke
		FTimerDelegate SmokeSpawnTimerDelegate;
		FTimerHandle SmokeSpawnDelayHandle;
		float SmokeDelay = SpawnDelayAfterPreSpawnEffect + 2.f;
		
		SmokeSpawnTimerDelegate.BindUFunction(this, FName(TEXT("Callback_DestroySmoke")), Effect);
		GetWorldTimerManager().SetTimer(SmokeSpawnDelayHandle, SmokeSpawnTimerDelegate, SmokeDelay, false);
	}
}


void ASpawner::Callback_DestroySmoke(UParticleSystemComponent* Effect)
{
	if (Effect)
	{
		Effect->DestroyComponent();
	}
}


void ASpawner::Callback_SpawnMiniEnemy(FVector SpawnLocation, UClass* ClassToSpawn)
{
	
	if (ClassToSpawn)
	{
		AEnemy* SpawnedActor = GetWorld()->SpawnActor<AEnemy>(ClassToSpawn, FTransform(FRotator3d(EForceInit::ForceInitToZero), SpawnLocation, FVector3d(1.f)), Parameters);

		// Spawn Effect Slightly above Enemy
		FVector EffectSpawnLocation = SpawnLocation;
		EffectSpawnLocation.Z += 150.f;

		if (ClassToSpawn == SmallEnemy.Get())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PreSpawnLightFlashEnemyEasy, FTransform(FRotator(0.f), EffectSpawnLocation, FVector(1.f)));

		}
		if (ClassToSpawn == StandardEnemy.Get())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PreSpawnLightFlashEnemyEasy, FTransform(FRotator(0.f), EffectSpawnLocation, FVector(1.f)));

		}
		if (ClassToSpawn == BigEnemy.Get())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PreSpawnLightFlashEnemyDangerous, FTransform(FRotator(0.f), EffectSpawnLocation, FVector(1.f)));
		}		

		if (PlayerCharacter && SpawnedActor)
		{
			SpawnedActor->SetPlayerCharacterAsTarget(PlayerCharacter);
		}

		if (!WaveTimerStarted)
		{	
			// Make call to Controller to start WaveTimer
			AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (PlayerController)
			{
				PlayerController->StartWaveTimer();
				WaveTimerStarted = true;

			}
		}		
	}
}

FVector ASpawner::CalculateSpawnCordinates()
{	
	// Get Player Location
	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerPosition = Player->GetActorLocation();
	// Convert to 2D Vectors
	FVector2D PlayerPosition2D(PlayerPosition.X, PlayerPosition.Y);
	
	// Get ValidSpawnPoint
	FVector2D ValidRandomPoint2D = GetPointInsideBorder();
			
	FVector SpawnLocation(ValidRandomPoint2D.X, ValidRandomPoint2D.Y, GetActorLocation().Z);

	// Debug
	if (bShowDebug)
	{
		DrawDebugCone(GetWorld(), SpawnLocation, FVector(.0f, .0f, .1f), 150.f, FMath::DegreesToRadians(30.f), FMath::DegreesToRadians(30.f), 10, FColor::Red, false, 5.f);
		DrawDebugCircle(GetWorld(), PlayerPosition, SpawnSafeRadius, 36, FColor::Green, false, SpawnTime, 0, 3.f, FVector(0.f, 1, 0),FVector(1,0,0));
	}
	return SpawnLocation;
}

FVector2D ASpawner::GetPointInsideBorder()
{
	// Amount of Cycle to Spawn an Enemy on a random Location
	int32 Cycles = 0;

	for (; Cycles < 5; Cycles++)
	{
		// Get Random Point
		FVector2D RandomPoint2D = RandomPointWithinBorder();

		if (PointIsInsideBorder(RandomPoint2D))
		{
			// Cycles again
		}
		else
		{
			if(bShowCylceTries)
			UE_LOG(SpawnLog, Log, TEXT("SpawnLocation Found on %d Cycles."), Cycles);
			return RandomPoint2D;
		}
	}
	
	// ForNow Return predefiend Position to Spawn!
	// TODO: Figrue out a Way where to Spawn Enemy instead.
	UE_LOG(SpawnLog, Warning, TEXT("NO Random SpawnLocation Found. Cycles is %d"), Cycles);
	return FVector2D(FallBackSpawnLocation.X + GetActorLocation().X, FallBackSpawnLocation.Y + GetActorLocation().Y);

}

float ASpawner::SquareDistance(FVector2D RandomPoint2D, FVector2D PlayerPosition2D)
{
	// Calculate Distance Root2(c) = Root2(X ^ 2 + Y ^ 2)
	// Resolve Root with square (exponent 2)
	float Distance = (RandomPoint2D.X - PlayerPosition2D.X) * (RandomPoint2D.X - PlayerPosition2D.X) +
					 (RandomPoint2D.Y - PlayerPosition2D.Y) * (RandomPoint2D.Y - PlayerPosition2D.Y);

	return Distance;
}

bool ASpawner::PointIsInsideBorder(FVector2D RandomPoint2D)
{
	// Get Player Location
	AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerPosition = Player->GetActorLocation();

	// Convert to 2D Vectors
	FVector2D PlayerPosition2D(PlayerPosition.X, PlayerPosition.Y);

	float Distance = SquareDistance(RandomPoint2D, PlayerPosition2D);

	if (Distance <= SpawnSafeRadius * SpawnSafeRadius)
	{
		return true;
	}
	else
	{
		return false;
	}

}

FVector2D ASpawner::RandomPointWithinBorder()
{
	FVector Origin = GetActorLocation();
	float MaxSpawnBorderX = MaxSpawnArea->GetScaledBoxExtent().X;
	float MaxSpawnBorderY = MaxSpawnArea->GetScaledBoxExtent().Y;

	//  Calculate RandomLocationWithinBorder
	float XLocation = FMath::FRandRange(Origin.X - MaxSpawnBorderX, Origin.X + MaxSpawnBorderX);
	float YLocation = FMath::FRandRange(Origin.Y - MaxSpawnBorderY, Origin.Y + MaxSpawnBorderY);
	FVector2D RandomPoint2D(XLocation, YLocation);

	return RandomPoint2D;
}

int32 ASpawner::DecrementEnemySpawnAmount()
{
	EnemySpawnAmount--;
	return EnemySpawnAmount;
}

void ASpawner::StoreRefToPlayer()
{
	 PlayerCharacter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void ASpawner::RegisterCharacterIsDeadDelegate()
{	
	if (PlayerCharacter)
	{
		PlayerCharacter->CharacterIsDeadDelegateBinding(this);
	}
}

void ASpawner::MainCharacterIsDead(bool State)
{
#if EDITOR 
	FString Name = GetActorLabel();
	UE_LOG(DelegateLog, Log, TEXT("MainCharacterIsDead has been called! on %s"), *Name);
#endif

	bMainCharacterIsDead = true;
	
	// Clear Spawn Timer to prohibite further spawing in this wave
	/*if (GetWorldTimerManager().IsTimerActive(SpawnDelayHandle))
	{
		GetWorldTimerManager().ClearTimer(SpawnDelayHandle);
	}*/
	// Also prohibite Another Spawning of Enemys, need to Call both Timers
	StopSpawnTimerHandle();


}