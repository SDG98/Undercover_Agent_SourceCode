

#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "EngineGlobals.h"


AEnemyAIController::AEnemyAIController() {

	//	AiPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component")));
	SetHearingConfig();
	SetSightConfig();
	aiHostile = false;
	reachedDestination = false;
	attackPlayer = false;
	damageStage = 0;
	aiScared = false;

	sawFriendsDied = 0; 
	aggressionStart = false;
	aiBraveryCheck = 0.0f;
	startRegen = false;
}

void AEnemyAIController::BeginPlay()
{



	Super::BeginPlay();
	


	if (enemy->isVip) {

	}





	
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::PerceptionSenseUpdateTargeted);

}


void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (player != nullptr) {

		if (player->GetPlayerDeath() == true) {
			aiHostile = false;
		}


		else if (aiScared == true) {
			AiFollowPlayerTask(defensePosition, 300.0f, 1000.0f);
		}


		else if (enemy->ammo <= 0 && enemy->isReloading != true) {
			enemy->ReloadEvent();
		}



		else if (aiHostile && player->GetPlayerDeath() == false && aiScared == false && player->isArmed == true) {


			if(reachedDestination != true){
				AiStanceTask();
			}
			
			SetFocus(player);

			//AiAttackPlayerTask();
			if (attackPlayer != true && enemy->isReloading !=true && enemy->ammo > 0) {

			
					AiAttackPlayerTask();
		
		
			}

	



		}


	}


	if (enemy->aiBravery < (enemy->aiBraveryMax / 1.50f) && startRegen !=true ) {

		RegenerateBravery();
	}



	//if ai took to much damage check stuff  will constantly call if they keep taking more and more damage and if they are still hostile


	if (enemy->health <= (enemy->maxHealth / 2) && enemy->isDead == false && damageStage == 0 && enemy->foolhardly != true) {
		aiBraveryCheck = FMath::RandRange(0.0f, 1.0f);

		//if the check  fails they will be alot less brave and more cautious

		if (aiBraveryCheck > enemy->aiBravery) {

			enemy->aiBravery = (enemy->aiBravery / 1.50f);
			

		}
		//if bravery check succeeds essentially they will still fight however they will move more cautiously and be less aggressive and slightly less brave
		else if (aiBraveryCheck <= enemy->aiBravery) {

			enemy->aiBravery = (enemy->aiBravery / 1.15f);

		}

		damageStage = 1;
	}


	else if (enemy->health <= (enemy->maxHealth / 3) && enemy->isDead == false && damageStage == 1 && enemy->foolhardly != true) {
		aiBraveryCheck = FMath::RandRange(0.0f, 1.0f);

		//if check fails they will be a bit more scared
		if (aiBraveryCheck > enemy->aiBravery) {

			AiFleeTask();
			enemy->aiBravery = (enemy->aiBravery / 1.50f);
			enemy->aiAggression = (enemy->aiAggression / 1.50f);


		}
		//if bravery check succeeds essentially they will still fight however they will move more cautiously and be less aggressive
		else if (aiBraveryCheck <= enemy->aiBravery) {


			enemy->aiBravery = (enemy->aiBravery / 1.25f);


		}
		damageStage = 2;


	}



	else if (enemy->health <= (enemy->maxHealth / 4) && enemy->isDead == false && damageStage == 2 && enemy->foolhardly != true) {
		aiBraveryCheck = FMath::RandRange(0.30f, 1.00f);

		//if check fails they will be a bit more scared
		if (aiBraveryCheck > enemy->aiBravery && aiScared != true) {


			AiFleeTask();




		}

		damageStage = 3;


	}











	if (enemy->isDead) {
		UnPossess();
		Destroy();
	}

}

void AEnemyAIController::RegenerateBravery() {
	startRegen = true;
	
	GetWorldTimerManager().SetTimer(TimerHandle_RegenBravery, this, &AEnemyAIController::EndRegenBravery, enemy->regenTimerBravery, true);

}

void AEnemyAIController::EndRegenBravery() {
	startRegen = false;

	enemy->aiBravery = enemy->aiBraveryMax;
	GetWorldTimerManager().ClearTimer(TimerHandle_RegenBravery);
}

