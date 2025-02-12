// Fill out your copyright notice in the Description page of Project Settings.


#include "GPMGraphicLevel.h"
#include "GPMCXXBridge.h"

namespace GMSDK
{
	GPMGraphicLevel::GPMGraphicLevel()
	{
	}

	GPMGraphicLevel::~GPMGraphicLevel()
	{
	}

	int GPMGraphicLevel::RequestGraphicLevel()
	{
		return GPMCXXBridge::RequestGraphicLevel();
	}

}
