#include "SGridPanelEx.h"

SGridPanelEx::SGridPanelEx() : SGridPanel()
{
	_itemInterMax.X = 0;
	_itemInterMax.Y = 0;
}

FVector2D SGridPanelEx::ComputeDesiredSize(float f) const
{
	FVector2D size = SGridPanel::ComputeDesiredSize(f);
	size += _itemInterMax;;
	return  size;
}

void SGridPanelEx::SetItemInterval(FVector2D interval, bool xMax, bool yMax)
{
	if(xMax)
	{
		if(_itemInterMax.X < interval.X)
			_itemInterMax.X = interval.X;
	}
	else
	{
		if(_itemInterMax.X > interval.X)
			_itemInterMax.X = interval.X;
	}
	if(yMax)
	{
		if(_itemInterMax.Y < interval.Y)
			_itemInterMax.Y  = interval.Y;
	}
	else
	{
		if(_itemInterMax.Y > interval.Y)
			_itemInterMax.Y  = interval.Y;
	}
		
}