void AEnemyAIController::EnableAggression() {
	aggressionStart = true;

	GetWorldTimerManager().ClearTimer(TimerHandle_AggressionDelay);
}
void AEnemyAIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);
	if (GetPawn() != nullptr) {
		enemy = Cast<AEnemyAI>(GetPawn());
		defensePosition = enemy->GetActorLocation();
	}

}

//This will handle all the different stances ai will take when fighting player so if they decide to do something on the fly when updating their

void AEnemyAIController::AiStanceTask() {


		//this will handle if they decide to go move
		if (reachedDestination != true) {
			float aistance = FMath::RandRange(0.0f, 1.0f);
			
			if (aistance > enemy->aiAggression || enemy->isReloading == true) {

				AiFollowPlayerTask(defensePosition, -500.0f, 500.0f);


			}

			else if (aistance <= enemy->aiAggression && enemy->isReloading == false) {

				AiFollowPlayerTask(player->GetActorLocation(), -500.0f, 500.0f);
	
			}


		}






	


	



}

void AEnemyAIController::AiFollowPlayerTask(FVector targetLocation, float min, float max) {
	float waitTimer = FMath::RandRange(2.0f, 3.0f);
	float randomLocationMove = FMath::RandRange(min, max);

	//Get navigation mesh and create  a random location
	FNavLocation loc;
	UNavigationSystemV1* const navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (reachedDestination != true) {

		GetWorldTimerManager().SetTimer(TimerHandle_Movement_Delay, this, &AEnemyAIController::AiFollowReachedDestination, waitTimer, true);
		if (navSys->GetRandomPointInNavigableRadius(targetLocation, 1000.0f + randomLocationMove, loc, nullptr)) {
			MoveTo(loc.Location);
			Super::OnMoveCompleted(GetCurrentMoveRequestID(), EPathFollowingRequestResult::Type());
			reachedDestination = true;
		}

	}

}

void AEnemyAIController::AiStandUpTask() {
	enemy->StandEvent();
	GetWorldTimerManager().ClearTimer(TimerHandle_StandUp_Delay);
	LostSight();
}
void AEnemyAIController::AiCrouchTask(float min, float max) {

	float waitTimer = FMath::RandRange(min, max);
	GetWorldTimerManager().SetTimer(TimerHandle_Movement_Delay, this, &AEnemyAIController::AiStandUpTask, waitTimer, true);
	enemy->CrouchEvent();
}
void AEnemyAIController::AiFollowReachedDestination() {
	reachedDestination = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_Movement_Delay);
	defensePosition = enemy->GetActorLocation();
	if (aiHostile) {
		LostSight();
	}

}
void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{


	float randomWaitDelay = FMath::RandRange(0.10f, 2.0f);

	if (Result.Code == EPathFollowingResult::Success) {

		GetWorldTimerManager().SetTimer(TimerHandle_Movement_Delay, this, &AEnemyAIController::AiFollowReachedDestination, 0.10f + randomWaitDelay, true);
	}


	if (Result.Code == EPathFollowingResult::Blocked) {

		GetWorldTimerManager().SetTimer(TimerHandle_Movement_Delay, this, &AEnemyAIController::AiFollowReachedDestination, 0.10f + randomWaitDelay, true);
	}
}



void AEnemyAIController::SetHearingConfig() {

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

	if (HearingConfig) {

		HearingConfig->HearingRange = 5000.0f;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->SetMaxAge(0.0f);
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		GetPerceptionComponent()->ConfigureSense(*HearingConfig);

	}

}

void AEnemyAIController::SetSightConfig() {


	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	if (SightConfig) {

		SightConfig->SightRadius = 10000.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 50.0f;
		SightConfig->PeripheralVisionAngleDegrees = 180.0f;
		SightConfig->SetMaxAge(0.0f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 10000.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
		
	}
}


void AEnemyAIController::AiStopAttackingPlayerTask() {

	attackPlayer = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Attack_Delay);;
}

void AEnemyAIController::AiAttackPlayerTask() {


	if (!enemy->isDead) {
		attackPlayer = true;
		enemy->DrawWeapon();
		enemy->AttackEvent();
		
		float randomWaitDelay = FMath::RandRange(enemy->fireRateWaitMin, enemy->fireRateWaitMax) + FMath::RandRange(enemy->attackDelayMin, enemy->attackDelayMax);
		GetWorldTimerManager().SetTimer(TimerHandle_Attack_Delay, this, &AEnemyAIController::AiStopAttackingPlayerTask, enemy->fireRateInitial + randomWaitDelay, true);

	}

}

