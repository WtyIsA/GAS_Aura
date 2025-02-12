#include "AlphaTestHittestGrid.h"
#include "SAlphaTestButton.h"

const FAlphaTestHittestGrid::FWidgetData& FAlphaTestHittestGrid::FWidgetIndex::GetWidgetData() const
{
	check(Grid->WidgetArray.IsValidIndex(WidgetIndex));
	return Grid->WidgetArray[WidgetIndex];
}

//
// FHittestGrid::FCell
//

void FAlphaTestHittestGrid::FCell::AddIndex(int32 WidgetIndex)
{
	check(!WidgetIndexes.Contains(WidgetIndex));
	WidgetIndexes.Add(WidgetIndex);
}

void FAlphaTestHittestGrid::FCell::RemoveIndex(int32 WidgetIndex)
{
	WidgetIndexes.RemoveSingleSwap(WidgetIndex);
}

TArray<TSharedPtr<SWidget>> FAlphaTestHittestGrid::GetAppropriateWidget(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	checkSlow(IsInGameThread());
	checkSlow(MyGeometry.HasRenderTransform());

	FVector2D ScreenMousePos = MouseEvent.GetScreenSpacePosition();
	const FVector2D CursorPositionInGrid = MyGeometry.GetAccumulatedRenderTransform().TransformPoint(MyGeometry.AbsoluteToLocal(ScreenMousePos));
	
	if (WidgetArray.Num() > 0 && Cells.Num() > 0)
	{
		FGridTestingParams TestingParams;
		TestingParams.CursorPositionScreenSpace = ScreenMousePos;
		TestingParams.CellCoord = GetCellCoordinate(CursorPositionInGrid);
		TestingParams.Radius = 0.0f;
		TestingParams.bTestWidgetIsInteractive = false;

		// First add the exact point test results
		const FIndexAndDistance BestHit = GetHitIndexFromCellIndex(TestingParams);
		if (BestHit.IsValid())
		{
			TArray<TSharedPtr<SWidget>> Path;
			const FWidgetData& BestHitWidgetData = BestHit.GetWidgetData();
			const TSharedPtr<SWidget> FirstHitWidget = BestHitWidgetData.GetWidget();

			TSharedPtr<SWidget> CurWidget = FirstHitWidget;
			while (CurWidget.IsValid())
			{
				Path.Emplace(CurWidget);
				CurWidget = CurWidget->Advanced_GetPaintParentWidget();
			}

			//Algo::Reverse(Path);
			
			// Make Sure we landed on a valid widget
			if (Path.Num() > 0)
			{
				return Path;
			}
		}
	}

	return TArray<TSharedPtr<SWidget>>();
}

