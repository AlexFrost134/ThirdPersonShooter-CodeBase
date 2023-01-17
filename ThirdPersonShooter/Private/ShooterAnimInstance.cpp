// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "Weapon.h"
#include "ShooterPlayerController.h"


UShooterAnimInstance::UShooterAnimInstance()
{
	
	LastMovementOffsetYaw = 0.f;
	MovementOffsetYaw = 0.f;
	bAiming = false;
	bIsAccelerating = false;
	bIsInAir = false;
	Speed = 0.f;
	TIPCharacterYaw = 0.f;
	TIPCharacterYawLastFrame = 0.f;
	TIPTotalRootYawOffset = 0.f;
	YawDelta = 0.f;
	ControllerPitch = 0.f;
	bReloading = false;
	OffsetState = EAimOffsetState::EAOS_HipAiming;
	CharacterRotationLastFrame = FRotator(0.f);
	CharacterRotation = FRotator(0.f);
	RecoilWeight = 1.f;
	bTIP = false;
	EquippedWeaponType = EWeaponType::EWT_Default;
	bShouldUseFABRIK = false;
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());

}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr)
	{
		return;
	}

	// Get Pitch from the Controller of the ShooterCharacter.
	 ControllerPitch = ShooterCharacter->GetBaseAimRotation().Pitch;
		
	
	if (Speed > 0.f || bIsInAir) // Don't want to turn in place, Character is moving
	{
		// Reset Variables / Zeroing out
		TIPTotalRootYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;

		RotationCruveValueLastFrame = 0.f;
		RotationCurveValue = 0.f;
		
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		float DeltaYawThisFrame = TIPCharacterYaw - TIPCharacterYawLastFrame;

		// Normalize Axis Clamps the difference to [-180, 180]
		// TIPTotalRootYawOffset > 0 -> turning left,
		// TIPTotalRootYawOffset < 0 -> turning right
		TIPTotalRootYawOffset = UKismetMathLibrary::NormalizeAxis(TIPTotalRootYawOffset - DeltaYawThisFrame);

		
		// Get Value form MetaData-Curve | 1.0 when playing turning animation, 0.0 when not playing turning animation
		float Turning;
		GetCurveValue(FName(TEXT("Turning")), Turning);

		// Turning iy greater > 0 if Turining Animations is playing
		if (Turning > 0)
		{
			// Character is Turning
			bTIP = true;

			RotationCruveValueLastFrame = RotationCurveValue;
			RotationCurveValue = GetCurveValue(FName(TEXT("RotationCurve")));
			float DeltaRotationValue = RotationCurveValue - RotationCruveValueLastFrame;
				
			// > 0 turning left, < 0 turning right
			TIPTotalRootYawOffset > 0 ? TIPTotalRootYawOffset -= DeltaRotationValue : TIPTotalRootYawOffset += DeltaRotationValue;

			// Make sure TIPTotalRootYawOffset does not go beyond 90.f
			// Also corrects if TIPTotalRootYawOffset is to great.
			float AbsoluteRootYawOffset = FMath::Abs(TIPTotalRootYawOffset);

			if (AbsoluteRootYawOffset > 90.f)
			{
				float YawExcess = AbsoluteRootYawOffset - 90.f;
				TIPTotalRootYawOffset > 0 ? TIPTotalRootYawOffset -= YawExcess : TIPTotalRootYawOffset += YawExcess;
			}
		}
		else
		{ 
			// Character is not Turning
			bTIP = false;
		}
	/*
		GEngine->AddOnScreenDebugMessage(1, -1.f, FColor::White, FString::Printf(TEXT("TIPCharacterYaw: %f"), TIPCharacterYaw));	
		GEngine->AddOnScreenDebugMessage(2, -1.f, FColor::White, FString::Printf(TEXT("DeltaYawThisFrame: %f"), DeltaYawThisFrame));
		GEngine->AddOnScreenDebugMessage(3, -1.f, FColor::White, FString::Printf(TEXT("TIPTotalRootYawOffset: %f"), TIPTotalRootYawOffset));
		*/
	}

	SetRecoilWeight();
}

