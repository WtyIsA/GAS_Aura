// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGPMSDK.h"
using namespace GMSDK;
/**
 * 
 */
class GPM_API GPMSDKMgr
{
public:
	GPMSDKMgr(const GPMSDKMgr&) = delete;
	GPMSDKMgr& operator=(const GPMSDKMgr&) = delete;
	static GPMSDKMgr& GetInstance();
	~GPMSDKMgr();
	BaseGPMSDK& GetSDK();
private:
	BaseGPMSDK* SDK = nullptr;
	GPMSDKMgr();
};