void AEnemyAIController::LostSight() {
	if (player != nullptr) {
		AiStanceTask();
	}


	//AiFollowPlayerTask(player->GetActorLocation());
	GetPerceptionComponent()->ForgetAll();
	aiHostile = false;
	ClearFocus(EAIFocusPriority::LastFocusPriority);

}

void AEnemyAIController::StopAttacking() {
	GetWorldTimerManager().SetTimer(TimerHandle_LostSight, this, &AEnemyAIController::LostSight, SightConfig->GetMaxAge(), true);
}

void AEnemyAIController::AiFleeTask() {
	ClearFocus(EAIFocusPriority::LastFocusPriority);
	AiFollowReachedDestination();
	Super::OnMoveCompleted(GetCurrentMoveRequestID(), EPathFollowingRequestResult::Type());
	float scareDelay = FMath::RandRange(5.0f, 10.0f);
	enemy->FleeEvent();
	aiScared = true;
	AiFollowPlayerTask(defensePosition, -500.0f, 500.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_AI_Scared, this, &AEnemyAIController::OnScaredCompleted, scareDelay, true);

	AiStandUpTask();



}


void AEnemyAIController::OnScaredCompleted() {
	enemy->HostileEvent();
	LostSight();
	aiScared = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_AI_Scared);
}


// LEGACY STUFF GOES UNUSED AT THE MOMENT
void AEnemyAIController::PerceptionSenseUpdate(const TArray<AActor*>& actors) {
}



//PERCEPTION UPDATE TARGET

void AEnemyAIController::PerceptionSenseUpdateTargeted(AActor* actor, FAIStimulus const stimuli) {

	FAIStimulus stim;



	AAgentPlayerClass* const playerCharacter = Cast<AAgentPlayerClass>(actor);


	if (stimuli.Type == HearingConfig->GetSenseID() && playerCharacter) {
		player = playerCharacter;
		if (player->GetPlayerDeath() == false && player->isArmed == true && aiHostile != true && aiScared !=true) {
			if (enemy->investigateSounds == true) {
			
				enemy->DrawWeapon();
				enemy->HostileEvent();

				float followDistance = FVector::Distance(player->GetActorLocation(), enemy->GetActorLocation());
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("World delta for current frame equals %f"), followDistance));
				if (followDistance <= enemy->followDistance) {
					AiFollowPlayerTask(player->GetActorLocation(), -1250.0f, 1250.0f);
				}

			}

		}

	}



	else if (stimuli.Type == SightConfig->GetSenseID() && playerCharacter) {

		player = playerCharacter;
		if (player->GetPlayerDeath() == false && player->isArmed) {
			aiHostile = true;
		}
		if (player->isArmed == false) {
			GetPerceptionComponent()->ForgetAll();
		}


		stim = stimuli;

	}

	else if (aiHostile != true && stimuli.Type == HearingConfig->GetSenseID() && stimuli.Tag != "FriendlyDeathSound") {
		//AiFollowPlayerTask(stimuli.StimulusLocation, -500.0f, 500.0f);

	}


	else if (stimuli.Type == HearingConfig->GetSenseID() && stimuli.Tag == "FriendlyDeathSound" && aiHostile == true) {




		if (enemy->foolhardly != true) {
			enemy->aiBravery = (enemy->aiBravery / 1.10f);

			sawFriendsDied += 1;




		}

		if (enemy->isDead == false && enemy->foolhardly != true) {



			if (sawFriendsDied == 2) {
				aiBraveryCheck = FMath::RandRange(0.00f, 0.85f);


				if (aiBraveryCheck > enemy->aiBravery && aiScared != true) {

					AiFleeTask();

				}



			}

			else if (sawFriendsDied == 3 || sawFriendsDied == 4) {
				aiBraveryCheck = FMath::RandRange(0.00f, 1.00f);


				if (aiBraveryCheck > enemy->aiBravery && aiScared != true) {

					AiFleeTask();

				}



			}


			else if (sawFriendsDied == 5 && enemy->isDead == false) {
				aiBraveryCheck = FMath::RandRange(0.20f, 1.00f);


				if (aiBraveryCheck > enemy->aiBravery && aiScared != true) {

					AiFleeTask();

				}

				sawFriendsDied = 0;


			}






		}







	}


	else if (stim.IsExpired()) {
		aiHostile = false;


	}



}
