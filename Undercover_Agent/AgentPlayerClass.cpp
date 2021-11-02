// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AgentPlayerClass.h"
#include "Animation/AnimInstance.h"
#include "WeaponProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "Engine/GameEngine.h"
#include "GameFramework/InputSettings.h"
#include "Perception/AISense_Hearing.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AAgentPlayerClass

AAgentPlayerClass::AAgentPlayerClass()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	BulletCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Bullet Capsule"));
	BulletCapsule->SetupAttachment(GetCapsuleComponent());
	BulletCapsule->InitCapsuleSize(55.f, 96.0f);
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	PerceptionStimuli = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuli"));
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	starterWeapon = 0;
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	Mesh1P->SetRelativeLocation(FVector(-34.770557f, -0.34237f, -250.0f));
	Mesh1P->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.05f));
	hasLanded = true;
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));

	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	//FP_Gun->SetupAttachment(Mesh1P, TEXT("PistolWeaponSocket"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_Gun->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
	//FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(
		0.0f, 0.0f, 0.0f);
	fallDamage = 0;
	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
	currentVelocity = GetCharacterMovement()->Velocity;
	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	previousVelocity = GetVelocity();
	safeFallVelocity = 1000.0f;
	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.
	canFire = true;
	weaponIndex = 0;
	lastWeapon = 0;
	PlayerHealth = 100;
	PlayerHealthMax = 100;
	//weapon = nullptr;
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true; UAIPerceptionStimuliSourceComponent
	playerStimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PlayerStimulus"));
	playerStimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	playerStimulus->RegisterWithPerceptionSystem();
	GetCharacterMovement()->MaxWalkSpeed = 775.0f;
	GetCharacterMovement()->JumpZVelocity = 510.0f;

	GetCharacterMovement()->InitialPushForceFactor = 5.0f;
	fallTimeCheck = 0.0f;
	isReloading = false;
	isScoped = false;
	isDead = false;
	isArmed = false;
	defaultFOV = 90.0f;
	GunCrosshairOffset = 0.0f;
}

void AAgentPlayerClass::BeginPlay()
{
	// Call the base class  


	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor

	FP_MuzzleLocation->AttachToComponent(FP_Gun, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("MuzzleEnd"));
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));


	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
	weaponIndex = starterWeapon;
	lastWeapon = weaponIndex;



	for (int i = 0; i < WeaponLoadout.Num(); i++) {
		SpawnWeaponOnPlayer(i);
	}

	weapons[weaponIndex]->SetHasWeapon(true);

	for (int i = 0; i < WeaponLoadout.Num(); i++) {
		if (i != 0) {
			weapons[i]->SetTotalAmmo(weapons[i]->GetClipAmmo());
		}
	}


	hud = Cast<AUndercover_AgentHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

	if (hud != nullptr) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GOT THE HUD"));
		hud->SetPlayerHealth(PlayerHealth);
		hud->SetCurrentObjective("Objective:");
		hud->SetPlayerAmmo(0,0	);

		hud->SetInteractionLabel(" ");

	}
	BulletCapsule->OnComponentHit.AddDynamic(this, &AAgentPlayerClass::OnCompHit);



	if (isArmed == true) {
		Mesh1P->SetVisibility(true);
		FP_Gun->SetVisibility(true);
		SwitchWeaponType();

		if (hud != nullptr) {

			hud->SetInteractionLabel(" ");

		}

	}



	else if (isArmed != true) {
		Mesh1P->SetVisibility(false);
		FP_Gun->SetVisibility(false);

		if (hud != nullptr) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GOT THE HUD"));

			hud->SetInteractionLabel("Press G to Arm Up");

		}


	}

	


}

