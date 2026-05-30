// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_FSM.h"

#include "FSMComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DecisionMaking/GameAIController.h"


// Sets default values
ALevel_FSM::ALevel_FSM()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_FSM::BeginPlay()
{
	Super::BeginPlay();
	
	Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Agent->SetDebugRenderingEnabled(false);
	
	
	auto ThiefAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
		FVector{700,0,90}, FRotator::ZeroRotator);
	

	
	//TODO
	if (AGameAIController* AIController = Cast<AGameAIController>(Agent->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			auto bb = AIController->GetBlackboardComponent();
			auto TestState = FSM->AddState(std::make_unique<GameAI::FSM::TestState>());
			auto TestState2 = FSM->AddState(std::make_unique<GameAI::FSM::ChaseState>());
			auto lambda = [](UBlackboardComponent* blackboard) 
			{
				return blackboard->GetValueAsBool(FName("TargetVisible"));
			};
			
			FSM->AddTransition(FSM->GetStates()[0].get(), FSM->GetStates()[1].get(), lambda);
			FSM->AddTransition(FSM->GetStates()[1].get(), FSM->GetStates()[0].get(), 
			[](UBlackboardComponent* blackboard) {
				return !blackboard->GetValueAsBool(FName("TargetVisible"));
			});
			

			bb->SetValueAsObject(FName("SteeringAgent"),Agent );
			bb->SetValueAsObject(FName("ThiefAgent"),ThiefAgent );
			AIController->RunFiniteStateMachine();
		}
	}
	
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

