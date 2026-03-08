#include "SpacePartitioning.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;
	CellOrigin.X = -Width/2.f;
	CellOrigin.Y = -Height/2.f;
	Cells.reserve(NrOfRows * NrOfCols);

	for (int row = 0; row < NrOfRows; ++row)
	{
		for (int col = 0; col < NrOfCols; ++col)
		{
			float left   = col * CellWidth + CellOrigin.X;
			float bottom = row * CellHeight + CellOrigin.Y;

			float right  = left + CellWidth;
			float top    = bottom + CellHeight;

			FRect bounds;
			bounds.Min = FVector2D(left,  bottom);
			bounds.Max = FVector2D(right, top);

			Cells.emplace_back(left, bottom, CellWidth, CellHeight);
			Cells.back().BoundingBox = bounds;
		}
	}
}

void CellSpace::AddAgent(ASteeringAgent* Agent)
{
	int index = PositionToIndex(Agent->GetPosition());
	if (index < NrOfRows * NrOfCols) Cells[index].Agents.push_back(Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent* Agent, const FVector2D& OldPos)
{
	int oldIndex = PositionToIndex(OldPos);
	int newIndex = PositionToIndex(Agent->GetPosition());

	if (oldIndex == newIndex) return;
	
	Cells[oldIndex].Agents.remove(Agent);
	Cells[newIndex].Agents.push_back(Agent);

}

void CellSpace::RegisterNeighbors(ASteeringAgent* Agent, float QueryRadius)
{
	NrOfNeighbors = 0;
	const FVector2D pos = Agent->GetPosition();
	const float radiusSq = QueryRadius * QueryRadius;

	FRect queryRect;
	queryRect.Min = FVector2D(pos.X - QueryRadius,pos.Y - QueryRadius);
	queryRect.Max = FVector2D(pos.X + QueryRadius,pos.Y + QueryRadius);
	
	
	for (int row = 0; row < NrOfRows; ++row)
	{
		for (int col = 0; col < NrOfCols; ++col)
		{
			int index = row * NrOfCols + col;
			if (!DoRectsOverlap( Cells[index].BoundingBox, queryRect))
				continue;

			for (ASteeringAgent* other :  Cells[index].Agents)
			{
				if (other == Agent || other == nullptr) continue;
				float distSq = FVector2D::DistSquared(pos, other->GetPosition());

				if (distSq <= radiusSq)
				{
					if (NrOfNeighbors < Neighbors.Num())
					{
						Neighbors[NrOfNeighbors] = other;
						++NrOfNeighbors;
					}
				}
			}
		}
	}
}


void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	if (!pWorld) return;

	for (const Cell& cell : Cells)
	{
		const FVector2D& min = cell.BoundingBox.Min;
		const FVector2D& max = cell.BoundingBox.Max;
		
		FVector bl(min.X, min.Y, 0.f); 
		FVector br(max.X, min.Y, 0.f); 
		FVector tr(max.X, max.Y, 0.f); 
		FVector tl(min.X, max.Y, 0.f); 
		
		DrawDebugLine(pWorld, bl, br, FColor::White, false, -1.f, 0, 2.f);
		DrawDebugLine(pWorld, br, tr, FColor::White, false, -1.f, 0, 2.f);
		DrawDebugLine(pWorld, tr, tl, FColor::White, false, -1.f, 0, 2.f);
		DrawDebugLine(pWorld, tl, bl, FColor::White, false, -1.f, 0, 2.f);


		FVector center((min.X + max.X) * 0.5f,(min.Y + max.Y) * 0.5f,0.f);
		FString countString = FString::FromInt(cell.Agents.size());
		DrawDebugString(pWorld,center,countString,nullptr,FColor::Green,0.f,true);
	}
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	int col = static_cast<int>((Pos.X - CellOrigin.X) / CellWidth);
	int row = static_cast<int>((Pos.Y - CellOrigin.Y) / CellHeight);
	col = FMath::Clamp(col, 0, NrOfCols - 1);
	row = FMath::Clamp(row, 0, NrOfRows - 1);
	return row * NrOfCols + col;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
	return true;
}