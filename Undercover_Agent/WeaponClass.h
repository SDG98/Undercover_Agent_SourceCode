// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"

#include "WeaponClass.generated.h"





UENUM(BlueprintType)
enum class EWeapon : uint8
{
	E_AutoRifle UMETA(DisplayName = "AUTOMATIC RIFLE"),
	E_Rifle UMETA(DisplayName = "RIFLE"),
	E_Pistol UMETA(DisplayName = "PISTOL"),
	E_HeavyRifle UMETA(DisplayName = "HEAVY RIFLE"),
	E_ShotGun UMETA(DisplayName = "SHOTGUN"),
	E_DoubleShotGun UMETA(DisplayName = "DOUBLE BARREL SHOTGUN"),
	E_GrenadeLauncher UMETA(DisplayName = "GRENADE LAUNCHER"),
	E_Melee UMETA(DisplayName = "MELEE")
};



UCLASS()
class UNDERCOVER_AGENT_API AWeaponClass : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties

	//Used to store AssetValues for each Weapon

		/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* ReloadSound;



	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* WeaponFireAnimation;
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* ArmFireAnimation;


	/** AnimMontage to play each time we draw Weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* WeaponDrawAnimation;

	/** AnimMontage to play each time we draw Weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* ArmDrawAnimation;

	/** AnimMontage to play each time we reload Weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* WeaponReloadAnimation;


	/** AnimMontage to play each time we Swap Weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* ArmReloadAnimation;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* WeaponReloadAnimationAlt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* ArmReloadAnimationAlt;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UClass* ArmAnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UClass* WeaponAnimClass;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
		USkeletalMesh* GunMesh;

	AWeaponClass();

	//HANDLES GETTERS AND SETTERS FOR ALL WEAPON vARIABLES
	void SetReloadTime(float rTime);
	void SetMaxTotalAmmo(float mAmmo);
	void SetTotalAmmo(float mAmmo);
	void SetMaxClipAmmo(float mClip);
	void SetClipAmmo(float mClip);
	void SetInfAmmo(bool iAmmo);
	void SetZoomValue(float zoom);
	void SetIsSilenced(bool silence);
	void SetWeaponIndex(int wIndex);
	void SetProjectileAmount(int pAmmount);
	void SetHasWeapon(bool hWeapon);
	bool GetIsSilenced();
	float GetReloadTime();
	float GetMaxTotalAmmo();
	float GetTotalAmmo();
	float GetMaxClipAmmo();
	float GetClipAmmo();
	float GetFireRateDelay();
	float GetFireRateDelayAuto();
	float GetSwapDelay();
	float GetYawInput();
	
	float GetPitchInput();
	float GetMinSpread();
	float GetMaxSpread();
	float GetZoomValue();
	float GetReloadAnimSpeed();
	float GetFireAnimSpeed();
	float GetDrawAnimSpeed();
	int GetProjectileAmount();
	FName GetWeaponSocket();

	bool GetInfAmmo();

	bool GetHasWeapon();

	int GetWeaponIndex();

	EWeapon GetWeaponType();

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AWeaponProjectile> ProjectileClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//HANDLES WEAPON PROPERTIES


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool isSilenced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float zoomValue;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool hasWeapon;

	//Sets what the weapon index will be on the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int weaponIndex;

	//Sets the Delay before firing weapon again (the lower the delay, the faster the weapon can fire again)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float fireRateDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float fireRateDelayAutoRifle;


	//CONTROLLER YAW INPUT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float yawInput;
	//CONTROLLER PITCH INPUT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float pitchInput;
	//Minimum Weapon Spread
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float minSpread;
	//Maximum Weapon Spread
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float maxSpread;

	//Sets if the weapon has infinite ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool infAmmo;
	//the maximum ammo of what the weapon is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int maxTotalAmmo;
	// total ammo the gun will have when its picked up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int totalAmmo;
	//clip size of the gun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int clipAmmo;
	//max possible clip size of the gun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int maxClipAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int projectileAmount;

	//reload time of the gun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float reloadTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float reloadAnimSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float drawAnimSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float fireAnimSpeed;
	//variable delay so the player cannot shoot his gun while the swapping animation is playing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float swapDelay;
	//variable for the socket that the gun will be attached to on the players skeleton
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FName WeaponSocket;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeapon weapon;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
