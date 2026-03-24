#include "NavGraph.h"

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	for (auto edge : pNavPoly.get()->GetEdges())
	{
		int index = pNavPoly.get()->FindEdgeIndex(edge).value();
		int count = 0;
		for (const auto& tri : pNavPoly->GetTriangles())
		{
			if (tri.HasEdge(edge))
				count++;
		}
		
		FVector p1 = edge.GetP1(*pNavPoly);
		FVector p2 = edge.GetP2(*pNavPoly);

		FVector middle = (p1 + p2) * 0.5f;

		int nodeId = AddNode(std::make_unique<NavGraphNode>(FVector2D(middle), index));
		
		
		
	}
	
	for (const auto& tri : pNavPoly.get()->GetTriangles())
	{
		std::vector<int> nodeIds;

		// Get triangle edges
		auto triEdges = tri.GetEdges();

		for (const auto& edge : triEdges)
		{
			// Convert edge → edgeIdx
			auto edgeIdxOpt = pNavPoly->FindEdgeIndex(edge);

			if (!edgeIdxOpt.has_value())
				continue;

			int edgeIdx = edgeIdxOpt.value();

			// Get node from edgeIdx
			int nodeId = GetNodeIdFromEdgeIndex(edgeIdx);

			if (nodeId != Graphs::InvalidNodeId)
			{
				nodeIds.push_back(nodeId);
			}
		}
		
		if (nodeIds.size() == 2)
		{
			int a = nodeIds[0];
			int b = nodeIds[1];

			float cost = FVector::Dist(
				FVector(Nodes[a]->GetPosition(),0),
				FVector(Nodes[b]->GetPosition(),0)
			);

			AddConnection(a, b);
			FindConnection(a, b)->SetWeight(cost);
		}

		// If 3 nodes → fully connect
		else if (nodeIds.size() == 3)
		{
			for (int i = 0; i < 3; ++i)
			{
				for (int j = i + 1; j < 3; ++j)
				{
					int a = nodeIds[i];
					int b = nodeIds[j];

					float cost = FVector::Dist(
						FVector(Nodes[a]->GetPosition(),0),
						FVector(Nodes[b]->GetPosition(),0)
					);

					
					AddConnection(a, b);
					FindConnection(a, b)->SetWeight(cost);
				}
			}
		}
	}
	
	
	//1. Go over all the edges of the navigation mesh and create nodes
			// Create node here

	//2. Create connections now that every node is created	
		//2 valid nodes -> 1 connection
		//3 valid nodes -> 3 connections
		
	//3. Set the connections cost to the actual distance
}
