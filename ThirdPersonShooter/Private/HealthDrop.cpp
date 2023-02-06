// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthDrop.h"
#include "CustomLogs.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"

AHealthDrop::AHealthDrop()
{
	HealthAmount = 5;
	ChanceToIncreaseHealAmount = 35.f; 
	AreaSphere->SetSphereRadius(PickUpRange);
}

void AHealthDrop::BeginPlay()
{
	Super::BeginPlay();

	IncreaseHealthAmountByChance(ChanceToIncreaseHealAmount);
	ChangeMeshSizeByCHealthAmount(HealthAmount);
}

void AHealthDrop::DoItemRoutine()
{
	if (GetMainCharacter())
	{
		// Add Cash value to player Charcter
		GetMainCharacter()->AddPlayerHealth(HealthAmount);

		if (GetMainCharacter()->GetHealthPickupSound())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GetMainCharacter()->GetHealthPickupSound());
		}

		UE_LOG(ItemLog, Log, TEXT("Added %d Health to the Player"), HealthAmount);

	}
	else
	{
	#if EDITOR 
		UE_LOG(ItemLog, Warning, TEXT("%s: in AHealthDrop::DoItemRoutine(), Could not Heal PlayerCharacter!"), *this->GetActorLabel());
	#endif
	}

	Destroy();
}

void AHealthDrop::ChangeMeshSizeByCHealthAmount(int32 InHealthAmount)
{
	if (GetStaticMeshComponent())
	{
		// Increase Scale if the Mesh for each 5 HealthAmount
		float ValueX = FMath::Clamp((GetStaticMeshComponent()->GetComponentScale().X + (InHealthAmount / 5) * 0.075), 0.3f, GetMeshMaxScaleSize());
		float ValueY = FMath::Clamp((GetStaticMeshComponent()->GetComponentScale().Y + (InHealthAmount / 5) * 0.075), 0.3f, GetMeshMaxScaleSize());
		float ValueZ = FMath::Clamp((GetStaticMeshComponent()->GetComponentScale().Z + (InHealthAmount / 5) * 0.075), 0.3f, GetMeshMaxScaleSize());

		GetStaticMeshComponent()->SetWorldScale3D(FVector(ValueX, ValueY, ValueZ));
	}
}

void AHealthDrop::IncreaseHealthAmountByChance(float Chance)
{
	float Roll = -1.f;
	bool bStop = false;

	for (uint32 Counter = 0; Roll < Chance && Counter <= 10 && !bStop; Counter++)
	{
		Roll = FMath::FRandRange(0.f, 100.f);

		if (Roll < Chance)
		{
			// Increase CashValue
			HealthAmount += 5;
		}
		else
		{
			bStop = true;
			// Nothing
		}
	}
	HealthAmount = FMath::Clamp<float>(HealthAmount, 5.f, MaxHealthAmount);
}
