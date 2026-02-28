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
	
#ifndef GAMEAI_USE_SPACE_PARTITIONING
	Neighbors.SetNum(FlockSize);	
#else
	
	//float floatcellSize = WorldSize / static_cast<float>(NrOfCellsX);
	//UE_LOG(LogTemp, Warning, TEXT("Value: %f"), floatcellSize);
	pPartitionedSpace = std::make_unique<CellSpace>(pWorld, WorldSize*2, WorldSize*2, NrOfCellsX, NrOfCellsX, FlockSize);
	OldPositions.SetNum(FlockSize);
#endif
	NrOfNeighbors = 0;
	
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pSeparationBehavior = std::make_unique<Separation>(this);
	pVelMatchBehavior = std::make_unique<Alignment>(this);
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();
	
	pBlendedSteeringBehavior = std::make_unique<BlendedSteering>(std::vector<BlendedSteering::WeightedBehavior>{BlendedSteering::WeightedBehavior{pCohesionBehavior.get(),1.f}, BlendedSteering::WeightedBehavior{pSeparationBehavior.get(),1.f},
	BlendedSteering::WeightedBehavior{pVelMatchBehavior.get(),1.f}, BlendedSteering::WeightedBehavior{pSeekBehavior.get(),0.5f}, BlendedSteering::WeightedBehavior{pWanderBehavior.get(), 0.5f}});
	
	for (int i = 0; i < FlockSize; ++i)
	{
		FVector spawnPos = FVector(FMath::RandRange(-WorldSize, WorldSize),FMath::RandRange(-WorldSize, WorldSize),90.f);

		ASteeringAgent* pAgent = pWorld->SpawnActor<ASteeringAgent>(AgentClass, spawnPos, FRotator::ZeroRotator);
		
		while (!pAgent)
		{
			spawnPos = FVector(FMath::RandRange(-WorldSize, WorldSize),FMath::RandRange(-WorldSize, WorldSize),90.f);
			pAgent = pWorld->SpawnActor<ASteeringAgent>(AgentClass, spawnPos, FRotator::ZeroRotator);
		}
		
		if (pAgent)
		{
			pAgent->SetActorTickEnabled(false);
			Agents[i] = pAgent;
			
#ifdef GAMEAI_USE_SPACE_PARTITIONING
			pPartitionedSpace.get()->AddAgent(pAgent);
			OldPositions[i] = pAgent->GetPosition();
#endif
		}
	}
	
	
 // TODO: initialize the flock and the memory pool
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
	

	
}

void Flock::Tick(float DeltaTime)
{

#ifndef GAMEAI_USE_SPACE_PARTITIONING
		for (ASteeringAgent* Agent : Agents)
		{
			if (!Agent) continue;
			RegisterNeighbors(Agent);
			Agent->SetSteeringBehavior(pBlendedSteeringBehavior.get());
			Agent->Tick(DeltaTime);
		}
#else
		for (int i = 0; i < Agents.Num(); ++i)
		{
			if (!Agents[i]) continue;
			pPartitionedSpace->UpdateAgentCell(Agents[i], OldPositions[i]);
			pPartitionedSpace->RegisterNeighbors(Agents[i], NeighborhoodRadius);
			Agents[i]->SetSteeringBehavior(pBlendedSteeringBehavior.get());
			Agents[i]->Tick(DeltaTime);
			OldPositions[i] = Agents[i]->GetPosition();
		}
#endif
		

		
	
	
 // TODO: update the flock
 // TODO: for every agent:
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
  // TODO: trim the agent to the world
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
	RenderNeighborhood();
	pPartitionedSpace->RenderCells();
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

		ImGuiHelpers::ImGuiSliderFloatWithSetter("Cohesion",
		pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			 [this](float InVal) { pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Separation",
		pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Alignment",
		pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[2].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[2].Weight = InVal; }, "%.2f");
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
		pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[3].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[3].Weight = InVal; }, "%.2f");
		
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[4].Weight, 0.f, 1.f,
		[this](float InVal) { pBlendedSteeringBehavior->GetWeightedBehaviorsRef()[4].Weight = InVal; }, "%.2f");
		
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
	DrawDebugCircle(First->GetWorld(), First->GetActorLocation(), 200.f, 32, FColor::Red, false, -1.f, 0, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
 // TODO: Implement
	NrOfNeighbors = 0;
	for (ASteeringAgent* Other : Agents)
	{
		if (Other == pAgent || !Other)
			continue;

		float Dist = FVector2D::Distance(
			pAgent->GetPosition(),
			Other->GetPosition()
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

	

#ifndef GAMEAI_USE_SPACE_PARTITIONING
		for (int i = 0; i < NrOfNeighbors; ++i)
		{
			avgPosition += Neighbors[i]->GetPosition();
		}
		return avgPosition / NrOfNeighbors;
#else
	for (int i = 0; i < pPartitionedSpace->GetNrOfNeighbors(); ++i)
	{
		avgPosition += pPartitionedSpace->GetNeighbors()[i]->GetPosition();
	}
	return avgPosition / pPartitionedSpace->GetNrOfNeighbors();
#endif
		
	

	
	
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

	
	
	
#ifndef GAMEAI_USE_SPACE_PARTITIONING
	if (NrOfNeighbors == 0) return avgVelocity;
	for (int i = 0; i < NrOfNeighbors; ++i)
	{
		avgVelocity += FVector2D(Neighbors[i]->GetVelocity());
	}
	return avgVelocity / NrOfNeighbors;
#else
	if (pPartitionedSpace->GetNrOfNeighbors() == 0) return avgVelocity;
	for (int i = 0; i < pPartitionedSpace->GetNrOfNeighbors(); ++i)
	{
		avgVelocity += FVector2D(pPartitionedSpace->GetNeighbors()[i]->GetVelocity());
	}
	return avgVelocity / pPartitionedSpace->GetNrOfNeighbors();
#endif
	

	
	
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
	pBlendedSteeringBehavior->SetTarget(Target);
}

