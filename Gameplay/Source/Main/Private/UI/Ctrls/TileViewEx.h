#pragma once

#include "Components/TileView.h"
#include "TileViewEx.generated.h"

UCLASS()
class UTileViewEx : public  UTileView
{
	GENERATED_UCLASS_BODY()
public:
	virtual TSharedRef<STableViewBase> RebuildListWidget() override;
    
	UFUNCTION()
	void SetListAlignment(EListItemAlignment alignment);
	
};
