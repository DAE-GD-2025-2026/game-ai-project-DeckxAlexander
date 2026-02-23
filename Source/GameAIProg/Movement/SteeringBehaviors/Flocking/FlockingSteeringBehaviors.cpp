#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steeringOutput{};
	//pAgent.SetMaxLinearSpeed(300.f);
	
	FVector2D targetPos = pFlock->GetAverageNeighborPos();

	steeringOutput.LinearVelocity = targetPos - pAgent.GetPosition();
	steeringOutput.LinearVelocity.Normalize();
	
	
	return steeringOutput;
}

//*********************
//SEPARATION (FLOCKING)

SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steeringOutput{};
	for (int index{}; index < pFlock->GetNrOfNeighbors(); ++index)
	{
		ASteeringAgent* pNeighbor = pFlock->GetNeighbors()[index];
		//if (!pNeighbor) continue;
		float distance = FVector2D::Distance(pNeighbor->GetPosition(), pAgent.GetPosition());
		
		FVector2D velocity{};
		velocity = pAgent.GetPosition() - pNeighbor->GetPosition();
		velocity /= distance;
		velocity /= distance;
		
		steeringOutput.LinearVelocity += velocity;
	}
	steeringOutput.LinearVelocity.Normalize();
	return steeringOutput;
}



//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput Alignment::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput steeringOutput{};
	steeringOutput.LinearVelocity = pFlock->GetAverageNeighborVelocity(); 
	steeringOutput.LinearVelocity.Normalize();
	return steeringOutput;
}