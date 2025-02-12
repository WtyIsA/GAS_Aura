#pragma once

#include "Tickable.h"
#include "lua.hpp"

class FLuaGcTickable: public FTickableGameObject
{
public:
	FLuaGcTickable();
	
	virtual ~FLuaGcTickable(){}
	
	virtual void Tick( float DeltaTime );
	
	virtual TStatId GetStatId() const{ return TStatId(); }

	virtual ETickableTickType GetTickableTickType() const { return ETickableTickType::Always; }

private:
	lua_State* LuaState;
	
	double NextGcTime;
	
	double GcOnceTime;
};
