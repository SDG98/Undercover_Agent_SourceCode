// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CivilianAI.generated.h"

UCLASS()
class UNDERCOVER_AGENT_API ACivilianAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACivilianAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	FTimerHandle TimerHandle_LostSight;

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* PanicSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isAfraid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool willCower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool hasStartUpAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float getUpTime;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		float ragdollMass;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* MeshChar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* StartUpMontage;

	void SetFleeAnim();
	void CowerInFear();
	void DeathEvent();

	void PlayStartUpAnimation();

};
