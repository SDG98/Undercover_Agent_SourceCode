// Fill out your copyright notice in the Description page of Project Settings.


#include "CivilianAI.h"
#include "Engine/GameEngine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "WeaponProjectile.h"
#include "AIController.h"
// Sets default values
ACivilianAI::ACivilianAI()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshChar = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));

	MeshChar->SetupAttachment(RootComponent);
	MeshChar->bCastDynamicShadow = true;
	MeshChar->CastShadow = true;
	MeshChar->SetAllMassScale(5000.0f);
	//MeshChar->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	MeshChar->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MeshChar->SetRelativeScale3D(FVector(1.00f, 1.00f, 1.00f));

	MeshChar->BodyInstance.SetCollisionProfileName("BlockAllDynamic");


	MeshChar->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	MeshChar->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);


	MeshChar->OnComponentHit.AddDynamic(this, &ACivilianAI::OnCompHit);
	//sightConfig->SightRadius = 1000.0f;
	//sightConfig->LoseSightRadius = 3000.0f;
	//sightConfig->PeripheralVisionAngleDegrees = 90.0f;
	hasStartUpAnimation = false;
	isDead = false;
	isAfraid = false;
	ragdollMass = 7.5f;
	getUpTime = 1.0f;

}

// Called when the game starts or when spawned
void ACivilianAI::BeginPlay()
{
	Super::BeginPlay();

	MeshChar->OnComponentHit.AddDynamic(this, &ACivilianAI::OnCompHit);

	MeshChar->SetAllMassScale(ragdollMass);

	if (hasStartUpAnimation) {
		PlayStartUpAnimation();
	}
}

// Called every frame
void ACivilianAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACivilianAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ACivilianAI::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	if (OtherActor->IsA(AWeaponProjectile::StaticClass())) {


		DeathEvent();
		
	}

}

void ACivilianAI::DeathEvent() {

	if (!isDead) {

		isDead = true;
		if (DeathSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

		}

		UAnimInstance* AnimInstance = MeshChar->GetAnimInstance();
		if (AnimInstance != NULL)
		{

			AnimInstance->Montage_Play(DeathMontage, 0.0001f);
			AnimInstance->Montage_Pause(DeathMontage);
		}

		GetCharacterMovement()->DisableMovement();
		
		MeshChar->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshChar->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		MeshChar->SetAllBodiesSimulatePhysics(true);
		MeshChar->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


void ACivilianAI::CowerInFear() {


	GetCharacterMovement()->Deactivate();

	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();

	UBoolProperty* afraidProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsAfraid");
	UBoolProperty* cowerProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsCrouching");

	afraidProp->SetPropertyValue_InContainer(AnimInst, true);
	cowerProp->SetPropertyValue_InContainer(AnimInst, true);


	if (hasStartUpAnimation) {

		if (AnimInst != NULL)
		{


			AnimInst->Montage_Stop(0.0f, StartUpMontage);

		}

	}

}

void ACivilianAI::SetFleeAnim() {

	GetCharacterMovement()->Activate();
	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();


	if (hasStartUpAnimation) {

		if (AnimInst != NULL)
		{


			AnimInst->Montage_Stop(0.0f, StartUpMontage);

		}

	}


	UBoolProperty* afraidProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsAfraid");
	UBoolProperty* cowerProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsCrouching");
	afraidProp->SetPropertyValue_InContainer(AnimInst, true);
	cowerProp->SetPropertyValue_InContainer(AnimInst, false);




}



void ACivilianAI::PlayStartUpAnimation() {


	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();

	if (AnimInst != NULL)
	{


		AnimInst->Montage_Play(StartUpMontage, 1.0f);

	}



}

