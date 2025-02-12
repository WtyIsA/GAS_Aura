// Copyright Epic Games, Inc. All Rights Reserved.

#include "TouchTurn.h"
#include "STouchTurn.h"

UTouchTurn::UTouchTurn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}


TSharedRef<SWidget> UTouchTurn::RebuildWidget()
{
	MyTouchTurn = SNew(STouchTurn);
	MyTouchTurn->SetTouchTurnUmg(this);
	return  MyTouchTurn.ToSharedRef();
}
void UTouchTurn::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyTouchTurn.Reset();
}

void UTouchTurn::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if(MyTouchTurn.IsValid())
	{
		MyTouchTurn->_fPitchScale = PitchScale;
		MyTouchTurn->_fYawScale = YawScale;
		MyTouchTurn->bIgnoreTextureSize = bIgnoreTextureSize;
		MyTouchTurn->InitValue = InitValue ;
		MyTouchTurn->SlopeValue= SlopeValue;
		MyTouchTurn->SpeedUpLimitPixel = SpeedUpLimitPixel;
		MyTouchTurn->InputPitchScale = InputPitchScale;

		MyTouchTurn->SetOnMouseButtonDownEvent(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
		MyTouchTurn->SetOnMouseButtonUpEvent(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp));
	}
}

void UTouchTurn::SetTurnValue(float i, float s,float limitPixel)
{
	if(MyTouchTurn.IsValid())
	{
		MyTouchTurn->InitValue = i ;
		MyTouchTurn->SlopeValue= s;
		MyTouchTurn->SpeedUpLimitPixel = limitPixel;
		InitValue =i;
		SlopeValue = s;
		SpeedUpLimitPixel = limitPixel;
	}
}

void UTouchTurn::SetInputScale(float fYawscale, float fPitchscale)
{
	PitchScale = fPitchscale;
	YawScale = fYawscale;
	if(MyTouchTurn.IsValid())
	{
		MyTouchTurn->_fPitchScale = PitchScale;
		MyTouchTurn->_fYawScale = YawScale;
	}
}

void UTouchTurn::SetScopeValue(float s)
{
	if(MyTouchTurn.IsValid())
	{
		MyTouchTurn->SlopeValue= s;
		SlopeValue = s;
	}
}

void UTouchTurn::SetPitchValue(float pitchvalue)
{
	MyTouchTurn->InputPitchScale= pitchvalue;
	PitchScale = pitchvalue;
}

void UTouchTurn::SetInitValue(float value)
{
	MyTouchTurn->InitValue= value;
	InitValue = value;
}

FReply UTouchTurn::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonDownEvent.IsBound())
	{
		return OnMouseButtonDownEvent.Execute(Geometry,MouseEvent).NativeReply;
	}
	return FReply::Unhandled();
}

FReply UTouchTurn::HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonUpEvent.IsBound())
	{
		return OnMouseButtonUpEvent.Execute(Geometry,MouseEvent).NativeReply;
	}
	return FReply::Unhandled();
}