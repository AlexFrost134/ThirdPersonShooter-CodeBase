// Fill out your copyright notice in the Description page of Project Settings.


#include "DroppableItem.h"
#include "CustomLogs.h"
#include "Ammunition.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "AmmoDrop.h"
#include "HealthDrop.h"
#include "CashDrop.h"
#include "ShooterCharacter.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ADroppableItem::ADroppableItem()
{	
	MeshRandomSpawnDeviationMin = 1.f;
	MeshRandomSpawnDeviationMax = 1.f;
	MeshMaxScaleSize = 1.f;
		
	AbsoluteSpawnPercentageOfAllItems = 0.f;
	ForceSize = 10'000.f;
	
	ItemRarity = EItemRarity::EIR_Default;
	ItemState = EItemState::EIS_OnGround;
	ItemType = EItemType::EIT_Droppable;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	StaticMeshComponent->SetRenderCustomDepth(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	StaticMeshComponent->BodyInstance.bOverrideMass = true;
	StaticMeshComponent->BodyInstance.SetMassOverride(1.f);
	/// NOTE:
	// Do not use in Constructor, uses call to GEngine,which is not valid durning construction
	//StaticMeshComponent->SetMassOverrideInKg(NAME_None, 1.f);
	SetRootComponent(StaticMeshComponent);
	
	// Set AreaSphere Properties
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetGenerateOverlapEvents(false);
	AreaSphere->SetupAttachment(StaticMeshComponent);
	
	// Deactive Components
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetComponentTickEnabled(false);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetGenerateOverlapEvents(false);
	CollisionBox->SetVisibility(false, false);
	CollisionBox->SetupAttachment(StaticMeshComponent);

	ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetComponentTickEnabled(false);
	ItemSkeletalMesh->SetVisibility(false, false);
	ItemSkeletalMesh->SetupAttachment(StaticMeshComponent);

	PickupWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupWidget->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupWidget->SetComponentTickEnabled(false);
	PickupWidget->SetVisibility(false, false);
	PickupWidget->SetupAttachment(StaticMeshComponent);
}


void ADroppableItem::BeginPlay()
{
	Super::BeginPlay();

	MakeItemUnPickableforShortTIme();

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ADroppableItem::OnComponentHit);
	// Remove and add New Methode
	AreaSphere->OnComponentBeginOverlap.Clear();
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ADroppableItem::OnSphereOverlap);
		
	// Set Static Ref to PlayerCharacter
	SetMainCharacterRef();
	// ReScale Object
	SetRandomStaticMeshSize();
	//Clear Pulse Timer, side effect of Calling BeginPlay
	ClearPulseTimer();

	// Give Outline Color Green
	StaticMeshComponent->SetCustomDepthStencilValue(251);
	// Activate Render StencleDepth
	StaticMeshComponent->SetRenderCustomDepth(true);

	// Add Tag to the Enemy
	Tags.Add(FName(TEXT("Loot")));	
}

void ADroppableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADroppableItem::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(ItemLog, Log, TEXT("Hit: %s"), *OtherComp->GetName());

	if(GetStaticMeshComponent())
	{
		GetStaticMeshComponent()->SetSimulatePhysics(false);
		GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetStaticMeshComponent()->SetNotifyRigidBodyCollision(false);
		GetStaticMeshComponent()->SetEnableGravity(false);
	}
}

void ADroppableItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (PlayerDownCast == OtherActor)
	{
		if (MainCharacter)
		{
			DoItemRoutine();
		}
		else
		{
		#if EDITOR 
			UE_LOG(ItemLog, Warning, TEXT("%s: DoItemRoutine failed!"), *this->GetActorLabel());
		#endif
		}
	}
	else
	{
		// 'OtherActor' is not the Player
	}
	
	
}

void ADroppableItem::SetMainCharacterRef()
{
	// Get PlayerCharacter
	MainCharacter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// DownCast
	PlayerDownCast = Cast<AActor>(MainCharacter);
}


void ADroppableItem::EnableCustomDepth()
{
	StaticMeshComponent->SetRenderCustomDepth(true);
}

void ADroppableItem::DisableCustomDepth()
{
	StaticMeshComponent->SetRenderCustomDepth(false);
}

void ADroppableItem::SetRandomStaticMeshSize()
{
	float Scale = FMath::FRandRange(MeshRandomSpawnDeviationMin, MeshRandomSpawnDeviationMax);
	
	StaticMeshComponent->SetWorldScale3D(StaticMeshComponent->GetComponentScale() + FVector(Scale));
}

void ADroppableItem::MakeItemUnPickableforShortTIme()
{
	FTimerHandle SphereCollision;
	// Amount of Time the Item is not Pickable
	float Time = 0.25f;

	GetWorldTimerManager().SetTimer(SphereCollision, this, &ADroppableItem::ActivateAreaSphereCollision, Time);
}

void ADroppableItem::ActivateAreaSphereCollision()
{
	if(AreaSphere)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetGenerateOverlapEvents(true);
	}
}

void ADroppableItem::InitializeDynamicSpawnParameter(ADroppableItem* Item)
{	
	if (Item)
	{
		Item->GetStaticMeshComponent()->SetSimulatePhysics(true);
		Item->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Item->GetStaticMeshComponent()->SetNotifyRigidBodyCollision(true);
		Item->GetStaticMeshComponent()->SetEnableGravity(true);
		Item->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Item->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		// Add Force
		Item->GetStaticMeshComponent()->AddForce(CalculateRandomThrowAngle() * ForceSize);
	}		
}

