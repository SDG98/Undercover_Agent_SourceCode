// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AgentPlayerClass.h"
#include "EnemyAI.h"
#include "Components/PrimitiveComponent.h"
#include "CivilianAI.h"
AExplosiveProjectile::AExplosiveProjectile()
{
	Radius = 500.0f;
	ExplosiveDamage = 80.0f;
	PlayerExplosiveDamage = 40.0f;
}
void AExplosiveProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) 
{


	OnExplode();




}



void AExplosiveProjectile::OnExplode() {



	UParticleSystemComponent* Explosion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticle, GetActorTransform(), true);
		

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodeSound, GetActorLocation());
	 
	TArray<FHitResult> HitActors;

	FVector startTrace = GetActorLocation();
	FVector endTrace = startTrace;
	endTrace.Z = 300.0f;
	
	FCollisionShape Collision;
	Collision.ShapeType = ECollisionShape::Sphere;
	Collision.SetSphere(Radius);

	if (GetWorld()->SweepMultiByChannel(HitActors, startTrace, endTrace, FQuat::FQuat(), ECC_WorldDynamic, Collision)) {

		for (auto Actors = HitActors.CreateIterator(); Actors; Actors++) {

		
			UStaticMeshComponent* SM = Cast<UStaticMeshComponent>((*Actors).Actor->GetRootComponent());
			AAgentPlayerClass* playerCharacter = Cast<AAgentPlayerClass>((*Actors).Actor);
			ACivilianAI* civilianCharacter = Cast<ACivilianAI>((*Actors).Actor);
			AEnemyAI* enemyCharacter = Cast<AEnemyAI>((*Actors).Actor);
			if (playerCharacter) {
				playerCharacter->HandleDamage(PlayerExplosiveDamage);
				
			}


			if (civilianCharacter) {
				civilianCharacter->DeathEvent();

				civilianCharacter->MeshChar->AddRadialImpulse(GetActorLocation(), 200.0f, 200.0f, ERadialImpulseFalloff::RIF_Linear, true);
		
			}


			if (enemyCharacter) {

				if (enemyCharacter->canTakeExplosionDamage == true) {
					enemyCharacter->HandleDamage(ExplosiveDamage);
				}
				
				
				



				enemyCharacter->MeshChar->AddRadialImpulse(GetActorLocation(), 200.0f, 200.0f, ERadialImpulseFalloff::RIF_Linear, true);
			
			}


			if (SM) {
				SM->AddRadialImpulse(GetActorLocation(), 600.0f, 600.0f, ERadialImpulseFalloff::RIF_Linear, true);
			


			}




		}

	}

	Destroy();
}