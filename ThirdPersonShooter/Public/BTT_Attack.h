// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Attack.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API UBTT_Attack : public UBTTaskNode
{
	GENERATED_BODY()

private:
	bool bDataInitialized;

public:
	// Constructor
	UBTT_Attack(const FObjectInitializer& ObjectInitializer);

	// Essential a BeginPlay function, this is called as soons as the behavior Tree reaches this node
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
