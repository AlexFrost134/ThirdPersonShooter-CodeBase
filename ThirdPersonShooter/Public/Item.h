// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"


UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged			UMETA(DisplayName = "Damaged"),
	EIR_Common			UMETA(DisplayName = "Common"),
	EIR_Uncommon		UMETA(DisplayName = "Uncommon"),
	EIR_Rare			UMETA(DisplayName = "Rare"),
	EIR_Epic			UMETA(DisplayName = "Epic"),
	EIR_Default			UMETA(DisplayName = "Default")
};

// FTableRowBase is "struct Type"
USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

	FItemRarityTable():
		GlowColor(EForceInit::ForceInitToZero),
		LightColor(EForceInit::ForceInitToZero),
		DarkColour(EForceInit::ForceInitToZero),
		NumberOfStars(0),
		CustomDepthStencil(0)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;

};



UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_OnGround		UMETA(DisplayName = "OnGround"),
	EIS_EquipInterping	UMETA(DisplayName = "EquipInterping"),
	EIS_Carrying		UMETA(DisplayName = "Carrying"),
	EIS_Equppied		UMETA(DisplayName = "Equppied"),
	EIS_Falling			UMETA(DisplayName = "Falling"),
	EIS_Default			UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammuniton UMETA(DisplayName = "Ammo"),
	EIT_Weapon	  UMETA(DisplayName = "Weapon"),
	EIT_Cash	  UMETA(DisplayName = "Cash"),
	EIT_Health	  UMETA(DisplayName = "Health"),
	EIT_Droppable UMETA(DisplayName = "Droppable"),
	EIT_Default	  UMETA(DisplayName = "Default")
};

UCLASS()
class THIRDPERSONSHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when overlapping AreaSphere
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when End overlapping AreaSphere
	UFUNCTION()
		void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Sets the ActiveStars array of bools based on rarity
	void SetActiveStars();

	// Sets properties of the Item's components based on State
	virtual void SetItemProperties(EItemState state);

	// Called when ItemInterpolationTimer is finished
	void FinishInterpolation();

	// Handles item interpolation when in the EIS_EquipInterping State
	void ItemInterpolation(float DeltaTime);

	// Get interpLocation based on the item type
	FVector GetInterpLocation();

	// Plays PickupSound and starts TimerHandle
	void PlayPickUpSound();

	// Set the Default CustomDepth Behavior; Default = Disable
	virtual void InitializeCutomDepth();

	// Get Called Before BeginPlay and EveryTime the Object Transform changed
	virtual void OnConstruction(const FTransform& Transform) override;

	// Creates and set the Dynamic Material for a specific Index
	virtual void CreateDynamicMaterial();

	// Set Material Properties
	virtual void SetGlowMaterialProperties();

	// pure virtual
	virtual void HideBones() {};
	
	void ResetPulseTimer();

	void StartPulseTimer();

	void ClearPulseTimer();

	void UpdatePulse();

	void HandleDestory();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Plays PickupSound and starts TimerHandle; Called in ASHooterCharacter::GetPickupItem
	void PlayEquipSound();

protected:
	/// <summary>
	/// ONLY For Testing purpose
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		class USceneComponent* NewRootComponent;

	// Line trace collides with box to show HUD widgets
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		class UBoxComponent* CollisionBox;

	// SkleetalMesh for the Item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		class USkeletalMeshComponent* ItemSkeletalMesh;
	
	// PopUp Widget for when the player looks at the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		class UWidgetComponent* PickupWidget;

	// Enable item tracing when overlapped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		class USphereComponent* AreaSphere;

	// ItemRarity determines number of starts in Pickup Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RarityDataTable", meta = (AllowPrivateAccess))
		EItemRarity ItemRarity;

	// State of the Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		EItemState ItemState;

	// Enum for the Type of Item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess))
		EItemType ItemType;

	// Item Rarity DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RarityDataTable", meta = (AllowPrivateAccess))
	class UDataTable* ItemRarityDataTable;

	// Color in the glow material
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RarityDataTable", meta = (AllowPrivateAccess))
		FLinearColor GlowColor;

	// Light Color in the Pickup Widget
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RarityDataTable", meta = (AllowPrivateAccess))
		FLinearColor LightColor;

	// Dark Color in the HUD Inventory
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RarityDataTable", meta = (AllowPrivateAccess))
		FLinearColor DarkColor;

	// Number of stars in the pickupWidget
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RarityDataTable", meta = (AllowPrivateAccess))
		int32 NumberOfStars;

	// Background icon the for HUD Inventory
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "RarityDataTable", meta = (AllowPrivateAccess))
		UTexture2D* IconBackground;

	// Value to set the CustomDepthStencil within in OnConstruciton fuction
	int32 CustomDepthStencil;

