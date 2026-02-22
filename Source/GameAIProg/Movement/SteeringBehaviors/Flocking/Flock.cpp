#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"


Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	Agents.SetNum(FlockSize);
	Neighbors.SetNum(FlockSize);	
	NrOfNeighbors = 0;
	for (int i = 0; i < FlockSize; ++i)
	{
		FVector spawnPos = FVector(
			FMath::RandRange(-WorldSize, WorldSize),
			FMath::RandRange(-WorldSize, WorldSize),
			0.f
		);

		ASteeringAgent* pAgent =
			pWorld->SpawnActor<ASteeringAgent>(AgentClass, spawnPos, FRotator::ZeroRotator);

		Agents[i] = pAgent;
	}
	
	
 // TODO: initialize the flock and the memory pool
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
	
	for (ASteeringAgent* agent: Agents)
	{
		agent->Destroy();
	}
	
}

void Flock::Tick(float DeltaTime)
{
	for (ASteeringAgent* Agent : Agents)
	{
		RegisterNeighbors(Agent);

		//Update agent (steering uses Neighbors + NrOfNeighbors)
		
		FVector Pos = Agent->GetActorLocation();

		float WorldLimit = 1000.f; // or WorldSize
		if (Pos.X > WorldLimit) Pos.X = -WorldLimit;
		if (Pos.X < -WorldLimit) Pos.X = WorldLimit;
		if (Pos.Y > WorldLimit) Pos.Y = -WorldLimit;
		if (Pos.Y < -WorldLimit) Pos.Y = WorldLimit;

		Agent->SetActorLocation(Pos);
	}
	
 // TODO: update the flock
 // TODO: for every agent:
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
  // TODO: trim the agent to the world
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

  // TODO: implement ImGUI sliders for steering behavior weights here
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
	
	if (Agents.Num() == 0)
		return;

	ASteeringAgent* First = Agents[0];
	DrawDebugCircle(pWorld,First->GetActorLocation(),NeighborhoodRadius,32,FColor::Green,false,-1.f,0,1.f,FVector(1,0,0),FVector(0,1,0),false);
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		DrawDebugLine(pWorld,First->GetActorLocation(),Neighbors[i]->GetActorLocation(),FColor::Red,false,-1.f,0,1.f);
	}
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
 // TODO: Implement
	NrOfNeighbors = 0;
	for (ASteeringAgent* Other : Agents)
	{
		if (Other == pAgent)
			continue;

		float Dist = FVector::Dist(
			pAgent->GetActorLocation(),
			Other->GetActorLocation()
		);

		if (Dist < NeighborhoodRadius)
		{
			if (NrOfNeighbors < Neighbors.Num())
			{
				Neighbors[NrOfNeighbors] = Other; 
				++NrOfNeighbors;
			}
		}
	}
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;
	
	if (NrOfNeighbors == 0) return avgPosition;
	
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgPosition += FVector2D(Neighbors[i]->GetActorLocation());
	}

	return avgPosition / NrOfNeighbors;
	
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

	if (NrOfNeighbors == 0) return avgVelocity;
	
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgVelocity += FVector2D(Neighbors[i]->GetVelocity());
	}

	return avgVelocity / NrOfNeighbors;
	
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
}

