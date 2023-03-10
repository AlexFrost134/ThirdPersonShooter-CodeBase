// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "EnemyAIController.h"
#include "Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Attack::UBTT_Attack(const FObjectInitializer& ObjectInitializer)
{
    // Renaming the node for Blueprint
    this->NodeName = FString(TEXT("GruxAttack"));
    /*bDataInitialized = false;*/
}


EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{    
   auto AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

    // Referenz to the Owner
   auto  Owner = Cast<AEnemy>(AIController->GetPawn());

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

    // Not Needed for now
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
    #if EDITOR 
        UE_LOG(AILog, Warning, TEXT("%s: BlackboardComp Null on: BBT_Attack"),  *Owner->GetActorLabel());
    #endif
        return  EBTNodeResult::Failed;
    }
      
    // Play Attack Animation
   if (Owner->PlayAttackMontage(Owner->GetRandomAttackSectionName(), Owner->GetAttackAnimationSpeed()))
   {
       return EBTNodeResult::Succeeded;
   }

#if EDITOR 
    UE_LOG(AILog, Warning, TEXT("%s: Animation failed on: BBT_Attack"), *Owner->GetActorLabel());
#endif
   return  EBTNodeResult::Failed;
}
