
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
	// TODO: Calculate the weighted average steeringbehavior
	
	float val{};
	float numerator{float(WeightedBehaviors.size())};
	
	for (WeightedBehavior const& behavior : WeightedBehaviors)
	{
		behavior.pBehavior->SetTarget(Target);
		val += behavior.Weight;
		
		
	}
	float weightedAverage = val / numerator;
	Agent.SetMaxLinearSpeed(400.f);
	
	FVector2D finalVelocity{};
	for (WeightedBehavior const& behavior : WeightedBehaviors)
	{
		finalVelocity += behavior.Weight/val * behavior.pBehavior->CalculateSteering(DeltaT, Agent).LinearVelocity;
	}
	
	BlendedSteering.LinearVelocity = finalVelocity;
	BlendedSteering.LinearVelocity.Normalize();
	
	
	// TODO: Add debug drawing

	return BlendedSteering;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = find_if(WeightedBehaviors.begin(),
		WeightedBehaviors.end(),
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if(it!= WeightedBehaviors.end())
		return &it->Weight;
	
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		//pBehavior->SetTarget(Target);
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);
		

		if (Steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return Steering;
}