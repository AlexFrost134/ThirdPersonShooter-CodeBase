// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "DroppableItem.h"
#include "Ammunition.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Curves/CurveVector.h"
#include "CustomLogs.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "ShooterCharacter.h"

// Sets default values
AItem::AItem()
{
	// Items Attributes
	ItemName = TEXT("Default");
	ItemCount = 0;
	ItemRarity = EItemRarity::EIR_Common;
	ItemState = EItemState::EIS_OnGround;
	ItemType = EItemType::EIT_Default;

	// Interpolation Settings
	// Amount of the the ZCurve Asset in the editor takes
	ZCurveTime = 0.7f;
	ItemInterpStartLocation = FVector::Zero();
	CameraTargetLocation = FVector::Zero();
	bInterping = false;
	bLocalRotationResetDone = false;
	ItemInterpX = 0.f;
	ItemInterpY= 0.f;
	InterpolationInitialYawOffset = 0.f;
	InterpLocationIndex = 0.f;

	// Material
	MaterialIndex = 0;
	bCanChangeCustomDepth = true;

	// Dynamic Material Instances
	GlowAmount = 1.0f;
	FresnelExponent = 1.1f; 
	FresnelReflectFraction = -0.1f;
	PulseCurveTime = 5.f;

	// UI
	InventorySlot = 0;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NewRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(NewRootComponent);

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkletalMesh"));
	ItemSkeletalMesh->SetupAttachment(NewRootComponent);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox-> SetupAttachment(NewRootComponent);
	
	//Make Sure Only Visibility channel is blocked
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); 

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickupWidget->SetupAttachment(NewRootComponent);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(NewRootComponent);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	HidePickUpWidget();
	
	// Set activ Stars Array based on Rarity
	SetActiveStars();
	
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	SetItemProperties(ItemState);

	// Set The Color of the Glow Material
	SetGlowMaterialProperties();

	// Set Custom Depth to Disabled
	InitializeCutomDepth();

	StartPulseTimer();	
}

void AItem::OnConstruction(const FTransform& Transform)
{
	if (ItemRarityDataTable)
	{
		FItemRarityTable* Row = nullptr;

		switch (ItemRarity)
		{
		case EItemRarity::EIR_Damaged:
			Row = ItemRarityDataTable->FindRow<FItemRarityTable>(TEXT("Damaged"), TEXT(""));
			break;

		case EItemRarity::EIR_Common:
			Row = ItemRarityDataTable->FindRow<FItemRarityTable>(TEXT("Common"), TEXT(""));
			break;

		case EItemRarity::EIR_Uncommon:
			Row = ItemRarityDataTable->FindRow<FItemRarityTable>(TEXT("Uncommon"), TEXT(""));
			break;

		case EItemRarity::EIR_Rare:
			Row = ItemRarityDataTable->FindRow<FItemRarityTable>(TEXT("Rare"), TEXT(""));
			break;

		case EItemRarity::EIR_Epic:
			Row = ItemRarityDataTable->FindRow<FItemRarityTable>(TEXT("Epic"), TEXT(""));
			break;

		case EItemRarity::EIR_Default:
			Row = ItemRarityDataTable->FindRow<FItemRarityTable>(TEXT("Damaged"), TEXT(""));
			break;

		}
		if (Row)
		{
			// Assign Variables with Values form the DataTable
			GlowColor = Row->GlowColor;
			LightColor = Row->LightColor;
			DarkColor = Row->DarkColour;
			NumberOfStars = Row->NumberOfStars;
			IconBackground = Row->IconBackground;
			CustomDepthStencil = Row->CustomDepthStencil;

			// Set Stencile Value, depending form the Value of the DataTable
			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(CustomDepthStencil);
			}
		}
	}

	CreateDynamicMaterial();
	EnableGlowMaterial();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Start Interpolation Animation when bInterping is true
	ItemInterpolation(DeltaTime);

	// Get curve values form PulseCurve and set dynamic material parameter
	UpdatePulse();
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);

			// Disables the UI Animation when the 
			ShooterCharacter->UnHighlightInventorySlot();
		}
	}
}

void AItem::SetActiveStars()
{
	// The 0 element isn't used
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
		case EItemRarity::EIR_Damaged:
			ActiveStars[1] = true;
			break;

		case EItemRarity::EIR_Common:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			break;

		case EItemRarity::EIR_Uncommon:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			break;

		case EItemRarity::EIR_Rare:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			break;

		case EItemRarity::EIR_Epic:
			ActiveStars[1] = true;
			ActiveStars[2] = true;
			ActiveStars[3] = true;
			ActiveStars[4] = true;
			ActiveStars[5] = true;
			break;
	}
}