void AAgentPlayerClass::Tick(float DeltaTime) {
	
	Super::Tick(DeltaTime);
	currentVelocity = GetCharacterMovement()->Velocity;

	//this handles all the falling damage stuff, 
	//basically its calculating the characters movement speed from getting the velocity from the character 

	if (GetCharacterMovement()->IsFalling()==true) {

		fallTimeCheck += GetWorld()->GetDeltaSeconds();

		hasLanded = false;
			
		FVector DeltaVelocitySquared = (currentVelocity - previousVelocity) * (currentVelocity - previousVelocity);
		float DeltaSpeed = DeltaVelocitySquared.X + DeltaVelocitySquared.Y + DeltaVelocitySquared.Z;
		
		if (DeltaSpeed > (safeFallVelocity * safeFallVelocity) && fallTimeCheck >= 1.10f)
		{
			if(fallTimeCheck < 2.00f)
			fallDamage = sqrtf(DeltaSpeed) / 18;
			
			if (fallTimeCheck > 2.00f) {
				fallDamage = sqrtf(DeltaSpeed) / 10;
			}

		}

	}

	else if (GetCharacterMovement()->IsFalling() !=true)
	{

		if (hasLanded == false) {
			HandleDamage(fallDamage);
		}
		hasLanded = true;
		previousVelocity = currentVelocity;
		fallDamage = 0;
		fallTimeCheck = 0.0f;
	}
	


}
//////////////////////////////////////////////////////////////////////////
// Input


void AAgentPlayerClass::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event 
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AAgentPlayerClass::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AAgentPlayerClass::OnRelease);

	//Binds Weapon Swapping Numbers

	PlayerInputComponent->BindAction("WeaponKey1", IE_Pressed, this, &AAgentPlayerClass::OnWeaponSwapOne);
	PlayerInputComponent->BindAction("WeaponKey2", IE_Pressed, this, &AAgentPlayerClass::OnWeaponSwapTwo);
	PlayerInputComponent->BindAction("WeaponKey3", IE_Pressed, this, &AAgentPlayerClass::OnWeaponSwapThree);
	PlayerInputComponent->BindAction("WeaponKey4", IE_Pressed, this, &AAgentPlayerClass::OnWeaponSwapFour);
	PlayerInputComponent->BindAction("WeaponKey5", IE_Pressed, this, &AAgentPlayerClass::OnWeaponSwapFive);
	PlayerInputComponent->BindAction("SwaptoMelee", IE_Pressed, this, &AAgentPlayerClass::SwapToMelee);
	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AAgentPlayerClass::EquipWeapon);


	PlayerInputComponent->BindAction("RestartLevel", IE_Pressed, this, &AAgentPlayerClass::RestartLevel);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AAgentPlayerClass::StartReload);

	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &AAgentPlayerClass::ZoomIn);
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AAgentPlayerClass::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AAgentPlayerClass::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAgentPlayerClass::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAgentPlayerClass::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAgentPlayerClass::LookUpAtRate);
}



void::AAgentPlayerClass::EquipWeapon() {

	if (isArmed !=true && isDead !=true) {
		isArmed = true;
		Mesh1P->SetVisibility(true);
		FP_Gun->SetVisibility(true);
		SwitchWeaponType();

		if (hud != nullptr) {

			hud->SetInteractionLabel(" ");

		}


	}
	

}




void AAgentPlayerClass::OnFire()
{
	if (isArmed) {


		if (isReloading == true && weapons[weaponIndex]->GetWeaponType() == EWeapon::E_ShotGun) {
			isReloading = false;
			GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Reload);
		}



		if (isReloading != true) {

			if (canFire) {
				if (weapons[weaponIndex]->GetWeaponType() == EWeapon::E_HeavyRifle || weapons[weaponIndex]->GetWeaponType() == EWeapon::E_AutoRifle) {
					canFire = false;
					SpawnProjectile();
					GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetFireRateDelay(), true);
					GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::SpawnProjectile, weapons[weaponIndex]->GetFireRateDelayAuto(), true);

				}

				else
				{
					canFire = false;
					SpawnProjectile();
					GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetFireRateDelay(), true);

				}
			}


		}









	}



}

