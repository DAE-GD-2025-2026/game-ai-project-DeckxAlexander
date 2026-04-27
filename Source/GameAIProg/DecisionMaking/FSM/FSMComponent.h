// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <memory>

#include "CoreMinimal.h"
#include "BrainComponent.h"
#include "FSMComponent.generated.h"
class ASteeringAgent;
namespace GameAI::FSM
{
	class State
	{
	public:
		State() = default;
		virtual void ExecuteBehaviour(ASteeringAgent* agent) = 0;
	};
	
	class TestState : public State
	{
	public:
		virtual void ExecuteBehaviour(ASteeringAgent* agent);
	};
	
	class Transition
	{
	public:
		Transition(State* From, State* To, std::function<bool()> EvalFunc){}
		State* CheckTransition()
		{
			if (m_TransitionFunction) return m_ToState;
			return m_FromState;
		}
		
		State* GetFrom() { return m_FromState; }
	private:
		State* m_FromState;
		State* m_ToState;
		std::function<bool()> m_TransitionFunction;
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
	
	void AddState(std::unique_ptr<GameAI::FSM::State>&& NewState);
	void AddTransition(GameAI::FSM::State* From, GameAI::FSM::State* To, std::function<bool()> EvalFunc);
		
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
