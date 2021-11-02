// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponClass.h"
#include "Undercover_AgentHUD.h"
#include "AgentPlayerClass.generated.h"

class UInputComponent;

UCLASS(config = Game)
class AAgentPlayerClass : public ACharacter
{
	GENERATED_BODY()


		/** Location on gun mesh where projectiles should spawn. */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;

public:
	AAgentPlayerClass();

protected:
	virtual void BeginPlay();
	virtual void Tick (float DeltaTime) override;
public:
	bool GetPlayerDeath();
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AWeaponProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isArmed;
	bool hasLanded;
	void HandleDamage(int damage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int PlayerHealth;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int PlayerHealthMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float GunCrosshairOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TArray<TSubclassOf<AWeaponClass>> WeaponLoadout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int starterWeapon;
	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		uint32 bUsingMotionControllers : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		TArray<AWeaponClass*> weapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAIPerceptionStimuliSourceComponent* PerceptionStimuli;

	float fallTimeCheck;

	UFUNCTION(BlueprintImplementableEvent, Category = Gameplay)
		void SpawnLoadout();
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		class UCapsuleComponent* BulletCapsule;
	FVector currentVelocity;
	float safeFallVelocity;
	int fallDamage;
	FVector previousVelocity;
	
	void EquipWeapon();

	void PickUpWeapon(AActor* otherActor, int wIndex, int cAmmo);

	void PickUpHealth(AActor* otherActor, int hIndex);
	class AUndercover_AgentHUD* hud;
protected:
	float defaultFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool canFire;
	class UAIPerceptionStimuliSourceComponent* playerStimulus;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		int weaponIndex;
	int lastWeapon;
	/**Timers Used to handle Autofire for weapons*/
	FTimerHandle TimerHandle_Handle_Autofire;
	/**Timers Used to handle the swap delay for weapons*/
	FTimerHandle TimerHandle_Delay_Fire;
	/**Timers Used to delay the animation swap for weapons*/
	FTimerHandle TimerHandle_Delay_Draw;
	/**Timers Used to handle the reload delay for weapons*/
	FTimerHandle TimerHandle_Delay_Reload;
	//Handles the Input for Firing and Releasing the attack button
	void OnFire();
	void OnRelease();
	/** Fires a projectile. */
	void SpawnProjectile();
	void OnWeaponSwapOne();
	void OnWeaponSwapTwo();
	void OnWeaponSwapThree();
	void OnWeaponSwapFour();
	void OnWeaponSwapFive();
	void SwapToMelee();
	void RestartLevel();
	void ReloadWeapon();
	void ReloadWeaponAnim();
	void StartReload();
	void DrawAnimation();
	void ReloadShotgunWeapon();
	void DelayFire();
	void SwitchWeaponType();
	void ZoomIn();
	void ZoomOut();
	void SpawnWeaponOnPlayer(int wIndex);

	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void PickUpAmmo();
	/** Resets HMD orientation and position in VR. */
	void OnResetVR();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

	bool isDead;
	bool isReloading;
	bool isScoped;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

