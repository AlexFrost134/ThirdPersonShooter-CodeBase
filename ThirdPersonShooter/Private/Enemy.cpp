// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "CustomLogs.h"
#include "DrawDebugHelpers.h"
#include "DroppableItem.h"
#include "EnemyAIController.h"
#include "Explosiv.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacter.h"
#include "StatsCalculator.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	// Functionality
	bActivateAgressionSphereFunctionality = false;
	bActivatePatrolPointsFunctionality = false;
	
	PointerToSelf = this;
	MaxHealth = 100.f;
	BaseDamage = 10.f;
	bShowHealthBar = false;
	bShowHealthAmount = false;

	ExperiencePointsOnKill = 1;
	
	// TODO Write own function
	CurrentHealth = MaxHealth;
	
	bStunned = false;
	bDead = false;
	StunResistance = 10.f;
	StunChance = 5.f;
	DefaultMovementSpeed = 600.f;
	HalfMontageSectionTime = 0.f;
	HitNumberDestroyTime = 2.f;

	// Animation
	AttackMontageBlendInTime = 0.25f;
	AttackMontageBlendOutTime = 0.25f;
	
	AttackAnimationSpeed = 1.f;

	// Attack Cooldown
	bIsOnAttackCooldown = false;
	bAttackAnimationIsPlaying = false;
	AttackCooldownTime = 3.f;
	AttackCooldownDeviationMax = 0.f;
	AttackCooldownDeviationMin = 0.f;
	AttackCooldownDeviation = 0.f;
	
	FollowTimeAfterShoot = 15.f;
	bForceAggressionOnPlayer = false;

	bAllowTurntoTargetWhileAttacking = false;
	bAllowToTurn = false;
		
	DestroyDelayAfterDeath = 10.f;
	bMainCharacterIsDead = false;

	LeftWeaponSocket = TEXT("FX_Trail_L_01");
	RightWeaponSocket = TEXT("FX_Trail_R_01");

	// Effect
	ImpactParticlesSize = { 0.5f, 0.5f, 0.5f };

	// AI
	AIAcceptableRadius = 1.f;

	//Loot
	AmountofLoot = 1;
	MinAmountofLoot = 1;
	MaxAmountofLoot = 1;

	// Declare the Montage section in the Array
	AttackMontageSections =	{
		FName(TEXT("PrimaryAttack_LA")),
		FName(TEXT("PrimaryAttack_LAF")),
		FName(TEXT("PrimaryAttack_RA")),
		FName(TEXT("PrimaryAttack_RAF"))
	};
	// Set Montage Sections for DeathMontage
	DeathMontageSections = {
		FName(TEXT("Death_A")),
		FName(TEXT("Death_B"))
	};

 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup Agression Sphere
	AgressionSphere2 = CreateDefaultSubobject<USphereComponent>(FName(TEXT("AggressionSphere")));
	AgressionSphere2->SetupAttachment(RootComponent);

	// Create and setup Melee Sphere
	MeleeRangeSphere = CreateDefaultSubobject<USphereComponent>(FName(TEXT("MeleeRangeSphere")));
	MeleeRangeSphere->SetupAttachment(RootComponent);

	// Create Right and Left Weapon Collision Box
	RightWeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("RWeaponCollision")));
	RightWeaponCollisionBox->SetupAttachment(GetMesh(), FName(TEXT("RWeaponSocket")));

	LeftWeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("LWeaponCollision")));
	LeftWeaponCollisionBox->SetupAttachment(GetMesh(), FName(TEXT("LWeaponSocket")));

	// Hitzones
	InitializeHitZoneImpactSound();
	InitializeHitZoneImpactEffects();	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Block Camera Movement into Enemy
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	// Set AiController and runs BehaviorTree
	SetupEnemyBehaviorTree();
	
	// Get Ref to the Player
	StoreRefToPlayer();

	// Take care of Health
	EnsureMaxHealthisCurrentHealth();	
	
	// Set Movement Speed
	GetCharacterMovement()->MaxWalkSpeed = DefaultMovementSpeed;

	// Patrol Functionality
	SetUpPatrolPointsFunctionanlity();

	// Set bMainCharacterisDead State on the BehaviorTree
	SetMainCharacterIsDead_DepedingOnCharacterState();
	
	// Set the Attack Target based on bActivateAgressionSphereFunctionality
	HandleAttackTarget();
	
	// Bind Methods
	MeleeRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::MeleeRangeSpherBeginOverlap);
	MeleeRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::MeleeRangeSphereEndOverlap);
	LeftWeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::LeftWeaponCollisionBoxBeginOverlap);
	RightWeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::RightWeaponCollisionBoxBeginOverlap);
		
	// Set Collision Settings for Weaponboxes
	SetupWeaponCollisionBoxes();

	// Register Delegate
	RegisterCharacterIsDeadDelegate();

	// Add Tag to the Enemy
	SetEnemyTag();
	
	// MOVE
	InitializeDefaultFHitZoneValues();
	UpdateFHitZoneValues();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Take care of Animating HitValues
	UpdateHitNumbersScreenPosition();

	// Makes the Character turn to the Target, If Target is Valid
	TurnToTarget();
}