void AAgentPlayerClass::OnRelease() {
	GetWorldTimerManager().ClearTimer(TimerHandle_Handle_Autofire);

	GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetFireRateDelay(), true);
}


void AAgentPlayerClass::ZoomIn() {
	if (isArmed) {

		if (isReloading != true) {

			if (isScoped != true) {
				FirstPersonCameraComponent->SetFieldOfView(defaultFOV * weapons[weaponIndex]->GetZoomValue());
				isScoped = true;
			}
			else if (isScoped == true) {
				ZoomOut();
			}

		}



	}


}

void AAgentPlayerClass::ZoomOut() {
	FirstPersonCameraComponent->SetFieldOfView(defaultFOV);
	isScoped = false;
}



void AAgentPlayerClass::StartReload() {

	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	UAnimInstance* AnimWeaponInstance = FP_Gun->GetAnimInstance();





	if (isDead != true && isArmed) {



		if (weapons[weaponIndex]->GetClipAmmo() != weapons[weaponIndex]->GetMaxClipAmmo() && weapons[weaponIndex]->GetTotalAmmo() != 0 && isReloading != true) {

			ZoomOut();


			if (weapons[weaponIndex]->GetWeaponType() == EWeapon::E_ShotGun || weapons[weaponIndex]->GetWeaponType() == EWeapon::E_GrenadeLauncher) {



				if (weapons[weaponIndex]->GetClipAmmo() != weapons[weaponIndex]->GetMaxClipAmmo()) {



					isReloading = true;


					if (AnimInstance != NULL)
					{

						AnimInstance->Montage_Play(weapons[weaponIndex]->ArmReloadAnimation, weapons[weaponIndex]->GetReloadAnimSpeed());


					}


					if (AnimWeaponInstance != NULL)
					{

						AnimWeaponInstance->Montage_Play(weapons[weaponIndex]->WeaponReloadAnimation, weapons[weaponIndex]->GetReloadAnimSpeed());
					}


					GetWorldTimerManager().SetTimer(TimerHandle_Delay_Reload, this, &AAgentPlayerClass::ReloadShotgunWeapon, weapons[weaponIndex]->GetReloadTime(), false);


				}

			}

			else
			{

				isReloading = true;


				if (AnimInstance != NULL)
				{

					AnimInstance->Montage_Play(weapons[weaponIndex]->ArmReloadAnimation, weapons[weaponIndex]->GetReloadAnimSpeed());


				}

				if (AnimWeaponInstance != NULL)
				{

					AnimWeaponInstance->Montage_Play(weapons[weaponIndex]->WeaponReloadAnimation, weapons[weaponIndex]->GetReloadAnimSpeed());
				}

				GetWorldTimerManager().SetTimer(TimerHandle_Delay_Reload, this, &AAgentPlayerClass::ReloadWeapon, weapons[weaponIndex]->GetReloadTime(), true);
			}



		}


	}


}


void AAgentPlayerClass::ReloadWeapon() {
	int totalAmmo = weapons[weaponIndex]->GetTotalAmmo();
	if (weapons[weaponIndex]->GetTotalAmmo() - (weapons[weaponIndex]->GetMaxClipAmmo() - weapons[weaponIndex]->GetClipAmmo()) >= 0) {

		weapons[weaponIndex]->SetTotalAmmo(totalAmmo -= (weapons[weaponIndex]->GetMaxClipAmmo() - weapons[weaponIndex]->GetClipAmmo()));

		weapons[weaponIndex]->SetClipAmmo(weapons[weaponIndex]->GetMaxClipAmmo());


	}
	else {
		weapons[weaponIndex]->SetClipAmmo(weapons[weaponIndex]->GetTotalAmmo());
		weapons[weaponIndex]->SetTotalAmmo(0);
	}
	GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Reload);
	isReloading = false;
	hud->SetPlayerAmmo(weapons[weaponIndex]->GetClipAmmo(), weapons[weaponIndex]->GetTotalAmmo());
}


