// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponClass.h"

// Sets default values
AWeaponClass::AWeaponClass()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	minSpread = 0.01;
	maxSpread = 0.15;
	yawInput = 0.05;
	pitchInput = -0.1;
	maxTotalAmmo = 90;
	maxClipAmmo = 15;
	totalAmmo = 90;
	clipAmmo = 15;
	reloadTime = 0.0f;
	fireRateDelay = 0.1025f;
	fireRateDelayAutoRifle = 0.095f;
	zoomValue = 1.0f;
	swapDelay = 0.25f;
	reloadAnimSpeed = 1.5f;
	drawAnimSpeed = 1.5f;
	fireAnimSpeed = 1.5f;
	reloadTime = 2.25f;
	isSilenced = false;
	projectileAmount = 1;
	// Create a gun mesh component
		// Create a gun mesh component

	WeaponSocket = "PistolWeaponSocket";

	weapon = EWeapon::E_Pistol;
}


// Called when the game starts or when spawned
void AWeaponClass::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWeaponClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void  AWeaponClass::SetInfAmmo(bool iAmmo) {

	infAmmo = iAmmo;
}


void  AWeaponClass::SetIsSilenced(bool silence) {

	isSilenced = silence;
}


void  AWeaponClass::SetReloadTime(float rTime) {

	reloadTime = rTime;
}
void  AWeaponClass::SetMaxTotalAmmo(float mAmmo) {

	maxTotalAmmo = mAmmo;
}

void  AWeaponClass::SetTotalAmmo(float mAmmo) {

	totalAmmo = mAmmo;
}
void  AWeaponClass::SetMaxClipAmmo(float mClip) {
	maxClipAmmo = mClip;
}

void  AWeaponClass::SetZoomValue(float zoom) {
	zoomValue = zoom;
}

void  AWeaponClass::SetProjectileAmount(int pAmount) {
	projectileAmount = pAmount;
}

void  AWeaponClass::SetClipAmmo(float mClip) {
	clipAmmo = mClip;
}

void AWeaponClass::SetHasWeapon(bool hWeapon) {
	hasWeapon = hWeapon;
}

void AWeaponClass::SetWeaponIndex(int wIndex) {
	wIndex = weaponIndex;
}

int AWeaponClass::GetWeaponIndex() {
	return weaponIndex;
}

bool AWeaponClass::GetHasWeapon() {
	return hasWeapon;
}

bool  AWeaponClass::GetInfAmmo() {

	return infAmmo;
}
bool  AWeaponClass::GetIsSilenced() {

	return isSilenced;
}

float  AWeaponClass::GetReloadTime() {
	return reloadTime;
}

float  AWeaponClass::GetMaxTotalAmmo() {
	return maxTotalAmmo;
}

float  AWeaponClass::GetTotalAmmo() {
	return totalAmmo;
}


float  AWeaponClass::GetMaxClipAmmo() {
	return maxClipAmmo;
}

float  AWeaponClass::GetZoomValue() {
	return zoomValue;
}
float  AWeaponClass::GetClipAmmo() {
	return clipAmmo;
}




float  AWeaponClass::GetDrawAnimSpeed() {
	return drawAnimSpeed;
}

float  AWeaponClass::GetReloadAnimSpeed() {
	return reloadAnimSpeed;
}
float  AWeaponClass::GetFireAnimSpeed() {
	return fireAnimSpeed;
}


float AWeaponClass::GetFireRateDelay() {
	return fireRateDelay;
}

float AWeaponClass::GetSwapDelay() {
	return swapDelay;
}
float AWeaponClass::GetFireRateDelayAuto() {
	return fireRateDelayAutoRifle;
}
FName  AWeaponClass::GetWeaponSocket() {
	return WeaponSocket;
}
EWeapon AWeaponClass::GetWeaponType() {
	return weapon;
}

float  AWeaponClass::GetYawInput() {
	return yawInput;
}
float  AWeaponClass::GetPitchInput() {
	return pitchInput;
}
float  AWeaponClass::GetMinSpread() {
	return minSpread;
}
float AWeaponClass::GetMaxSpread() {
	return maxSpread;
}

int AWeaponClass::GetProjectileAmount() {
	return projectileAmount;
}