FAlphaTestHittestGrid::FIndexAndDistance FAlphaTestHittestGrid::GetHitIndexFromCellIndex(const FGridTestingParams& Params) const
{
	//check if the cell coord 
	if (IsValidCellCoord(Params.CellCoord))
	{
		// Get the cell and sort it 
		FCollapsedWidgetsArray WidgetIndexes;
		GetCollapsedWidgets(WidgetIndexes, Params.CellCoord.X, Params.CellCoord.Y);

		// Consider front-most widgets first for hittesting.
		for (int32 i = WidgetIndexes.Num() - 1; i >= 0; --i)
		{
			check(WidgetIndexes[i].IsValid());
			const FWidgetData& TestCandidate = WidgetIndexes[i].GetWidgetData();
			const TSharedPtr<SWidget> TestWidget = TestCandidate.GetWidget();

			// When performing a point hittest, accept all hittestable widgets.
			// When performing a hittest with a radius, only grab interactive widgets.
			const bool bIsValidWidget = TestWidget.IsValid() && (!Params.bTestWidgetIsInteractive || TestWidget->IsInteractable());
			if (bIsValidWidget)
			{
				const FVector2D GridSpaceCoordinate = Params.CursorPositionScreenSpace;

				const FGeometry& TestGeometry = TestWidget->GetPaintSpaceGeometry();

				bool bPointInsideClipMasks = true;

				{
					const TOptional<FSlateClippingState>& WidgetClippingState = TestWidget->GetCurrentClippingState();
					if (WidgetClippingState.IsSet())
					{
						// TODO: Solve non-zero radius cursors?
						bPointInsideClipMasks = WidgetClippingState->IsPointInside(GridSpaceCoordinate);
					}
				}

				if (bPointInsideClipMasks)
				{
					if(SAlphaTestButton* AlphaTestInterface = reinterpret_cast<SAlphaTestButton*>(TestWidget.Get()))
					{
						bPointInsideClipMasks = AlphaTestInterface->CursorPosAlphaNotZero(Params.CursorPositionScreenSpace);
					}
				}

				if (bPointInsideClipMasks)
				{
					// Compute the render space clipping rect (FGeometry exposes a layout space clipping rect).
					FSlateLayoutTransform LayoutTransform = TestGeometry.GetAccumulatedLayoutTransform();
					auto LayoutRenderTransform = Concatenate(
							Inverse(LayoutTransform),
							TestGeometry.GetAccumulatedRenderTransform());
					const FSlateRotatedRect WindowOrientedClipRect = TransformRect(LayoutRenderTransform,
						FSlateRotatedRect(TestGeometry.GetLayoutBoundingRect())
					);

					//screen to layout;layout add (layout difference with render)  Chinese English
					FVector2D transformedPos = LayoutRenderTransform.TransformPoint(LayoutTransform.TransformPoint(TestWidget->GetCachedGeometry().AbsoluteToLocal(GridSpaceCoordinate)));
					
					if (IsOverlappingSlateRotatedRect(transformedPos, Params.Radius, WindowOrientedClipRect))
					{
						// For non-0 radii also record the distance to cursor's center so that we can pick the closest hit from the results.
						const bool bNeedsDistanceSearch = Params.Radius > 0.0f;
						const float DistSq = (bNeedsDistanceSearch) ? DistanceSqToSlateRotatedRect(GridSpaceCoordinate, WindowOrientedClipRect) : 0.0f;
						return FIndexAndDistance(WidgetIndexes[i], DistSq);
					}
				}
			}
		}
	}

	return FIndexAndDistance();
}

#define UE_VERIFY_WIDGET_VALIDITE 0
void FAlphaTestHittestGrid::GetCollapsedWidgets(FCollapsedWidgetsArray& OutResult, const int32 X, const int32 Y) const
{
	const int32 CellIndex = Y * NumCells.X + X;
	check(Cells.IsValidIndex(CellIndex));

	const TArray<int32>& WidgetsIndexes = CellAt(X, Y).GetWidgetIndexes();
	for (int32 WidgetIndex : WidgetsIndexes)
	{
#if UE_VERIFY_WIDGET_VALIDITE
		ensureAlways(WidgetArray.IsValidIndex(WidgetIndex));
#endif
		OutResult.Emplace(this, WidgetIndex);
	}

	OutResult.StableSort([](const FWidgetIndex& A, const FWidgetIndex& B)
		{
			const FWidgetData& WidgetDataA = A.GetWidgetData();
			const FWidgetData& WidgetDataB = B.GetWidgetData();
			return WidgetDataA.PrimarySort < WidgetDataB.PrimarySort; /*|| (WidgetDataA.PrimarySort == WidgetDataB.PrimarySort && WidgetDataA.SecondarySort < WidgetDataB.SecondarySort);*/
		});
}
#undef UE_VERIFY_WIDGET_VALIDITE

FIntPoint FAlphaTestHittestGrid::GetCellCoordinate(FVector2D Position) const
{
	return FIntPoint(
		FMath::Min(FMath::Max(FMath::FloorToInt(Position.X / CellSize.X), 0), NumCells.X - 1),
		FMath::Min(FMath::Max(FMath::FloorToInt(Position.Y / CellSize.Y), 0), NumCells.Y - 1));
}

bool FAlphaTestHittestGrid::IsValidCellCoord(const FIntPoint& CellCoord) const
{
	return IsValidCellCoord(CellCoord.X, CellCoord.Y);
}

bool FAlphaTestHittestGrid::IsValidCellCoord(const int32 XCoord, const int32 YCoord) const
{
	return XCoord >= 0 && XCoord < NumCells.X && YCoord >= 0 && YCoord < NumCells.Y;
}

