// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "Spawner.h"
#include "ShooterCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AShooterPlayerController::AShooterPlayerController()
{
	// UI
	bShowPauseMenu = false;
}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*
	// Check HUDOverlayClass Variable
	if (HUDOverlayClass)
	{
		// Create Instance of HUDOverlayClass and store it in HUDOverlay
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	*/
}

// Call this when player Exits shop // AND PLayer start the game
void AShooterPlayerController::BeginStartLevelCountDown()
{
	// Call show Contdown
	ShowCountDownNumber();
	float CountDownTime = 2.9f;
	GetWorldTimerManager().SetTimer(CountDownTimerHandle,this, &AShooterPlayerController::Callback_BeginStartLevelCountDown, CountDownTime, false);

}

void AShooterPlayerController::Callback_BeginStartLevelCountDown()
{
	
	// Stop Showing Countdown Number on Screen
	FinishedCountDown();
	if (LevelSpawner)
	{
		// Start LevelCountDown
		LevelSpawner->StartWave();
	}
}

void AShooterPlayerController::StartWaveTimer()
{
	float TimeForThisLevel;
	int32 CurrentLevel = GetCurrentLevel();

	switch (CurrentLevel)
	{
	case 1:
		TimeForThisLevel = 20.f;
		break;
	case 2:
		TimeForThisLevel = 25.f;
		break;
	case 3:
		TimeForThisLevel = 30.f;
		break;
	case 4:
		TimeForThisLevel = 35.f;
		break;
	case 5:
		TimeForThisLevel = 40.f;
		break;
	case 6:
		TimeForThisLevel = 45.f;
		break;
	case 7:
		TimeForThisLevel = 50.f;
		break;
	case 8:
		TimeForThisLevel = 55.f;
		break;
	case 9:
		TimeForThisLevel = 60.f;
		break;
	case 10:
		TimeForThisLevel = 120.f;
		break;
	default:
		TimeForThisLevel = 60.f;
		break;
	}
	//Set Global Timer for this Wave, when this TImer finishes force the Wave to end and entering shop
	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AShooterPlayerController::Finished_WaveTimer, TimeForThisLevel);
	// SHow CountDownTImer UI
	ShowLevelTime();
}

int32 AShooterPlayerController::GetCurrentLevel()
{
	if (LevelSpawner)
	{
		return LevelSpawner->GetCurrentLevel();
	}
	return 0;
}


void AShooterPlayerController::Finished_WaveTimer()
{
	// Hide WaveCountDOwn
	HideLevelTime();
	// Notify spawn that wave ended, Set WaveState
	if (LevelSpawner)
	{
		LevelSpawner->StopSpawnTimerHandle();
		LevelSpawner->SetbWaveFinished(true);
		LevelSpawner->SetWaveTimerStarted(false);
		LevelSpawner->SetFirstWave(true);
	}

	// Player is Alive
	 AShooterCharacter* PlayerChar = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerChar)
	{
		if (PlayerChar->GetCharacterIsDead())
		{
			// DO Nothing
		}
		else
		{
			//Give Player 10 Credits
			PlayerChar->AddPlayerHealth(2);
			PlayerChar->AddPlayerMoney(10);
			PlayerChar->LevelAmmoFillUp();
			// open Shopmenu
			LevelFinished();
		}
	}
	
	
}

void AShooterPlayerController::PrepareNextWaveOnSpawner()
{	
	if (LevelSpawner)
	{
		//Increment Level
		LevelSpawner->IncrementLevel();
		LevelSpawner->SetUpLevelSpawn(LevelSpawner->GetCurrentLevel());
				
		LevelSpawner->SetbWaveFinished(false);
		GetWorldTimerManager().ClearAllTimersForObject(LevelSpawner);
	}
}


float AShooterPlayerController::GetCountDownTime()
{
	if (GetWorldTimerManager().IsTimerActive(CountDownTimerHandle))
	{
		
		return GetWorldTimerManager().GetTimerRemaining(CountDownTimerHandle);

	}
	return 0.f;

}

float AShooterPlayerController::GetLevelCountDownTime()
{
	if (GetWorldTimerManager().IsTimerActive(WaveTimerHandle))
	{

		return GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);

	}
	return 0.f;

}


void AShooterPlayerController::LevelFinished()
{
	if (LevelSpawner)
	{	
		// Check win, Check for MaxLevel 10
		if (LevelSpawner->GetCurrentLevel() >= 10)
		{
			FinishedGame();
		}
		else
		{
			PrepareNextWaveOnSpawner();

			//  open shop UI and Remove SHooterHUD
			ShowShop();
			// Move PlayerTo Start Location
			MovePlayerToStartArea();
			// clean up level
			DestroyRemaindActorsInWorld();
		}
	}
	
		
}

void AShooterPlayerController::DestroyRemaindActorsInWorld()
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Enemy")), FoundActors);
	for (size_t i = 0; i < FoundActors.Num(); i++)
	{
		FoundActors[i]->Destroy();
	}
	FoundActors.Empty();

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Loot")), FoundActors);
	for (size_t i = 0; i < FoundActors.Num(); i++)
	{
		FoundActors[i]->Destroy();
	}
	FoundActors.Empty();

}


void AShooterPlayerController::MovePlayerToStartArea()
{	
	AShooterCharacter* PlayerChar = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerChar)
	{
		PlayerChar->SetActorLocation(PlayerChar->GetStartPosition());
	}
}

