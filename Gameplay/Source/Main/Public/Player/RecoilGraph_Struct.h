// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RecoilGraph_Struct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct  FRecoilGraph_Struct:public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RGDT")
	bool Empty = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RGDT")
	TArray <FVector2D> pos;
};
