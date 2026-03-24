#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail
		if (IsConnected())
		{
			int32_t OddNodesNr{};
			std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		
			for (Node* pNode : Nodes)
			{
				if (m_pGraph->FindConnectionsFrom(pNode->GetId()).size() % 2 != 0)
				{
					OddNodesNr++;
				}
			}
			
			if (OddNodesNr > 2) return Eulerianity::notEulerian;
			
			if (m_pGraph->GetNodeCount() > 2 && OddNodesNr == 2) return Eulerianity::semiEulerian;
			
			if (OddNodesNr <= 0) return Eulerianity::eulerian;
			
		}

		// Count nodes with odd degree 

		//  A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian

		//  A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		//  An Euler trail can be made, but only starting and ending in these 2 nodes

		//  A connected graph with no odd nodes is Eulerian
		
		return Eulerianity::notEulerian;
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		
		// TODO Check if there can be an Euler path
		// TODO If this graph is not eulerian, return the empty path
		if (eulerianity == Eulerianity::notEulerian) return Path;
		
		int startIndex{0};
		int AmountOfOdd{};
		for (Node* pNode : Nodes)
		{
			if (m_pGraph->FindConnectionsFrom(pNode->GetId()).size() % 2 != 0)
			{
				AmountOfOdd++;
				
			}
			if (AmountOfOdd == 2) startIndex = pNode->GetId();
		}

		// TODO Start algorithm loop
		std::stack<int> nodeStack;
		Node* pCurrentNode = Nodes[startIndex];
		std::vector<Connection*> Neighbors = graphCopy.FindConnectionsFrom(pCurrentNode->GetId());
		while (Neighbors.size() > 0 || nodeStack.size() > 0)
		{
			if (Neighbors.size() > 0 )
			{
				nodeStack.push(pCurrentNode->GetId());
				pCurrentNode = Nodes[Neighbors[0]->GetToId()];
				graphCopy.RemoveConnection(Neighbors[0]);
			}
			else
			{
				Path.push_back(m_pGraph->GetNode(pCurrentNode->GetId()).get());

				if (!nodeStack.empty())
				{
					int nodeId = nodeStack.top();
					nodeStack.pop();
					pCurrentNode = Nodes[nodeId];
				}
			}
			
			Neighbors = graphCopy.FindConnectionsFrom(pCurrentNode->GetId());
			

		}
		
		Path.push_back(m_pGraph->GetNode(pCurrentNode->GetId()).get());
		
		

		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		// TODO Mark the visited node
		
		visited[startIndex] = true;
		std::vector<Connection*> connectedNodes = m_pGraph->FindConnectionsFrom(startIndex);
		
		for (Connection* connection : connectedNodes)
		{
			int ID{connection->GetToId()};
			if (visited[ID]) continue;
			VisitAllNodesDFS(Nodes, visited,ID );
			
		}
		

		// TODO Ask the graph for the connections from that node
		// TODO recursively visit any valid connected nodes that were not visited before
		// TODO Tip: use an index-based for-loop to find the correct index
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		
		
		
		if (Nodes.size() == 0)
			return false;

		std::vector<bool> visitedNodes{};
		visitedNodes.resize(Nodes.size());
		int startIndex{0};
		
		
		//Find Node with Odd Degree
		int AmountOfOdd{};
		for (Node* pNode : Nodes)
		{
			if (m_pGraph->FindConnectionsFrom(pNode->GetId()).size() % 2 != 0)
			{
				AmountOfOdd++;
				//if (AmountOfOdd == 2) 
					startIndex = pNode->GetId();
				break;
				//if (AmountOfOdd > 2) return false;
			}
		}
		
		
		VisitAllNodesDFS(Nodes,visitedNodes, startIndex);
		
		//Check for unvisisted Node
		for (bool isVisited : visitedNodes)
		{
			if (!isVisited) return false;
		}
		return true;
		
		
		// TODO choose a starting node
		
		// TODO start a depth-first-search traversal from the node that has at least one connection
		
		// TODO if a node was never visited, this graph is not connected
	}
}