void AEnemy::SetupEnemyBehaviorTree()
{
	// Get AIController
	EnemyAIController = Cast<AEnemyAIController>(GetController());

	if (EnemyAIController)
	{	// BehaviorTree
		EnemyAIController->RunBehaviorTree(BehaviorTree);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bDead")), false);
	}
}

void AEnemy::HandleAttackTarget()
{
	// Overridding UBaseShape Methods
	if (bActivateAgressionSphereFunctionality)
	{
		SetupAgressionSphereFunctionality();
	}
	else
	{
		AShooterCharacter* Player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (Player)
		{
			if (!(Player->GetCharacterIsDead())) // If the player is not Dead set Target
			{
				AttackTarget = Player;
			}
		}
	}
}

void AEnemy::SetupAgressionSphereFunctionality()
{
	IsPlayerInsideAggressionSphere(true);
	AgressionSphere2->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgressionSphereEndOverlap);
	AgressionSphere2->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgressionSphereOverlap);
}

bool AEnemy::IsPlayerInsideAggressionSphere(bool SetAsTarget)
{
	// Get Overlap Actors within the AgressionSphere, only search for ASHooterCharacters
	SpawingOverlappingActor.Empty();
	AgressionSphere2->GetOverlappingActors(SpawingOverlappingActor, TSubclassOf<AShooterCharacter>(AShooterCharacter::StaticClass()));

	for (AActor* Found : SpawingOverlappingActor)
	{		
		if (Found)
		{
			AttackTarget = Cast<AShooterCharacter>(Found);
			
			// Found is APlayerCharacter
			if (AttackTarget)
			{
				// Set the Target to the Character
				if (SetAsTarget && EnemyAIController)
				{
					EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetActor")), AttackTarget);
				}

				return true;
			}
			
		}
	}
	return false;
}

void AEnemy::TranformLocalVectorToWorldVector(FVector& InLocalVector)
{
	//UE_LOG(LogTemp, Warning, TEXT("LocalPosition: %s"), *InLocalVector.ToCompactString());

	// Convert PatrolPosition to WorldPosition and Draw DebugSphere
	InLocalVector = UKismetMathLibrary::TransformLocation(GetActorTransform(), InLocalVector);

	//UE_LOG(LogTemp, Warning, TEXT("WorldPosition: %s"), *InLocalVector.ToCompactString());
}

void AEnemy::UpdatePatrolPointsLocations()
{
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolPoint")), PatrolPoint);
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolPoint2")), PatrolPoint2);
	}
}

void AEnemy::DrawDebugBoxOnPatrolPoints()
{
	DrawDebugBox(GetWorld(), PatrolPoint, FVector(20.f), FColor(FColor::Emerald), false, -1.f, 0, 5.f);
	DrawDebugBox(GetWorld(), PatrolPoint2, FVector(20.f), FColor(FColor::Emerald), false, -1.f, 0, 5.f);
}

void AEnemy::AgressionSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}
		
	if (EnemyAIController)
	{ 
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetActor")), OtherActor);
		bAllowToTurn = true;

	}	
}

void AEnemy::AgressionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	// Disallow appeasement of the Enemy as long as bForceAggressionOnPlayer is true
	// The Character shot the Enemy and therfore is running after him
	if (bForceAggressionOnPlayer)
	{
		return;
	}

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);

	if (Character)
	{
		if (EnemyAIController)
		{
			bAllowToTurn = false;
			EnemyAIController->GetBlackboardComponent()->ClearValue(FName(TEXT("TargetActor")));
			AttackTarget = nullptr;
		}
	}
}

void AEnemy::MeleeRangeSpherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	if (Cast<AShooterCharacter>(OtherActor))
	{
		if (EnemyAIController)
		{
			bAllowToTurn = true;
			bInAttackRange = true;
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bInMeleeRange")), true);
		}
	}
}

void AEnemy::MeleeRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	if (Cast<AShooterCharacter>(OtherActor))
	{	
		if (EnemyAIController)
		{
			
			bInAttackRange = false;
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bInMeleeRange")), false);
		}
	}
}

