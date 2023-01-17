// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosiv.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CustomLogs.h"
#include "ShooterCharacter.h"
#include "Weapon.h"	

// Sets default values
AExplosiv::AExplosiv()
{
	PointerToSelf = this;
	MaxHealth = 10.f;
	ExplosionRadius = 350.f;
	ExplosionDamageClose = 80.f;
	ExplosionDamageFar = 30.f;
	ExplosionStunChanceClose = 30.f;;
	ExplosionStunChanceFar = 10.f;
	ExplosionDelay = 0.25f;
	ExplosionDeviationDelay = 0.25f;

	
	CurrentHealth = MaxHealth;
	
	bShouldDestroyNextTick = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("StaticMesh")));
	//StaticMesh->SetupAttachment(RootComponent);

	ExplosionRange = CreateDefaultSubobject<USphereComponent>(FName(TEXT("ExplosionRange")));
	ExplosionRange->SetSphereRadius(ExplosionRadius, false);
	ExplosionRange->SetupAttachment(StaticMesh);

	

	
}

// Called when the game starts or when spawned
void AExplosiv::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AExplosiv::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


 void AExplosiv::BulletHit_Implementation(FHitResult HitResult, FHitZone& HitZone)
 {
	 // Sound
	 if (ImpactSound)
	 {
		 UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, HitResult.Location);
	 }
	 // Particles
	 if (HitParticles)
	 {
		 UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, HitResult.Location, FRotator(0.f), true);
	 }
	 
	 AShooterCharacter* Character = Cast< AShooterCharacter>(HitZone.InstigatorRef);
	 if (Character->GetEquippedWeapon() && HitZone.InstigatorRef)
	 {
		UGameplayStatics::ApplyDamage(this, Character->GetEquippedWeapon()->GetWeaponDamage(), HitZone.InstigatorRef->GetInstigatorController(), HitZone.InstigatorRef, UDamageType::StaticClass());
	 }


 }

 void AExplosiv::PlayDestroySound()
 {
	 if (ExplosionSound)
	 {
		 UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
	 }
 }

 void AExplosiv::EmmitDestroyEffect()
 {
	 if (ExplosionParticles)
	 {
		 UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, GetActorLocation(), FRotator(0.f), true);
	 }

 }

 void AExplosiv::CheckforActorWithinExplosionRange()
 { 
	 // Get Actors withinRange, Note: Self gets included
	 TArray<AActor*> ActorInRange;
	 ExplosionRange->GetOverlappingActors(ActorInRange);
	 
	 // DebugTesting
	 /*
	 FString ArrayContains;
	 for (int32 i = 0; i < ActorInRange.Num(); i++)
	 {
		 // Exclude itself
		 if (ActorInRange[i] != PointerToSelf)
		 {
			 //UE_LOG(CombatLog, Log, TEXT("%s has in reach :%s"), *this->GetActorLabel(), *ActorInRange[i]->GetActorLabel());
			 ArrayContains.Append(*ActorInRange[i]->GetActorLabel());
			 ArrayContains.AppendChar(*TEXT(" | "));
		 }
	 }
	 UE_LOG(CombatLog, Log, TEXT("ArrayContains: %s"), *ArrayContains);
	*/

	 for ( int32 i = 0; i < ActorInRange.Num(); i++)
	 {
		 // Exclude itself
		 if (ActorInRange[i] != PointerToSelf)
		 {
			 IExplosionHitInterface* ExplosionHitInterface = Cast<IExplosionHitInterface>(ActorInRange[i]);

			 // If one has the Interface, deal Damage
			 if (ExplosionHitInterface && ActorInRange[i])
			 {
				 DealExplosivDamage(ExplosionHitInterface, ActorInRange[i]);
			 }
		 }
	 }

	 // TODO: Remove out of Function
	 // Should not be here, but okey for now
	 Destroy();

 }

 void AExplosiv::DealExplosivDamage(IExplosionHitInterface* Interface, AActor* Target)
 {
	 float MaxDistance = ExplosionRange->GetScaledSphereRadius();
	 float OutDamage;
	 double	DistanceToTarget = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

	 // Do FarDamage Check if the Target is withinRange and check if the Target is close enough to do close Damage
	 if ((DistanceToTarget > MaxDistance /2) && (DistanceToTarget <= MaxDistance))
	 {
		 // Far Damage is Dealt depending of the Distance to the Target. But never more than ExplosionDamageClose
		float DistanceClamped = FMath::Clamp<float>(DistanceToTarget, MaxDistance / 2, MaxDistance);

		OutDamage = ExplosionDamageFar * MaxDistance / DistanceClamped;

		Interface->ExplosionHit_Implementation(OutDamage, ExplosionStunChanceFar);
	 }

	 // Do CloseDamage
	 if (DistanceToTarget <= MaxDistance / 2)
	 {
		 // Close Damage is Done to 100% every Time
		 Interface->ExplosionHit_Implementation(ExplosionDamageClose, ExplosionStunChanceClose);
	 }

	 
 }

 float AExplosiv::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventIstigator, AActor* DamageCauser)
 {
	 
	 // When Takind Damage through a Bullet or Melee attack, the time to explode is reduced
	 ExplosionDeviationDelay = 0.01f;
	 ExplosionDelay = 0.01f;
	HandleDamage(DamageAmount);
	return DamageAmount;
 }

 void AExplosiv::ExplosionHit_Implementation(float Damage, float StunChance)
 {
	 HandleDamage(Damage);
	 //UE_LOG(CombatLog, Log, TEXT("%s Got Hit for %f by ExplosionDamage"), *this->GetActorLabel(), Damage);
 }

 void AExplosiv::HandleDamage(float Damage)
 { 
		 if (CurrentHealth - Damage <= 0.f)
		 {
			 CurrentHealth = 0.f;
			 if (!bShouldDestroyNextTick)
			 {
				 HandleDestroy();
			 }
		 }
		 else
		 {
			 CurrentHealth = CurrentHealth - Damage;
		 }	 
 }

 void AExplosiv::HandleDestroy()
 { 
		 bShouldDestroyNextTick = true;
		 PlayDestroySound();
		 EmmitDestroyEffect();
		 ExplodeTimer();
		
 }

 void AExplosiv::ExplodeTimer()
 {
	 // Small Delay before exploding
	 FTimerHandle ExplosionDelayTimerHandle;
	 	 
	 float ExplosionDeviation = FMath::FRandRange(-ExplosionDeviationDelay, ExplosionDeviationDelay);
	 ExplosionDeviation = FMath::Clamp(ExplosionDelay, 0.05f, 0.5f);
	 ExplosionDelay += ExplosionDeviation;
	 GetWorldTimerManager().SetTimer(ExplosionDelayTimerHandle, this, &AExplosiv::CheckforActorWithinExplosionRange, ExplosionDelay);

 }