private:
	
	
	// Name which appear on the item Pickup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		FString ItemName;

	// Amount of Item ( Ammo, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		int32 ItemCount;

	// Array that determies wich stars are visible on the Widget
	UPROPERTY(VisibleDefaultsOnly, AdvancedDisplay, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		TArray<bool> ActiveStars;

	// Curve asset to use for the item's Z location when interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		class UCurveFloat* ItemZCurve;

	// Curve used to sclae the item when interpolating
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		UCurveFloat* ItemScaleCurve;

	// Starting Location when interping begins
	UPROPERTY(VisibleDefaultsOnly, AdvancedDisplay, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		FVector ItemInterpStartLocation;

	// Target interp location in front of the camera
	UPROPERTY(VisibleDefaultsOnly, AdvancedDisplay, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		FVector CameraTargetLocation;

	// true when interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		bool bInterping;

	// Plays when we start interping
	FTimerHandle ItemInterpolationTimer;

	// Duration of the curve and Timer Default Value is 0.7 Sec. Siehe Constructer
	UPROPERTY(VisibleDefaultsOnly, AdvancedDisplay, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
		float ZCurveTime;

	// Pointer to the Character
	UPROPERTY(BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
	class AShooterCharacter* PlayerCharacter;

	// Makes sure that MeshLocalRotation gets reset ones before the weapon gets Interpolated
	bool bLocalRotationResetDone;

	// X Position for the Item while in the EIS_EquipInterping state
	float ItemInterpX;
	// Y Position for the Item while in the EIS_EquipInterping state
	float ItemInterpY;

	// Initial Yaw offset between the camera and the interpolating Item
	float InterpolationInitialYawOffset;

	// SoundPlayed when picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Sound", meta = (AllowPrivateAccess))
	class USoundCue* PickUpSound;

	// Sound Play when equiped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Sound", meta = (AllowPrivateAccess))
	USoundCue* EquipSound;

	// Index of the Location this item is interping to
	UPROPERTY(VisibleInstanceOnly, AdvancedDisplay, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess))
	int32 InterpLocationIndex;

	// Index for the Material we change while runtime
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Material", meta = (AllowPrivateAccess))
	int32 MaterialIndex;

	// Dynamic Instance we can change while runtime
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	// Used by the DynamicMaterialInstance
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Material", meta = (AllowPrivateAccess))
	UMaterialInstance* MaterialInstance;

	// Does Allow to Change to CustomDepth Value
	bool bCanChangeCustomDepth;

	// Curve to drive dynamic material parameters
	// @param X = GlowAmount
	// @param Y = FresnelExponent
	// @param Z = FresnelReflectFraction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess))
	class UCurveVector* PulseCurve;

	// Curve to drive dynamic material parameters while interping
	// @param X = GlowAmount
	// @param Y = FresnelExponent
	// @param Z = FresnelReflectFraction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess))
	UCurveVector* PulseCurveInterping;

	FTimerHandle PulseTimer;

	// Time for the PulseTimer
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess))
		float PulseCurveTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess))
		float GlowAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess))
		float FresnelExponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess))
		float FresnelReflectFraction;

	/*// BackgroundImage for this Item in the Inventory UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess))
		UTexture2D* IconBackground;
	*/

	// Icon for this Item in the Inventory UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess))
	UTexture2D* IconItem;
	
	// Ammunition Icon for this Item in the Inventory UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess))
	UTexture2D* IconAmmunition;

	// Inventory Slot of this Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess))
	int32 InventorySlot;
	

public: // Getter and Setter

	UWidgetComponent* GetPickUpWidget(); //const;

	FORCEINLINE UDataTable* GetItemRarityDataTable() { return ItemRarityDataTable; };

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; };

	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; };

	void SetItemState(EItemState State);

	FORCEINLINE EItemState GetItemState() const { return ItemState; };

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemSkeletalMesh; };
	
	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; };

	bool GetbLocalRotationResetDone() const { return bLocalRotationResetDone; };

	void SetbLocalRotationResetDone(bool State) { bLocalRotationResetDone = State; };

	// Called from the AShooterCharacter class
	void StartItemCurve(AShooterCharacter* Character);

	FORCEINLINE USoundCue* GetPickUpSound() const { return PickUpSound; };
	FORCEINLINE void SetPickUpSound(USoundCue* SoundCue)  {  PickUpSound = SoundCue; };

	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; };
	FORCEINLINE void SetEquipSound(USoundCue* SoundCue)  { EquipSound = SoundCue; };

	FORCEINLINE int32 GetItemCount() const { return ItemCount; };
	FORCEINLINE int32 GetMaterialIndex() const { return MaterialIndex; };
	FORCEINLINE void SetMaterialIndex(int32 Index) { MaterialIndex = Index; };

	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance() { return DynamicMaterialInstance; };

	FORCEINLINE void SetDynamicMaterialInstance(UMaterialInstanceDynamic* Material) { DynamicMaterialInstance = Material; };

	FORCEINLINE UMaterialInstance* GetMaterialInstance() { return MaterialInstance; };

	FORCEINLINE void SetMaterialInstance(UMaterialInstance* Material) {  MaterialInstance = Material; };

	void SetItemType(EItemType NewItemType) { ItemType = NewItemType; };

	EItemType GetItemType() { return ItemType; };

	virtual void EnableCustomDepth();

	virtual void DisableCustomDepth();

	void EnableGlowMaterial();

	void DisableGlowMaterial();

	FORCEINLINE EItemRarity GetItemRarity() { return ItemRarity; };

	// Set the ItemRarity
	void SetItemRarity(EItemRarity ItemTier) { ItemRarity = ItemTier; };

	FORCEINLINE bool CanChangeCustomDepth() { return bCanChangeCustomDepth; };

	FORCEINLINE int32 GetInventorySlot() const	{ return InventorySlot; };

	FORCEINLINE void SetInventorySlot(int32 Index) {  InventorySlot = Index; };

	FORCEINLINE void SetIconItem(UTexture2D* Icon) { IconItem = Icon; };

	FORCEINLINE void SetIconAmmunition(UTexture2D* Icon) { IconAmmunition = Icon; };

};
