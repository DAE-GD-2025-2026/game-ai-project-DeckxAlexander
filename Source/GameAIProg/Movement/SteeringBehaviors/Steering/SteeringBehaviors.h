#pragma once

#include <Movement/SteeringBehaviors/SteeringHelpers.h>
#include "Kismet/KismetMathLibrary.h"

class ASteeringAgent;

// SteeringBehavior base, all steering behaviors should derive from this.
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	// Override to implement your own behavior
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) = 0;

	virtual void SetTarget(const FTargetData& NewTarget) { Target = NewTarget; }
	
	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	FTargetData Target;
};

// Your own SteeringBehaviors should follow here...

class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Arrive : public ISteeringBehavior
{
public:
	Arrive() = default;
	virtual ~Arrive() override = default;


	float m_TargetRadius = 200.f;
	float m_SlowRadius{ 1000.f };

	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	void SetTargetRadius(float radius) {
m_TargetRadius = radius;
	}
	
	void SetSlowRadius(float radius) {
		m_SlowRadius = radius;
	}
};

class Face : public ISteeringBehavior
{
public:
	Face() = default;
	virtual ~Face() override = default;

	float m_MaxTurnSpeed {360.f};
	
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	void SetMaxTurnSpeed(float speed) {
		m_MaxTurnSpeed = speed;
	}
	
};

class Pursuit : public ISteeringBehavior
{
public:
	Pursuit() = default;
	virtual ~Pursuit() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Evade : public ISteeringBehavior
{
public:
	Evade() = default;
	virtual ~Evade() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	
	void SetEvadeRadius(float radius) {m_EvadeRadius = radius;};
	
protected:
	float m_EvadeRadius = 200.f;
};

class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
	
	void SetOffsetDistance(float speed) {
		m_OffsetDistance = speed;
	}
	void SetRadius(float speed) {
		m_Radius = speed;
	}
	void SetMaxAngleChange(float speed) {
		m_MaxAngleChange = FMath::DegreesToRadians(speed);
	}

	
protected:
	float m_OffsetDistance = 150.f;
	float m_Radius = 75.f;
	float m_MaxAngleChange = FMath::DegreesToRadians(45.f);
	float m_WanderAngle = 0.f;
};

