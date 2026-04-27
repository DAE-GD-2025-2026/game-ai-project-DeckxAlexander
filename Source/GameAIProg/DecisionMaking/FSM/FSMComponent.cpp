// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"


void GameAI::FSM::TestState::ExecuteBehaviour(ASteeringAgent* agent)
{
	DrawDebugCircle(agent->GetWorld(), agent->GetActorLocation(), 10.f, 
		32, FColor::Red, false,-1.f,0,5.f,
		FVector(1, 0, 0),  FVector(0, 1, 0),  false);
}

// Sets default values for this component's properties
UFSMComponent::UFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


void UFSMComponent::AddState(std::unique_ptr<GameAI::FSM::State>&& NewState)
{
	m_States.emplace_back(std::move(NewState));
}

void UFSMComponent::AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To, std::function<bool()> EvalFunc)
{
	m_Transitions.emplace_back(std::make_unique<GameAI::FSM::Transition>(From, To, EvalFunc));
}

// Called when the game starts
void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();
}

std::vector<GameAI::FSM::Transition*> UFSMComponent::FindTransitionsFrom(GameAI::FSM::State* state)
{
	std::vector<GameAI::FSM::Transition*> Transitions;
	for (auto& t : m_Transitions)
	{
		auto tptr = t.get();
		if (tptr->GetFrom() == state)
		{
			Transitions.push_back(tptr);
		}
	}
	return Transitions;
}

void UFSMComponent::CheckThiefVisibility()
{
	if (!m_CurrentAgent||!m_ThiefAgent) return;
	
	FVector startLocation = m_CurrentAgent->GetActorLocation();
	FVector EndLocation = m_ThiefAgent->GetActorLocation();
	
	FCollisionQueryParams Params;
	
	FHitResult hitResult;
	
	Params.AddIgnoredActor(m_CurrentAgent); 
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult,startLocation,EndLocation,ECC_Visibility,Params);
	
	if (bHit)
	{
		if (hitResult.GetActor() == m_ThiefAgent)
		{
			GetBlackboardComponent()->SetValueAsBool(FName("TargetVisible"), true);
		} else GetBlackboardComponent()->SetValueAsBool(FName("TargetVisible"), false);
	}
	
	
	
}


// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsRunning) return;
	m_CurrentState->ExecuteBehaviour(dynamic_cast<ASteeringAgent*>(m_CurrentAgent));
	CheckThiefVisibility();
	for (auto Transition : FindTransitionsFrom(m_CurrentState))
	{
		m_CurrentState = Transition->CheckTransition();
	}
}

void UFSMComponent::StartLogic()
{
	Super::StartLogic();
	bIsRunning = true;
	m_CurrentState = m_States[0].get();
	auto bb = GetBlackboardComponent();
	auto object = bb->GetValueAsObject(FName("SteeringAgent"));
	m_CurrentAgent = Cast<ASteeringAgent>(object);

}

void UFSMComponent::StopLogic(const FString& Reason)
{
	bIsRunning = false;
}

bool UFSMComponent::IsRunning() const
{
	return bIsRunning;
}

