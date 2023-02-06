// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoDrop.h"
#include "CustomLogs.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "ShooterCharacter.h"
#include "Components/WidgetComponent.h" // temp
#include "Components/BoxComponent.h" // temp
#include "Kismet/GameplayStatics.h"

AAmmoDrop::AAmmoDrop()
{
	AmmoType = EAmmoType::EAT_Default;
	AmmunitionAmount = 30;
	RandomDeviationMax = 5;
	RandomDeviationMin = 5;
	PickUpRange = 200.f;
	

}

void AAmmoDrop::BeginPlay()
{
	Super::BeginPlay();
	RollAmmoType();
	// AreaSphere Radius should be indepentet of scale of the Mesh, AreaSphere gets scaled up by the lowest Value of all tree AxisScale
	// So to revert the effect, x is taken as the divisor. Mesh should scale linar to all axes.
	AreaSphere->SetSphereRadius(PickUpRange / GetStaticMeshComponent()->GetRelativeScale3D().X);
	AreaSphere->SetVisibility(false); // need to set Manualy for some unkown Reason yet.
	
}

void AAmmoDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmoDrop::DoItemRoutine()
{
	if (GetMainCharacter())
	{
		// Handles Picking up Ammunition
		// And later destorying this Item
		StartItemCurve(GetMainCharacter());

		if (GetMainCharacter()->GetAmmoPickupSound())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GetMainCharacter()->GetAmmoPickupSound());
		}

	}
	
	// Items gets Destroyed after doing the interpolation Effect

}

void AAmmoDrop::SetItemProperties(EItemState state)
{
	//Super::SetItemProperties(state);

	switch (state)
	{

	case EItemState::EIS_Default:
		break;

	case EItemState::EIS_EquipInterping:

		// Set Mesh Properties
		GetStaticMeshComponent()->SetSimulatePhysics(false);
		GetStaticMeshComponent()->SetVisibility(true);
		GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetStaticMeshComponent()->SetEnableGravity(false);
		break;

	}
}

void AAmmoDrop::RollAmmoType()
{	
	UEnum* EnumObj = StaticEnum<EAmmoType>();
	uint8 TotalEnumElemts;
	if (EnumObj)
	{
		TotalEnumElemts = UKismetSystemLibrary::MakeLiteralByte(EnumObj->NumEnums());
		// Usally -1 but EAT_Default and EAT_INFINITE enum are not Valid
		uint8 ValidElemts = TotalEnumElemts - 3 ;
		
		// How to change?
		///'FMath::FRandRange': Arguments cause function resolution ambiguity, expected a floating point type.
		///Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.
		
		// Random AmmoType()
		uint8 RandomByte = (FMath::FRandRange<uint8>((uint8)0, ValidElemts));
		AmmoType = EAmmoType(RandomByte);
		
		switch (AmmoType)
		{
		case EAmmoType::EAT_9mm:
			AmmoType = EAmmoType(uint8(0));
			if(PistolAmmoStaticMesh)
			{
				GetStaticMeshComponent()->SetStaticMesh(PistolAmmoStaticMesh);
			}
			break;

		case EAmmoType::EAT_AR:
			AmmoType = EAmmoType(uint8(1));
			if (ARAmmoStaticMesh)
			{
				GetStaticMeshComponent()->SetStaticMesh(ARAmmoStaticMesh);
			}
			break;

		case EAmmoType::EAT_SMG:
			AmmoType = EAmmoType(uint8(2));
			if (SMGAmmoStaticMesh)
			{
				GetStaticMeshComponent()->SetStaticMesh(SMGAmmoStaticMesh);
			}
			break;

		case EAmmoType::EAT_Default:
			AmmoType = EAmmoType(uint8(0));
		#if EDITOR 
			UE_LOG(ItemLog, Log, TEXT("s% Random AmmoType failed. Should not switch to this case EAT_DEFAULT, using Default AmmoType"), *GetActorLabel());
		#endif
			break;

		case EAmmoType::EAT_INFINITE:
			AmmoType = EAmmoType(uint8(0));
		#if EDITOR 
			UE_LOG(ItemLog, Log, TEXT("s% Random AmmoType failed. Should not switch to this case EAT_INFINITE, using Default AmmoType"), *GetActorLabel());
		#endif
			break;
		}
	}
	else
	{
		// Defaul Pistol Ammo
		AmmoType = EAmmoType(uint8(0));
		#if EDITOR
			UE_LOG(ItemLog, Log, TEXT("s% Random AmmoType failed, using Default AmmoType"), *GetActorLabel());
		#endif
	}		
}
