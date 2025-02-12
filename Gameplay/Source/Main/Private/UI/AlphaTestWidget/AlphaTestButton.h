//gjy

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "AlphaTestButton.generated.h"

UCLASS()
class MAIN_API UAlphaTestButton : public UButton
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AlphaTest")
	UTexture2D* HitTestTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AlphaTest", meta=(ClampMin="0", ClampMax="255"))
	uint8 TestAlpha;
	
protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;
};
