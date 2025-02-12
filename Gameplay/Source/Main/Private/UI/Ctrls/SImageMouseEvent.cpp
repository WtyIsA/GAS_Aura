#include "SImageMouseEvent.h"


#include "LuaManger.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerInput.h"
#include "Input/ShortcutkeyMgr.h"
#include "Slate/SceneViewport.h"


SImageMouseEvent::SImageMouseEvent()
{
	_SyncInputTouch = false;
	m_pUWidget = nullptr;
	_bDrawLine = false;
	_lineColor = FLinearColor::White;
	_fThickness = 1.0f;
}

FReply SImageMouseEvent::OnMouseWheel( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if(_OnMouseWheel.IsBound())
		return _OnMouseWheel.Execute(MyGeometry, MouseEvent);
	return FReply::Unhandled();
	
}

void SImageMouseEvent::SetOnMouseWheel(FOnDragDetected EventHandler)
{
	_OnMouseWheel = EventHandler;
}

FReply SImageMouseEvent::OnTouchMoved( const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent )
{
	if(!_SyncInputTouch)
		return FReply::Unhandled();
	UShortcutkeyMgr::Get()->UpdateIntput(InTouchEvent.GetPointerIndex(), ETouchType::Moved, InTouchEvent.GetScreenSpacePosition(), InTouchEvent.GetTouchForce(), FDateTime::Now(), InTouchEvent.GetTouchpadIndex() );
	APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(p != nullptr)
	{			
		FVector2D touch1;
		FVector2D touch2;
		bool bPress = true;;
		bool bPress2 = true;;
		p->GetInputTouchState(ETouchIndex::Touch1, touch1.X, touch1.Y, bPress);
		p->GetInputTouchState(ETouchIndex::Touch2, touch2.X, touch2.Y, bPress2);
		
		if(bPress && bPress2 && ULuaManger::Get()->IsPostionUnderWidget(m_pUWidget, touch1) &&
		ULuaManger::Get()->IsPostionUnderWidget(m_pUWidget, touch2))
		{
			return FReply::Handled();
		}
	}
	return FReply::Unhandled();
}

FReply SImageMouseEvent::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	if(_SyncInputTouch)
		UShortcutkeyMgr::Get()->UpdateIntput(InTouchEvent.GetPointerIndex(), ETouchType::Ended, InTouchEvent.GetScreenSpacePosition(), InTouchEvent.GetTouchForce(), FDateTime::Now(), InTouchEvent.GetTouchpadIndex() );
	
	return FReply::Unhandled();
}

FReply SImageMouseEvent::OnTouchStarted( const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent )
{
	if(_SyncInputTouch)
		UShortcutkeyMgr::Get()->UpdateIntput(InTouchEvent.GetPointerIndex(), ETouchType::Began, InTouchEvent.GetScreenSpacePosition(), InTouchEvent.GetTouchForce(), FDateTime::Now(), InTouchEvent.GetTouchpadIndex() );
	
	return FReply::Unhandled();
}

int32 SImageMouseEvent::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 nRe = SImage::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if(_bDrawLine)
	{
		FVector2D temp = AllottedGeometry.GetLocalSize();
		
		//LayerId++;
		for(auto point : _Points)
		{
			bool bCull =false;
			for(auto& item:point)
			{
				if(item.X < 0 || item.X > temp.X)
					bCull = true;
				else if(item.Y < 0 || item.Y > temp.Y)
				{
					bCull = true;
				}
			}
			if(bCull)
				continue;
			FSlateDrawElement::MakeLines(
             OutDrawElements,
             LayerId,
             AllottedGeometry.ToPaintGeometry(),
             point,
             ESlateDrawEffect::None,
             _lineColor,
             true,
             _fThickness);
		}
	}
	return  nRe;
}

void SImageMouseEvent::SetDrawLine(bool bDraw, FVector2D pos1, FVector2D pos2, float fInter, float nPerDis, float Thickness, FLinearColor color)
{
	if(pos1 == _linePos1 && pos2 == _linePos2)
		return;
	_linePos1 = pos1;
	_linePos2 = pos2;
	_bDrawLine = bDraw;
	_Points.Empty();
	if(!_bDrawLine)
	{		
		return;
	}
	if(fInter <= 0 || nPerDis <= 0)
	{
		TArray<FVector2D> line;	
		line.Add(pos1);
		line.Add(pos2);
		_Points.Add(line);
	}
	else
	{
		FVector2D Offset = pos2 - pos1;
		float allLength = Offset.Size();
		float minDis = fInter + nPerDis;
		if(allLength < minDis)
		{
			TArray<FVector2D> line;	
			line.Add(pos1);
			line.Add(pos2);
			_Points.Add(line);
		}
		else
		{
			float Angle = FMath::Atan2(Offset.Y, Offset.X);
			float CosAngle = FMath::Cos(Angle);
			float SinAngle = FMath::Sin(Angle);
			float fTimes = (allLength + nPerDis) / minDis;
			int n = FMath::FloorToInt(fTimes);
			TArray<FVector2D> line;
			FVector2D interVector( fInter * CosAngle,fInter * SinAngle);
			FVector2D lenVector(nPerDis * CosAngle,nPerDis * SinAngle);			
			FVector2D temp = pos2;
			for(int i = 0; i < n; i++)
			{
				line.Empty();
				line.Add(temp);
				temp -= lenVector;
				line.Add(temp);
				_Points.Add(line);
				temp -= interVector;
			}
		}
		
	}
	
	_fThickness = Thickness;
	_lineColor = color;
}