void AEnemy::DealDamageToPlayerCharacter(AShooterCharacter* PlayerCharacter)
{	
	if (PlayerCharacter && !bDealtDamageInThisAnimation)
	{
		float Damage = 0.f;
		FHitZone HitZone;
		HitZone.DamagedActor = PlayerCharacter;
		HitZone.InstigatorRef = this;
		Damage = StatsCalculator::CalculateDamage(HitZone);

		//UE_LOG(CombatLog, Warning, TEXT("DealDamage to Player"));
		UGameplayStatics::ApplyDamage(PlayerCharacter, Damage, EnemyAIController, this, UDamageType::StaticClass());
		
		// Check for stunning the Player Character
		if (PlayerCharacter->RollForStunResistance(GetStunChance()))
		{
			PlayerCharacter->PlayHitReactMontage();
		}
		
		// Play Impact Sound
		if (PlayerCharacter->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), PlayerCharacter->GetMeleeImpactSound());
		}

		// Disallow to deal Damage to the Player if the Player got already hit once in this Animation
		bDealtDamageInThisAnimation = true;
	}
}

void AEnemy::RightWeaponCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{	
		//TO DO: Implement Melee Interface?
		// Cast Propably to CPU intensif but okey for now
		AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
		AExplosiv* Explosiv = Cast<AExplosiv>(OtherActor);

		if (Player)
		{
			DealDamageToPlayerCharacter(Player);

			SpawnBloodEmitter(Player->GetBloodParticles(), OtherComp, 0.6f);
		}
		if (Explosiv)
		{
			UGameplayStatics::ApplyDamage(Explosiv, BaseDamage, EnemyAIController, this, UDamageType::StaticClass());
			if (Explosiv->GetMeleeImpactSound())
			{ 
				UGameplayStatics::PlaySound2D(GetWorld(), Explosiv->GetMeleeImpactSound());
			}
		}
	}
}

void AEnemy::LeftWeaponCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
		AExplosiv* Explosiv = Cast<AExplosiv>(OtherActor);

		if (Player)
		{
			DealDamageToPlayerCharacter(Player);
			SpawnBloodEmitter(Player->GetBloodParticles(), OtherComp, 0.6f);
		}
		if (Explosiv)
		{			
			UGameplayStatics::ApplyDamage(Explosiv, BaseDamage, EnemyAIController, this, UDamageType::StaticClass());
			if (Explosiv->GetMeleeImpactSound())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), Explosiv->GetMeleeImpactSound());
			}
		}
	}


}


void AEnemy::SpawnBloodEmitter(UParticleSystem* BloodParticle, UPrimitiveComponent* OtherComp, double EmitterScale)
{		
	// Create Blood Effect
	if (BloodParticle && this->GetWorld())
	{
		FTransform ScaledTransform = OtherComp->GetComponentTransform().GetScaled(EmitterScale);
		UGameplayStatics::SpawnEmitterAttached(BloodParticle, OtherComp, FName(NAME_None), FVector(ForceInit), FRotator(0.f), FVector(EmitterScale));
	}
}

 void AEnemy::ShowHealthAmount_Implementation()
{	
	GetWorld()->GetTimerManager().ClearTimer(ShowHealthAmountTimer);
	GetWorld()->GetTimerManager().SetTimer(ShowHealthAmountTimer, this, &AEnemy::HideHealthAmount, ShowHealthAmountForTimeX);
		
}

void AEnemy::ShowHealthBar()
{
	if (bDead)
	{
		return;
	}

	bShowHealthBar = true;
	GetWorld()->GetTimerManager().SetTimer(ShowHealthBarDisplayTimer, this, &AEnemy::HideHealthBar, ShowHealthbarForTimeX);
}

void AEnemy::HideHealthBar()
{
	 bShowHealthBar = false;
}

bool AEnemy::PlayAttackMontage(FName MontageSectionName, float PlayRate, bool bAllowAttackTurning)
{
	if (PlayRate <= 0.f || MontageSectionName == "" || MontageSectionName == "NONE")
	{
	#if EDITOR 
		UE_LOG(MyLog, Error, TEXT("PlayAttackMontage on %s, has a Playrate of %f, which is should be allowed! Or MontageSectionName is invalid! "), *GetActorLabel(), PlayRate);
	#endif
		return false;
	}

	float SectionLength = 0.f;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		// Set the BlendTime based on custom Value derrived from Blueprint.
		// Each EnemyType may have different Values
		AttackMontage->BlendIn.SetBlendTime(AttackMontageBlendInTime);
		AttackMontage->BlendOut.SetBlendTime(AttackMontageBlendOutTime);
					
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(MontageSectionName, AttackMontage);
			
		// Get SectionLength
		int32 SectionIndex = AnimInstance->GetCurrentActiveMontage()->GetSectionIndex(MontageSectionName);
		SectionLength = AnimInstance->GetCurrentActiveMontage()->GetSectionLength(SectionIndex);
		
		if (bAllowAttackTurning)
		{
			FTimerHandle AttackAnimationHandle;
			// Start Timer, Timer Length is set to Length of the MontageSection
			GetWorldTimerManager().SetTimer(AttackAnimationHandle, this, &AEnemy::AttackAnimationCallback, SectionLength / PlayRate);
		}

		// Set Cooldown and ActivateTimer for Cooldown
		StartAttackCooldownTimer(SectionLength / PlayRate);
		
		// Set bFinishedAttackAnimation to true, due Animation is starting
		SetAttackAnimationIsPlaying(true);

		// Allow Turning while Attacking
		SetAllowTurntoTargetWhileAttacking(true);
		
		return true;
	}
	return false;
}

