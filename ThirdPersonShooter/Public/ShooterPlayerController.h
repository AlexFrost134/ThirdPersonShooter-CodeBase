// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	// Teleport Player to Start Area
	void MovePlayerToStartArea();

	void Callback_BeginStartLevelCountDown();

	UFUNCTION(BlueprintImplementableEvent)
	void FinishedCountDown();

	// Call this to show endscreen
	UFUNCTION(BlueprintImplementableEvent)
	void FinishedGame();

	void PrepareNextWaveOnSpawner();

	void Finished_WaveTimer();

	void DestroyRemaindActorsInWorld();

private:
	// Reference to the Overall HUD Overlay Blueprint Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	// EndScreenWidget
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> EndScreenWidget;

	// Instance of EndScreenWidget
	UPROPERTY()
	UUserWidget* EndScreenWidgetInstance;

	// Variable to holds the HUD Overlay Widget
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;

	// True if the Pause menu button is pressed
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	bool bShowPauseMenu;

	// TimerHandle for the Countdown
	FTimerHandle CountDownTimerHandle;

	FTimerHandle WaveTimerHandle;

	// Ref to the Spawner in the Level
	class ASpawner* LevelSpawner = nullptr;


public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowCountDownNumber();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowLevelTime();

	UFUNCTION(BlueprintImplementableEvent)
	void HideLevelTime();
	
	// Call this to Show PauseMenu
	UFUNCTION(BlueprintImplementableEvent)
	void ShowPauseMenu();

	// Call this to Hide PauseMenu
	UFUNCTION(BlueprintImplementableEvent)
	void HidePauseMenu();

	// Call this to Show UI
	UFUNCTION(BlueprintImplementableEvent)
	void ShowMainMenu();

	// Call this to Hide UI
	UFUNCTION(BlueprintImplementableEvent)
	void HideMainMenu();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowShop();

	bool GetShowPauseMenu() { return bShowPauseMenu; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCountDownTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetLevelCountDownTime();

	void SetLevelSpawner( ASpawner* Spawner) { LevelSpawner = Spawner; };

	void LevelFinished();

	// This Starts the Wave, call this to begin Game
	UFUNCTION(BlueprintCallable)
	void BeginStartLevelCountDown();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentLevel();

	// Start Wave Timer just when the first Enemy spawned
	void StartWaveTimer();

	// Create EndScreen and show to the PlayerViewport, also set UI Inpute Mode and Show Mouse Cursor
	void ShowEndScreen_Lost();

};
