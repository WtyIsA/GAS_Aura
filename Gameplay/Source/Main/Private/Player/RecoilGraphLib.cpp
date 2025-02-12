// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RecoilGraphLib.h"

UDataTable* URecoilGraphLib::RPM_AddRow(UDataTable* DataTable, FName RowName, FRecoilGraph_Struct Value)
{
	DataTable->AddRow(RowName,Value);
	return DataTable;
}

UDataTable* URecoilGraphLib::RPM_RemoveRow(UDataTable* DataTable, FName RowName)
{
	DataTable->RemoveRow(RowName);
	return DataTable;
}
