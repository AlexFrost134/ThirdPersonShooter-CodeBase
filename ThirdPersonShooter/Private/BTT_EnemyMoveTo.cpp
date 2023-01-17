// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_EnemyMoveTo.h"
#include "EnemyAIController.h"
#include "Enemy.h"


UBTT_EnemyMoveTo::UBTT_EnemyMoveTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "MoveToCustom";
	
}

EBTNodeResult::Type UBTT_EnemyMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	AEnemy* Owner = Cast<AEnemy>(AIController->GetPawn());

	if (!AIController)
	{
		UE_LOG(AILog, Warning, TEXT("%s: Controller Null"), *OwnerComp.GetName());
		return  EBTNodeResult::Failed;
	}

	if (!Owner)
	{
		UE_LOG(AILog, Warning, TEXT("%s: Owner Null"), *OwnerComp.GetName());
		return  EBTNodeResult::Failed;
	}
	// Set Radius
	SetAcceptableRadius(Owner->GetAIAcceptableRadius());

	//UE_LOG(AILog, Warning, TEXT("AcceptableRadius is: %f on %s"), AcceptableRadius, *Owner->GetActorLabel());
	// Overriding Default Calculation in Parent Constructor
	// This should make sure that the ObservedBlackboardValueTolerance is corecct calculated
	ObservedBlackboardValueTolerance = AcceptableRadius * 0.95f;

	EBTNodeResult::Type NodeResult = Super::ExecuteTask(OwnerComp, NodeMemory);
	return NodeResult;
}