FVector ADroppableItem::CalculateRandomThrowAngle()
{
	float Degree = FMath::FRandRange(0.f, 2 * 3.1415f);

	// Create Vector with 45° away from origin
	FVector AddForceVector = (GetActorUpVector() + GetActorRightVector());
	AddForceVector.X = AddForceVector.X * FMath::Cos(Degree) - AddForceVector.Y * FMath::Sin(Degree);
	AddForceVector.Y = AddForceVector.X * FMath::Sin(Degree) + AddForceVector.Y * FMath::Cos(Degree);
	AddForceVector.Z = AddForceVector.Z;
		
	return AddForceVector;
}

 void ADroppableItem::SpawingItems(FTransform Transform, AActor* SpawnParent)
{	
	 FActorSpawnParameters Parameters;
	 Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	 ADroppableItem* SpawnedItemReferenz = nullptr;	 	 
	 
	 CheckingReferenzes();
	 InitializeTSet();
			
	 // Roll Random Value
	 float Roll = FMath::FRandRange(0.f, AbsoluteSpawnPercentageOfAllItems);
	 
	 int32 SlotResult = 0;
	 FSetElementId SetElementId = FSetElementId::FromInteger(SlotResult);
	 bool bStop = false; 
	 auto CurrentMaxBorder = ItemSet[SetElementId].Key;
	 
	 /// Random Selection Mechanics
	 // Go Through each Pair in TSet and compore its SpawnChance to the rolled Chance
	 // if the rolled Value is lower than the Border then this items get selected to Spawn
	 // if rolled Value is higer the CurrentBorder increased by the percentage of the last item
	 for (auto Iter = ItemSet.CreateIterator(); Iter.ElementIt.GetIndex() < ItemSet.Num() && !bStop; ++Iter)
	 {
		 if (Roll <= (CurrentMaxBorder))
		 {
			 SlotResult = Iter.GetId().AsInteger();
			 bStop = true;
		 }
		 else
		 {
			 SetElementId = FSetElementId::FromInteger(Iter.GetId().AsInteger() + 1);
			 if (ItemSet.IsValidId(SetElementId))
			 {
				CurrentMaxBorder += ItemSet[SetElementId].Key;
			 }
		 }
	 }

	 // Return the Pointer Referenz form the for Loop to get the correct Index.
	 //	And set Result to the Value of the Pointer at the given Index.
	 SetElementId.FromInteger(SlotResult);
	 auto Result = ItemSet[SetElementId].Value;
	
	 if (Result.IsValid())
	 {
		 // Spawn
		 SpawnedItemReferenz = GetWorld()->SpawnActor<ADroppableItem>(Result.Get(), Transform, Parameters);

		 if (SpawnedItemReferenz)
		 {
			 SpawnedItemReferenz->InitializeDynamicSpawnParameter(SpawnedItemReferenz);
		 }
	 }
	 else
	 {
		 //NO Spawn
	 }

	 SpawnParent->Destroy();
	 return;
}

 void ADroppableItem::DoItemRoutine()
 {
	 // Pure Virtual
	 unimplemented();
 }

 void ADroppableItem::CheckingReferenzes()
 {
	 // Causes a Crash in Packaged Project or Haltpoint in VS if the condition aren't met.
	  check(HealthDrop.Get());
	  check(CashDrop.Get());
	  check(AmmoDrop.Get());
 }

 void ADroppableItem::InitializeTSet()
 {
	 // Create TPair for each Item to Spawn
	 // Set the ChancetoSpawn as the KEY
	 // Set the Pointer Refernez as VALUE
	 //Health
	 TPair<float, TSoftClassPtr<ADroppableItem>> ItemHealthPair;
	 ItemHealthPair.Key = HealthDropChance;
	 ItemHealthPair.Value = HealthDrop;

	 // Cash
	 TPair<float, TSoftClassPtr<ADroppableItem>> ItemCashPair;
	 ItemCashPair.Key = CashDropChance;
	 ItemCashPair.Value = CashDrop;

	 // Ammunition Drop
	 TPair<float, TSoftClassPtr<ADroppableItem>> ItemAmmoPair;
	 ItemAmmoPair.Key = AmmoDropChance;
	 ItemAmmoPair.Value = AmmoDrop;
	
	 // No Item
	 TPair<float, TSoftClassPtr<ADroppableItem>> ItemNOItemPair;
	 ItemNOItemPair.Key = 100.f - ItemDropChance;
	 ItemNOItemPair.Value = nullptr;

	 // Add all Pairs to a TSet
	 ItemSet.Add(ItemNOItemPair);
	 ItemSet.Add(ItemCashPair);
	 ItemSet.Add(ItemHealthPair);
	 ItemSet.Add(ItemAmmoPair);

	 // Remove empty Percentages form ItemSet
	 for (auto Iterator = ItemSet.CreateIterator(); Iterator.ElementIt.GetIndex() < ItemSet.Num(); ++Iterator)
	 {
		 if (ItemSet[Iterator.GetId()].Key == 0.f)
		 {
			 ItemSet.Remove(Iterator.GetId());
		 }

	 }
	 // Resort indices
	 ItemSet.CompactStable();
	 
	 // Calcualte AbsoluteSpawnPercentageOfAllItems
	 for (auto Iterator = ItemSet.CreateIterator(); Iterator.ElementIt.GetIndex() < ItemSet.Num(); ++Iterator)
	 {
		 AbsoluteSpawnPercentageOfAllItems += ItemSet[Iterator.GetId()].Key;
	 }
	 
 }