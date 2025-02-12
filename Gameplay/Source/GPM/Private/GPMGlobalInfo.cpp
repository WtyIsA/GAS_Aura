// Fill out your copyright notice in the Description page of Project Settings.


#include "GPMGlobalInfo.h"
#include "GPMMonitor.h"
#include "GenericPlatform/GenericPlatformDriver.h"

namespace GMSDK
{
    GPMGlobalInfo::GPMGlobalInfo()
    {
    }

    GPMGlobalInfo::~GPMGlobalInfo()
    {
    }

    void GPMGlobalInfo::Init()
    {
        UGPMMonitor::LogGlobalInfo("memory_ram");
        UGPMMonitor::LogGlobalInfo("cpu_core");
        UGPMMonitor::LogGlobalInfo("cpu_frequency");
        UGPMMonitor::LogGlobalInfo("cpu_model");
        UGPMMonitor::LogGlobalInfo("memory_rom");
        FString GPUBrand = FPlatformMisc::GetPrimaryGPUBrand();
        FGPUDriverInfo GPUDriverInfo = FPlatformMisc::GetGPUDriverInfo(GPUBrand);
        FString GPUVendor = GPUDriverInfo.ProviderName;
        FString GPURHI = GPUDriverInfo.RHIName;
        UGPMMonitor::LogGlobalInfo("gpu_model", TCHAR_TO_UTF8(*GPUBrand));
        UGPMMonitor::LogGlobalInfo("gpu_vendor", TCHAR_TO_UTF8(*GPUVendor));
        UGPMMonitor::LogGlobalInfo("gl_version", TCHAR_TO_UTF8(*GPURHI));
    }
}

