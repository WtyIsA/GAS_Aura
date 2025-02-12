

#include "SAlphaTestButton.h"

#include "SAlphaTestParent.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

bool SAlphaTestButton::CursorPosAlphaNotZero(const FVector2D& CursorPositionInScreenSpace) const
{
	if (!IsValid(AdvancedHitTexture))
	{
		return true;
	}

	bool AlphaNotZero = true;
	FVector2D LocalPosition = GetCachedGeometry().AbsoluteToLocal(CursorPositionInScreenSpace);
	LocalPosition.X = floor(LocalPosition.X);
	LocalPosition.Y = floor(LocalPosition.Y);
	LocalPosition /= GetCachedGeometry().GetLocalSize();

	if (LocalPosition.X >= 1.f || LocalPosition.Y >= 1.f || LocalPosition.X < 0.f || LocalPosition.Y < 0.f)
		return false;

	int ImageWidth = AdvancedHitTexture->GetSizeX();
	LocalPosition.X *= ImageWidth;
	LocalPosition.Y *= AdvancedHitTexture->GetSizeY();
	int BufferPosition = (static_cast<int>(LocalPosition.Y) * ImageWidth) + static_cast<int>(LocalPosition.X);

#if WITH_EDITOR
	TArray64<uint8>PicData;
	//UE_LOG(LogTemp,Log,TEXT("40 Time %lld"),UKismetMathLibrary::Now().GetTicks())
	AdvancedHitTexture->Source.GetMipData(PicData, 0);
	//UE_LOG(LogTemp,Log,TEXT("42 Time %lld"),UKismetMathLibrary::Now().GetTicks())
	//FColor* ImageData = (FColor*)PicData.GetData();
//
	//UE_LOG(LogTemp,Log,TEXT("45 Time %lld"),UKismetMathLibrary::Now().GetTicks())
	//if (ImageData && ImageData[BufferPosition].A <= TestAlpha) 
	//{
	//	AlphaNotZero = false;
	//}
	//if (PicData.Num() == 0)
	{
		auto bd = AdvancedHitTexture->GetPlatformMips()[0].BulkData;
		FColor* ImageData = static_cast<FColor*>(bd.Lock(LOCK_READ_ONLY));
		if (ImageData && ImageData[BufferPosition].A <= TestAlpha) 
		{
			AlphaNotZero = false;
		}
		bd.Unlock();
	}
#else
	auto bd = AdvancedHitTexture->GetPlatformMips()[0].BulkData;
	FColor* ImageData = (FColor*)(bd.Lock(LOCK_READ_ONLY));
	if (ImageData && ImageData[BufferPosition].A <= TestAlpha) 
	{
		AlphaNotZero = false;
	}
	bd.Unlock();
#endif
	return AlphaNotZero;
}

FReply SAlphaTestButton::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	FReply Reply = FReply::Unhandled();
	const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);
	const bool bMustBePressed = InputClickMethod == EButtonClickMethod::DownAndUp || InputClickMethod == EButtonClickMethod::PreciseClick;
	const bool bMeetsPressedRequirements = (!bMustBePressed || (IsPressed() && bMustBePressed));

	if (bMeetsPressedRequirements && ( ( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton || MouseEvent.IsTouchEvent())))
	{
		Release();

		if ( IsEnabled() )
		{
			if ( InputClickMethod == EButtonClickMethod::MouseDown )
			{
				// NOTE: If we're configured to click on mouse-down/precise-tap, then we never capture the mouse thus
				//       may never receive an OnMouseButtonUp() call.  We make sure that our bIsPressed
				//       state is reset by overriding OnMouseLeave().
			}
			else
			{
				bool bEventOverButton = IsHovered();

				if ( !bEventOverButton && MouseEvent.IsTouchEvent() )
				{
					bEventOverButton = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());
				}

				if ( bEventOverButton )
				{
					// If we asked for a precise tap, all we need is for the user to have not moved their pointer very far.
					const bool bTriggerForTouchEvent = InputClickMethod == EButtonClickMethod::PreciseClick;

					// If we were asked to allow the button to be clicked on mouse up, regardless of whether the user
					// pressed the button down first, then we'll allow the click to proceed without an active capture
					const bool bTriggerForMouseEvent = (InputClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture() || AnyParentHasCapture() );

					if ( ( bTriggerForTouchEvent || bTriggerForMouseEvent ) )
					{
						Reply = ExecuteOnClick();
					}
				}
			}
		}
		
		//If the user of the button didn't handle this click, then the button's
		//default behavior handles it.
		if ( Reply.IsEventHandled() == false )
		{
			Reply = FReply::Handled();
		}
	}

	//If the user hasn't requested a new mouse captor and the button still has mouse capture,
	//then the default behavior of the button is to release mouse capture.
	if ( ((Reply.GetMouseCaptor().IsValid() == false) && HasMouseCapture()) || AnyParentHasCapture() )
	{
		Reply.ReleaseMouseCapture();
	}

	return Reply;
}

