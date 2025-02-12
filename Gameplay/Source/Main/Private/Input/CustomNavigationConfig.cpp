#include "CustomNavigationConfig.h"


FCustomNavigationConfig::FCustomNavigationConfig()
{
	KeyEventRules.Remove(EKeys::Left);
	KeyEventRules.Remove(EKeys::Right);
	KeyEventRules.Remove(EKeys::Up);
	KeyEventRules.Remove(EKeys::Down);
}

EUINavigation FCustomNavigationConfig::GetNavigationDirectionFromKey(const FKeyEvent& InKeyEvent) const
{
	if (const EUINavigation* Rule = KeyEventRules.Find(InKeyEvent.GetKey()))
	{
		if (bKeyNavigation)
		{
			return *Rule;
		}
	}
	else if (bTabNavigation && InKeyEvent.GetKey() == EKeys::Tab )
	{
		const bool bAllowEatingKeyEvents = (InKeyEvent.IsControlDown() || InKeyEvent.IsCommandDown()) && !InKeyEvent.IsAltDown();

		if ( bAllowEatingKeyEvents )
		{
			return ( InKeyEvent.IsShiftDown() ) ? EUINavigation::Previous : EUINavigation::Next;
		}
	}

	return EUINavigation::Invalid;
}

EUINavigationAction FCustomNavigationConfig::GetNavigationActionFromKey(const FKeyEvent& InKeyEvent) const
{
	// const FKey& InKey = InKeyEvent.GetKey();
	// if (InKey == EKeys::Enter || InKey == EKeys::SpaceBar || InKey == EKeys::Virtual_Accept)
 //    {
 //    	// By default, enter, space, and gamepad accept are all counted as accept
 //    	return EUINavigationAction::Accept;
 //    }
 //    else if (InKey == EKeys::Escape || InKey == EKeys::Virtual_Back)
 //    {
 //    	// By default, escape and gamepad back count as leaving current scope
 //    	return EUINavigationAction::Back;
 //    }
    
    return EUINavigationAction::Invalid;
}
