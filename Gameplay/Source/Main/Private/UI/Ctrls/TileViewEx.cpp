#include "TileViewEx.h"

UTileViewEx::UTileViewEx(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

TSharedRef<STableViewBase> UTileViewEx::RebuildListWidget()
{
	
	return ConstructTileView<STileView>();
}

void UTileViewEx::SetListAlignment(EListItemAlignment alignment)
{	
	if(MyTileView.IsValid())
	{
		// MyTileView->ItemsPanel->ItemAlignment = alignment;
		MyTileView->RebuildList();
	}
}