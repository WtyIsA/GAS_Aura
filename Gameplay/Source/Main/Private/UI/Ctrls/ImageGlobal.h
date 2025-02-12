#pragma once
#include "Components/Image.h"

#include "ImageGlobal.generated.h"

UCLASS()
class UImageGlobal : public  UImage
{
	GENERATED_UCLASS_BODY()
public:
	virtual void SynchronizeProperties() override;

	UPROPERTY(EditAnywhere,  Category=Global)
	int32  _imageid ;
};
