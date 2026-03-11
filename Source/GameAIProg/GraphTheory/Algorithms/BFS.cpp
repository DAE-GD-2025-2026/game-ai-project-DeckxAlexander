#include "BFS.h"

#include <map>
#include <queue>

#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: pGraph(pGraph)
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const
{
	std::vector<Node*> Path;
	
	TQueue<Node*> OpenList;
	TSet<Node*> ClosedList;
	TMap<Node*, Node*> Parent;

	// Start node
	OpenList.Enqueue(pStartNode);
	ClosedList.Add(pStartNode);

	Node* CurrentNode = nullptr;

	while (OpenList.Dequeue(CurrentNode))
	{
		if (CurrentNode == pDestinationNode)
		{
			// Backtracking
			Node* Step = pDestinationNode;

			while (Step != nullptr)
			{
				Path.push_back(Step);
				
				if (!Parent.Contains(Step)) break;

				Step = Parent[Step];
			}

			std::reverse(Path.begin(), Path.end());
			return Path;
		}

		// Check neighbors
		for (Connection* NeighborConnection : pGraph->FindConnectionsFrom(CurrentNode->GetId()))
		{
			Node* Neighbor = pGraph->GetActiveNodes()[NeighborConnection->GetToId()];
			if (!ClosedList.Contains(Neighbor))
			{
				OpenList.Enqueue(Neighbor);
				ClosedList.Add(Neighbor);

				// Store previous node
				Parent.Add(Neighbor, CurrentNode);
			}
		}
	}

	return Path;
}
