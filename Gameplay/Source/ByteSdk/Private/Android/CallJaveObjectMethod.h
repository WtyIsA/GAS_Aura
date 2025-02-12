#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
class FCallJaveObjectMethod
{
public:
	//return void,arg void
	static void CallVoidVoidMethod(const char* methodName);
	//return bool, arg void
	static bool CallBoolVoidMethod(const char* methodName);
	//return bool, arg FString
	static bool CallBoolFStringMethod(const char* methodName, const FString& param1);
	//return void, arg bool
	static void CallVoidBoolMethod(const char* methodName, bool param);
	//return int, arg void
	static int CallIntVoidMethod(const char* methodName);
	//return void,arg int
	static void CallVoidIntMethod(const char* methodName, int32 param);
	//return void,arg Long
	static void CallVoidLongMethod(const char* methodName, uint64 param);
	//return FString,arg Void
	static FString CallStringVoidMethod(const char* methodName);
	//return double,arg void
	static double CallDoubleVoidMethod(const char* methodName);
	//return void,arg string 
	static void CallVoidOneStringMethod(const char* methodName, const FString& param1);
	//return void,arg string string 
	static void CallVoidTwoStringsMethod(const char* methodName, const FString& param1, const FString& param2);
	//return void,arg string string string
	static void CallVoidThreeStringsMethod(const char* methodName, const FString& param1, const FString& param2, const FString& param3);
	//return void,arg string string string string
	static void CallVoidFourStringsMethod(const char* methodName, const FString& param1, const FString& param2, const FString& param3, const FString& param4);
	static jmethodID GetJavaMethod(JNIEnv* Env, const char* methodName, const char* Singnature);
};

#endif