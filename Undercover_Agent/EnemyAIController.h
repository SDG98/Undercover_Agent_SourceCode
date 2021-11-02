// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAI.h"
#include "AgentPlayerClass.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNDERCOVER_AGENT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:


	virtual void Tick(float DeltaTime) override;

	void AiFollowPlayerTask(FVector targetLocation, float min, float max);

	void AiAttackPlayerTask();
	void AiStanceTask();
	void AiCrouchTask(float min, float max);
	void AiStandUpTask();
	void AiFleeTask();
	void AiStopAttackingPlayerTask();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAIPerceptionComponent* AiPerception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAISenseConfig_Hearing* HearingConfig;




	void SetHearingConfig();
	void SetSightConfig();

	AEnemyAIController();

	UFUNCTION()
		void PerceptionSenseUpdate(const TArray<AActor*>& actors);

	UFUNCTION()
		void PerceptionSenseUpdateTargeted(AActor* actor, FAIStimulus const stimuli);

	bool attackPlayer;
	void AiFollowReachedDestination();
	void BeginPlay() override;
	void OnPossess(APawn* const pawn) override;
	void LostSight();
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	void OnScaredCompleted();
	void EnableAggression();
	void RegenerateBravery();
	void EndRegenBravery();
	void StopAttacking();
	class AEnemyAI* enemy;
	bool aggressionStart;
	class AAgentPlayerClass* player;

private:

	bool aiHostile;
	FTimerHandle TimerHandle_Movement_Delay;
	FTimerHandle TimerHandle_AI_Scared;
	FTimerHandle TimerHandle_Attack_Delay;
	FTimerHandle TimerHandle_StandUp_Delay;
	FTimerHandle TimerHandle_LostSight;
	FTimerHandle TimerHandle_RegenBravery;
	FTimerHandle TimerHandle_AggressionDelay;
	float attackDelay;
	bool startRegen;
	TArray<AActor*> FoundActors;
	float aiBraveryCheck;
	
	FVector defensePosition;
	int sawFriendsDied;
	
	int damageStage;

	bool aiScared;

	bool reachedDestination;

};

