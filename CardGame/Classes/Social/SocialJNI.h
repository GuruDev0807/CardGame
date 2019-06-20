
#ifndef __CARDGAME_SOCIAL_JNI_H__
#define __CARDGAME_SOCIAL_JNI_H__

extern "C"
{
    std::string getIMEI();
    void captureAvatar(std::string user_id);

    void nativeCompletedCaptureAvatar(JNIEnv *env, jobject obj, jstring file_path);
}

#endif //__CARDGAME_SOCIAL_JNI_H__