void AAgentPlayerClass::ReloadWeaponAnim() {


	if (weapons[weaponIndex]->GetClipAmmo() == weapons[weaponIndex]->GetMaxClipAmmo()) {
		GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Reload);
	}

	else {
		StartReload();
	}
}

void AAgentPlayerClass::ReloadShotgunWeapon() {

	if (weapons[weaponIndex]->GetClipAmmo() == weapons[weaponIndex]->GetMaxClipAmmo()) {
		isReloading = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Reload);

	}

	else {
		isReloading = false;
		weapons[weaponIndex]->SetClipAmmo(weapons[weaponIndex]->GetClipAmmo() + 1);
		weapons[weaponIndex]->SetTotalAmmo(weapons[weaponIndex]->GetTotalAmmo() - 1);
		hud->SetPlayerAmmo(weapons[weaponIndex]->GetClipAmmo(), weapons[weaponIndex]->GetTotalAmmo());
		StartReload();


		if (weapons[weaponIndex]->GetWeaponType() == EWeapon::E_ShotGun && weapons[weaponIndex]->GetClipAmmo() == weapons[weaponIndex]->GetMaxClipAmmo() || weapons[weaponIndex]->GetWeaponType() == EWeapon::E_ShotGun && weapons[weaponIndex]->GetTotalAmmo () == 0) {

			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			UAnimInstance* AnimWeaponInstance = FP_Gun->GetAnimInstance();

			if (AnimInstance != NULL)
			{

				AnimInstance->Montage_Play(weapons[weaponIndex]->ArmReloadAnimationAlt, weapons[weaponIndex]->GetFireAnimSpeed());


			}


			if (AnimWeaponInstance != NULL)
			{

				AnimWeaponInstance->Montage_Play(weapons[weaponIndex]->WeaponReloadAnimationAlt, weapons[weaponIndex]->GetFireAnimSpeed());


			}

		}


	}

}


