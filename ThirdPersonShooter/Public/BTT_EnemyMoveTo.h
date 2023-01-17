// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// Ref to the parent Class must be included
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"

// Must have
#include "BTT_EnemyMoveTo.generated.h"


/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API UBTT_EnemyMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()
		
private:
	
	UBTT_EnemyMoveTo(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	
	// Override the Parent Attribute
	void SetAcceptableRadius(float Radius) { AcceptableRadius = Radius; };

};
