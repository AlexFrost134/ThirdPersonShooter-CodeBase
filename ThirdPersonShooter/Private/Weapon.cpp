// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Curves/CurveFloat.h"
#include "CustomLogs.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "ShooterCharacter.h"

//	#include "Components/SkeletalMeshComponent.h"

 AWeapon::AWeapon()
{
	ThrowWeaponTime = 1.2f;
	bFalling = false;
	AmmunitionInMagazin = 30;
	MagazineCapacity = 30;
	AmmunitionFireCost = 1;
	//Mesh = GetItemMesh(); // PACKAGING moved to beginplay

	BoneToHide = TEXT("");
	// PistolSlide
	SlideDisplacement = 0.f;
	SlideDisplacementTime = 0.15f;
	bMovingSlide = false;
	SlideDisplacementMaxValue = 3.f;
	


	// WeaponRecoil
	bGunHasRecoil = false;
	bCalculatedRecoilRandomAmount = false;
	MaxRecoilRotation = 15.f;
	BaseRecoilRotationX = 10.f;
	BaseRecoilRotationY = 10.f;
	BaseRecoilRotationZ = 5.f;
	RecoilRotationX = 0.f;
	RecoilRotationY = 0.f;
	RecoilRotationZ = 0.f;
	RecoilTargetX = 0.f;
	RecoilTargetY = 0.f;
	RecoilTargetZ = 0.f;
	RecoilRotationTime = 0.1f;
	RecoilCrouchScalingAmount = 3.f;

	// Default Weapon Type
	WeaponType = EWeaponType::EWT_SubMaschineGun;
	AmmoType = EAmmoType::EAT_9mm;

	// Reload Montage
	ReloadMontageSectionName = TEXT("Reload SMG");
	MagazineBoneName = TEXT("smg_clip");
	bMovingClip = false;

	bAutomaticGunFire = true;

	PrimaryActorTick.bCanEverTick = true;

	// If true allows the Manuel adjustment in PIE of the following Variable
	bOverrideWeaponProperties = false;
	

}

 void AWeapon::Tick(float DeltaTime)
 {
	 Super::Tick(DeltaTime);
	
	 // Keep the Weapon Upright
	 if (GetItemState() == EItemState::EIS_Falling && bFalling&& !bFalling)
	 {
		const FRotator MeshRotation{ 0.f, Mesh->GetComponentRotation().Yaw, 0.f };
		Mesh->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	 }

	 // Takes care of the Pistol Silde Animation
	 UpdateSildeDisplacement();

	 //Takes care of the Weapon Recoil Animation
	 UpdateWeaponRecoilRotation(DeltaTime);

	// Hide Bone if the BoneToHide Name is Valid and if the Bone is not already Hidden AND if BoneToHide Name exists in the WeaponSkeletal
	 if (BoneNameIsValid(BoneToHide) && !(GetItemMesh()->IsBoneHidden(GetItemMesh()->GetBoneIndex(BoneToHide))))
	 {	
		 if (GetItemMesh()->BoneIsChildOf(BoneToHide, GetItemMesh()->GetBoneName(GetItemMesh()->FindRootBodyIndex())))
		 {	
			 // Hide a Bone if BoneToHide is setin DataTable
			 HideBones();
		 }		 
	 }
 }

 void AWeapon::OnConstruction(const FTransform& Transform)
 {

	 FWeaponDataTable* Row = nullptr;

	 if (WeaponDataTable)
	 {
		 switch (WeaponType)
		 {
		 case EWeaponType::EWT_AssaultRifle:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("KA"), TEXT(""));
			 break;

		 case EWeaponType::EWT_SubMaschineGun:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("SMG"), TEXT(""));
			 break;

		 case EWeaponType::EWT_Pistol:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("Pistol"), TEXT(""));
			 break;

		 case EWeaponType::EWT_HeavyPistol:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("HeavyPistol"), TEXT(""));
			 break;

		 }
	 }

	 if (Row)
	 {
		 AmmoType = Row->AmmoType;
		 SetPickUpSound(Row->PickUpSound);
		 GetItemMesh()->SetSkeletalMesh(Row->WeaponMesh);
		 SetEquipSound(Row->EquipSound);
		 SetIconItem(Row->InventoryIcon);
		 SetIconAmmunition(Row->AmmunitonIcon);
		 SetItemName(Row->ItemName);
		 SetMaterialInstance(Row->MaterialInstance);
		 MagazineBoneName = Row->MagazineBoneName;
		 ReloadMontageSectionName = Row->ReloadMontageSectionName;
		 GetItemMesh()->SetAnimInstanceClass(Row->AnimationBP);
		 AutoFireFrequency = Row->AutoFireFrequency;
		 MuzzleFlash = Row->MuzzleFlash;
		 FireSound = Row->FireSound;
		 EmptyWeaponSound = Row->EmptyWeaponSound;
		 CrosshairIncreaseValue = Row->CrosshairIncreaseValue;
		 CrosshairCoolDownSpeed = Row->CrosshairCoolDownSpeed;
		 CrosshairMaxSpreadAmount = Row->CrosshairMaxSpreadAmount;
		 BoneToHide = Row->BoneToHide;
		 WeaponBaseSpreadFactor = Row->WeaponBaseSpreadFactor;
		 WeaponInAirSpreadFactor = Row->WeaponInAirSpreadFactor;
		 WeaponAimSpreadFactor = Row->WeaponAimSpreadFactor;
		 WeaponCrouchSpreadFactor = Row->WeaponCrouchSpreadFactor;
		 bAutomaticGunFire = Row->bAutomaticFire;

		 // Crosshairs Elements
		 CrosshairsCenter = Row->CrosshairsCenter;
		 CrosshairBottom = Row->CrosshairBottom;
		 CrosshairsLeft = Row->CrosshairsLeft;
		 CrosshairsRight = Row->CrosshairsRight;
		 CrosshairsTop = Row->CrosshairsTop;

		 // DataTable
		 WeaponTierDataTable = Row->WeaponTierDataTable;

		 // Clear Material
		 PreviousMaterialIndex = GetMaterialIndex();
		 GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);

		 SetMaterialIndex(Row->MaterialIndex);


		 /// Should be deprecated, is need that the pick up functionalty works
		 if (!bOverrideWeaponProperties)// if True allows later changing of the Variables
		 {
			 MagazineCapacity = Row->MagazingCapacity;
			 AmmunitionInMagazin = Row->AmmunitionInMagazin;
			  WeaponDamage = Row->WeaponDamage;
			 CriticalShotDamage = Row->CriticalShotDamage;
			 LuckyHitChance = Row->LuckyHitChance;
			 LuckyHitMultiplier = Row->LuckyHitMultiplier;
		 }
		 

	 }

	 // Needs to be called after reading form DataTable
	 Super::OnConstruction(Transform);

 }


 void AWeapon::BeginPlay()
 {
	 Super::BeginPlay();
	 Mesh = GetItemMesh();
	 
 }

 void AWeapon::PostBeginPlayerIntialisierung(EItemRarity ItemTier, EWeaponType InWeaponType)
 {	 
	 // Call order must be SetupWeaponProperties()
	 // /->ReadFrom WeaponValuesDataTable()
	 // /->ReadFrom ItemPropertiesDataTable()
	 //	/->ReadFrom TierDataTable()
	 // Otherwise Problems with dynamic Material, unsure why for now
	 SetupWeaponProperties(InWeaponType);
	 SetWeaponTierProperties(ItemTier); 
 }

 void AWeapon::SetupWeaponProperties(EWeaponType InWeaponType)
 {
	 SetWeaponType(InWeaponType);
	 ReadFromWeaponValuesDataTable();
 }

 void AWeapon::SetWeaponTierProperties(EItemRarity ItemTier)
 {
	 SetItemRarity(ItemTier);
	 ReadFromItemPropertiesDataTable(); 
	 ReadFromTierDataTable();
 }

 void AWeapon::ReadFromItemPropertiesDataTable()
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

 void AWeapon::ReadFromWeaponValuesDataTable()
 {
	 FWeaponDataTable* Row = nullptr;

	 if (WeaponDataTable)
	 {
		 switch (WeaponType)
		 {
		 case EWeaponType::EWT_AssaultRifle:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("KA"), TEXT(""));
			 break;

		 case EWeaponType::EWT_SubMaschineGun:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("SMG"), TEXT(""));
			 break;

		 case EWeaponType::EWT_Pistol:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("Pistol"), TEXT(""));
			 break;

		 case EWeaponType::EWT_HeavyPistol:
			 Row = WeaponDataTable->FindRow<FWeaponDataTable>(TEXT("HeavyPistol"), TEXT(""));
			 break;

		 }
	 }

	 if (Row)
	 {
		 AmmoType = Row->AmmoType;
		 SetPickUpSound(Row->PickUpSound);
		 GetItemMesh()->SetSkeletalMesh(Row->WeaponMesh);
		 SetEquipSound(Row->EquipSound);
		 SetIconItem(Row->InventoryIcon);
		 SetIconAmmunition(Row->AmmunitonIcon);
		 SetItemName(Row->ItemName);
		 SetMaterialInstance(Row->MaterialInstance);
		 MagazineBoneName = Row->MagazineBoneName;
		 ReloadMontageSectionName = Row->ReloadMontageSectionName;
		 GetItemMesh()->SetAnimInstanceClass(Row->AnimationBP);
		  MuzzleFlash = Row->MuzzleFlash;
		 FireSound = Row->FireSound;
		 EmptyWeaponSound = Row->EmptyWeaponSound;
		 BoneToHide = Row->BoneToHide;

		 WeaponBaseSpreadFactor = Row->WeaponBaseSpreadFactor;
		 WeaponInAirSpreadFactor = Row->WeaponInAirSpreadFactor;
		 WeaponAimSpreadFactor = Row->WeaponAimSpreadFactor;
		 WeaponCrouchSpreadFactor = Row->WeaponCrouchSpreadFactor;
		 bAutomaticGunFire = Row->bAutomaticFire;

		 // Crosshairs Elements
		 CrosshairsCenter = Row->CrosshairsCenter;
		 CrosshairBottom = Row->CrosshairBottom;
		 CrosshairsLeft = Row->CrosshairsLeft;
		 CrosshairsRight = Row->CrosshairsRight;
		 CrosshairsTop = Row->CrosshairsTop;

		 // DataTable
		 WeaponTierDataTable = Row->WeaponTierDataTable;

		 // Clear Material
		 PreviousMaterialIndex = GetMaterialIndex();
		 GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);

		 SetMaterialIndex(Row->MaterialIndex);


	 }

 }

 void AWeapon::ReadFromTierDataTable()
 {
	 FWeaponTierDataTable* Row = nullptr;

	 if (WeaponTierDataTable)
	 {

		 switch (ItemRarity)
		 {
		 case EItemRarity::EIR_Damaged:
			 Row = WeaponTierDataTable->FindRow<FWeaponTierDataTable>(TEXT("Tier1"),TEXT(""));
			 break;

		 case EItemRarity::EIR_Common:
			 Row = WeaponTierDataTable->FindRow<FWeaponTierDataTable>(TEXT("Tier2"), TEXT(""));
			 break;

		 case EItemRarity::EIR_Uncommon:
			 Row = WeaponTierDataTable->FindRow<FWeaponTierDataTable>(TEXT("Tier3"), TEXT(""));
			 break;

		 case EItemRarity::EIR_Rare:
			 Row = WeaponTierDataTable->FindRow<FWeaponTierDataTable>(TEXT("Tier4"), TEXT(""));
			break;

		 case EItemRarity::EIR_Epic:
			 Row = WeaponTierDataTable->FindRow<FWeaponTierDataTable>(TEXT("Tier5"), TEXT(""));
			 break;

		 case EItemRarity::EIR_Default:
			 Row = WeaponTierDataTable->FindRow<FWeaponTierDataTable>(TEXT("Tier1"), TEXT(""));
			 unimplemented();
			 break;

		 default:
			 unimplemented();
			 break;
		 }
	 }

	 if (Row)
	 {
		 MagazineCapacity = Row->MagazingCapacity;
		 AmmunitionInMagazin = Row->MagazingCapacity;
		 AutoFireFrequency = Row->AutoFireFrequency;
		 WeaponDamage = Row->WeaponDamage;
		 CriticalShotDamage = Row->CriticalShotDamage;
		 LuckyHitChance = Row->LuckyHitChance;
		 LuckyHitMultiplier = Row->LuckyHitMultiplier;
		 CrosshairIncreaseValue = Row->CrosshairIncreaseValue;
		 CrosshairCoolDownSpeed = Row->CrosshairCoolDownSpeed;
		 CrosshairMaxSpreadAmount = Row->CrosshairMaxSpreadAmount;
		 WeaponCost = Row->WeaponCost;
	 }

 }

 bool AWeapon::BoneNameIsValid(FName BoneName)
 {
	 if (BoneName != FName("") || BoneName != FName("None"))
	 {
		 return true;
	 }
	 else
	 {
		 return false;
	 }
 }

 bool AWeapon::RollLuckyHit()
 {
	 float RandomNumber = FMath::RandRange(0.f, 100.f);
	 // UE_LOG(CombatLog, Warning, TEXT("RandomNumber: %f, LuckyHitChance: %f "), RandomNumber, GetLuckyHitChance());
	 
	 // Critical Hit, A Critchance of 1, should reflect a one Percentage Chance
	 if (GetLuckyHitChance() >= RandomNumber)
	 {
		//  UE_LOG(CombatLog, Warning, TEXT("CriticalHit: true"));
		 return true;
	 }
	 else
	 {
		// UE_LOG(CombatLog, Warning, TEXT("CriticalHit: false"));
		 return false;
	 }
	 	 
 }

 void AWeapon::ThrowWeapon()
{
	// Only Allow Throwing when PlayerCharacter is Owner of the Weapon
	if (Owner)
	{
		// Get Forward and Right Actor of the Character
		FVector ActorForwardVector = Owner->GetActorForwardVector();
		FVector ActorRightVector = Owner->GetActorRightVector();
		// Intiate Random Numbers of ThrowHeight and ThrowDirection
		float RandomHeightRotation = FMath::FRandRange(-30, 0.f);
		float RandomYawRotation = FMath::FRandRange(-15.f, 15.f);

		// Roate ImpulsAxis UP to get a force that hurls the gun in the air
		FVector ImpulseDirection = ActorForwardVector.RotateAngleAxis(RandomHeightRotation, ActorRightVector);
		// Roate ImpulsAxis Right Or Left to  have a variance pattern
		ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomYawRotation, FVector(0.f, 0.f, 1.f));
		
		// Amount of Impulse
		ImpulseDirection *= 8'500;
	
		Mesh->AddImpulse(ImpulseDirection);

		// Weapon is Falling
		bFalling = true;
		// Reset Owner
		SetNewOwner(nullptr);
		// Set Timer that changes state and disable physics
		GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime, false);
	}
}

 void AWeapon::ResetLocalMeshRotation()
 {
	 Mesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
 }

 void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_OnGround);

	// Compensate SideEffects that happends due to applying force to the Mesh.
	GetRootComponent()->SetWorldLocation(Mesh->GetComponentLocation(), false, nullptr, ETeleportType::None);
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	//SetbLocalRotationResetDone(false);
	
	// Makes the PulseTimer run and makes the pulse effect visible
	StartPulseTimer();

}