void UShooterAnimInstance::NativeUpdateAnimation(float Deltatime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{	
		bCrouching = ShooterCharacter->GetCrouching();

		bEquipping = ShooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;

		SyncReloadingState();

		SyncSpeed();
		
		SyncIsCharacterInAir();

		SyncIsAccelerating();

		SyncStraving();

		SyncAimingState();
		
		SetOffsetState();

		ShouldUseFABRIK();

		// Sync Weapon Type
		SyncWeaponType();

		

		
					
			
		

		/*
		// Debug
		FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation %f"), AimRotation.Yaw);
		FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation %f"), MovementRotation.Yaw);
		FRotator NonNormalizedRotation = MovementRotation - AimRotation;
		FString NonNormalizedRotations = FString::Printf(TEXT("Non Normailzed Rotation %f"), NonNormalizedRotation.Yaw);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, RotationMessage);
			GEngine->AddOnScreenDebugMessage(2, 0, FColor::White, MovementRotationMessage);
			GEngine->AddOnScreenDebugMessage(3, 0, FColor::White,FString::Printf(TEXT("MovementOffsetYaw: %f"), MovementOffsetYaw));
			GEngine->AddOnScreenDebugMessage(0, 0, FColor::White, NonNormalizedRotations);
		}
		*/
	}

	// Handles Turning Animation and AImOffset Animation as well
	TurnInPlace();

	// Handels Leaning while running
	Lean(Deltatime);
}

// Need Delta Time for interpolation
void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		return;
	}
	
	// Get Yaw Values from Character
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	// Handels signChanges gives us the Delta of Rotation, independet of sign changes
	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);

	// Increae Target to get a bigger Number,
	float Target = Delta.Yaw * 50;

	// Interpolate Yaw
	float Interp = FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f);
	
	// Clamp Yaw
	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
	
	//GEngine->AddOnScreenDebugMessage(2, -1.f, FColor::White, FString::Printf(TEXT("YawDelta: %f"), YawDelta));
	//GEngine->AddOnScreenDebugMessage(3, -1.f, FColor::White, FString::Printf(TEXT("Delta.Yaw: %f"), Delta.Yaw));


}

void UShooterAnimInstance::SyncReloadingState()
{
	// Set CombatState
	ECombatState::ECS_Reloading == ShooterCharacter->GetCombatState() ? bReloading = true : bReloading = false;

}

void UShooterAnimInstance::SyncSpeed()
{
	//Get the lateral speed of the character form velocity
	FVector Velocity = ShooterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

}

void UShooterAnimInstance::SyncIsCharacterInAir()
{
	//Is the Character in the air?
	bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

}

void UShooterAnimInstance::SyncIsAccelerating()
{
	//Is the Character Accelerating?
	if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
	{
		bIsAccelerating = true;
		LastMovementOffsetYaw = MovementOffsetYaw;
	}
	else
	{
		bIsAccelerating = false;
	}
}

void UShooterAnimInstance::SyncStraving()
{
	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();

	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

	// Difference between AimingRotaion and MovementRotation
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;  //Straving Right Positiv Value, Straiving Left Negativ Value

}

void UShooterAnimInstance::SyncAimingState()
{
	// Get Aiming State from ShooterCharacter
	bAiming = ShooterCharacter->GetAimingState();
}
void UShooterAnimInstance::SetOffsetState()
{
	if (bReloading)
	{
		OffsetState = EAimOffsetState::EAOS_Reloading;
	}
	else if (bIsInAir)
	{
		OffsetState = EAimOffsetState::EAOS_InAir;
	}
	else if (ShooterCharacter->GetAimingState())
	{
		OffsetState = EAimOffsetState::EAOS_RMBAiming;
	}
	else
	{
		OffsetState = EAimOffsetState::EAOS_HipAiming;
	}
}

void UShooterAnimInstance::SetRecoilWeight()
{
	if (bTIP)
	{
		if (bReloading || bEquipping)
		{
			RecoilWeight = 1.f;
		}
		else
		{
			RecoilWeight = 0.f;
		}
	}
	else // not TIP
	{

		if (bCrouching)
		{
			if (bReloading || bEquipping)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.2f;
			}
		}
		else
		{
			if (bAiming || bReloading || bEquipping)
			{
				RecoilWeight = 1.f;

			}
			else
			{
				RecoilWeight = 0.5f;
			}
		}
	}
}

void UShooterAnimInstance::ShouldUseFABRIK()
{
	// Depending on EWeaponType we toggle FABRIK
	if (ShooterCharacter)
	{
		switch (ShooterCharacter->GetCombatState())
		{
		case ECombatState::ECS_Unoccupied:
			bShouldUseFABRIK = true;
			break;

		case ECombatState::ECS_FireTimerInProgress:
			bShouldUseFABRIK = true;
			break;

		case ECombatState::ECS_Reloading:
			bShouldUseFABRIK = false;
			break;

		case ECombatState::ECS_Equipping:
			bShouldUseFABRIK = false;
			break;

		case ECombatState::ECS_Default:
			bShouldUseFABRIK = false;
			break;
		}
	}
}

void UShooterAnimInstance::SyncWeaponType()
{
	if (ShooterCharacter->GetEquippedWeapon())
	{
		EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
	}
}