void AEnemy::StartAttackCooldownTimer(float Length)
{
	GetWorldTimerManager().SetTimer(AttackCooldownHandle, this, &AEnemy::AttackCooldownCallback, Length + AttackCooldownTime + CalculateAttackCooldownDeviation());
	
	SetIsOnAttackCooldownState(true);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bIsOnAttackCooldown")), true);
}

float AEnemy::CalculateAttackCooldownDeviation()
{
	AttackCooldownDeviation = FMath::FRandRange(AttackCooldownDeviationMin, AttackCooldownDeviationMax);
	
	// if the Deviation is smaller than the AttackCooldownTime then set the Deviation the to negativ AttackCooldownTime
	if (AttackCooldownTime - AttackCooldownDeviation <= 0.f)
	{
		AttackCooldownDeviation = -AttackCooldownTime;
	}

	return AttackCooldownDeviation;
}

void AEnemy::SetIsOnAttackCooldownState(bool State)
{
	bIsOnAttackCooldown = State;
}

void AEnemy::AttackCooldownCallback()
{
	SetIsOnAttackCooldownState(false);
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bIsOnAttackCooldown")), false);
}

void AEnemy::AttackAnimationCallback()
{
	SetAllowTurntoTargetWhileAttacking(false);
	SetAttackAnimationIsPlaying(false);
}

void AEnemy::TurnToTarget()
{
	// Dont allow Turing if the Enemy is Dead
	if (bDead || bMainCharacterIsDead)
	{
		return;
	}

	if (bAllowTurntoTargetWhileAttacking || bAllowToTurn)
	{
		FVector VectorToTarget;

		if (AttackTarget)
		{
			// Calculate TargetPosition
			VectorToTarget = AttackTarget->GetActorLocation() - GetActorLocation();
			// DrawDebugLine(GetWorld(), GetActorLocation(), AttackTarget->GetActorLocation(), FColor::Red, false, -1.f, 0, 5.f);
			// DrawDebugBox(GetWorld(), VectorToTarget + GetActorLocation(), FVector(10.f), FColor::Cyan, false, -1.f, 0, 5.f);

			// Convert Vector to Rotation
			FRotator NewRotation = VectorToTarget.Rotation();
			//	GEngine->AddOnScreenDebugMessage(3, -1.f, FColor::Blue, FString::Printf(TEXT("Target to Rotation: %s"), *Test.ToString()));

			// Allowing slighty to look up
			if (FMath::Abs(NewRotation.Pitch) >= 25.f)
			{
				NewRotation.Pitch = 25.f;
			}
			
			NewRotation.Roll = 0.f;

			// Set New Rotation to Actor
			SetActorRotation(NewRotation);
		}
	}
}

FName AEnemy::GetRandomAttackSectionName()
{
	int32 RandomSection = FMath::RandRange(0, AttackMontageSections.Num() -1);

	FName ReturnSectionName = AttackMontageSections[RandomSection];

	return ReturnSectionName;
}

void AEnemy::PlayHitMontage(FName MontageSectionName, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (HitMontage && AnimInstance)
	{
		// Set the Length of the timer to a Random Duration of the MontageSectionLength
		float FullSectionLength = HitMontage->GetSectionLength(HitMontage->GetSectionIndex(MontageSectionName));
		float RandomSectionPlayRatioAmount = FMath::FRandRange(0.5f, 0.9f);
		HalfMontageSectionTime = FullSectionLength * RandomSectionPlayRatioAmount;

		// Only allow playing HitMontage when the timer has fired
		if (!GetWorldTimerManager().IsTimerActive(HitMontageReactTimer))
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			
			AnimInstance->Montage_JumpToSection(MontageSectionName, HitMontage);
			// Delaying next play of the HitMontage
			GetWorldTimerManager().SetTimer(HitMontageReactTimer, HalfMontageSectionTime, false);
		}		
	}
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumerDelegate;

	// Bind a Function to a Delegate, the benefit is we can send Parameters within the Delegate, 
	// so when the timer has finished the function gets called with the "HitNumber-Parameter"
	HitNumerDelegate.BindUFunction<AEnemy, UUserWidget*>(this, FName(TEXT("DestoryHitNumberWidget")), HitNumber);
	GetWorldTimerManager().SetTimer(HitNumberTimer, HitNumerDelegate, HitNumberDestroyTime, false);
}

