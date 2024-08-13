// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// = 0 纯虚函数 当有了纯虚函数 这个类会变成抽象类：无法实例化对象，子类必须重写纯虚函数,如果子类没重写，子类也会变成抽象类
	virtual void HighlightActor() = 0;
	virtual void UnHighlightActor() = 0;
};