/* Deprecated
void AWeapon::SetGlowMaterialProperties()
{
	
	if (GetDynamicMaterialInstance())
	{
		// Grey ColourVector
		FVector4 GreyColourVector = FVector4(0.576389f, 0.574319f, 0.463313f, 1.f);
		// Blue ColourVector
		FVector4 BlueColourVector = FVector4(0.f, 0.439998f, 1.f, 1.f);
		// Green ColourVector
		FVector4 GreenColourVector = FVector4(0.04861f, 1.f, 0.206638f, 1.f);
		// Magenta ColourVector
		FVector4 MagentaColourVector = FVector4(1.f, 0.f, 0.795009f, 1.f);
		// Orange ColourVector
		FVector4 OrangeColourVector = FVector4(1.f, 0.268264f, 0.f, 1.f);
		// Grey ColourVector
		GreyColourVector = FVector4(0.576389f, 0.574319f, 0.463313f, 1.f);

		switch (GetItemRarity())
		{
		case EItemRarity::EIR_Damaged:
			
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColour"), GreyColourVector);
			break;

		case EItemRarity::EIR_Common:
			
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColour"), GreenColourVector);
			break;

		case EItemRarity::EIR_Uncommon:
			
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColour"), BlueColourVector);
			break;

		case EItemRarity::EIR_Rare:
			
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColour"), MagentaColourVector);
			break;

		case EItemRarity::EIR_Epic:
			
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColour"), OrangeColourVector);
			break;

		case EItemRarity::EIR_Default:
			
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColour"), GreyColourVector);
			break;

		default:
			break;
		}
			
	}
}
*/



