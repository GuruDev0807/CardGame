#ifndef  __CARDGAME_APP_DELEGATE_H__
#define  __CARDGAME_APP_DELEGATE_H__

#include "cocos2d.h"

class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    virtual bool applicationDidFinishLaunching();
    virtual void applicationDidEnterBackground();
    virtual void applicationWillEnterForeground();
    
    void completedCaptureAvatar(std::string file_name);
    void completedUploadAvatar(std::string avatarName);
    void completedDownloadAvatar(std::string file_name);
    void addBank(int plus_bank);
};

#endif // __CARDGAME_APP_DELEGATE_H__
