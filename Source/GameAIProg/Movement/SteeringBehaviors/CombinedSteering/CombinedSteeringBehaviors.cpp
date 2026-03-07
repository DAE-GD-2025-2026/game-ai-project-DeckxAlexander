
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
	// TODO: Calculate the weighted average steeringbehavior
	
	float val{};
	float numerator{float(WeightedBehaviors.size())};
	
	for (const WeightedBehavior& behavior : WeightedBehaviors)
	{
		val += behavior.Weight;
		
		
	}
	float weightedAverage = val / numerator;
	Agent.SetMaxLinearSpeed(400.f);
	
	FVector2D finalVelocity{};
	for (const WeightedBehavior& behavior : WeightedBehaviors)
	{
		finalVelocity += behavior.Weight/val * behavior.pBehavior->CalculateSteering(DeltaT, Agent).LinearVelocity;
	}
	
	BlendedSteering.LinearVelocity = finalVelocity;
	BlendedSteering.LinearVelocity.Normalize();
	

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


void BlendedSteering::SetTarget(const FTargetData& NewTarget)
{
	for (const WeightedBehavior& behavior : WeightedBehaviors)
	{
		behavior.pBehavior->SetTarget(Target);
		
		
	}
}

SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);
		

		if (Steering.IsValid)
			break;
	}
	
	return Steering;
}

void PrioritySteering::SetTarget(const FTargetData& NewTarget)
{
	for (ISteeringBehavior* pBehavior : m_PriorityBehaviors)
	{
		pBehavior->SetTarget(Target);
	}
}
