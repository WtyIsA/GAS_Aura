#pragma once
#include "Framework/Application/NavigationConfig.h"

struct FCustomNavigationConfig : FNavigationConfig
{
public:
	FCustomNavigationConfig();
	
	/** Gets the navigation direction from a given key event. */
	virtual EUINavigation GetNavigationDirectionFromKey(const FKeyEvent& InKeyEvent) const override;

	virtual EUINavigationAction GetNavigationActionFromKey(const FKeyEvent& InKeyEvent) const override;
};
