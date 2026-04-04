#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
{
public:
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly)
	{
		std::vector<NavLine> Portals = {};
		Portals.emplace_back(NavLine{Path.front()->GetPosition(), Path.front()->GetPosition()});
		
		for (size_t i = 0; i < Path.size() - 1; ++i)
		{
			NavLine portal;
			auto* currentNode = Path[i];
			auto* nextNode = Path[i+1];
			auto* currentNavNode = static_cast<NavGraphNode*>(currentNode);
			
			int edgeIndex = currentNavNode->GetEdgeIdx();
			if (edgeIndex < 0) continue;
			
			auto edge = NavPoly.GetEdges()[edgeIndex];
			FVector2D p1{FVector2D(edge.GetP1(NavPoly))};
			FVector2D p2{FVector2D(edge.GetP2(NavPoly))};
			
			
			
			FVector2D pathDir = (nextNode->GetPosition() - currentNode->GetPosition()).GetSafeNormal();
			FVector2D center = currentNavNode->GetPosition();
			FVector2D toP1 = p1-center;
			FVector2D toP2 = p2-center;
			float cross1 = FVector2D::CrossProduct(pathDir, toP1);
			float cross2 = FVector2D::CrossProduct(pathDir, toP2);
			

			if (cross1 < cross2)
			{
				portal.P1 = p1; 
				portal.P2 = p2; 
			}
			else
			{
				portal.P1 = p2; 
				portal.P2 = p1; 
			}
			Portals.emplace_back(portal);
		}
		

		Portals.emplace_back(NavLine{Path.back()->GetPosition(),Path.back()->GetPosition()});
		
		return Portals;


	}


		
	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
		std::vector<FVector2D> Path{};
		

		
		FVector2D apex = Portals[0].P1;
		Path.push_back(apex);

		FVector2D leftLeg  = Portals[0].P2 - apex;
		FVector2D rightLeg = Portals[0].P1 - apex;

		int apexIndex = 0;
		int leftLegIndex = 0;
		int rightLegIndex = 0;
		
		
		
		
		for (int portalIndex = 1; portalIndex < Portals.size(); ++portalIndex)
		{
			const NavLine& portal = Portals[portalIndex];
			FVector2D newRightLeg = portal.P1 - apex;
    	
			if (FVector2D::CrossProduct(newRightLeg, rightLeg) <= 0)
			{
				if (FVector2D::CrossProduct(newRightLeg, leftLeg) < 0)
				{
					apex = Portals[leftLegIndex].P2;
					Path.push_back(apex);

					apexIndex = leftLegIndex;
            	
					portalIndex = apexIndex +1;
            	
					leftLegIndex = portalIndex;
					rightLegIndex = portalIndex;

					if (portalIndex < Portals.size())
					{
						leftLeg  = Portals[portalIndex].P2 - apex;
						rightLeg = Portals[portalIndex].P1 - apex;
                		continue;
					}
				}
        		else
        		{
        			rightLeg = newRightLeg;
        			rightLegIndex = portalIndex;
        		}


        }
        FVector2D newLeftLeg = portal.P2 - apex;

        if (FVector2D::CrossProduct(newLeftLeg, leftLeg) >= 0)
        {

            if (FVector2D::CrossProduct(newLeftLeg, rightLeg) > 0)
            {

            	apex = Portals[rightLegIndex].P1;
                Path.push_back(apex);

                apexIndex = rightLegIndex;
                portalIndex = apexIndex + 1;
                leftLegIndex = portalIndex;
                rightLegIndex = portalIndex;

                if (portalIndex < Portals.size())
                {
                    leftLeg  = Portals[portalIndex].P2 - apex;
                    rightLeg = Portals[portalIndex].P1 - apex;
                	continue;
                }

            }
			else
			{
				leftLeg = newLeftLeg;
				leftLegIndex = portalIndex;				
			}

            
        }
    }
		
    Path.push_back(Portals.back().P1);

    return Path;
		
	}
		
private:
	SSFA() {};
	~SSFA() {};
};
}
