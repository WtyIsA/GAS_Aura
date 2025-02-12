// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameplayCueNotify_Actor.h"
#include "UnLuaBase.h"


void ACustomGameplayCueNotify_Actor::PostCDOContruct()
{
	UnLua::ClassBindLuaFile(this,false);
}

FString ACustomGameplayCueNotify_Actor::GetModuleName_Implementation() const
{
	return "";
}
