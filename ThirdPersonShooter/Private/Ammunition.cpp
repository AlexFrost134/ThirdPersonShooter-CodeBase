// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammunition.h"
#include "ShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AAmmunition::AAmmunition()
{
	AutoPickUpRange = 50.f;

	// Set the Item Type to Ammunition
	SetItemType(EItemType::EIT_Ammuniton);
	
	// Construct the Mesh component and set it as the root
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	AmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Auto PickUpRange"));
	AmmoCollisionSphere->SetupAttachment(RootComponent);
	AmmoCollisionSphere->SetSphereRadius(AutoPickUpRange);
	
}

void AAmmunition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmunition::BeginPlay()
{
	Super::BeginPlay();

	// Binding Function
	AmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmunition::AmmoSphereOverlap);
	
}

void AAmmunition::SetItemProperties(EItemState state)
{
	Super::SetItemProperties(state);

	switch (state)
	{

	case EItemState::EIS_Default:
		break;

	case EItemState::EIS_OnGround:
		// Set Mesh Properties
		Mesh->SetSimulatePhysics(false);
		Mesh->SetVisibility(true);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetEnableGravity(false);	
		break;

	case EItemState::EIS_Equppied:
		// Set Mesh Properties
		Mesh->SetSimulatePhysics(false);
		Mesh->SetVisibility(true);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetEnableGravity(false);
		break;

	case EItemState::EIS_Falling:
		// Set Mesh properties
		Mesh->SetSimulatePhysics(true);
		Mesh->SetEnableGravity(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		break;

	case EItemState::EIS_EquipInterping:
		// Set Mesh Properties
		Mesh->SetSimulatePhysics(false);
		Mesh->SetVisibility(true);
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetEnableGravity(false);
		break;

	case EItemState::EIS_Carrying:
		break;

	default:
	#if EDITOR 
		UE_LOG(ItemLog, Warning, TEXT("Default Case Hit on s%"), *GetActorLabel());
	#endif
		break;

	}
}

void AAmmunition::AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* OverlappedCharacter = Cast<AShooterCharacter>(OtherActor);

		if (OverlappedCharacter)
		{
			StartItemCurve(OverlappedCharacter);
			AmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AAmmunition::CreateDynamicMaterial()
{
	if (GetMaterialInstance())
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(GetMaterialInstance(), this);
		SetDynamicMaterialInstance(DynamicMaterial);
		Mesh->SetMaterial(0, GetDynamicMaterialInstance());
	}
}

void AAmmunition::EnableCustomDepth()
{
	if (CanChangeCustomDepth())
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void AAmmunition::DisableCustomDepth()
{
	if (CanChangeCustomDepth())
	{
		Mesh->SetRenderCustomDepth(false);
	}
}
