// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "CivilianAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNDERCOVER_AGENT_API ACivilianAIController : public AAIController
{
	GENERATED_BODY()

public:


	virtual void Tick(float DeltaTime) override;

	void AiTakeCoverTask(float fResponse);

	void AiCowerInFearTask();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAIPerceptionComponent* AiPerception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAISenseConfig_Hearing* HearingConfig;


	UFUNCTION()
		void PerceptionSenseUpdateTargeted(AActor* actor, FAIStimulus const stimuli);


	void SetHearingConfig();

	void SetSightConfig();

	ACivilianAIController();

	UFUNCTION()
		void PerceptionSenseUpdate(const TArray<AActor*>& actors);



	void BeginPlay() override;
	void OnPossess(APawn* const pawn) override;
	void MoveToLocation();
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UBehaviorTree* behaviorTree;

	class ACivilianAI* civilian;


private:

	bool aiStartRunningAway;

	FTimerHandle TimerHandle_Get_Up_Delay;
	FTimerHandle TimerHandle_Move_Delay;

	float scareValue;

	bool reachedDestination;

	float aiFearResponse;

	float aiFearResponseMax;

	void SetAiRunAwayDelay();
	
	bool willCrouch;

	void StartRunAwayNow();

	float waitMin;

	float waitMax;
};

	