void AAgentPlayerClass::SpawnProjectile() {
	
	if (isReloading != true) {







		if (!isDead) {

			//float randomSpread;

		//randomSpread = FMath::RandRange(-82, 82);

		// try and fire a projectile
			if (ProjectileClass != NULL)
			{

				float tempAmmo = weapons[weaponIndex]->GetClipAmmo();
				if (weapons[weaponIndex]->GetClipAmmo() > 0) {
					UWorld* const World = GetWorld();
					if (World != NULL)
					{
						if (bUsingMotionControllers)
						{



							const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();

							const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
							World->SpawnActor<AWeaponProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
						}
						else
						{
							float spread = FMath::RandRange(weapons[weaponIndex]->GetMinSpread(), weapons[weaponIndex]->GetMaxSpread());
							//	const FRotator SpawnRotation = GetControlRotation();

							const FRotator SpawnRotation = FRotator(GetControlRotation().Pitch + spread + GunCrosshairOffset, GetControlRotation().Yaw, GetControlRotation().Roll + spread + GunCrosshairOffset);
							// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
							const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);



							//Set Spawn Collision Handling Override
							FActorSpawnParameters ActorSpawnParams;
							ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

							if (weapons[weaponIndex]->GetWeaponType() == EWeapon::E_ShotGun || weapons[weaponIndex]->GetWeaponType() == EWeapon::E_DoubleShotGun) {


								// THIS IS REALLY DISGUSTING BUT ITS HOW I AM HANDLING DOING SHOTGUN WEAPON SPREAD
								
								


								for (size_t i = 0; i < weapons[weaponIndex]->GetProjectileAmount(); i++)
								{


									float spread2 = FMath::RandRange(-4.05f, 4.05f);
									float additionalSpread = FMath::RandRange(-4.05f, 4.05f);
									float spread3 = FMath::RandRange(1.5f, 2.5f);
									float division = FMath::RandRange(5.0f, 10.0f);


									if (i % 2 == 0) {
										additionalSpread *= -1;
										division *= -1;
										spread2 *= -1;
									}
									const FRotator SpawnRotation1 = FRotator(GetControlRotation().Pitch + (spread + GunCrosshairOffset) * (spread2 + additionalSpread) / division + ((i * spread3) / division), GetControlRotation().Yaw + (spread + GunCrosshairOffset) * (spread2 + additionalSpread + ((i * spread3) / division)), GetControlRotation().Roll + (spread2 + additionalSpread) * (spread + GunCrosshairOffset) + ((i * spread3) / division));


									World->SpawnActor<AWeaponProjectile>(ProjectileClass, SpawnLocation, SpawnRotation1, ActorSpawnParams);

								}

								World->SpawnActor<AWeaponProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

							}
							// spawn the projectile at the muzzle
							else {
								World->SpawnActor<AWeaponProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
							}


							AddControllerYawInput(weapons[weaponIndex]->GetYawInput());
							AddControllerPitchInput(weapons[weaponIndex]->GetPitchInput());

						}
					}

					// try and play the sound if specified
					if (FireSound != NULL)
					{
				
						UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
						
					}


					if (weapons[weaponIndex]->GetIsSilenced() != true) {
						UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, this, 0.0f, "Gun");
					}

					// try and play a firing animation if specified

						// Get the animation object for the arms mesh
					UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
					UAnimInstance* AnimWeaponInstance = FP_Gun->GetAnimInstance();

					if (AnimInstance != NULL)
					{

						AnimInstance->Montage_Play(weapons[weaponIndex]->ArmFireAnimation, weapons[weaponIndex]->GetFireAnimSpeed());


					}


					if (AnimWeaponInstance != NULL)
					{

						AnimWeaponInstance->Montage_Play(weapons[weaponIndex]->WeaponFireAnimation, weapons[weaponIndex]->GetFireAnimSpeed());


					}




					if (!weapons[weaponIndex]->GetInfAmmo() || weapons[weaponIndex]->GetWeaponType() != EWeapon::E_Melee)

						weapons[weaponIndex]->SetClipAmmo(tempAmmo -= 1);
					hud->SetPlayerAmmo(weapons[weaponIndex]->GetClipAmmo(), weapons[weaponIndex]->GetTotalAmmo());

					GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Reload);
				}


			}




		}
	}

}


void AAgentPlayerClass::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AAgentPlayerClass::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AAgentPlayerClass::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}


void AAgentPlayerClass::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AAgentPlayerClass::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AAgentPlayerClass::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AAgentPlayerClass::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AAgentPlayerClass::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AAgentPlayerClass::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AAgentPlayerClass::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AAgentPlayerClass::TouchUpdate);
		return true;
	}

	return false;
}



void AAgentPlayerClass::DelayFire() {

	canFire = true;
	GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Fire);
}

void AAgentPlayerClass::PickUpAmmo() {

	//Implementing Later as IDK What I want to do with this yet tbh
}

//This Will Handle the Weapon Swapping Based On Keyboard Inputs from 1-3
void AAgentPlayerClass::OnWeaponSwapOne() {
	if (isDead != true && isArmed == true) {

		OnRelease();
		canFire = false;


		if (weaponIndex != 0) {
			weaponIndex = 0;
			if (weapons[weaponIndex]->GetHasWeapon()) {
				SwitchWeaponType();
				lastWeapon = weaponIndex;
				GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetSwapDelay(), true);
			}
			else {
				weaponIndex = lastWeapon;

			}
		}


	}

}

void AAgentPlayerClass::OnWeaponSwapTwo() {
	
	if (isDead != true && isArmed == true) {

		OnRelease();
		canFire = false;

		if (weaponIndex != 1) {
			weaponIndex = 1;
			if (weapons[weaponIndex]->GetHasWeapon()) {
				SwitchWeaponType();
				lastWeapon = weaponIndex;
				GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetSwapDelay(), true);
			}
			else {
				weaponIndex = lastWeapon;

			}
		}





	}

}

