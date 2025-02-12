#pragma once
#include "Widgets/Layout/SGridPanel.h"

class SGridPanelEx : public  SGridPanel
{
public:
	SGridPanelEx();
	virtual FVector2D ComputeDesiredSize(float) const override;
	void SetItemInterval(FVector2D interval, bool xMax, bool yMax);
private:
	FVector2D _itemInterMax;
};
