// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GruxAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API UGruxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	// The CurrentSpeed of the Character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess))
	float Speed;

	// Reference to the Character, set in C++
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess))
	class AEnemy* Enemy;

	// Is true when Character is Moving
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess))
	bool bMoving;

	// True if Stunned;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess))
	bool bStunned;

protected:
	// Sync the bStunned Status with the Character
	void SyncbStunned();

public:
	UGruxAnimInstance();

	//Like Beginn Play for actor, only for AnimInstance Class
	virtual void NativeInitializeAnimation() override;

	//Like Tick Function for actor classes
	virtual void NativeUpdateAnimation(float Deltatime) override;

	
	
};