void AItem::SetItemProperties(EItemState state)
{	
	switch (state)
	{	
		case EItemState::EIS_Default:		
			break;

		case EItemState::EIS_OnGround:
			// Set Mesh Properties
			ItemSkeletalMesh->SetSimulatePhysics(false);
			ItemSkeletalMesh->SetVisibility(true);
			ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ItemSkeletalMesh->SetEnableGravity(false);

			// Set AreaSphere Properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			// Set CollisionBox Properties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;

		case EItemState::EIS_Equppied:

			PickupWidget->SetVisibility(false);
			// Set Mesh Properties
			ItemSkeletalMesh->SetSimulatePhysics(false);
			ItemSkeletalMesh->SetVisibility(true); 
			ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ItemSkeletalMesh->SetEnableGravity(false);
						
			// Set AreaSphere Properties
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// Set CollisionBox Properties
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;

		case EItemState::EIS_Falling:
			// Set Mesh properties
			ItemSkeletalMesh->SetSimulatePhysics(true);
			ItemSkeletalMesh->SetEnableGravity(true);
			ItemSkeletalMesh->SetVisibility(true);
			ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
						
			// AreaSphere
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// CollisionBox
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;

		case EItemState::EIS_EquipInterping:
			PickupWidget->SetVisibility(false);
			// Set Mesh Properties
			ItemSkeletalMesh->SetSimulatePhysics(false);
			ItemSkeletalMesh->SetVisibility(true);
			ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ItemSkeletalMesh->SetEnableGravity(false);
			// AreaSphere
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// CollisionBox
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;


		case EItemState::EIS_Carrying :
			PickupWidget->SetVisibility(false);
			// Set Mesh Properties
			ItemSkeletalMesh->SetSimulatePhysics(false);
			ItemSkeletalMesh->SetVisibility(false);
			ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ItemSkeletalMesh->SetEnableGravity(false);
			// AreaSphere
			AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// CollisionBox
			CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;					
	}
}

void AItem::FinishInterpolation()
{
	bInterping = false;
	SetbLocalRotationResetDone(false);
	if (PlayerCharacter)
	{
		// Subtract 1 from the Item Count of the InterpLocation Struct
		PlayerCharacter->IncrementInterpLocationItemCount(InterpLocationIndex, -1);
		PlayerCharacter->GetPickupItem(this);

		// UnhighlightInventorySlot for Ui Animation
		PlayerCharacter->UnHighlightInventorySlot(); 
	}
	// Set Scale back to normal
	SetActorScale3D(FVector(1.f));

	// Enables the outline effect again on this Items
	bCanChangeCustomDepth = true;

	DisableGlowMaterial();
	DisableCustomDepth();
}

FVector AItem::GetInterpLocation()
{
	if (PlayerCharacter == nullptr)
	{
		return FVector(0.f);
	}

	switch (ItemType)
	{
		case EItemType::EIT_Ammuniton:
			return PlayerCharacter->GetInterpLocationArray(InterpLocationIndex).SceneComponent->GetComponentLocation();
			break;

		case EItemType::EIT_Droppable:
			return PlayerCharacter->GetInterpLocationArray(InterpLocationIndex).SceneComponent->GetComponentLocation();
			break;

		case EItemType::EIT_Weapon: // 0 reserved for Weapon InterpolationLocation Index
			return PlayerCharacter->GetInterpLocationArray(0).SceneComponent->GetComponentLocation();
			break;

		case EItemType::EIT_Default:
			return PlayerCharacter->GetInterpLocationArray(0).SceneComponent->GetComponentLocation();
			break;
		default:
			break;		
	}
	return FVector(0.f);
}

void AItem::PlayPickUpSound()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->ShouldPlayPickupSound())
		{
			if (PickUpSound)
			{
				UGameplayStatics::PlaySound2D(this, PickUpSound);
				PlayerCharacter->StartPickUpSoundTimer();
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemSkeletalMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemSkeletalMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCutomDepth()
{
	DisableCustomDepth();
}

void AItem::CreateDynamicMaterial()
{
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		ItemSkeletalMesh->SetMaterial(GetMaterialIndex(), DynamicMaterialInstance);
	}
}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow_Blend_Alpha"), 0);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColour"), GlowColor);
	}
}

void AItem::SetGlowMaterialProperties()
{
	// virtual	
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_OnGround)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::ClearPulseTimer()
{
	if (GetWorldTimerManager().IsTimerActive(PulseTimer))
	{
		GetWorldTimerManager().ClearTimer(PulseTimer);
	}
}

void AItem::UpdatePulse()
{
	float ElapsedTime;
	FVector CurveValue;

	switch (ItemState)
	{
		case EItemState::EIS_OnGround:
			if (PulseCurve)
			{
				ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
				CurveValue = PulseCurve->GetVectorValue(ElapsedTime);

			}
			break;

		case EItemState::EIS_EquipInterping:
			if (PulseCurveInterping)
			{
				ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpolationTimer);
				CurveValue = PulseCurveInterping->GetVectorValue(ElapsedTime);
			}
			break;

		case EItemState::EIS_Default:
			break;

		default:
			break;
	}
	if (DynamicMaterialInstance)
	{
		// Set Scalar Parameter in the DynamicMaterial and Multiply it with a scaling Factor
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);

		// Does not look good, Maybe further tweakig can fix it, for now it is allright.
		// DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);
	}
}

