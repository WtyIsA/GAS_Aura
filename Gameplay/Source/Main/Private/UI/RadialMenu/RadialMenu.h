// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialMenu.generated.h"

UENUM(BlueprintType)
enum class EWidgetCenter : uint8
{
	ScreenCenter 		UMETA(DisplayName = "Center of the screen"),
	MousePosition		UMETA(DisplayName = "Mouse position")

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRadialButtonClickEvent, int32,btnId);

/**
 * 
 */
UCLASS()
class  URadialMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called every frame
	
#pragma region Variables
private:	

	UPROPERTY()
		int32 CurrentSegmentID;

public:
	/*
	 * 选项之间的间隔度数，360
	 */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float fInterval = 2;
	UPROPERTY(EditAnywhere, Category = "Parameters")
	FVector2D btnSize = FVector2D(300,300);
	/*响应鼠标事件的最小半径*/
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float clickRadiaMin = 0;
	/*响应鼠标事件的最大半径*/
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float clickRadiaMax = 150;
public:
	//Collection of all widget radial buttons.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buttons")
		TMap<int32, class URadialMenu_Button*>Buttons;

#pragma endregion Variables


#pragma region Functions
protected:
	virtual bool Initialize() override;
public:

	//Destroy menu.
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void Destructor();


	/*Make hover effect over the button - Gamepad
	*@ThumbValue_X - Value between -1 and 1 depending on thumbstick leaning left or right 
	*@ThumbValue_Y - Value between -1 and 1 depending on thumbstick leaning up or down 
	*/
	UFUNCTION(BlueprintCallable,Category = "Functions")
		void PreselectButtonGamepad(float ThumbValue_X, float ThumbValue_Y);

	//Makes button active
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void ActivateButton(int32 ActivateIndex);
	//Makes button disabled
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void DisableButton(int32 ActivateIndex);
	//Deactivate all buttons
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void DeactivateButtons();

	//Calls hover event on interface
	UFUNCTION(Category = "Functions")
		virtual void HoverOverEvent();
	
	//Calls blueprint native event inside this widget
	UFUNCTION(BlueprintNativeEvent, Category = "Functions")
		void OnHoverOver();
	//Set default bindings for this widget (left click is select)

	UPROPERTY()
	FOnRadialButtonClickEvent OnClicked;
#pragma endregion Functions
	float GetCurrentSegment(float angle);
	float GetCurrentSegment(float x1,float y1, float centerX,float centerY, float scale);
	float GetCurrentSegment(const FVector2D& mousePos, const FVector2D& centerPos, float scale);
public:
	virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent );
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent );
	
	UFUNCTION()
	void SetButtonPath(FString path);
	UFUNCTION()
	void CreateButtons(int32 nCount);
	UFUNCTION()
	void SetButtonCount(int32 nCount);
	UFUNCTION()
	void CreateOneButton(int32 index, int32 nButtonId, FString iconPath, FString tip);

private:
	UClass*	  _buttonClass;
	float	 _radiaLen;
	float	 _clickRadiaMin;
	float	 _clickRadiaMax;
	float	 _nPerRadia;
	float    _nPercent;
};
