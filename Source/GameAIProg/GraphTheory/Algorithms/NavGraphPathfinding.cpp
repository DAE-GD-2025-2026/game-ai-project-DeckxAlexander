#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{

	std::vector<FVector2D> finalPath{};
	const TriPolygon* pPoly = pNavGraph->GetNavPolygon();

	auto* startTri = pPoly->GetTriangleAtPosition(startPos, true);
	auto* endTri   = pPoly->GetTriangleAtPosition(endPos, true);
	

	if (!startTri || !endTri)
		return {};
	
	if (*startTri == *endTri)
	{
		return { startPos, endPos };
	}
	auto pClonedGraph = pNavGraph->Clone();
	NavGraph* pGraph = pClonedGraph.get();
	
	int startNodeId = pGraph->AddNode(std::make_unique<NavGraphNode>(FVector2D(startPos), -1));

	for (const auto& edge : startTri->GetEdges())
	{
		auto edgeIdxOpt = pPoly->FindEdgeIndex(edge);
		if (!edgeIdxOpt.has_value()) continue;
		int edgeIdx = edgeIdxOpt.value();
		int nodeId = pGraph->GetNodeIdFromEdgeIndex(edgeIdx);
		if (nodeId != Graphs::InvalidNodeId)
		{
			FVector2D a = pGraph->GetNodes()[startNodeId]->GetPosition();
			FVector2D b = pGraph->GetNodes()[nodeId]->GetPosition();
			float cost = FVector2D::Distance(a, b);
			pGraph->AddConnection(startNodeId, nodeId);
			pGraph->FindConnection(startNodeId, nodeId)->SetWeight(cost);
			
		}
	}

	int endNodeId = pGraph->AddNode(std::make_unique<NavGraphNode>(FVector2D(endPos), -1));

	for (const auto& edge : endTri->GetEdges())
	{
		auto edgeIdxOpt = pPoly->FindEdgeIndex(edge);
		if (!edgeIdxOpt.has_value()) continue;
		int edgeIdx = edgeIdxOpt.value();
		int nodeId = pGraph->GetNodeIdFromEdgeIndex(edgeIdx);
		if (nodeId != Graphs::InvalidNodeId)
		{
			FVector2D a = pGraph->GetNodes()[endNodeId]->GetPosition();
			FVector2D b = pGraph->GetNodes()[nodeId]->GetPosition();
			float cost = FVector2D::Distance(a, b);
			pGraph->AddConnection(endNodeId, nodeId);
			pGraph->FindConnection(endNodeId, nodeId)->SetWeight(cost);
			
		}
	}
	
	AStar astar(pGraph, HeuristicFunctions::Euclidean);

	Node* pStart = pGraph->GetNode(startNodeId).get();
	Node* pEnd   = pGraph->GetNode(endNodeId).get();

	std::vector<Node*> nodePath = astar.FindPath(pStart, pEnd);
	for (Node* pNode : nodePath)
	{
		FVector2D pos = pNode->GetPosition();

		finalPath.emplace_back(pos);
		debugNodePositions.emplace_back(pos);
	}
	

	//Debug Visualisation

	// Extra: Run optimiser on new graph (First check if everything works without SSFA!)
	debugPortals = SSFA::FindPortals(nodePath, *pNavGraph->GetNavPolygon());
	finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	return finalPath;	

}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}