void AEnemy::DestoryHitNumberWidget(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemy::UpdateHitNumbersScreenPosition()
{
	// TMap is Empty
	if (HitNumbers.IsEmpty())
	{
		return;
	}

	// TMap has Elements
	// Iterate through TMap and Update its Widget Location
	for (TPair<UUserWidget*, FVector> HitPair : HitNumbers)
	{
		UUserWidget* Widget = HitPair.Key;
		const FVector Position = HitPair.Value;
		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Position, ScreenPosition);
		Widget->SetPositionInViewport(ScreenPosition);
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::PlayHitSoundForHitZone(FHitZone* HitZone)
{
	// Get SurfaceType form HitZone
	FHitZone* AnotherHitZone = *ZoneDamage.Find(UPhysicalMaterial::DetermineSurfaceType(HitZone->PhysicalMaterial));
	EPhysicalSurface Type = UPhysicalMaterial::DetermineSurfaceType(AnotherHitZone->PhysicalMaterial);
		
	switch (Type)
	{
	case SurfaceType_Default:
		CheckSoundForHitZone(99);
		break;
	case SurfaceType8:
		CheckSoundForHitZone(0);
		break;
	case SurfaceType9:
		CheckSoundForHitZone(1);
		break;
	case SurfaceType10:
		CheckSoundForHitZone(2);
		break;
	case SurfaceType11:
		CheckSoundForHitZone(3);
		break;
	case SurfaceType12:
		CheckSoundForHitZone(4);
		break;
	default:
		CheckSoundForHitZone(99);
		break;
	}
}

void AEnemy::PlayEffectForHitZone(const FHitResult& HitResult, FHitZone* HitZone)
{
	// Get SurfaceType form HitZone
	FHitZone* AnotherHitZone = *ZoneDamage.Find(UPhysicalMaterial::DetermineSurfaceType(HitZone->PhysicalMaterial));
	EPhysicalSurface Type = UPhysicalMaterial::DetermineSurfaceType(AnotherHitZone->PhysicalMaterial);

	switch (Type)
	{
	case SurfaceType_Default:
		CheckEffectForHitZone(HitResult,99);
		break;
	case SurfaceType8:
		CheckEffectForHitZone(HitResult, 0);
		break;
	case SurfaceType9:
		CheckEffectForHitZone(HitResult, 1);
		break;
	case SurfaceType10:
		CheckEffectForHitZone(HitResult, 2);
		break;
	case SurfaceType11:
		CheckEffectForHitZone(HitResult, 3);
		break;
	case SurfaceType12:
		CheckEffectForHitZone(HitResult, 4);
		break;
	default:
		CheckEffectForHitZone(HitResult, 99);
		break;
	}
}

void AEnemy::CheckEffectForHitZone(const FHitResult& HitResult, uint32 Index)
{
	// If There is a Sound play it, otherwise play Default Sound
	if (HitZoneImpactEffects[Index] == nullptr || !HitZoneImpactEffects.IsValidIndex(Index))
	{
		if (DefaultImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactParticles, HitResult.Location, FRotator(0.f, 0.f, 0.f), ImpactParticlesSize);
			return;
		}
	}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitZoneImpactEffects[Index], HitResult.Location, FRotator(0.f, 0.f, 0.f), ImpactParticlesSize);

}

void AEnemy::CheckSoundForHitZone(uint32 Index)
{
	// If There is a Sound play it, otherwise play Default Sound
	if (HitZoneImpactSound[Index] == nullptr || !HitZoneImpactSound.IsValidIndex(Index))
	{
		if (ImpactSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ImpactSound);
			return;
		}
	}
	UGameplayStatics::PlaySound2D(GetWorld(), HitZoneImpactSound[Index]);
}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, FHitZone& HitZone)
{	
	float Damage = 0.f;
	
	// Need to be called before CalculateDamage
	HitZone.PhysicalMaterial = HitResult.PhysMaterial.Get();
	Damage = StatsCalculator::CalculateDamage(HitZone);

	if (HitZone.InstigatorRef )
	{ 
		// TODO:: Move Out of Shooter Character, Move To Enemy, for now manualy keep up with Enemy TakeDamage Calculation
		// Show HitNumber in a Widget
		this->ShotHitNumbers(Damage, HitResult.Location, HitZone.bCriticalHitZone, HitZone.bLuckyHit);
		// Apply Damage
		UGameplayStatics::ApplyDamage(this, Damage, HitZone.InstigatorRef->GetInstigatorController(), HitZone.InstigatorRef, UDamageType::StaticClass());
	}

	// Play Sound
	this->PlayHitSoundForHitZone(&HitZone);
	// Play Effect
	this->PlayEffectForHitZone(HitResult, &HitZone);

	// Check if impact stuns the enemy
	// TODO: Implement a Stun Value for each Weapon
	// For now just use a fixed Value
	SetStunned(RollForStunResistance());
	
	// HealthBar
	ShowHealthBar();

	//  HealthAmount
	ShowHealthAmount();

	// Use this for Debugging the hit Material.
	// UE_LOG(CombatLog, Warning, TEXT("PM hit: %s "), *(HitResult.PhysMaterial.Get()->GetName()));
	
}

