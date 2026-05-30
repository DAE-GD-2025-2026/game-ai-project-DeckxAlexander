// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"



void GameAI::FSM::TestState::ExecuteBehaviour(ASteeringAgent* agent, ASteeringAgent* targetAgent) 
{
	DrawDebugCircle(agent->GetWorld(), agent->GetActorLocation(), 10.f, 
		32, FColor::Red, false,-1.f,0,5.f,
		FVector(1, 0, 0),  FVector(0, 1, 0),  false);
}

void GameAI::FSM::ChaseState::ExecuteBehaviour(ASteeringAgent* agent, ASteeringAgent* targetAgent) 
{
	if (targetAgent == nullptr) return;
	FTargetData Target{targetAgent->GetPosition(),targetAgent->GetRotation(),
		targetAgent->GetLinearVelocity(), targetAgent->GetAngularVelocity() };
	
	m_pPursuit->SetTarget(Target);
	agent->SetSteeringBehavior(m_pPursuit.get());
	
	
	
}

// Sets default values for this component's properties
UFSMComponent::UFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


GameAI::FSM::State* UFSMComponent::AddState(std::unique_ptr<GameAI::FSM::State>&& NewState)
{
	m_States.emplace_back(std::move(NewState));
	return m_States.back().get();
}

void UFSMComponent::AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To, std::function<bool(UBlackboardComponent* bb)> EvalFunc)
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
		
		if (t->GetFrom() == state)
		{
			Transitions.push_back(t.get());
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
	
	if (bHit &&  hitResult.GetActor() != m_ThiefAgent )
	{
		GetBlackboardComponent()->SetValueAsBool(FName("TargetVisible"), false);
	}
	else GetBlackboardComponent()->SetValueAsBool(FName("TargetVisible"), true);
	
	
	
}


// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bIsRunning) return;
	
	m_CurrentState->ExecuteBehaviour(m_CurrentAgent, m_ThiefAgent);
	CheckThiefVisibility();
	for (auto Transition : FindTransitionsFrom(m_CurrentState))
	{
		m_CurrentState = Transition->CheckTransition(GetBlackboardComponent());
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
	object = bb->GetValueAsObject(FName("ThiefAgent"));
	m_ThiefAgent = Cast<ASteeringAgent>(object);

}

void UFSMComponent::StopLogic(const FString& Reason)
{
	bIsRunning = false;
}

bool UFSMComponent::IsRunning() const
{
	return bIsRunning;
}

