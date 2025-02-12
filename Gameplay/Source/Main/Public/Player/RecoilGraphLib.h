// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "RecoilGraph_Struct.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RecoilGraphLib.generated.h"

/**
 * 
 */
UCLASS()
class  URecoilGraphLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable, Category = "RPM_Functions", meta = (WorldContext = WorldContextObject))
    static UDataTable* RPM_AddRow(UDataTable* DataTable, FName RowName, FRecoilGraph_Struct Value);

	UFUNCTION(BlueprintCallable, Category = "RPM_Functions", meta = (WorldContext = WorldContextObject))
    static UDataTable* RPM_RemoveRow(UDataTable* DataTable, FName RowName);
};