void FAlphaTestHittestGrid::AddWidget(const SWidget* InWidget, int32 InBatchPriorityGroup, int32 InLayerId/*, FSlateInvalidationWidgetSortOrder InSecondarySort*/)
{
	check(InWidget);
	if (!InWidget->GetVisibility().IsHitTestVisible())
	{
		return;
	}

#if UE_SLATE_ENABLE_HITTEST_STATS
	SCOPE_CYCLE_COUNTER(STAT_SlateHTG_AddWidget);
#endif

	// Track the widget and identify it's Widget Index
	FGeometry GridSpaceGeometry = InWidget->GetPaintSpaceGeometry();

	// Currently using grid offset because the grid covers all desktop space.
	const FSlateRect BoundingRect = GridSpaceGeometry.GetRenderBoundingRect();

	// Starting and ending cells covered by this widget.	
	const FIntPoint UpperLeftCell = GetCellCoordinate(BoundingRect.GetTopLeft());
	const FIntPoint LowerRightCell = GetCellCoordinate(BoundingRect.GetBottomRight());

	const int64 PrimarySort = (((int64)InBatchPriorityGroup << 32) | InLayerId);

	bool bAddWidget = true;
	if (int32* FoundIndex = WidgetMap.Find(InWidget))
	{
		FWidgetData& WidgetData = WidgetArray[*FoundIndex];
		if (WidgetData.UpperLeftCell != UpperLeftCell || WidgetData.LowerRightCell != LowerRightCell)
		{
			// Need to be updated
			RemoveWidget(InWidget);
		}
		else
		{
			// Only update
			bAddWidget = false;
			WidgetData.PrimarySort = PrimarySort;
			/*WidgetData.SecondarySort = InSecondarySort;*/
		}
	}

	if (bAddWidget)
	{
		int32& WidgetIndex = WidgetMap.Add(InWidget);
		WidgetIndex = WidgetArray.Emplace(const_cast<SWidget*>(InWidget)->AsShared(), UpperLeftCell, LowerRightCell, PrimarySort);/*, InSecondarySort);*/
		for (int32 XIndex = UpperLeftCell.X; XIndex <= LowerRightCell.X; ++XIndex)
		{
			for (int32 YIndex = UpperLeftCell.Y; YIndex <= LowerRightCell.Y; ++YIndex)
			{
				if (IsValidCellCoord(XIndex, YIndex))
				{
					CellAt(XIndex, YIndex).AddIndex(WidgetIndex);
				}
			}
		}
	}
}

void FAlphaTestHittestGrid::RemoveWidget(const SWidget* InWidget)
{
#if UE_SLATE_ENABLE_HITTEST_STATS
	SCOPE_CYCLE_COUNTER(STAT_SlateHTG_RemoveWidget);
#endif

	int32 WidgetIndex = INDEX_NONE;
	if (WidgetMap.RemoveAndCopyValue(InWidget, WidgetIndex))
	{
		const FWidgetData& WidgetData = WidgetArray[WidgetIndex];

		// Starting and ending cells covered by this widget.	
		const FIntPoint& UpperLeftCell = WidgetData.UpperLeftCell;
		const FIntPoint& LowerRightCell = WidgetData.LowerRightCell;

		for (int32 XIndex = UpperLeftCell.X; XIndex <= LowerRightCell.X; ++XIndex)
		{
			for (int32 YIndex = UpperLeftCell.Y; YIndex <= LowerRightCell.Y; ++YIndex)
			{
				checkSlow(IsValidCellCoord(XIndex, YIndex));
				CellAt(XIndex, YIndex).RemoveIndex(WidgetIndex);
			}
		}

		WidgetArray.RemoveAt(WidgetIndex);
	}
}

void FAlphaTestHittestGrid::Clear()
{
	const int32 TotalCells = Cells.Num();
	ClearInternal(TotalCells);
}

void FAlphaTestHittestGrid::ClearInternal(int32 TotalCells)
{
	Cells.Reset(TotalCells);
	Cells.SetNumZeroed(TotalCells);

	WidgetMap.Reset();
	WidgetArray.Reset();
}

bool FAlphaTestHittestGrid::SetHittestArea(const FVector2D& HittestDimensions)
{
	bool bWasCleared = false;

	// If the size of the hit test area changes we need to clear it out
	if (GridSize != HittestDimensions)
	{
		GridSize = HittestDimensions;
		NumCells = FIntPoint(FMath::CeilToInt(GridSize.X / CellSize.X), FMath::CeilToInt(GridSize.Y / CellSize.Y));
		
		const int32 NewTotalCells = NumCells.X * NumCells.Y;
		ClearInternal(NewTotalCells);

		bWasCleared = true;
	}

	return bWasCleared;
}