const bool SAlphaTestButton::AnyParentHasCapture() const
{
	TSharedPtr<SWidget> CurWidget = Advanced_GetPaintParentWidget();
	while (CurWidget.IsValid())
	{
		if (CurWidget->GetVisibility() == EVisibility::Visible)
		{
			if (CurWidget->HasMouseCapture())
			{
				return true;
			}
		}

		CurWidget = CurWidget->Advanced_GetPaintParentWidget();
	}

	return false;
}
bool SAlphaTestButton::HasAlphaTestParent()
{
	TSharedPtr<SWidget> CurWidget = Advanced_GetPaintParentWidget();
	while (CurWidget.IsValid())
	{
		if (CurWidget.Get()->GetTypeAsString().Equals("SAlphaTestParent"))
		{
			return true;
		}

		CurWidget = CurWidget->Advanced_GetPaintParentWidget();
	}

	return false;
}

void SAlphaTestButton::CallButtonRelease()
{
	Release();
}

bool SAlphaTestButton::ShouldHandle(const FVector2D& CursorPositionInScreenSpace)
{
	if (!bHasAlphaTestParent.IsSet())
	{
		bHasAlphaTestParent = HasAlphaTestParent();
	}

	if (!bHasAlphaTestParent.Get(true))
	{
		if (!CursorPosAlphaNotZero(CursorPositionInScreenSpace))
		{
			return false;
		}
	}

	return true;
}

FReply SAlphaTestButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (ShouldHandle(MouseEvent.GetScreenSpacePosition()))
	{
		return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
	}

	return FReply::Unhandled().CaptureMouse( AsShared());
}

FReply SAlphaTestButton::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (ShouldHandle(InMouseEvent.GetScreenSpacePosition()))
	{
		return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	}

	return FReply::Unhandled();
}

FReply SAlphaTestButton::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) 
{
	/*Mouse enter or leave.-Start-*/
	if (!MouseEvent.GetCursorDelta().IsNearlyZero())
	{
		bool bCurrentShouldHandle = ShouldHandle(MouseEvent.GetScreenSpacePosition());
		if (!!bEnterLeave != bCurrentShouldHandle)
		{
			bEnterLeave = bCurrentShouldHandle ? 1 : 0;
			if (bCurrentShouldHandle)
			{
				SButton::OnMouseEnter(MyGeometry, MouseEvent);
			}
			else
			{
				SButton::OnMouseLeave(MouseEvent);
			}
		}
	}
	
	return SButton::OnMouseMove(MyGeometry, MouseEvent);
}

void SAlphaTestButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//do nothing when not attached to AlpahTestParent, do real enter in move.
	if (!bHasAlphaTestParent.IsSet())
	{
		bHasAlphaTestParent = HasAlphaTestParent();
	}
	
	if (!bHasAlphaTestParent.Get(false))
	{
		return;
	}

	SButton::OnMouseEnter(MyGeometry,MouseEvent);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
