// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAimOffsetState : uint8
{
	EAOS_RMBAiming	 UMETA(Displayname = "RMBAiming"),
	EAOS_HipAiming	 UMETA(Displayname = "HipAiming"),
	EAOS_Reloading	 UMETA(Displayname = "Reloading"),
	EAOS_InAir		 UMETA(Displayname = "InAir"),
	EAOS_Default	 UMETA(Displayname = "Default")

};

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UShooterAnimInstance();

	//Like Tick Function for actor classes
	virtual void NativeUpdateAnimation(float Deltatime) override;

	//Like Beginn Play for actor, only for AnimInstance Class
	virtual void NativeInitializeAnimation() override;

protected:
	// Handle turining in place variables
	void TurnInPlace();

	// Handle Calculations for leaning while running
	void Lean(float DeltaTime);

	// Call in Tick, make sure to get the right State of bReloading
	void SyncReloadingState();

	// Call in Tick, get the lateral speed of the Character
	void SyncSpeed();

	// Call in Tick, get the bool IsInAir of the Character
	void SyncIsCharacterInAir();

	// Call in Tick, get the bIsAccelerating State from the Character
	void SyncIsAccelerating();

	// Call in Tick, Set YawMOvementOffset for Straving Movement
	void SyncStraving();

	// Call in Tick, get bAiming State from the Character
	void SyncAimingState();

	// Call in Tick, decides if bShouldUseFABRIK is true or false. resulting in using Fabrik or not.
	void ShouldUseFABRIK();

	// Call in Tick, gets the current holding WeaponType from ShooterCharacter
	void SyncWeaponType();

	// Sets the right OffsetState determined by other boolean of the Character
	void SetOffsetState();

	// Set the recoil Weight
	void SetRecoilWeight();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	//Speed of the Character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	//Whether or not the Character is in the air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	//Whether or not the Character is in the air
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	// Offset Yaw used for strafing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// Offset Yaw the frame before we stopped moving
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;
	
	// Yaw of the Character this frame Turn in Place TIP, Only updated when standing Still and not in Air
	float TIPCharacterYaw;

	// Yaw of the Character the previous frame TurninPlace TIP, Only updated when standing Still and not in Air
	float TIPCharacterYawLastFrame;

	// Yaw Offset between Character Root Bone and PlayController Yaw
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true"))
	float TIPTotalRootYawOffset;

	// Yaw of the Character this frame
	FRotator CharacterRotation;

	// Yaw of the Character the previous frame 
	FRotator CharacterRotationLastFrame;

	// Yaw Delta used for leaning in the runnnin Blendspace
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lean", meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	// Roation Curve Value this frame
	float RotationCurveValue;

	// Roation Curve Value last frame
	float RotationCruveValueLastFrame;

	// The pitch of the aim Rotation used by Aim Offset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true"))
	float ControllerPitch;

	// True when reloading, used to prevent Aim Offset while Reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	// Offset State , used to determine which Aim Offset to use
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn in Place", meta = (AllowPrivateAccess = "true"))
	EAimOffsetState OffsetState;

	// True when Character crouches
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crouching", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bEquipping;

	// Value determines amount of visual recoil on the animation
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	// Turning in Place, true when player is turning
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bTIP;

	// WeaponType for the currently equipped Weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;

	// When true FABRIK is Used in the Animation
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;

};