void AItem::HandleDestory()
{	
	Destroy();
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow_Blend_Alpha"), 1);
	}
}

void AItem::PlayEquipSound() 
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->ShouldPlayEquipSound())
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
				PlayerCharacter->StartEquipSoundTimer();
			}
		}
	}
}

void AItem::StartItemCurve(AShooterCharacter* Character)
{
	// Stores a handle to the character
	PlayerCharacter = Character;

	// Get Array Index in InterpLocations with the lowest Item count
	InterpLocationIndex = Character->GetInterpLocationIndex();

	// Add one Item count on the InterpLocation Struct 
	Character->IncrementInterpLocationItemCount(InterpLocationIndex, 1);

	PlayPickUpSound();

	// Start Location , used in ItemInterpolation()
	ItemInterpStartLocation = GetActorLocation();
	
	// Allows ItemInterpolation to do Interpolation. Gets Called in Tick()
	bInterping = true;

	SetItemState(EItemState::EIS_EquipInterping);
	GetWorldTimerManager().ClearTimer(PulseTimer);

	GetWorldTimerManager().SetTimer(ItemInterpolationTimer, this, &AItem::FinishInterpolation, ZCurveTime);

	// Get Inital Yaw of the Camera
	const float CameraRotationYaw = Character->GetCameraComp()->GetComponentRotation().Yaw;
	// Get inital Yaw of the Item
	const float ItemRotationYaw = GetActorRotation().Yaw;
	//Inital Yaw offset between Camera and Item
	InterpolationInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	// Make that the outline effect is still in effect althoug the object is no longer the Target
	// Switches after finishing interping
	bCanChangeCustomDepth = false;
}

void AItem::ItemInterpolation(float DeltaTime)
{
	if (!bInterping)
	{
		return;
	}

	if (PlayerCharacter && ItemZCurve)
	{
		// Make sure that Local Rotation are reseted ONCE before performing Interpolation
		if (!bLocalRotationResetDone)
		{
			bLocalRotationResetDone = true;
			// Reset Local Mesh Rotation to 0,0,0
			ItemSkeletalMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));			
		}

		// Elapsed time since we started ItemInterpTimer
		float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpolationTimer);
		
		// Get CurvedValue corresponding to ElapsedTime from ItemZCurve
		float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		// Get the item's initial location when the curve started
		FVector ItemStartLocation = ItemInterpStartLocation;

		//DrawDebugBox(GetWorld(), ItemStartLocation, FVector(10.f), FColor::Green, true); // temp

		// Get location to Interp to
		const FVector TargetInterpLocation = GetInterpLocation();
	
		// Vector from Item to Camera Interp Location, X and Y are zeroed out
		const FVector ItemToCameraZDistance = FVector(0.f, 0.f, (TargetInterpLocation - ItemStartLocation).Z);
		// Scale Factor to multiply with CurveValue
		float DeltaZ = ItemToCameraZDistance.Size();

		const FVector ItemCurrentLocation = GetActorLocation();

		//DrawDebugBox(GetWorld(), ItemCurrentLocation, FVector(10.f), FColor::Yellow, true); // temp
				
		float InterpXValue = FMath::FInterpTo(ItemCurrentLocation.X, TargetInterpLocation.X, DeltaTime, 10.f);	
		float InterpYValue = FMath::FInterpTo(ItemCurrentLocation.Y, TargetInterpLocation.Y, DeltaTime, 10.f);
		
		// Set X and Y ItemLocation to Interped Values
		FVector FinalLocation = ItemStartLocation;
		FinalLocation.X = InterpXValue;
		FinalLocation.Y = InterpYValue;

		// Adding curve value to the Z Component of the initial Location, (scaled by DeltaZ)
		FinalLocation.Z = FinalLocation.Z + CurveValue * DeltaZ;
		SetActorLocation(FinalLocation, true, nullptr, ETeleportType::TeleportPhysics);				

		/// Rotation
		// Camera Rotation this frame
		const FRotator CameraRotation = PlayerCharacter->GetCameraComp()->GetComponentRotation();
		// Camera Rotation + Yaw Offset
		FRotator NewItemRoataion = { 0.f, CameraRotation.Yaw + InterpolationInitialYawOffset, 0.f };
		SetActorRotation(NewItemRoataion, ETeleportType::TeleportPhysics);

		/// Set Scale of the Item Based on ElapsedTime
		if (ItemScaleCurve)
		{
			float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue));
		}
	}
}

UWidgetComponent* AItem::GetPickUpWidget() 
{
	return PickupWidget;
}

void AItem::SetItemState(EItemState State)
{ 
	ItemState = State;
	SetItemProperties(ItemState);
}

void AItem::HidePickUpWidget()
{
	//Hide PickUp Widget
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}
