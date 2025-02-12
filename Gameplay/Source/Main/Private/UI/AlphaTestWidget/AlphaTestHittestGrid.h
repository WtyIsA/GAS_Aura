#pragma once
#include "Input/HittestGrid.h"

class MAIN_API FAlphaTestHittestGrid
{
public:
	//
	// FHittestGrid::FGridTestingParams
	//
	struct FGridTestingParams
	{
		/** Ctor */
		FGridTestingParams()
		: CellCoord(-1, -1)
		, CursorPositionScreenSpace(FVector2D::ZeroVector)
		, Radius(-1.0f)
		, bTestWidgetIsInteractive(false)
		{}

		FIntPoint CellCoord;
		FVector2D CursorPositionScreenSpace;
		float Radius;
		bool bTestWidgetIsInteractive;
	};
	
	TArray<TSharedPtr<SWidget>> GetAppropriateWidget(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;
	
	/** Add SWidget from the HitTest Grid */
	void AddWidget(const SWidget* InWidget, int32 InBatchPriorityGroup, int32 InLayerId/*, FSlateInvalidationWidgetSortOrder InSecondarySort*/);
	
	/** Clear the grid */
	void Clear();

	/** Remove SWidget from the HitTest Grid */
	void RemoveWidget(const SWidget* InWidget);
	
	/**
 	* Set the position and size of the hittest area in desktop coordinates
 	*
 	* @param HittestDimensions			The dimensions of this hit testing area.
 	*
 	* @return      Returns true if a clear of the hittest grid was required. 
 	*/
	bool SetHittestArea(const FVector2D& HittestDimensions);

	/** Set the owner SWidget to be used by the parent grid (in case we are appended to another grid). */
	void SetOwner(const SWidget* InOwner) { check(Owner == nullptr || Owner == InOwner); Owner = InOwner; }
	
	/** Set the culling rect to be used by the parent grid (in case we are appended to another grid). */
	void SetCullingRect(const FSlateRect& InCullingRect) { CullingRect = InCullingRect; }
	
private:
	FVector2D ClosestPointOnSlateRotatedRect(const FVector2D &Point, const FSlateRotatedRect& RotatedRect) const
	{
		//no need to do any testing if we are inside of the rect
		if (RotatedRect.IsUnderLocation(Point))
		{
			return Point;
		}

		const static int32 NumOfCorners = 4;
		FVector2D Corners[NumOfCorners];
		Corners[0] = FVector2D(RotatedRect.TopLeft);
		Corners[1] = FVector2D(Corners[0]) + FVector2D(RotatedRect.ExtentX);
		Corners[2] = FVector2D(Corners[1]) + FVector2D(RotatedRect.ExtentY);
		Corners[3] = FVector2D(Corners[0]) + FVector2D(RotatedRect.ExtentY);

		FVector2D RetPoint;
		float ClosestDistSq = FLT_MAX;
		for (int32 i = 0; i < NumOfCorners; ++i)
		{
			//grab the closest point along the line segment
			const FVector2D ClosestPoint = FMath::ClosestPointOnSegment2D(Point, Corners[i], Corners[(i + 1) % NumOfCorners]);

			//get the distance between the two
			const float TestDist = FVector2D::DistSquared(Point, ClosestPoint);

			//if the distance is smaller than the current smallest, update our closest
			if (TestDist < ClosestDistSq)
			{
				RetPoint = ClosestPoint;
				ClosestDistSq = TestDist;
			}
		}

		return RetPoint;
	}

	FORCEINLINE float DistanceSqToSlateRotatedRect(const FVector2D &Point, const FSlateRotatedRect& RotatedRect) const
	{
		return FVector2D::DistSquared(ClosestPointOnSlateRotatedRect(Point, RotatedRect), Point);
	}

	FORCEINLINE bool IsOverlappingSlateRotatedRect(const FVector2D& Point, const float Radius, const FSlateRotatedRect& RotatedRect) const
	{
		return DistanceSqToSlateRotatedRect( Point, RotatedRect ) <= (Radius * Radius);
	}

	bool ContainsInteractableWidget(const TArray<FWidgetAndPointer>& PathToTest)
	{
		for (int32 i = PathToTest.Num() - 1; i >= 0; --i)
		{
			const FWidgetAndPointer& WidgetAndPointer = PathToTest[i];
			if (WidgetAndPointer.Widget->IsInteractable())
			{
				return true;
			}
		}
		return false;
	}
	
	/**
 	* Widget Data we maintain internally store along with the widget reference
 	*/
	struct FWidgetData
	{
		FWidgetData(const TWeakPtr<SWidget>& InWidget, const FIntPoint& InUpperLeftCell, const FIntPoint& InLowerRightCell, int64 InPrimarySort)/*, FSlateInvalidationWidgetSortOrder InSecondarySort)*/
			: WeakWidget(InWidget)
			, UpperLeftCell(InUpperLeftCell)
			, LowerRightCell(InLowerRightCell)
			, PrimarySort(InPrimarySort)
			/*, SecondarySort(InSecondarySort)*/
		{}
		TWeakPtr<SWidget> WeakWidget;
		TWeakPtr<ICustomHitTestPath> CustomPath;
		FIntPoint UpperLeftCell;
		FIntPoint LowerRightCell;
		int64 PrimarySort;
		/*FSlateInvalidationWidgetSortOrder SecondarySort;*/

		TSharedPtr<SWidget> GetWidget() const { return WeakWidget.Pin(); }
	};
	
	struct FWidgetIndex
	{
		FWidgetIndex()
			: Grid(nullptr)
			, WidgetIndex(INDEX_NONE)
		{}
		FWidgetIndex(const FAlphaTestHittestGrid* InHittestGrid, int32 InIndex)
			: Grid(InHittestGrid)
			, WidgetIndex(InIndex)
		{}
		bool IsValid() const { return Grid != nullptr && Grid->WidgetArray.IsValidIndex(WidgetIndex); }
		const FWidgetData& GetWidgetData() const;
		const FAlphaTestHittestGrid* GetGrid() const { return Grid; }

	private:
		const FAlphaTestHittestGrid* Grid;
		int32 WidgetIndex;
	};
	
	struct FIndexAndDistance : FWidgetIndex
	{
		FIndexAndDistance()
			: FWidgetIndex()
			, DistanceSqToWidget(0)
		{}
		FIndexAndDistance(FWidgetIndex WidgetIndex, float InDistanceSq)
			: FWidgetIndex(WidgetIndex)
			, DistanceSqToWidget(InDistanceSq)
		{}
		float GetDistanceSqToWidget() const { return DistanceSqToWidget; }

	private:
		float DistanceSqToWidget;
	};
	
	const FVector2D CellSize = FVector2D(128.0f, 128.0f);
	bool IsValidCellCoord(const FIntPoint& CellCoord) const;
	bool IsValidCellCoord(const int32 XCoord, const int32 YCoord) const;
	void ClearInternal(int32 TotalCells);
	FIntPoint GetCellCoordinate(FVector2D Position) const;

	using FCollapsedWidgetsArray = TArray<FWidgetIndex, TInlineAllocator<100>>;
	/** Return the list of all the widget in that cell. */
	void GetCollapsedWidgets(FCollapsedWidgetsArray& Out, const int32 X, const int32 Y) const;
	/** Return the Index and distance to a hit given the testing params */
	FIndexAndDistance GetHitIndexFromCellIndex(const FGridTestingParams& Params) const;
	
	/**
 	* All the available space is partitioned into Cells.
 	* Each Cell contains a list of widgets that overlap the cell.
 	* The list is ordered from back to front.
 	*/
	struct FCell
	{
	public:
		FCell() = default;

		void AddIndex(int32 WidgetIndex);
		void RemoveIndex(int32 WidgetIndex);

		const TArray<int32>& GetWidgetIndexes() const { return WidgetIndexes; }
		
	private:
		TArray<int32> WidgetIndexes;
	};
	
	/** Access a cell at coordinates X, Y. Coordinates are row and column indexes. */
	FORCEINLINE_DEBUGGABLE FCell& CellAt(const int32 X, const int32 Y)
	{
		checkfSlow((Y*NumCells.X + X) < Cells.Num(), TEXT("HitTestGrid CellAt() failed: X= %d Y= %d NumCells.X= %d NumCells.Y= %d Cells.Num()= %d"), X, Y, NumCells.X, NumCells.Y, Cells.Num());
		return Cells[Y*NumCells.X + X];
	}
	
	/** Access a cell at coordinates X, Y. Coordinates are row and column indexes. */
	FORCEINLINE_DEBUGGABLE const FCell& CellAt( const int32 X, const int32 Y ) const
	{
		checkfSlow((Y*NumCells.X + X) < Cells.Num(), TEXT("HitTestGrid CellAt() failed: X= %d Y= %d NumCells.X= %d NumCells.Y= %d Cells.Num()= %d"), X, Y, NumCells.X, NumCells.Y, Cells.Num());
		return Cells[Y*NumCells.X + X];
	}

	/** Map of all the widgets currently in the hit test grid to their stable index. */
	TMap<const SWidget*, int32> WidgetMap;

	/** Stable indexed sparse array of all the widget data we track. */
	TSparseArray<FWidgetData> WidgetArray;

	/** The size of the grid in cells. */
	FIntPoint NumCells;
	
	/** The cells that make up the space partition. */
	TArray<FCell> Cells;

	/** The Size of the current grid. */
	FVector2D GridSize;

	/** A grid needs a owner to be appended. */
	const SWidget* Owner;

	/** Culling Rect used when the widget was painted. */
	FSlateRect CullingRect;
};
