#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "DrawDebugHelpers.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)
SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent) 
{ 
	SteeringOutput steeringOutput{};
	Agent.SetMaxLinearSpeed(300.f);

	steeringOutput.LinearVelocity = Target.Position - Agent.GetPosition();
	steeringOutput.LinearVelocity.Normalize();


	//Debug Render 
	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f};
	FVector TargetPosition{ Target.Position.X , Target.Position.Y, 0.f};
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f};
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f};
	FVector2D Forward2D(
		Agent.GetActorForwardVector().X,
		Agent.GetActorForwardVector().Y
	);
	Forward2D.Normalize();

	float Angle = FMath::Atan2(Forward2D ^ steeringOutput.LinearVelocity, FVector2D::DotProduct(Forward2D, steeringOutput.LinearVelocity));
	FVector AngularDebug = Agent.GetActorRightVector() * Angle;

	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart+AgentVelocityEnd * 0.5f,  FColor::Green);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart+ AgentForwardEnd * 100.f,  FColor::Magenta);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AngularDebug * steeringOutput.LinearVelocity.Length() * 100.f, FColor::Cyan);
	DrawDebugCircle(Agent.GetWorld(), TargetPosition, 10.f, 32, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return steeringOutput;


}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput steeringOutput{};
	Agent.SetMaxLinearSpeed(300.f);

	steeringOutput.LinearVelocity = Agent.GetPosition() - Target.Position;
	steeringOutput.LinearVelocity.Normalize();

	//Debug Render 
	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f };
	FVector TargetPosition{ Target.Position.X , Target.Position.Y, 0.f };
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f };
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f };
	FVector2D Forward2D(
		Agent.GetActorForwardVector().X,
		Agent.GetActorForwardVector().Y
	);
	Forward2D.Normalize();

	float Angle = FMath::Atan2(Forward2D ^ steeringOutput.LinearVelocity, FVector2D::DotProduct(Forward2D, steeringOutput.LinearVelocity));
	FVector AngularDebug = Agent.GetActorRightVector() * Angle;

	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentVelocityEnd * 0.5f, FColor::Green);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentForwardEnd * 100.f, FColor::Magenta);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AngularDebug * steeringOutput.LinearVelocity.Length() * 100.f, FColor::Cyan);
	DrawDebugCircle(Agent.GetWorld(), TargetPosition, 10.f, 32, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	

	return steeringOutput;


}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{

	float TargetRadius{ 100.f };
	float SlowRadius{ 1000.f };
	Agent.SetMaxLinearSpeed(300.f);


	SteeringOutput steeringOutput{};
	float speed = Agent.GetMaxLinearSpeed();

	FVector2D toTarget = Target.Position - Agent.GetPosition();
	float distance = toTarget.Length();

	if (distance < TargetRadius)
	{
		steeringOutput.LinearVelocity = FVector2D::ZeroVector;
	}
	else 
	{
		if (distance < SlowRadius)
		{
			speed *= (distance / (SlowRadius+TargetRadius));
			Agent.SetMaxLinearSpeed(speed);

		}
		toTarget.Normalize();
		steeringOutput.LinearVelocity = toTarget;
	}
	
	//Debug
	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f };
	FVector TargetPosition{ Target.Position.X , Target.Position.Y, 0.f };
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f };
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f };
	FVector2D Forward2D(Agent.GetActorForwardVector().X,Agent.GetActorForwardVector().Y);
	Forward2D.Normalize();

	float Angle =FMath::Atan2(Forward2D ^ steeringOutput.LinearVelocity,FVector2D::DotProduct(Forward2D, steeringOutput.LinearVelocity));

	FVector AngularDebug =Agent.GetActorRightVector() * Angle;
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentVelocityEnd * 0.5f, FColor::Green);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentForwardEnd * 100.f, FColor::Magenta);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AngularDebug * steeringOutput.LinearVelocity.Length() * 100.f, FColor::Cyan);
	DrawDebugCircle(Agent.GetWorld(), TargetPosition, 10.f, 32, FColor::Red, false,-1.f,0,5.f,FVector(1, 0, 0),  FVector(0, 1, 0),  false);
	DrawDebugCircle(Agent.GetWorld(), lineStart, 1000.f, 32, FColor::Blue, false,-1.f,0,1.f,FVector(1, 0, 0),  FVector(0, 1, 0),  false);
	DrawDebugCircle(Agent.GetWorld(), lineStart, TargetRadius, 32, FColor::Red, false, -1.f, 0, 1.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return steeringOutput;


}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Output{};
	Agent.SetMaxLinearSpeed(300.f);
	FVector2D ToTarget = Target.Position - Agent.GetPosition();
	ToTarget.Normalize();
	float DesiredYaw = FMath::RadiansToDegrees(
		FMath::Atan2(ToTarget.Y, ToTarget.X)
	);
	float CurrentYaw = Agent.GetActorRotation().Yaw;

	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw);
	float MaxTurnSpeed = 360.f; 
	Output.AngularVelocity = FMath::Clamp(DeltaYaw / DeltaT, -MaxTurnSpeed, MaxTurnSpeed);


	//Debug Render 
	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f };
	FVector TargetPosition{ Target.Position.X , Target.Position.Y, 0.f };
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f };
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f };
	FVector2D Forward2D(Agent.GetActorForwardVector().X,Agent.GetActorForwardVector().Y);
	Forward2D.Normalize();

	FVector AngularDebug =Agent.GetActorRightVector() * Output.AngularVelocity;

	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentForwardEnd * 100.f, FColor::Magenta);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AngularDebug * 0.2f, FColor::Cyan);
	DrawDebugCircle(Agent.GetWorld(), TargetPosition, 10.f, 32, FColor::Red, false,-1.f,0,5.f,FVector(1, 0, 0), FVector(0, 1, 0), false);

	return Output;


}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput SteeringOutput;
	Agent.SetMaxLinearSpeed(300.f);
	const float MaxSpeed = 500.f;

	FVector2D AgentPos = FVector2D(Agent.GetActorLocation());
	FVector2D TargetPos = Target.Position;
	FVector2D ToTarget = TargetPos - AgentPos;
	float Distance = Target.LinearVelocity.Size();

	float AgentSpeed = Agent.GetMaxLinearSpeed();
	float PredictionTime = 0.f;

	if (AgentSpeed > 0.f)
	{
		PredictionTime = Distance / AgentSpeed;
	}
	FVector2D PredictedPosition = TargetPos + FVector2D(Target.LinearVelocity) * PredictionTime;
	FVector2D DesiredVelocity = (PredictedPosition - AgentPos).GetSafeNormal() * MaxSpeed;

	SteeringOutput.LinearVelocity = DesiredVelocity;
	SteeringOutput.LinearVelocity.Normalize();

	//Debug Render 
	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f };
	FVector TargetPosition{ Target.Position.X , Target.Position.Y, 0.f };
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f };
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f };
	FVector2D Forward2D(Agent.GetActorForwardVector().X,Agent.GetActorForwardVector().Y);
	Forward2D.Normalize();

	float Angle =FMath::Atan2(Forward2D ^ SteeringOutput.LinearVelocity,FVector2D::DotProduct(Forward2D, SteeringOutput.LinearVelocity));

	FVector AngularDebug =Agent.GetActorRightVector() * Angle;

	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentVelocityEnd * 0.5f, FColor::Green);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentForwardEnd * 100.f, FColor::Magenta);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AngularDebug * SteeringOutput.LinearVelocity.Length() * 100.f, FColor::Cyan);
	DrawDebugCircle(Agent.GetWorld(), TargetPosition, 10.f, 32, FColor::Red, false,-1.f,0,5.f,FVector(1, 0, 0), FVector(0, 1, 0), false);

	return SteeringOutput;


}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput SteeringOutput;
	Agent.SetMaxLinearSpeed(300.f);
	const float MaxSpeed = 500.f;

	FVector2D AgentPos = FVector2D(Agent.GetActorLocation());
	FVector2D TargetPos = Target.Position;
	FVector2D ToTarget = TargetPos - AgentPos;
	float Distance = Target.LinearVelocity.Size();

	float AgentSpeed = Agent.GetMaxLinearSpeed();
	float PredictionTime = 0.f;

	if (AgentSpeed > 0.f)
	{
		PredictionTime = Distance / AgentSpeed;
	}

	FVector2D PredictedPosition = TargetPos + FVector2D(Target.LinearVelocity) * PredictionTime;
	FVector2D DesiredVelocity = (AgentPos - PredictedPosition).GetSafeNormal() * MaxSpeed;

	SteeringOutput.LinearVelocity = DesiredVelocity;

	//Debug Render 
	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f };
	FVector TargetPosition{ Target.Position.X , Target.Position.Y, 0.f };
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f };
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f };
	FVector2D Forward2D(
		Agent.GetActorForwardVector().X,
		Agent.GetActorForwardVector().Y
	);
	Forward2D.Normalize();

	float Angle = FMath::Atan2(Forward2D ^ SteeringOutput.LinearVelocity, FVector2D::DotProduct(Forward2D, SteeringOutput.LinearVelocity));

	FVector AngularDebug = Agent.GetActorRightVector() * Angle;

	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentVelocityEnd * 0.5f, FColor::Green);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentForwardEnd * 100.f, FColor::Magenta);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AngularDebug * SteeringOutput.LinearVelocity.Length() * 100.f, FColor::Cyan);
	DrawDebugCircle(Agent.GetWorld(), TargetPosition, 10.f, 32, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);

	return SteeringOutput;


}


SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput SteeringOutput;
	Agent.SetMaxLinearSpeed(300.);
	const float MaxSpeed = 500.f;
	const float CircleDistance = 150.f;
	const float CircleRadius = 75.f;

	FVector2D AgentPos = FVector2D(Agent.GetActorLocation());
	FVector2D Forward2D(
		Agent.GetActorForwardVector().X,
		Agent.GetActorForwardVector().Y
	);

	if (Forward2D.IsNearlyZero())
	{
		Forward2D = FVector2D(1.f, 0.f);
	}

	FVector2D CircleCenter = AgentPos + Forward2D * CircleDistance;
	float RandomAngle = FMath::FRandRange(0.f, 2.f * PI);
	FVector2D Displacement(
		FMath::Cos(RandomAngle),
		FMath::Sin(RandomAngle)
	);
	Displacement *= CircleRadius;
	FVector2D WanderTarget = CircleCenter + Displacement;
	FVector2D DesiredVelocity =
		(WanderTarget - AgentPos).GetSafeNormal() * MaxSpeed;
	SteeringOutput.LinearVelocity = DesiredVelocity;

	//Debug Render
	FVector lineStart{ Agent.GetPosition().X , Agent.GetPosition().Y, 0.f };
	FVector TargetPosition{ Target.Position.X , Target.Position.Y, 0.f };
	FVector AgentVelocityEnd{ Agent.GetVelocity().X  , Agent.GetVelocity().Y   , 0.f };
	FVector AgentForwardEnd{ Agent.GetActorForwardVector().X  , Agent.GetActorForwardVector().Y  , 0.f };
	Forward2D.Normalize();

	float Angle = FMath::Atan2(Forward2D ^ SteeringOutput.LinearVelocity, FVector2D::DotProduct(Forward2D, SteeringOutput.LinearVelocity));
	FVector AngularDebug = Agent.GetActorRightVector() * Angle;

	DrawDebugLine(Agent.GetWorld(), lineStart, FVector(WanderTarget.X, WanderTarget.Y, 0.f), FColor::Green);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AgentForwardEnd * 100.f, FColor::Magenta);
	DrawDebugLine(Agent.GetWorld(), lineStart, lineStart + AngularDebug * 300.f, FColor::Cyan);
	DrawDebugCircle(Agent.GetWorld(), TargetPosition, 10.f, 32, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
	DrawDebugCircle(Agent.GetWorld(), FVector(CircleCenter.X, CircleCenter.Y,0), CircleRadius, 32, FColor::Blue, false,-1.f,0,1.f,FVector(1, 0, 0),FVector(0, 1, 0),false);

	return SteeringOutput;



}