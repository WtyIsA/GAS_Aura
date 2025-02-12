#include "Components/ComboBoxString.h"

#include "ComboBoxStringEx.generated.h"
UCLASS()
class UComboBoxStringEx : public  UComboBoxString
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Style, meta=( DisplayName="Justification" ))
	TEnumAsByte<ETextJustify::Type> Justification;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Style, meta=( DisplayName="HideScrollbar" ))
	bool hideScrollbar = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Style, meta=( DisplayName="Down Arrow Transform" ))
	FWidgetTransform DownArrowTransform;

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> Item) const;

	/** The placement location of the summoned widget. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Menu Anchor")
	TEnumAsByte<EMenuPlacement> Placement;
	
	UFUNCTION(BlueprintCallable, Category = "Menu Anchor")
	void SetPlacement(TEnumAsByte<EMenuPlacement> InPlacement);

	UFUNCTION(BlueprintCallable, Category = "Menu Anchor")
	void SetDownArrowAngle(float InAngle);

	UFUNCTION(BlueprintCallable, Category = "Menu Anchor")
	void SetDownArrowRenderTransform(FWidgetTransform InDownArrowTransform);

private:
	void UpdateDownArrowRenderTransform();
};