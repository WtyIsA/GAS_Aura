#include "STouchTurn.h"


#include "LuaManger.h"
#include "TouchTurn.h"
#include "Kismet/GameplayStatics.h"

STouchTurn::STouchTurn()
{
	_CapturedPointerIndex = -1;
	_sendTouchOneMore = false;
	_fYawScale = 1.0f;
	_fPitchScale = -1.0f;
	m_pTouchTurnUmg = nullptr;
}

void STouchTurn::Construct(const FArguments& InArgs)
{
}

FReply STouchTurn::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if(Event.GetPointerIndex() == _CapturedPointerIndex)
	{
		_sendTouchOneMore = true;
		_touchMovePos = _touchStartPos;
		CaluTouchInter(MyGeometry.Size);
		_CapturedPointerIndex = -1;
		if (OnMouseButtonUpEvent.IsBound())
		{
			OnMouseButtonUpEvent.Execute(MyGeometry,Event);
		}
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

FReply STouchTurn::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if(_CapturedPointerIndex == -1)
	{
		_touchMovePos = _touchStartPos = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
		_CapturedPointerIndex = Event.GetPointerIndex();
		if (OnMouseButtonDownEvent.IsBound())
		{
			OnMouseButtonDownEvent.Execute(MyGeometry,Event);
		}
		return FReply::Handled().CaptureMouse(SharedThis(this));
	}
	
	return  FReply::Unhandled();
}

FReply STouchTurn::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if(Event.GetPointerIndex() == _CapturedPointerIndex)
	{
		_touchMovePos = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
		CaluTouchInter(MyGeometry.Size);
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

void STouchTurn::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if(_CapturedPointerIndex != -1 || _sendTouchOneMore)
	{
		_sendTouchOneMore = false;
		const FGamepadKeyNames::Type XAxis = FGamepadKeyNames::RightAnalogX;
		const FGamepadKeyNames::Type YAxis = FGamepadKeyNames::RightAnalogY;
		
		
		//FSlateApplication::Get().SetAllUserFocusToGameViewport();
		FSlateApplication::Get().OnControllerAnalog(XAxis, 0, 0);
		FSlateApplication::Get().OnControllerAnalog(YAxis, 0, 0);

		APlayerController * pController = m_pTouchTurnUmg->GetWorld()->GetGameInstance()->GetFirstLocalPlayerController();;
		if(pController)
		{
			static  float fAutoRunDeltaTime = 1 / 60.f;
			
			FKey keyx(XAxis);
			pController->InputAxis(keyx, 0.f,fAutoRunDeltaTime,1, keyx.IsGamepadKey());
			FKey keyy(YAxis);
			pController->InputAxis(keyy, 0.f, fAutoRunDeltaTime, 1, keyy.IsGamepadKey());
		}

		
	}
}

FVector2D STouchTurn::ComputeDesiredSize(float) const
{
	return FVector2D(100, 100);
}

void STouchTurn::CaluTouchInter(const FVector2D& allSize)
{
	FVector2D ThumbScaledOffset = (_touchMovePos - _touchStartPos)/100;
	_touchStartPos = _touchMovePos;
	
	FVector2D NormalizedOffset = ThumbScaledOffset;

	const FGamepadKeyNames::Type XAxis = FGamepadKeyNames::RightAnalogX;
	const FGamepadKeyNames::Type YAxis = FGamepadKeyNames::RightAnalogY;
	float initValue = InitValue;
	float slopeValue = SlopeValue;
	if(IsValid(m_pTouchTurnUmg) && m_pTouchTurnUmg->GetWorld())
	{
		APlayerController * pController = m_pTouchTurnUmg->GetWorld()->GetGameInstance()->GetFirstLocalPlayerController();;
		if(pController)
		{
			static  float fAutoRunDeltaTime = 1 / 60.f;
			
			if (FMath::Abs(NormalizedOffset.X) <= SpeedUpLimitPixel/100 && FMath::Abs(NormalizedOffset.Y) <= SpeedUpLimitPixel/100)
			{
				slopeValue = 0;
			}
			FKey keyx(XAxis);
			pController->InputAxis(keyx, NormalizedOffset.X*initValue*(1+Ease(NormalizedOffset.X,0.6,slopeValue)), fAutoRunDeltaTime, 1, keyx.IsGamepadKey());
			FKey keyy(YAxis);
			pController->InputAxis(keyy, -NormalizedOffset.Y*initValue*InputPitchScale*(1+Ease(NormalizedOffset.Y,0.6,slopeValue)), fAutoRunDeltaTime, 1, keyy.IsGamepadKey());
		}
	}
}

float STouchTurn::Ease( float current,float duration, float changeValue)
{
	current = FMath::Abs(current);
	current /= duration;
	if (current > 1)
	{
		current = 1;	
	}
	return  changeValue * current * current;
}

void STouchTurn::SetOnMouseButtonDownEvent(FPointerEventHandler EventHandler)
{
	OnMouseButtonDownEvent = EventHandler;
}

void STouchTurn::SetOnMouseButtonUpEvent(FPointerEventHandler EventHandler)
{
	OnMouseButtonUpEvent = EventHandler;
}

