#include "AStar.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	std::vector<Node*> path{};
	
	std::vector<NodeRecord> openList;
	std::vector<NodeRecord> closedList;
	NodeRecord currentNodeRecord{};
	NodeRecord startRecord{pStartNode, nullptr, 
		0.f,GetHeuristicCost(pStartNode, pGoalNode)};
	
	auto nodes = pGraph->GetActiveNodes();
	
	openList.push_back(startRecord);
	
	while (!openList.empty())
	{
		auto RecordIt = std::min_element(openList.begin(), openList.end());
		currentNodeRecord = *RecordIt;
		if (currentNodeRecord.pNode == pGoalNode)
		{
			while (currentNodeRecord.pNode != pStartNode)
			{
				path.push_back(currentNodeRecord.pNode);
				if (currentNodeRecord.pConnection == nullptr)
					break;
				Node* pFromNode = nodes[currentNodeRecord.pConnection->GetFromId()];

				auto it = std::find_if(
					closedList.begin(),
					closedList.end(),
					[pFromNode](const NodeRecord& record)
					{
						return record.pNode == pFromNode;
					});

				if (it == closedList.end())
					break;

				currentNodeRecord = *it;
			}
			
			path.push_back(pStartNode);
			std::reverse(path.begin(), path.end());
			
			return path;
		}
		
		
		for (Connection* connection : pGraph->FindConnectionsFrom(currentNodeRecord.pNode->GetId()))
		{
			Node* pNextNode = nodes[connection->GetToId()];
			float gCost = currentNodeRecord.costSoFar + connection->GetWeight();
			auto closedListNode = std::find_if(closedList.begin(),closedList.end(),[pNextNode](const NodeRecord& record){return record.pNode == pNextNode;});
			if (closedListNode != closedList.end())
			{
				if (closedListNode->costSoFar <= gCost)
				{
					
					continue;
				}
				closedList.erase(closedListNode);
			}
			
			auto openListNode = std::find_if(openList.begin(),openList.end(),[pNextNode](const NodeRecord& record){return record.pNode == pNextNode;});
			if (openListNode != openList.end())
			{
				if (openListNode->costSoFar <= gCost)
				{
					continue;
				}

				openList.erase(openListNode);
			}
			NodeRecord newRecord{pNextNode, connection, gCost,gCost+GetHeuristicCost(pNextNode, pGoalNode)};
			openList.push_back(newRecord);
			
			
		}
		closedList.push_back(currentNodeRecord);
			
		
		auto it = std::find(openList.begin(), openList.end(),
			currentNodeRecord);

		if (it != openList.end())
		{
			openList.erase(it);
		}
		
	}
	
	return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}