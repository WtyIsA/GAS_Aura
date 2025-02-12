#include "CallJaveObjectMethod.h"
#if PLATFORM_ANDROID


void FCallJaveObjectMethod::CallVoidVoidMethod(const char* methodName)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "()V");
		if(MethonId != 0)
		{
			return FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId);
		}
	}
}

bool FCallJaveObjectMethod::CallBoolVoidMethod(const char* methodName)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "()Z");
		if(MethonId != 0)
		{
			return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, MethonId);
		}
	}
	return false;
}

bool FCallJaveObjectMethod::CallBoolFStringMethod(const char* methodName, const FString& param1)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(Ljava/lang/String;)Z");
		if(MethonId != 0)
		{
			auto javaparam1 = FJavaHelper::ToJavaString(Env, param1);
			return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaparam1);
		}
	}
	return false;
}

void FCallJaveObjectMethod::CallVoidBoolMethod(const char* methodName, bool param)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(Z)V");
		if(MethonId != 0)
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,param);
	}
}

int FCallJaveObjectMethod::CallIntVoidMethod(const char* methodName)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "()I");
		if(MethonId != 0)
		{
			return FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, MethonId);
		}
	}
	return 0;	
}

void FCallJaveObjectMethod::CallVoidLongMethod(const char* methodName, uint64 param)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(J)V");
		if(MethonId != 0)
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,param);
	}
}

void FCallJaveObjectMethod::CallVoidIntMethod(const char* methodName, int32 param)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(I)V");
		if(MethonId != 0)
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,param);
	}
}

double FCallJaveObjectMethod::CallDoubleVoidMethod(const char* methodName)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "()D");
		if(MethonId != 0)
			return FJavaWrapper::CallDoubleMethod(Env, FJavaWrapper::GameActivityThis, MethonId);
	}
	return 0.0f;
}

FString FCallJaveObjectMethod::CallStringVoidMethod(const char* methodName)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "()Ljava/lang/String;");
		if(MethonId != 0)
		{
			jstring strRet = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, MethonId);
			FString ueStrRet = FJavaHelper::FStringFromParam(Env, strRet);
			return ueStrRet;
		}
	}
	return FString();
}

jmethodID FCallJaveObjectMethod::GetJavaMethod(JNIEnv* Env,const char* methodName, const char* Singnature)
{
	if(Env == nullptr ||  methodName == nullptr || Singnature == nullptr)
		return 0;
	
	bool bIsOptional = false;
	jmethodID MethonId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, methodName, Singnature, bIsOptional);
	return MethonId;
}

void FCallJaveObjectMethod::CallVoidOneStringMethod(const char* methodName, const FString& param1)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(Ljava/lang/String;)V");
		if(MethonId != 0)
		{
			auto javaparam1 = FJavaHelper::ToJavaString(Env, param1);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaparam1);
		}		
	}
}

void FCallJaveObjectMethod::CallVoidTwoStringsMethod(const char* methodName, const FString& param1, const FString& param2)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(Ljava/lang/String;Ljava/lang/String;)V");
		if(MethonId != 0)
		{
			auto javaparam1 = FJavaHelper::ToJavaString(Env, param1);
			auto javaparam2 = FJavaHelper::ToJavaString(Env, param2);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaparam1, *javaparam2);
		}		
	}
}

void FCallJaveObjectMethod::CallVoidThreeStringsMethod(const char* methodName, const FString& param1, const FString& param2, const FString& param3)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		if(MethonId != 0)
		{
			auto javaparam1 = FJavaHelper::ToJavaString(Env, param1);
			auto javaparam2 = FJavaHelper::ToJavaString(Env, param2);
			auto javaparam3 = FJavaHelper::ToJavaString(Env, param3);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaparam1, *javaparam2, *javaparam3);
		}		
	}
}

void FCallJaveObjectMethod::CallVoidFourStringsMethod(const char* methodName, const FString& param1, const FString& param2, const FString& param3, const FString& param4)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = GetJavaMethod(Env,methodName, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		if(MethonId != 0)
		{
			auto javaparam1 = FJavaHelper::ToJavaString(Env, param1);
			auto javaparam2 = FJavaHelper::ToJavaString(Env, param2);
			auto javaparam3 = FJavaHelper::ToJavaString(Env, param3);
			auto javaparam4 = FJavaHelper::ToJavaString(Env, param4);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaparam1, *javaparam2, *javaparam3, *javaparam4);
		}		
	}
}

#endif