void AEnemy::ExplosionHit_Implementation(float Damage, float ExplosivStunChance)
{
	if (CurrentHealth - Damage <= 0.f)
	{
		CurrentHealth = 0.f;
		EnemyDie();
	}
	else
	{
		CurrentHealth = CurrentHealth - Damage;

		// Check if impact stuns the enemy
		// TODO: Implement a Stun Value for the Barrel
		// For now just use a fixed Value
		SetStunned(RollForStunResistance(ExplosivStunChance));

		// HealthBar
		ShowHealthBar();

		//  HealthAmount
		ShowHealthAmount();
	}
#if EDITOR 
	UE_LOG(CombatLog, Warning, TEXT("%s Got Hit for %f by ExplosionDamage"), *PointerToSelf->GetActorLabel(), Damage);
#endif
}


bool AEnemy::RollForStunResistance( float InStunChance )
{
	// Allow Check only if enemy is not Stunned
	if (bStunned == false)
	{	
#if EDITOR 
		UE_LOG(CombatLog, Warning, TEXT("%s: InStunChance: %f, CalculatetStunChance would be: %f"), *this->GetActorLabel(), InStunChance, InStunChance - StunResistance);
#endif
		InStunChance -= StunResistance;
		
		// Check Limits
		if (InStunChance < 1.f)
		{
			InStunChance = 1.f; // 1% Chance
		}
		if (InStunChance > 99.f)
		{
			InStunChance = 99.f; // 99% Chance
		}

		float RandomRoll = FMath::FRandRange(0.f, 100.f);
						
		if (InStunChance >= RandomRoll)
		{
			//UE_LOG(CombatLog, Warning, TEXT("Stunned sucess :Rolled: %f, InStunChance: %f"), RandomRoll, InStunChance);
			// Stun successful
			return true;
		}
		else 
		{
			//UE_LOG(CombatLog, Warning, TEXT("Stunned failed :Rolled: %f, InStunChance: %f"), RandomRoll, InStunChance);
			return false;
			// Stun failed			
		}
	}

	//UE_LOG(CombatLog, Warning, TEXT("Stunned failed, Enemy already stunned!"));
	return false;
}

void AEnemy::SetStunned(bool Stunned)
{
	if (bDead)
	{
		return;
	}

	// Set Bool for this class
	bStunned = Stunned;

	if (Stunned)
	{
		PlayHitMontage(FName(TEXT("HitReactFront")));
	}

	if (EnemyAIController)
	{ // Set Bool in the BlackBoard
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bStunned")), Stunned);
	}
}

void AEnemy::ForceAggressionCallback()
{
	bForceAggressionOnPlayer = false;

	// Check if player is inside the Aggression sphere, if not unset TargetActor
	if (!IsPlayerInsideAggressionSphere(false))
	{		
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->ClearValue(FName(TEXT("TargetActor")));
			AttackTarget = nullptr;
		}
	}
}

