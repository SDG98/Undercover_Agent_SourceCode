// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "HealthPickUpClass.h"
#include "WeaponPickUpClass.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "WeaponProjectile.h"
#include "Components/BoxComponent.h"
#include "EnemyAI.generated.h"
UCLASS()
class UNDERCOVER_AGENT_API AEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* AlertSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isDead;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool investigateSounds;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float standupDelay;

	bool isArmed;

	bool isCrouched;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool canTakeExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int headshotMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int maxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float followDistance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		float ragdollMass;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* MeshChar;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* TPGun;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isHostile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isVip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int maxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float reloadTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isReloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool dropsWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool dropsHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<class AWeaponPickUpClass> WeaponPickUpClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TSubclassOf<class AHealthPickUpClass> HealthPickUpClass;


	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AWeaponProjectile> ProjectileClass;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;

	UPROPERTY(EditAnywhere, Category = "FX")
		class UParticleSystem* GunParticle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float particleScale;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool hasPrimaryFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float fireRateWaitMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float fireRateWaitMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float fireRateInitial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool foolhardly;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float aiBravery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float aiBraveryMax;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float aiAggression;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float regenTimerBravery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* DeathMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* AttackMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* DrawWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* ReloadMontage;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float spreadMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float spreadMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float attackDelayMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float attackDelayMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FName weaponSocketTag;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void DeathEvent();

	void AttackEvent();

	void ReloadEvent();

	void FinishedReloadEvent();

	void CrouchEvent();

	void EnableMovementDelay();

	void StandEvent();

	void FleeEvent();

	void HostileEvent();
	

	void HandleDamage(int damage);


	FTimerHandle TimerHandle_DrawWeapon;
	FTimerHandle TimerHandle_ReloadWeapon;
	FTimerHandle TimerHandle_MovementEnabled;
	void DrawWeapon();
};
