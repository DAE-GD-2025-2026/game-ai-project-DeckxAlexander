// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <memory>

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "FSMComponent.generated.h"

class ASteeringAgent;
namespace GameAI::FSM
{
	class State
	{
	public:
		State() = default;
		virtual ~State() = default;
		virtual void ExecuteBehaviour(ASteeringAgent* agent, ASteeringAgent* targetAgent = nullptr) = 0;
	};
	
	class TestState : public State
	{
	public:
		virtual ~TestState() = default;
		virtual void ExecuteBehaviour(ASteeringAgent* agent, ASteeringAgent* targetAgent = nullptr) override;
	};
	
	
	class ChaseState : public State
	{
	public:
		ChaseState() : State()
		{
			m_pPursuit = std::make_unique<Pursuit>();
		}
		virtual ~ChaseState() = default;
		virtual void ExecuteBehaviour(ASteeringAgent* agent, ASteeringAgent* targetAgent = nullptr) override;

	private:
		std::unique_ptr<Pursuit> m_pPursuit{};
	};
	
	class Transition
	{
	public:
		Transition(State* From, State* To, std::function<bool(UBlackboardComponent* bb)> EvalFunc) : m_FromState(From), m_ToState(To), m_TransitionFunction(EvalFunc){}
		
		State* CheckTransition(UBlackboardComponent* bb)
		{
			
			if (m_TransitionFunction(bb)) return m_ToState;
			return m_FromState;
		}
		
		State* GetFrom()
		{
			return m_FromState;
		}
	private:
		State* m_FromState;
		State* m_ToState;
		std::function<bool(UBlackboardComponent* bb)> m_TransitionFunction;
	};
	

}

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEAIPROG_API UFSMComponent : public UBrainComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFSMComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void StartLogic() override;
	virtual void StopLogic(const FString& Reason) override;
	
	virtual bool IsRunning() const override; 
	
	GameAI::FSM::State* AddState(std::unique_ptr<GameAI::FSM::State>&& NewState);
	void AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To, std::function<bool(UBlackboardComponent* bb)> EvalFunc);
	
	const std::vector<std::unique_ptr<GameAI::FSM::State>>& GetStates() const {return m_States;}
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	GameAI::FSM::State* m_CurrentState = nullptr;
	std::vector<std::unique_ptr<GameAI::FSM::State>> m_States;
	std::vector<std::unique_ptr<GameAI::FSM::Transition>> m_Transitions;
	
	std::vector<GameAI::FSM::Transition*> FindTransitionsFrom(GameAI::FSM::State* state);
	bool bIsRunning{false};
	ASteeringAgent* m_CurrentAgent{nullptr};
	ASteeringAgent* m_ThiefAgent{nullptr};
	
	
	void CheckThiefVisibility();
};