void AWeapon::HideBones()
{
	
		 GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);

		 if (false)
		 {
			 bool bState = GetItemMesh()->IsBoneHidden(GetItemMesh()->GetBoneIndex(BoneToHide));
			 FString Text = BoneToHide.ToString();

			 UE_LOG(LogTemp, Warning, TEXT("%s Bone is Hidden %s"), *Text, bState ? TEXT("True") : TEXT("False"));

		 }
}

void AWeapon::FinishMovingSlide()
{
	bMovingSlide = false;
}

void AWeapon::FinishRecoilRotation()
{
	bGunHasRecoil = false;
}

void AWeapon::UpdateSildeDisplacement()
{
	if (SlideDisplacementCurve && bMovingSlide)
	{
		float TimerElapsedTime = GetWorldTimerManager().GetTimerElapsed(SlideTimerHandle);
		const float CurrentCurveValue = SlideDisplacementCurve->GetFloatValue(TimerElapsedTime);
		SlideDisplacement = SlideDisplacementMaxValue * CurrentCurveValue;
				
	}
}

void AWeapon::UpdateWeaponRecoilRotation(float DeltaTime)
{
		
	if (bGunHasRecoil)
	{
		// Only Calculate a Random Value once, when gun is fired
		if (!bCalculatedRecoilRandomAmount)
		{
			// Add a Random Value to the Base Recoil Amount, to get slightly different results each shot. Do this for each Axis
			float RandomAmount = FMath::FRandRange(-BaseRecoilRotationX / 2.f, BaseRecoilRotationX / 2.f);
			RecoilTargetX = BaseRecoilRotationX + RandomAmount;
			
			RandomAmount = FMath::FRandRange(-BaseRecoilRotationY / 2.f, BaseRecoilRotationY / 2.f);
			RecoilTargetY = BaseRecoilRotationY + RandomAmount;

			RandomAmount = FMath::FRandRange(-BaseRecoilRotationZ / 2.f, BaseRecoilRotationZ / 2.f);
			RecoilTargetZ = BaseRecoilRotationZ + RandomAmount;

			bCalculatedRecoilRandomAmount = true;
		}
		
		RecoilRotationX = FMath::FInterpTo(RecoilRotationX, RecoilTargetX, DeltaTime, 30.f);
		RecoilRotationY = FMath::FInterpTo(RecoilRotationY, RecoilTargetY, DeltaTime, 30.f);
		RecoilRotationZ = FMath::FInterpTo(RecoilRotationZ, RecoilTargetZ, DeltaTime, 30.f);

		// When Crouching scale Down the recoil effect
		if (Owner->bIsCrouched)
		{
			RecoilRotationX = RecoilRotationX / RecoilCrouchScalingAmount;
			RecoilRotationY = RecoilRotationY / RecoilCrouchScalingAmount;
			RecoilRotationZ = RecoilRotationZ / RecoilCrouchScalingAmount;
		}		
	}
	else
	{
		// Inter to 0 , so the Bone rotates to its default Rotation
		RecoilRotationX = FMath::FInterpTo(RecoilRotationX, 0.f, DeltaTime, 30.f);
		RecoilRotationY = FMath::FInterpTo(RecoilRotationY, 0.f, DeltaTime, 30.f);
		RecoilRotationZ = FMath::FInterpTo(RecoilRotationZ, 0.f, DeltaTime, 30.f);
		bCalculatedRecoilRandomAmount = false; // Allow generating Random Number again.
	}


}


