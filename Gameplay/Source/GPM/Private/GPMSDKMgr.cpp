// Fill out your copyright notice in the Description page of Project Settings.


#include "GPMSDKMgr.h"


GPMSDKMgr::GPMSDKMgr()
{
    if (SDK == nullptr)
    {
        SDK = new BaseGPMSDK();
    }
}

GPMSDKMgr::~GPMSDKMgr()
{
	delete SDK;
}

GPMSDKMgr& GPMSDKMgr::GetInstance()
{
	static GPMSDKMgr Instance;
	return Instance;
}

BaseGPMSDK& GPMSDKMgr::GetSDK()
{
    return *SDK;
}