void AAgentPlayerClass::OnWeaponSwapThree() {
	if (isDead != true && isArmed == true) {



		OnRelease();
		canFire = false;


		if (weaponIndex != 2) {
			weaponIndex = 2;
			if (weapons[weaponIndex]->GetHasWeapon()) {
				SwitchWeaponType();
				lastWeapon = weaponIndex;
				GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetSwapDelay(), true);
			}
			else {
				weaponIndex = lastWeapon;

			}
		}





	}



}

void AAgentPlayerClass::OnWeaponSwapFour() {
	if (isDead != true && isArmed == true) {

		OnRelease();
		canFire = false;


		if (weaponIndex != 3) {
			weaponIndex = 3;
			if (weapons[weaponIndex]->GetHasWeapon()) {
				SwitchWeaponType();
				lastWeapon = weaponIndex;
				GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetSwapDelay(), true);
			}
			else {
				weaponIndex = lastWeapon;

			}
		}



	}

}

void AAgentPlayerClass::OnWeaponSwapFive() {

	if (isDead != true && isArmed == true) {

		OnRelease();
		canFire = false;


		if (weaponIndex != 4) {
			weaponIndex = 4;
			if (weapons[weaponIndex]->GetHasWeapon()) {
				SwitchWeaponType();
				lastWeapon = weaponIndex;
				GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetSwapDelay(), true);
			}
			else {
				weaponIndex = lastWeapon;

			}
		}



	}

}

void AAgentPlayerClass::SwapToMelee() {
	if (isDead != true && isArmed == true) {

		if (weaponIndex != 5) {
			weaponIndex = 5;
			if (weapons[weaponIndex]->GetHasWeapon()) {
				SwitchWeaponType();
				lastWeapon = weaponIndex;
				GetWorldTimerManager().SetTimer(TimerHandle_Handle_Autofire, this, &AAgentPlayerClass::DelayFire, weapons[weaponIndex]->GetSwapDelay(), true);
			}
			else {
				weaponIndex = lastWeapon;

			}





	}
	OnRelease();
	canFire = false;


}




}



void  AAgentPlayerClass::DrawAnimation() {




	UAnimInstance* AnimInst = Mesh1P->GetAnimInstance();
	UAnimInstance* WeaponInstance = FP_Gun->GetAnimInstance();

	AnimInst->Montage_Play(weapons[weaponIndex]->ArmDrawAnimation, weapons[weaponIndex]->GetDrawAnimSpeed());
	WeaponInstance->Montage_Play(weapons[weaponIndex]->WeaponDrawAnimation, weapons[weaponIndex]->GetDrawAnimSpeed());

	GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Draw);


}

void  AAgentPlayerClass::SwitchWeaponType() {

	if (isDead != true) {


		if (weapons[weaponIndex]->GetHasWeapon()) {
			ZoomOut();



			UAnimInstance* AnimInst = Mesh1P->GetAnimInstance();
			UAnimInstance* WeaponInstance = FP_Gun->GetAnimInstance();
			GetWorldTimerManager().ClearTimer(TimerHandle_Delay_Reload);
			isReloading = false;

			if (weapons[weaponIndex]->ArmAnimClass != NULL) {
				Mesh1P->SetAnimInstanceClass(weapons[weaponIndex]->ArmAnimClass);

			}

			if (weapons[weaponIndex]->WeaponAnimClass != NULL) {
				FP_Gun->SetSkeletalMesh(weapons[weaponIndex]->GunMesh);
				FP_Gun->SetAnimInstanceClass(weapons[weaponIndex]->WeaponAnimClass);

				GetWorldTimerManager().SetTimer(TimerHandle_Delay_Draw, this, &AAgentPlayerClass::DrawAnimation, 0.05f, false);



			}

			AnimInst->Montage_Play(weapons[weaponIndex]->ArmDrawAnimation, 1.f);
			WeaponInstance->Montage_Play(weapons[weaponIndex]->WeaponDrawAnimation, 1.f);

			if (weapons[weaponIndex]->ProjectileClass != NULL) {
				ProjectileClass = weapons[weaponIndex]->ProjectileClass;

			}
			FireSound = weapons[weaponIndex]->FireSound;

			FP_MuzzleLocation->AttachToComponent(FP_Gun, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("MuzzleEnd"));

		}
		FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		hud->SetPlayerAmmo(weapons[weaponIndex]->GetClipAmmo(), weapons[weaponIndex]->GetTotalAmmo());




	}



}