void AEnemy::ForceAgressionOnPlayer()
{
	if (MainCharacter)
	{		
		if (EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetActor")), MainCharacter);
			AttackTarget = MainCharacter;
		}
		GetWorldTimerManager().SetTimer(ForceAggressionTimerHandle, this, &AEnemy::ForceAggressionCallback, FollowTimeAfterShoot, false);
		bForceAggressionOnPlayer = true;
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventIstigator, AActor* DamageCauser)
{
	// Player shot the Enemy outside his aggression Sphere, the Enemy gets aggressiv
	if (AttackTarget == nullptr)
	{
		ForceAgressionOnPlayer();
	}
		
	CurrentHealth = CurrentHealth - DamageAmount;

	if (CurrentHealth <= 0.f)
	{
		CurrentHealth = 0.f;
		EnemyDie();					
	}
	else
	{
		// Damage Taken
	}

	return DamageAmount;
}

void AEnemy::ActivateLeftWeaponCollision()
{
	LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeaponCollision()
{
	LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ResetDealtDamageInThisAnimation();
}

void AEnemy::ActivateRightWeaponCollision()
{
	RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeaponCollision()
{
	RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ResetDealtDamageInThisAnimation();
}

void AEnemy::ResetDealtDamageInThisAnimation()
{
	bDealtDamageInThisAnimation = false;
}

void AEnemy::SetAllowTurntoTargetWhileAttacking(bool State)
{
	bAllowTurntoTargetWhileAttacking = State;
}

void AEnemy::SetAttackAnimationIsPlaying(bool State)
{
	if (EnemyAIController)
	{
		bAttackAnimationIsPlaying = State;
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bAttackAnimationIsPlaying")), State);
	}
}

void AEnemy::EnemyDie()
{
	if (bDead)
	{
		return;
	}
	// Send Experience Points to Player
	SendExperienceToPlayer();

	HideHealthBar();
	HideHealthAmount();

	// Spawn CashItems
	SpawnCashItems();

	// Make sure that the Enemy lies flat if killed
	// Side effects of the Function TurnToTarget();
	bAllowToTurn = false;
	FRotator DeadRotation = FRotator(0.f, GetActorRotation().Yaw, 0.f);
	SetActorRotation(DeadRotation);
	bAllowTurntoTargetWhileAttacking = false;

	PlayDeathAnimation();
	DisableCollisionOnDeath();
	
	// Deactivate Collision
	AgressionSphere2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeleeRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bDead")), true);
		bDead = true;
	}

	// Enemy can no longer Move
	GetController()->StopMovement();
}

void AEnemy::HandleDestroy()
{
	Destroy();	
}

void AEnemy::PlayDeathAnimation(float Playrate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (DeathMontage && AnimInstance)
	{		
		int64 randomNumber = FMath::RandRange(0, DeathMontageSections.Num()-1);
		
		AnimInstance->Montage_JumpToSection(DeathMontageSections[randomNumber], DeathMontage);
		AnimInstance->Montage_Play(DeathMontage, Playrate);				
	}
}

void AEnemy::FinishedDeathAnimation()
{
	PauseAnimationBeforeDeath();
}


void AEnemy::PauseAnimationBeforeDeath()
{
	GetMesh()->bPauseAnims = true;

	FTimerHandle DeathHandleTimerHandle;
	GetWorldTimerManager().SetTimer(DeathHandleTimerHandle, this, &AEnemy::HandleDestroy, DestroyDelayAfterDeath, false);
}

void AEnemy::SetMainCharacterIsDead_DepedingOnCharacterState()
{
	if (MainCharacter)
	{
		bool CharacterIsDead = MainCharacter->GetCharacterIsDead();

		if (EnemyAIController)
		{	
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bMainCharacterIsDead")), CharacterIsDead);
		}
	}
}

void AEnemy::MainCharacterIsDead(bool State)
{
#if EDITOR 
	FString Name = GetActorLabel();	
	UE_LOG(DelegateLog, Log, TEXT("MainCharacterIsDead has been called! on %s"), *Name );
#endif

	if (EnemyAIController)
	{
		bMainCharacterIsDead = true;
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bMainCharacterIsDead")), true);
	}	
}

void AEnemy::RegisterCharacterIsDeadDelegate()
{
	if (MainCharacter)
	{
		MainCharacter->CharacterIsDeadDelegateBinding(this);
	}	
}

void AEnemy::StoreRefToPlayer()
{
	// Get Ref to the Player
	MainCharacter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AEnemy::UpdateFHitZoneValues()
{	
	CriticalHitZone.DamageMultiplier = CriticalHitZoneOverride.DamageMultiplier;
	CriticalHitZone.EnemyArmor = CriticalHitZoneOverride.EnemyArmor;
	
	HighHitZone.DamageMultiplier = HighHitZoneOverride.DamageMultiplier;
	HighHitZone.EnemyArmor = HighHitZoneOverride.EnemyArmor;
	
	DefaultHitZone.DamageMultiplier = DefaultHitZoneOverride.DamageMultiplier;
	DefaultHitZone.EnemyArmor = DefaultHitZoneOverride.EnemyArmor;
	
	LowHitZone.DamageMultiplier = LowHitZoneOverride.DamageMultiplier;
	LowHitZone.EnemyArmor = LowHitZoneOverride.EnemyArmor;
	
	MinorHitZone.DamageMultiplier = MinorHitZoneOverride.DamageMultiplier;
	MinorHitZone.EnemyArmor = MinorHitZoneOverride.EnemyArmor;
	
	// Fill TMap
	ZoneDamage.Add(EPhysicalSurface::SurfaceType8, &CriticalHitZone);
	ZoneDamage.Add(EPhysicalSurface::SurfaceType9, &HighHitZone);
	ZoneDamage.Add(EPhysicalSurface::SurfaceType10, &DefaultHitZone);
	ZoneDamage.Add(EPhysicalSurface::SurfaceType11, &LowHitZone);
	ZoneDamage.Add(EPhysicalSurface::SurfaceType12, &MinorHitZone);

	// Create Pointer to ZoneDamage
	ZoneDamagePtr = &ZoneDamage;	
}

void AEnemy::InitializeHitZoneImpactSound()
{		
	HitZoneImpactSound.Add(CritImpactSound);
	HitZoneImpactSound.Add(HighImpactSound);
	HitZoneImpactSound.Add(DefaultImpactSound);
	HitZoneImpactSound.Add(LowImpactSound);
	HitZoneImpactSound.Add(MinorImpactSound);
}
void AEnemy::InitializeHitZoneImpactEffects()
{
	HitZoneImpactEffects.Add(CritImpactEffect);
	HitZoneImpactEffects.Add(HighImpactEffect);
	HitZoneImpactEffects.Add(DefaultImpactEffect);
	HitZoneImpactEffects.Add(LowImpactEffect);
	HitZoneImpactEffects.Add(MinorImpactEffect);
}

void AEnemy::InitializeDefaultFHitZoneValues()
{
	// Intialize DefaulValue of the FHit Struct
	// Crit
	CriticalHitZone.DamageMultiplier = 1.f;
	CriticalHitZone.EnemyArmor = 0.f;
	if (HitZoneMaterials[0])
	CriticalHitZone.PhysicalMaterial = HitZoneMaterials[0];	
		
	// High
	HighHitZone.DamageMultiplier = 2.f;
	HighHitZone.EnemyArmor = 0.f;
	if (HitZoneMaterials[1])
	HighHitZone.PhysicalMaterial = HitZoneMaterials[1];	

	// Default
	DefaultHitZone.DamageMultiplier = 1.f;
	DefaultHitZone.EnemyArmor = 0.f;
	if (HitZoneMaterials[2] != nullptr)
	DefaultHitZone.PhysicalMaterial = HitZoneMaterials[2];

	// Low
	LowHitZone.DamageMultiplier = 0.5f;
	LowHitZone.EnemyArmor = 0.f;
	if (HitZoneMaterials[3] != nullptr)
	LowHitZone.PhysicalMaterial = HitZoneMaterials[3];

	// Minor
	MinorHitZone.DamageMultiplier = 0.1f;
	MinorHitZone.EnemyArmor = 0.f;
	if (HitZoneMaterials[4])
	MinorHitZone.PhysicalMaterial = HitZoneMaterials[4];	
}

void AEnemy::SetPlayerCharacterAsTarget(AShooterCharacter* Target)
{
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName(TEXT("TargetActor")), Target);
		bAllowToTurn = true;

	}
}

void AEnemy::SetupWeaponCollisionBoxes()
{
	LeftWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AEnemy::SetUpPatrolPointsFunctionanlity()
{
	if (EnemyAIController)
	{
		if (bActivatePatrolPointsFunctionality)
		{
			TranformLocalVectorToWorldVector(PatrolPoint);
			TranformLocalVectorToWorldVector(PatrolPoint2);
			UpdatePatrolPointsLocations();
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bPatrolFunctionalityActivated")), true);
		}
		else
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bPatrolFunctionalityActivated")), false);
		}		
	}	
}

int32 AEnemy::SendExperienceToPlayer()
{
	if (MainCharacter)
	{
		MainCharacter->AddPlayerExperience(ExperiencePointsOnKill);

		return ExperiencePointsOnKill;
	}

	return 0;
}

void AEnemy::SpawnCashItems()
{
	AmountofLoot = FMath::RandRange(0, MaxAmountofLoot);
	if (AmountofLoot == 0)
	{
		AmountofLoot = MinAmountofLoot;
	}

	for (int32 i = 1; i <= AmountofLoot; i++)
	{
		FActorSpawnParameters Parameters;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ADroppableItem* SpawnedBaseItem = GetWorld()->SpawnActor<ADroppableItem>(ItemLootBP, GetActorTransform(), Parameters);

		if (ItemLootBP.Get())
		{
			SpawnedBaseItem->SpawingItems(GetActorTransform(), SpawnedBaseItem);
		}
	}
}

void AEnemy::EnsureMaxHealthisCurrentHealth()
{
	if (CurrentHealth > MaxHealth)
	{
		MaxHealth = CurrentHealth;
	}
}

void AEnemy::SetEnemyTag()
{
	Tags.Add(FName(TEXT("Enemy")));

#if EDITOR 
	if (ActorHasTag(FName(TEXT("Enemy"))))
	{
		UE_LOG(MyLog, Warning, TEXT("%s: is missing Tag \'Enemy\'"), *GetActorLabel());
	}
#endif
}