#include "AppDelegate.h"
#include "../Layer/LandingLayer.h"
#include "../Layer/LobbyLayer.h"
#include "../Layer/SettingLayer.h"
#include "../Layer/PurchaseLayer.h"

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(glContextAttrs);
}

static int register_all_packages()
{
    return 0;
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("CardGame", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("CardGame");
#endif
        director->setOpenGLView(glview);
    }
    
    director->setDisplayStats(false);
    director->setAnimationInterval(1.0f / 60);
    register_all_packages();
    
    if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    {
        auto scene = LobbyLayer::createScene();
        director->runWithScene(TransitionFade::create(1.0f, scene, Color3B(0, 0, 0)));
    }
    else
    {
        auto scene = LandingLayer::createScene();
        director->runWithScene(TransitionFade::create(1.0f, scene, Color3B(0, 0, 0)));
    }
    
    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
}

void AppDelegate::completedCaptureAvatar(std::string file_name)
{
    if(g_pCurrentLayer != NULL && g_pCurrentLayer->m_nType == LAYER_SETTING)
        ((SettingLayer*)g_pCurrentLayer)->updateProfile(file_name);
}

void AppDelegate::completedUploadAvatar(std::string avatarName)
{
    if(g_pCurrentLayer != NULL && g_pCurrentLayer->m_nType == LAYER_SETTING)
        ((SettingLayer*)g_pCurrentLayer)->uploadUserInfo(avatarName);
}

void AppDelegate::completedDownloadAvatar(std::string file_name)
{
    if(g_pCurrentLayer != NULL)
        g_pCurrentLayer->onDownloadedAvatar(file_name);
}

void AppDelegate::addBank(int plus_bank)
{
    if(g_pCurrentLayer != NULL && g_pCurrentLayer->m_nType == LAYER_PURCHASE)
        ((PurchaseLayer*)g_pCurrentLayer)->updateCoin(plus_bank);

/*    int bank_count = GameData::getInstance()->getBankCount();
    bank_count += plus_bank;
    GameData::getInstance()->setBankCount(bank_count);
    */
}
