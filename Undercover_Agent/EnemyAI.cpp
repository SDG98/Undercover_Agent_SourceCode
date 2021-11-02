// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"
#include "Engine/GameEngine.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Perception/AISense_Hearing.h"
#include "WeaponProjectile.h"
#include "AIController.h"

// Sets default values
AEnemyAI::AEnemyAI()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshChar = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));

	MeshChar->SetupAttachment(RootComponent);
	MeshChar->bCastDynamicShadow = true;
	MeshChar->CastShadow = true;
	MeshChar->SetAllMassScale(1000.0f);
	hasPrimaryFireMontage = false;
	MeshChar->SetupAttachment(GetCapsuleComponent());
	//MeshChar->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	MeshChar->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	MeshChar->SetRelativeScale3D(FVector(1.00f, 1.00f, 1.00f));

	aiAggression = 0.50f;
	MeshChar->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	MeshChar->OnComponentHit.AddDynamic(this, &AEnemyAI::OnCompHit);

	MeshChar->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	MeshChar->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	followDistance = 2000.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	health = 100;
	maxHealth = 100;
	dropsWeapon = false;
	dropsHealth = false;
	isCrouched = false;
	headshotMultiplier = 2;
	standupDelay = 0.35f;
	isReloading = false;
	reloadTime = 1.667f;
	ammo = 15;
	maxAmmo = 15;
	weaponSocketTag = "PistolWeaponSocket";
	fireRateWaitMin = -0.50f;
	fireRateWaitMax = 0.70f;
	fireRateInitial = 0.50f;
	attackDelayMax = 0.50f;
	attackDelayMin = 0.0f;
	spreadMin = -10.5f;
	spreadMax = 3.0f;
	aiBravery = 0.95f;
	aiBraveryMax = 0.95f;
	foolhardly = false;
	investigateSounds = true;
	canTakeExplosionDamage = true;
	TPGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunModel"));
	TPGun->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	TPGun->SetRelativeScale3D(FVector(0.025f, 0.025f, 0.025f));
	TPGun->SetupAttachment(RootComponent);
	TPGun->bCastDynamicShadow = true;
	TPGun->CastShadow = true;
	TPGun->SetVisibility(true);
	isArmed = false;
	particleScale = 0.25f;
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	FP_MuzzleLocation->SetupAttachment(GetCapsuleComponent());
	ragdollMass = 7.5f;
	regenTimerBravery = 10.0f;
	isVip = false;
}

// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	MeshChar->OnComponentHit.AddDynamic(this, &AEnemyAI::OnCompHit);
	MeshChar->SetAllMassScale(ragdollMass);
	TPGun->AttachToComponent(MeshChar, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), weaponSocketTag);
	FP_MuzzleLocation->AttachToComponent(TPGun, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("MuzzleEnd"));

	FP_MuzzleLocation->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	TPGun->SetVisibility(false);
	

}

// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Called to bind functionality to input
void AEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyAI::HandleDamage(int damage) {
	if (!isDead) {

		health -= damage;


		if (health <= 0 || damage > health) {
			DeathEvent();
		}


	}


}


void AEnemyAI::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {

	if (!isDead) {

		if (OtherActor->IsA(AWeaponProjectile::StaticClass())) {

			AWeaponProjectile* projectile = Cast<AWeaponProjectile>(OtherActor);

			if (projectile->ActorHasTag(TEXT("PlayerProjectile")))
				HandleDamage(projectile->ProjectileDamage);
			if (projectile->ActorHasTag(TEXT("PlayerProjectile")) && Hit.BoneName == "Head")
				HandleDamage(projectile->ProjectileDamage * headshotMultiplier);


		}



	}


}


