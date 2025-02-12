// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameplayCueNotify_Static.h"
#include "UnLuaBase.h"


void UCustomGameplayCueNotify_Static::PostCDOContruct()
{
	UnLua::ClassBindLuaFile(this,false);
}

FString UCustomGameplayCueNotify_Static::GetModuleName_Implementation() const
{
	return "";
}
