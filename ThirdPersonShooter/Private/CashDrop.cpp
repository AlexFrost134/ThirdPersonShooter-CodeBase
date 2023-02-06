// Fill out your copyright notice in the Description page of Project Settings.


#include "CashDrop.h"
#include "CustomLogs.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"


ACashDrop::ACashDrop()
{
	CashValue = 1;
	ChanceToIncreaseValue = 33.f;
	AreaSphere->SetSphereRadius(PickUpRange);
}

void ACashDrop::BeginPlay()
{
	Super::BeginPlay();

	// Increase CashValue
	IncreaseCashValueByChance(ChanceToIncreaseValue);
	ChangeMeshSizeByCashValue(CashValue);
}

void ACashDrop::DoItemRoutine()
{
	if (GetMainCharacter())
	{
		// Add Cash value to player Charcter
		GetMainCharacter()->AddPlayerMoney(CashValue);

		if (GetMainCharacter()->GetCashPickupSound())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GetMainCharacter()->GetCashPickupSound());
		}

		UE_LOG(ItemLog, Log, TEXT("Added %d Cash to the Player"), CashValue);
		
	}
	else
	{
	#if EDITOR 
		UE_LOG(ItemLog, Warning, TEXT("%s: in ACashDrop::DoItemRoutine(), Could not send cash to PlayerCharacter!"), *this->GetActorLabel());
	#endif
	}

	Destroy();
	
}

void ACashDrop::ChangeMeshSizeByCashValue(int32 InCashValue)
{
	if (GetStaticMeshComponent())
	{
		// Increase Scale by CashValue
		float ValueX = FMath::Clamp((GetStaticMeshComponent()->GetComponentScale().X + InCashValue * 0.075), 0.3f, GetMeshMaxScaleSize());
		float ValueY = FMath::Clamp((GetStaticMeshComponent()->GetComponentScale().Y + InCashValue * 0.075), 0.3f, GetMeshMaxScaleSize());
		float ValueZ = FMath::Clamp((GetStaticMeshComponent()->GetComponentScale().Z + InCashValue * 0.075), 0.3f, GetMeshMaxScaleSize());
				
		GetStaticMeshComponent()->SetWorldScale3D(FVector(ValueX, ValueY, ValueZ));	
	}
}

void ACashDrop::IncreaseCashValueByChance(float Chance)
{
	float Roll = -1.f;
	bool bStop = false;

	for (uint32 Counter = 0; Roll < Chance && Counter <= 10 && !bStop; Counter++)
	{
		Roll = FMath::FRandRange(0.f, 100.f);

		if (Roll < Chance)
		{
			// Increase CashValue
			CashValue += 1;
		}
		else
		{
			bStop = true;
			// Nothing
		}
	}
}