void AWeapon::CreateDynamicMaterial()
{
	if (GetMaterialInstance())
	{
		SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
		GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
	}
}

void AWeapon::SetNewOwner(AShooterCharacter* NewOwner)
{
	Owner = NewOwner;
}

void AWeapon::DecrementAmmunition()
{
	
	if (AmmunitionInMagazin - AmmunitionFireCost <= 0)
	{
		AmmunitionInMagazin = 0;
	}
	else
	{
		AmmunitionInMagazin -= AmmunitionFireCost;
	}
}

void AWeapon::SetAmmunitionInMagazin(int32 Amount)
{
	if (Amount > GetMaxMagazineCapacity() || Amount <= 0)
	{
		//FString DebugString = FString::Printf(TEXT("ReloadAmmount on %s is to INVLAID, Check Reload Amount in \"SetAmmunitionInMagazin\""), *GetName());
		//UE_LOG(LogTemp, Warning, FString::Printf(TEXT("ReloadAmmount on %s is to INVLAID, Check Reload Amount in \"SetAmmunitionInMagazin\""), *GetName()));
		Amount = GetMaxMagazineCapacity();
		UE_LOG(LogTemp, Warning, TEXT("Weapon Loads failed %s"), *GetName());
	}
	else
	{
		AmmunitionInMagazin = AmmunitionInMagazin + Amount;
		//UE_LOG(LogTemp, Warning, TEXT("Weapon Reloads on %s"), *GetName());
	}
}

