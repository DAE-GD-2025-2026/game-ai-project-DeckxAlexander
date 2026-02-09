#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "DrawDebugHelpers.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)
SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent) 
{ 
	SteeringOutput steeringOutput{};

	steeringOutput.LinearVelocity = Target.Position - Agent.GetPosition();
	steeringOutput.LinearVelocity.Normalize();


	//Debug Render ?

	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f};
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f};
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f};
	//FVector AgentAngularEnd{ Agent.GetAngularVelocity(), Agent.GetAngularVelocity() * Agent.GetActorRotation() , 0.f};
	
	//Agent.SetMaxLinearSpeed(500.f);
	

	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart+AgentVelocityEnd,  FColor::Green);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart+ AgentForwardEnd * 100.f,  FColor::Magenta);
	//DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentAngularEnd * AgentVelocityEnd.Length() , FColor::Cyan);


	


	

	return steeringOutput;


}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steeringOutput{};

	steeringOutput.LinearVelocity = Agent.GetPosition() - Target.Position;
	steeringOutput.LinearVelocity.Normalize();

	//Debug Render ?

	return steeringOutput;


}