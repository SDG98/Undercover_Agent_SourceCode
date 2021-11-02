// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponProjectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class UNDERCOVER_AGENT_API AExplosiveProjectile : public AWeaponProjectile
{
	GENERATED_BODY()
		
public: 
	AExplosiveProjectile();

	
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

		UPROPERTY(EditAnywhere, Category = "FX")
			class USoundBase* ExplodeSound;
		UPROPERTY(EditAnywhere, Category = "FX")
			class UParticleSystem* ExplodeParticle;

		UPROPERTY(EditAnywhere, Category = "Projectile")
			float Radius;
		UPROPERTY(EditAnywhere, Category = "Projectile")
			float ExplosiveDamage;

		UPROPERTY(EditAnywhere, Category = "Projectile")
			float PlayerExplosiveDamage;

		UFUNCTION()
		void OnExplode();


};
