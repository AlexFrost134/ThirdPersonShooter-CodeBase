// Fill out your copyright notice in the Description page of Project Settings.


#include "GruxAnimInstance.h"
#include "Enemy.h"

UGruxAnimInstance::UGruxAnimInstance()
{

}

void UGruxAnimInstance::NativeInitializeAnimation()
{
	Enemy = Cast<AEnemy>(GetOwningActor());	
}

void UGruxAnimInstance::NativeUpdateAnimation(float Deltatime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		// Get the lateral Speed of the character
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}

	if (Speed > 1.f)
	{
		bMoving = true;
	}
	else
	{
		bMoving = false;
	}

	SyncbStunned();
}

void UGruxAnimInstance::SyncbStunned()
{
	Enemy->IsStunned();
}