void AAgentPlayerClass::SpawnWeaponOnPlayer(int wIndex) {



	UWorld* const World = GetWorld();
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = Instigator;

	AWeaponClass* iWeapon;

	iWeapon = World->SpawnActor<AWeaponClass>(WeaponLoadout[wIndex], Location, Rotation, SpawnInfo);

	weapons.Insert(iWeapon, wIndex);




}


void AAgentPlayerClass::PickUpWeapon(AActor* otherActor, int wIndex, int cAmmo) {

	int tempAmmo = weapons[weaponIndex]->GetTotalAmmo();
	if (weapons[wIndex]->GetHasWeapon() != true)
	{
		weapons[wIndex]->SetHasWeapon(true);


		if (otherActor != NULL) {
			otherActor->Destroy();
		}

	}
	else if (weapons[wIndex]->GetHasWeapon() && weapons[wIndex]->GetTotalAmmo() != weapons[wIndex]->GetMaxTotalAmmo()) {
		weapons[wIndex]->SetTotalAmmo(tempAmmo + cAmmo);


		if (weapons[wIndex]->GetTotalAmmo() > weapons[wIndex]->GetMaxTotalAmmo()) {
			weapons[wIndex]->SetTotalAmmo(weapons[wIndex]->GetMaxTotalAmmo());

			hud->SetPlayerAmmo(weapons[weaponIndex]->GetClipAmmo(), weapons[weaponIndex]->GetTotalAmmo());


			if (otherActor != NULL) {
				otherActor->Destroy();
			}
		}

		if (otherActor != NULL) {
			otherActor->Destroy();
		}


	}
	hud->SetPlayerAmmo(weapons[weaponIndex]->GetClipAmmo(), weapons[weaponIndex]->GetTotalAmmo());


}


void AAgentPlayerClass::HandleDamage(int damage) {

		PlayerHealth -= damage;

		hud->SetPlayerHealth(PlayerHealth);


	if (PlayerHealth <= 0) {

		hud->SetInteractionLabel("Press Z to Restart!");
		hud->GetPlayerAmmo()->SetVisibility(ESlateVisibility::Hidden);
		hud->GetPlayerHealth()->SetVisibility(ESlateVisibility::Hidden);
		hud->GetDeathScreenTint()->SetVisibility(ESlateVisibility::Visible);
		GetCharacterMovement()->DisableMovement();
		isDead = true;
		Mesh1P->SetVisibility(false);
		FP_Gun->SetVisibility(false);
	}



}
void AAgentPlayerClass::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {


	if (OtherActor->IsA(AWeaponProjectile::StaticClass())) {

		AWeaponProjectile* projectile = Cast<AWeaponProjectile>(OtherActor);

		if (projectile->ActorHasTag(TEXT("EnemyProjectile")))

			HandleDamage(projectile->ProjectileDamage);

	}

}







bool AAgentPlayerClass::GetPlayerDeath() {

	return isDead;
}

void AAgentPlayerClass::RestartLevel() {
	if (isDead) {

		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

	}

}


void AAgentPlayerClass::PickUpHealth(AActor* otherActor, int hIndex) {
	if (PlayerHealth != PlayerHealthMax) {

		PlayerHealth = PlayerHealth + hIndex;

		if (PlayerHealth > PlayerHealthMax) {
			PlayerHealth = PlayerHealthMax;
		}

		hud->SetPlayerHealth(PlayerHealth);
		if (otherActor != NULL) {
			otherActor->Destroy();
		}


	}

}