void AEnemyAI::DeathEvent() {

	if (!isDead) {
		TPGun->SetVisibility(false);
		isDead = true;
		if (DeathSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
			
		}
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this, 0.0f, "FriendlyDeathSound");
		UAnimInstance* AnimInstance = MeshChar->GetAnimInstance();
		if (AnimInstance != NULL)
		{

			AnimInstance->Montage_Play(DeathMontage, 1.0f);
			AnimInstance->Montage_Pause(DeathMontage);
		}

		GetCharacterMovement()->DisableMovement();
		MeshChar->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshChar->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		MeshChar->SetAllBodiesSimulatePhysics(true);
		MeshChar->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UWorld* const World = GetWorld();
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		float randomSpread = FMath::RandRange(spreadMin, spreadMax);
	

		const FRotator SpawnRotation = FRotator(GetControlRotation().Pitch + randomSpread, GetControlRotation().Yaw + randomSpread, GetControlRotation().Roll + randomSpread);
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(FVector(30.0f + randomSpread, -20.0f + randomSpread, -50.0f + randomSpread));


		if (dropsHealth) {

			World->SpawnActor<AHealthPickUpClass>(HealthPickUpClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		if (dropsWeapon) {
			World->SpawnActor<AWeaponPickUpClass>(WeaponPickUpClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

	}



}


void AEnemyAI::ReloadEvent() {

	isReloading = true;

	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();
	UBoolProperty* crouchProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsReloading");
	crouchProp->SetPropertyValue_InContainer(AnimInst, true);


	GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AEnemyAI::FinishedReloadEvent, reloadTime, true);



}

void AEnemyAI::FinishedReloadEvent() {
	isReloading = false;
	ammo = maxAmmo;
	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();
	UBoolProperty* crouchProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsReloading");
	crouchProp->SetPropertyValue_InContainer(AnimInst, false);

	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
}


void AEnemyAI::AttackEvent() {

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Projectile Spawned");
	UWorld* const World = GetWorld();
	float spread = FMath::RandRange(spreadMin, spreadMax);
	const FRotator SpawnRotation = FRotator(GetControlRotation().Pitch + spread, GetControlRotation().Yaw + spread, GetControlRotation().Roll + spread);
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(FVector(30.0f, -20.0f, -10.0f));

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	UAnimInstance* WeaponAnimInstance = TPGun->GetAnimInstance();
	UAnimInstance* AnimInstance = MeshChar->GetAnimInstance();
	if (AnimInstance != NULL)
	{

		if(hasPrimaryFireMontage == true)
		AnimInstance->Montage_Play(AttackMontage, 1.0f);

	}



	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this, 0.0f, "Gun");
	}



	ammo--;
	UParticleSystemComponent* Explosion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunParticle, FP_MuzzleLocation->GetComponentTransform(), true);


	World->SpawnActor<AWeaponProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);


}


void AEnemyAI::FleeEvent() {
	FinishedReloadEvent();
	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();
	UBoolProperty* fleeProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsAfraid");
	fleeProp->SetPropertyValue_InContainer(AnimInst, true);
	TPGun->SetVisibility(false);
}


void AEnemyAI::CrouchEvent() {
	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();
	UBoolProperty* crouchProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsCrouching");
	crouchProp->SetPropertyValue_InContainer(AnimInst, true);
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	isCrouched = true;
}

void AEnemyAI::StandEvent() {
	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();
	UBoolProperty* crouchProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsCrouching");
	crouchProp->SetPropertyValue_InContainer(AnimInst, false);
	GetWorldTimerManager().SetTimer(TimerHandle_MovementEnabled, this, &AEnemyAI::EnableMovementDelay, standupDelay, true);

}

void AEnemyAI::EnableMovementDelay() {
	GetWorldTimerManager().ClearTimer(TimerHandle_MovementEnabled);
	GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
	isCrouched = false;
}

void AEnemyAI::HostileEvent() {
	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();
	UBoolProperty* hostileProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsAfraid");
	hostileProp->SetPropertyValue_InContainer(AnimInst, false);
	TPGun->SetVisibility(true);
}






void AEnemyAI::DrawWeapon() {

	TPGun->SetVisibility(true);
	UAnimInstance* AnimInst = MeshChar->GetAnimInstance();
	UBoolProperty* hostileProp = FindField<UBoolProperty>(AnimInst->GetClass(), "IsHostile");
	hostileProp->SetPropertyValue_InContainer(AnimInst, true);
	UFloatProperty* blendWeightProp = FindField<UFloatProperty>(AnimInst->GetClass(), "BlendWeight");
	blendWeightProp->SetPropertyValue_InContainer(AnimInst, 1.0f);

}