// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentPlayerClass.h"
#include "CivilianAIController.h"
#include "UObject/ConstructorHelpers.h"
#include "CivilianAI.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "EngineGlobals.h"

ACivilianAIController::ACivilianAIController() {


	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component")));
	SetHearingConfig();
	SetSightConfig();
	reachedDestination = false;
	aiStartRunningAway = true;
	aiFearResponse = 0.0f;
	aiFearResponseMax = 400.0f;
	scareValue = 10.0f;
	waitMin = 2.0f;
	waitMax = 10.0f;
	willCrouch = false;
}
void ACivilianAIController::BeginPlay()
{


	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ACivilianAIController::PerceptionSenseUpdateTargeted);

	aiFearResponseMax = aiFearResponseMax + FMath::RandRange(50.0f, 300.0f);
	scareValue = scareValue + FMath::RandRange(1.0f, 5.0f);
	Super::BeginPlay();

}


void ACivilianAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (civilian->isDead) {
		UnPossess();
		Destroy();
	}

	if (civilian->isAfraid) {


		if (willCrouch != true) {
			AiTakeCoverTask(0);
		}



	}

}


void ACivilianAIController::AiTakeCoverTask(float fResponse) {


	if (aiStartRunningAway && willCrouch == false) {
		willCrouch = true;
		//Sets Civilian Ai Animation to scared/fleeing mode
		civilian->SetFleeAnim();
		//Need to change this variable name but basically sets so that the Ai wont constantly run in different directions when scared and will
		// run to one location before resetting is ability to run
		aiStartRunningAway = false;
		reachedDestination = false;

		GetWorldTimerManager().SetTimer(TimerHandle_Move_Delay, this, &ACivilianAIController::MoveToLocation, 0.08f, true);


	}




}

void ACivilianAIController::StartRunAwayNow() {

	willCrouch = false;
	civilian->SetFleeAnim();
	GetWorldTimerManager().SetTimer(TimerHandle_Get_Up_Delay, this, &ACivilianAIController::SetAiRunAwayDelay, civilian->getUpTime, true);
	aiFearResponse = 0.0f;

}


void ACivilianAIController::AiCowerInFearTask() {

		civilian->isAfraid = true;
		civilian->CowerInFear();
		
		aiFearResponse = 0.0f;
		reachedDestination = true;

	
		GetWorldTimerManager().SetTimer(TimerHandle_Get_Up_Delay, this, &ACivilianAIController::StartRunAwayNow, FMath::RandRange(waitMin, waitMax), false);

}

void ACivilianAIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);
	if (GetPawn() != nullptr) {


		civilian = Cast<ACivilianAI>(GetPawn());
	}

}
void ACivilianAIController::MoveToLocation() {


	FVector const origin = civilian->GetActorLocation();
	//Get navigation mesh and create  a random location
	FNavLocation loc;
	UNavigationSystemV1* const navSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (navSys->GetRandomPointInNavigableRadius(origin, (2000.0f + FMath::RandRange(-500.0f, 500.0f)), loc, nullptr)) {

		if (reachedDestination != true) {
			MoveTo(loc.Location);
			Super::OnMoveCompleted(GetCurrentMoveRequestID(), EPathFollowingRequestResult::Type());
		}

	}
		
	GetWorldTimerManager().ClearTimer(TimerHandle_Move_Delay);

}
void ACivilianAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.Code == EPathFollowingResult::Success) {
		AiCowerInFearTask();

	}


	if (Result.Code == EPathFollowingResult::Blocked) {
		AiCowerInFearTask();

	}
}


void ACivilianAIController::SetAiRunAwayDelay()
{
	if (willCrouch != true) {
		aiStartRunningAway = true;
		AiTakeCoverTask(0);
		GetWorldTimerManager().ClearTimer(TimerHandle_Get_Up_Delay);


	}

}

void ACivilianAIController::SetHearingConfig() {

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

	if (HearingConfig) {

		HearingConfig->HearingRange = 5000.0f;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		GetPerceptionComponent()->ConfigureSense(*HearingConfig);

	}

}


void ACivilianAIController::SetSightConfig() {




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


void ACivilianAIController::PerceptionSenseUpdateTargeted(AActor* actor, FAIStimulus const stimuli) {

	FAIStimulus stim;



	AAgentPlayerClass* const playerCharacter = Cast<AAgentPlayerClass>(actor);


	

	if (stimuli.Type == SightConfig->GetSenseID() && playerCharacter && civilian->isAfraid == false) {


		

		if (playerCharacter->isArmed == true) {
			civilian->isAfraid = true;
		
		}

		if (playerCharacter->isArmed != true) {
			GetPerceptionComponent()->ForgetAll();
		}


	}


	if (stimuli.Type == HearingConfig->GetSenseID() && stimuli.Tag == "Gun") {


		if (willCrouch == false) {
			civilian->isAfraid = true;
		}
		


	}

}




void ACivilianAIController::PerceptionSenseUpdate(const TArray<AActor*>& actors) {

	for (size_t i = 0; i < actors.Num(); i++)
	{
		const FActorPerceptionInfo* info = GetPerceptionComponent()->GetActorInfo(*actors[i]);

		AAgentPlayerClass* playerCharacter = Cast<AAgentPlayerClass>((actors[i]));

		for (size_t x = 0; x < info->LastSensedStimuli.Num(); x++)
		{
			FAIStimulus const stim = info->LastSensedStimuli[x];

			if (stim.Tag == "Gun") {
			


			}


			if (stim.Type == SightConfig->GetSenseID() && playerCharacter) {

				
				if (playerCharacter->isArmed == true) {
					
				}



			}
			



		}


	}



}