
#include "../Common/Common.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#endif

#include "SocialJni.h"
#include "../Layer/SettingLayer.h"

#define CLASS_NAME "com/mcmahon/cardgame/AppActivity"

extern "C"
{
    std::string getIMEI()
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        JniMethodInfo methodInfo;

#ifdef ANDROID_AMAZON
#else
        if(! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "getIMEI", "()Ljava/lang/String;"))
#endif
            return "";

        jstring device_id = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        std::string result = JniHelper::jstring2string(device_id);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return result;
#endif
    }

    void captureAvatar(std::string user_id)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        JniMethodInfo methodInfo;

#ifdef ANDROID_AMAZON
#else
        if(! JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "captureAvatar", "(Ljava/lang/String;)V"))
#endif
            return;

        jstring param = methodInfo.env->NewStringUTF(user_id.c_str());

        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, param);
        methodInfo.env->DeleteLocalRef(param);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
#endif
    }

    void nativeCompletedCaptureAvatar(JNIEnv *env, jobject obj, jstring file_path)
    {
        jboolean isCopy;
        const char* path;
        path = env->GetStringUTFChars(file_path, &isCopy);

        if(g_pCurrentLayer != NULL && g_pCurrentLayer->m_nType == LAYER_SETTING)
            ((SettingLayer*)g_pCurrentLayer)->updateProfile(path);
    }
}

