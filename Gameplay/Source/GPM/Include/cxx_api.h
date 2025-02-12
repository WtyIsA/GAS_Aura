//
// Created by 达文西 on 2020/3/13.
//

#ifndef GPM_CXX_API_H
#define GPM_CXX_API_H

#include <cstdint>

/**
 * @brief c++层接口
 */
extern "C" {
__attribute__((visibility("default"))) void GPM_CXX_Init(const char *filePath);
__attribute__((visibility("default"))) void GPM_CXX_SetSwitch(bool fpsSwitch, bool cpuSwitch, bool memorySwitch, bool threadSwitch);
__attribute__((visibility("default"))) void GPM_CXX_SetInterval(uint32_t cpuInterval, uint32_t memoryInterval, uint32_t threadInterval);
__attribute__((visibility("default"))) void GPM_CXX_SetAPP(uint32_t aid, uint32_t region);
__attribute__((visibility("default"))) void GPM_CXX_LogSceneStart(const char *type, const char *name);
__attribute__((visibility("default"))) void GPM_CXX_LogSceneLoaded(const char *type, const char *name);
__attribute__((visibility("default"))) void GPM_CXX_LogSceneEnd(const char *type, const char *name, bool isUpload);
__attribute__((visibility("default"))) void GPM_CXX_LogFrameEnd();
__attribute__((visibility("default"))) void GPM_CXX_LogCPUUsage();
__attribute__((visibility("default"))) void GPM_CXX_LogMemoryUsage();
__attribute__((visibility("default"))) void GPM_CXX_LogThreadCount();
__attribute__((visibility("default"))) void GPM_CXX_LogGlobalInfoStr(const char *type, const char *key, const char *value);
__attribute__((visibility("default"))) void GPM_CXX_LogGlobalInfoInt(const char *type, const char *key, int value);
__attribute__((visibility("default"))) void GPM_CXX_LogGlobalInfoKey(const char *type, const char *key);
__attribute__((visibility("default"))) void GPM_CXX_LogSceneInfoStr(const char *type, const char *name, const char *key, const char *value);
__attribute__((visibility("default"))) void GPM_CXX_LogSceneInfoInt(const char *type, const char *name, const char *key, int value);
__attribute__((visibility("default"))) void GPM_CXX_LogSceneInfoKey(const char *type, const char *name, const char *key);
__attribute__((visibility("default"))) int GPM_CXX_RequestGraphicLevel();
__attribute__((visibility("default"))) void GPM_CXX_LogSceneReport(const char *type, const char* name, const char* data);
}

#endif //GPM_CXX_API_H