bool AWeapon::GunHasFullMagazine()
{
	if (AmmunitionInMagazin == MagazineCapacity)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AWeapon::StartSlideTimer()
{
	bMovingSlide = true;

	GetWorldTimerManager().SetTimer(SlideTimerHandle, this, &AWeapon::FinishMovingSlide, SlideDisplacementTime);
}

void AWeapon::StartRecoilRotationTimer()
{
	bGunHasRecoil = true;

	GetWorldTimerManager().SetTimer(RecoilTimerHandel, this, &AWeapon::FinishRecoilRotation, RecoilRotationTime);
}

void AWeapon::PlayEmptyWeaponSound()
{

	// Try to play specif Sound, else play default Sound
	if (EmptyWeaponSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EmptyWeaponSound);
	}
	else if (DefaultEmptyWeaponSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DefaultEmptyWeaponSound);
	}
	else
	{
		UE_LOG(MyLog, Warning, TEXT("No DefaultEmptyWeaponSound on Weapon!"));
	}
}

int32 AWeapon::RarityToInt32(EItemRarity InItemRarity)
{
	switch (InItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		return 1;
		break;
	case EItemRarity::EIR_Common:
		return 2;
		break;
	case EItemRarity::EIR_Uncommon:
		return 3;
		break;
	case EItemRarity::EIR_Rare:
		return 4;
		break;
	case EItemRarity::EIR_Epic:
		return 5;
		break;
	case EItemRarity::EIR_Default:
		return 1;
		break;
	default:
		return 1;
		break;
	}
}


