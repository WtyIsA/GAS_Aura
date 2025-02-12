// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/
#pragma once

#include "CoreMinimal.h"

#include "RadialMenu_Button.h"
#include "Blueprint/UserWidget.h"
#include "RadialMultLevelMenu.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMultRadialButtonClickEvent, int32,menuLevel, int32,btnId);

USTRUCT(BlueprintType)
struct FMultLevelMenuParam
{
	GENERATED_BODY()
	/*
	* 选项之间的间隔度数，360
	*/
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float Interval = 2;
	UPROPERTY(EditAnywhere, Category = "Parameters")
	FVector2D btnSize = FVector2D(300,300);
	UPROPERTY(EditAnywhere, Category = "Parameters")
	FVector2D Scale = FVector2D(1,1);
	/*响应鼠标事件的最小半径*/
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float ClickRadiaMin = 100;
	/*响应鼠标事件的最大半径*/
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float ClickRadiaMax = 150;
	/*按钮的icon离中心的距离*/
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float	  IconRadiaLen =100;
	/*按钮的icon大小*/
	UPROPERTY(EditAnywhere, Category = "Parameters")
	FVector2D iconSize = FVector2D(50,50);
	UPROPERTY(EditAnywhere, Category = "Parameters" ,meta = (MetaClass=URadialMenu_Button ))
	UClass*	  ButtonClass;
};

struct FMultMenuCalcParam
{
	TMap<int32, class URadialMenu_Button*>Buttons;
	float	 _fClickRadiaMin = 0;
	float	 _fClickRadiaMax = 0;
	float	 _nPerRadia = 0;
	float    _nPercent = 0;
	int32    _nCurrentSegmentID = -1;
	FVector2D	_iconPos;
};

/**
 * 
 */
UCLASS()
class  URadialMultLevelMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called every frame
	
#pragma region Variables
private:	

public:
	/*
	 * 选项之间的间隔度数，360
	 */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	TArray<struct  FMultLevelMenuParam>  MenuParams;
public:
	//Collection of all widget radial buttons.		

#pragma endregion Variables


#pragma region Functions
protected:
	virtual bool Initialize() override;
public:

	//Destroy menu.
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void Destructor();


	//Makes button active
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void ActivateButton(int32 menuLevel, int32 ActivateIndex);
	//Makes button disabled
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void DisableButton(int32 menuLevel,int32 ActivateIndex);
	//Deactivate all buttons
	UFUNCTION(BlueprintCallable, Category = "Functions")
		void DeactivateButtons(int32 menuLevel);

	//Calls hover event on interface
	UFUNCTION(Category = "Functions")
		virtual void HoverOverEvent();
	
	//Calls blueprint native event inside this widget
	UFUNCTION(BlueprintNativeEvent, Category = "Functions")
		void OnHoverOver();
	//Set default bindings for this widget (left click is select)

	UPROPERTY()
	FOnMultRadialButtonClickEvent OnClicked;
	UPROPERTY()
	FOnMultRadialButtonClickEvent OnMenuHover;
	UPROPERTY()
	FOnMultRadialButtonClickEvent OnMenuLeave;
#pragma endregion Functions
	int32 GetCurrentSegment(float x1,float y1, float centerX,float centerY, float scale, int32& buttonId);
	int32 GetCurrentSegment(const FVector2D& mousePos, const FVector2D& centerPos, float scale, int32& buttonId);
public:
	virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent );
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent );
		
	UFUNCTION()
	void SetButtonCount(int32 nMenuLevel, int32 nCount);
	UFUNCTION()
	UObject* CreateOneButton(int32 nMenuLevel,int32 index, int32 nButtonId,  FString tip);
private:
	FName GetMenuLevelName(int32 nLevel);
private:
	FString _tipText;
	int32	_nCurrentMenuLevel;
	TArray<FMultMenuCalcParam> m_menuCaluParam;
};
