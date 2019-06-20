#include "LobbyLayer.h"
#include "MessageLayer.h"
#include "SelectGameLayer.h"
#include "SettingLayer.h"
#include "NotificationLayer.h"
#include "FriendLayer.h"
#include "PurchaseLayer.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/Notification.h"
#include "../Widget/MyMenuItem.h"
#include "../Xml/NotificationParser.h"

enum LOBBY_STEP
{
    LS_CONNECTING = 0,
    LS_LOADING = 1,
    LS_LOBBY = 2
};

enum LOBBY_TAG
{
    LOT_LOGO            = 0,
    LOT_LABEL_STATUS    = 1,
    LOT_TOP             = 2,
    LOT_BOTTOM          = 3,
    LOT_MENU_MAIN       = 4,
    LOT_AVATAR          = 5,
    LOT_LABEL_NAME      = 6,
    LOT_LABEL_COIN      = 7,
    LOT_LABEL_LEVEL     = 8,
    LOT_LABEL_FRIEND    = 9,
    LOT_LABEL_MAIL      = 10,
    LOT_ACTIVE_FRIEND   = 11,
    LOT_ACTIVE_MAIL     = 12,
    LOT_MENU_DIALY      = 13,
    LOT_COIN_ANIM       = 14
};

Scene* LobbyLayer::createScene()
{
    Scene* scene = Scene::create();
    LobbyLayer* layer = LobbyLayer::create();
    scene->addChild(layer);
    return scene;
}

LobbyLayer::~LobbyLayer()
{
}

bool LobbyLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_LOBBY;
    if(!GameData::getInstance()->isConnected())
        m_nStep = LS_CONNECTING;
    else
        m_nStep = LS_LOBBY;
    m_nTimeIndex = 0;
    g_pCurrentLayer = this;
    
    createBackground();
    createLabel();
    createMenu();
    
    if(m_nStep == LS_CONNECTING)
    {
        getChildByTag(LOT_TOP)->setVisible(false);
        getChildByTag(LOT_BOTTOM)->setVisible(false);
        getChildByTag(LOT_MENU_MAIN)->setVisible(false);
        getChildByTag(LOT_MENU_DIALY)->setVisible(false);
        getChildByTag(LOT_COIN_ANIM)->setVisible(false);
    }
    else
    {
        getChildByTag(LOT_LOGO)->setVisible(false);
        getChildByTag(LOT_LABEL_STATUS)->setVisible(false);
        updateUserInfo();
        
        Menu* pMenuDialy = (Menu*) getChildByTag(LOT_MENU_DIALY);
        Sprite* pCoinAnimSpr = (Sprite*) getChildByTag(LOT_COIN_ANIM);
        
        if(GameData::getInstance()->isFreeCoin())
        {
            pMenuDialy->setVisible(true);
            pCoinAnimSpr->setVisible(true);
        }
        else
        {
            pMenuDialy->setVisible(false);
            pCoinAnimSpr->setVisible(false);
        }
    }
    
    return true;
}

void LobbyLayer::onEnter()
{
    BaseLayer::onEnter();
    
    if(m_nStep == LS_CONNECTING)
    {
        if(CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    }
    else
    {
        if(!CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/Bgm1.mp3", true);
        CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(GameData::getInstance()->getBgmEnable() ? 1.0f : 0.001f);
        CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(GameData::getInstance()->getEffectEnable() ? 1.0f : 0.001f);
    }

    if(m_nStep == LS_CONNECTING)
        startConnecting();
}

void LobbyLayer::onNetClientConnected()
{
    GameData::getInstance()->setConnected(true);
    loadUserInfo();
}

void LobbyLayer::onNetClientError(std::string error)
{
    Label* pLabelStatus = (Label*) getChildByTag(LOT_LABEL_STATUS);
    pLabelStatus->setString("");
    unschedule(schedule_selector(LobbyLayer::updateStatus));
    
    BaseLayer::onNetClientError(error);
}

void LobbyLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == PT_RES_USERINFO)
    {
        unschedule(schedule_selector(LobbyLayer::updateStatus));
        
        UserInfo* pUserInfo = PacketParser::getInstance()->parseUserInfo(message);
        if(pUserInfo == NULL) {
            unschedule(schedule_selector(LobbyLayer::updateStatus));
            Label* pLabelStatus = (Label*) getChildByTag(LOT_LABEL_STATUS);
            pLabelStatus->setString("");
            createMessageLayer(MSG_USERINFO_FAILED);
            return;
        }
        
        GameData::getInstance()->setUserInfo(pUserInfo);
        GameData::getInstance()->sendPacket(REQUEST_NOTIFICATION, PacketParser::getInstance()->makeRequestNotificationList(pUserInfo->m_strId));
    }
    else if(packet_name == RESPONSE_NOTIFICATION)
    {
        unschedule(schedule_selector(LobbyLayer::updateStatus));
        
        m_nStep = LS_LOBBY;
        updateUserInfo();
        showMainMenu();
    }
    else if(packet_name == NOTIFY_NOTIFICATION)
    {
        updateUserInfo();
    }
    else if(packet_name == NOTIFY_UPDATE_PROFIE)
    {
        updateUserInfo();
    }
    else if(packet_name == RESPONSE_UPDATE_COIN)
    {
        hideLoading();
        UserInfo* pUserInfo = PacketParser::getInstance()->parseUserInfo(message);
        GameData::getInstance()->setUserInfo(pUserInfo);
        GameData::getInstance()->updateDialyTime();
        
        getChildByTag(LOT_COIN_ANIM)->setVisible(false);
        getChildByTag(LOT_MENU_DIALY)->setVisible(false);
        
        Label* pLabelCoin = (Label*) getChildByTag(LOT_TOP)->getChildByTag(LOT_LABEL_COIN);
        pLabelCoin->setString(getStringFromInt(pUserInfo->m_nCoin));
    }
}

void LobbyLayer::OnPlay1(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Vector<UserInfo*> vecFriends;
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectGameLayer::createScene(ROOM_TYPE_1, vecFriends), Color3B(0, 0, 0)));
}

void LobbyLayer::OnPlay6(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Vector<UserInfo*> vecFriends;
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectGameLayer::createScene(ROOM_TYPE_5, vecFriends), Color3B(0, 0, 0)));
}

void LobbyLayer::OnPlayFriend(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, FriendLayer::createScene(FST_PLAY), Color3B(0, 0, 0)));
}

void LobbyLayer::OnSetting(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SettingLayer::createScene(), Color3B(0, 0, 0)));
}

void LobbyLayer::OnFriendList(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, FriendLayer::createScene(FST_REMOVE), Color3B(0, 0, 0)));
}

void LobbyLayer::OnNotificationList(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, NotificationLayer::createScene(), Color3B(0, 0, 0)));
}

void LobbyLayer::OnAddCoin(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, PurchaseLayer::createScene(), Color3B(0, 0, 0)));
}

void LobbyLayer::OnFreeCoins(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    showLoading();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(REQUEST_UPDATE_COIN, PacketParser::getInstance()->makeRequestUpdateCoinReq(pUserInfo->m_strId, 200));
}

void LobbyLayer::updateStatus(float dt)
{
    std::string strStatus = "";
    if(m_nStep == LS_CONNECTING)
        strStatus = "Connecting to server";
    else if(m_nStep == LS_LOADING)
        strStatus = "Loading data from server";
    
    m_nTimeIndex ++;
    if(m_nTimeIndex > 3)
        m_nTimeIndex = 1;
    
    switch (m_nTimeIndex) {
        case 1:
            strStatus += " .";
            break;
        case 2:
            strStatus += " ..";
            break;
        case 3:
            strStatus += " ...";
            break;
        default:
            break;
    }

    Label* pLabelStatus = (Label*) getChildByTag(LOT_LABEL_STATUS);
    pLabelStatus->setString(strStatus);
}

void LobbyLayer::startConnecting()
{
    m_nStep = LS_CONNECTING;
    m_nTimeIndex = 0;
    
    Label* pLabelStatus = (Label*) getChildByTag(LOT_LABEL_STATUS);
    pLabelStatus->setPosition(Vec2(m_szDevice.width / 2 - 213.0f * m_fSmallScale, m_szDevice.height / 2 - 245.0f * m_fSmallScale));
    
    schedule(schedule_selector(LobbyLayer::updateStatus), 0.5f);
    
    connectToServer();
}

void LobbyLayer::loadUserInfo()
{
    m_nStep = LS_LOADING;
    m_nTimeIndex = 0;
    
    Label* pLabelStatus = (Label*) getChildByTag(LOT_LABEL_STATUS);
    pLabelStatus->setString("");
    pLabelStatus->setPosition(Vec2(m_szDevice.width / 2 - 255.0f * m_fSmallScale, m_szDevice.height / 2 - 245.0f * m_fSmallScale));
    
    std::string device_id = "";
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    device_id = getUDID();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    device_id = getIMEI();
#endif
    
    if(device_id == "")
        return;
    
    GameData::getInstance()->sendPacket(PT_REQ_USERINFO, PacketParser::getInstance()->makeUserInfo(device_id));
}

void LobbyLayer::createBackground()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprLogo = Sprite::create("img/lobby/logo.png");
    sprLogo->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprLogo->setScale(m_fSmallScale);
    sprLogo->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 40.0f * m_fSmallScale));
    sprLogo->setTag(LOT_LOGO);
    addChild(sprLogo, LAYER_ZORDER1);
    
    auto sprTop = Sprite::create("img/lobby/lobby_top.png");
    sprTop->setAnchorPoint(Vec2(0.5f, 1.0f));
    sprTop->setScale(m_fScaleX);
    sprTop->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height));
    sprTop->setTag(LOT_TOP);
    addChild(sprTop, LAYER_ZORDER2);
    
    auto sprBottom = Sprite::create("img/lobby/lobby_bottom.png");
    sprBottom->setAnchorPoint(Vec2(0.5f, 0.0f));
    sprBottom->setScale(m_fScaleX);
    sprBottom->setPosition(Vec2(m_szDevice.width / 2, 0.0f));
    sprBottom->setTag(LOT_BOTTOM);
    addChild(sprBottom, LAYER_ZORDER2);
    
    Size topSize = sprTop->getContentSize();
    
    auto avatarLayer = createAvatarLayer("img/setting/profile_default.png");
    avatarLayer->setTag(LOT_AVATAR);
    sprTop->addChild(avatarLayer, LAYER_ZORDER1);
    
    auto sprAvatarBorder = Sprite::create("img/lobby/lobby_avatar_border.png");
    sprAvatarBorder->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprAvatarBorder->setPosition(Vec2(121.0f, topSize.height - 61.5f));
    sprTop->addChild(sprAvatarBorder, LAYER_ZORDER2);
    
    auto sprActiveFriend = Sprite::create("img/lobby/btn_friend_new.png");
    sprActiveFriend->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprActiveFriend->setPosition(Vec2(topSize.width - 301.0f, topSize.height - 64.0f));
    sprActiveFriend->setTag(LOT_ACTIVE_FRIEND);
    sprTop->addChild(sprActiveFriend, LAYER_ZORDER3);
    sprActiveFriend->setVisible(false);
    
    auto sprActiveMail = Sprite::create("img/lobby/btn_email_new.png");
    sprActiveMail->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprActiveMail->setPosition(Vec2(topSize.width - 177.0f, topSize.height - 64.0f));
    sprActiveMail->setTag(LOT_ACTIVE_MAIL);
    sprTop->addChild(sprActiveMail, LAYER_ZORDER3);
    sprActiveMail->setVisible(false);
    
    auto sprCoinAnim = Sprite::create("img/lobby/coin_anim1.png");
    sprCoinAnim->setScale(m_fSmallScale);
    sprCoinAnim->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprCoinAnim->setPosition(Vec2(m_szDevice.width - 252.0f * m_fSmallScale, m_szDevice.height / 2 + 179.0f * m_fSmallScale));
    sprCoinAnim->setTag(LOT_COIN_ANIM);
    addChild(sprCoinAnim, LAYER_ZORDER4);

    auto anim1 = SpriteFrame::create("img/lobby/coin_anim1.png", Rect(0, 0, 58, 59));
    auto anim2 = SpriteFrame::create("img/lobby/coin_anim2.png", Rect(0, 0, 58, 59));
    auto anim3 = SpriteFrame::create("img/lobby/coin_anim3.png", Rect(0, 0, 58, 59));
    auto anim4 = SpriteFrame::create("img/lobby/coin_anim4.png", Rect(0, 0, 58, 59));
    auto anim5 = SpriteFrame::create("img/lobby/coin_anim5.png", Rect(0, 0, 58, 59));
    auto anim6 = SpriteFrame::create("img/lobby/coin_anim6.png", Rect(0, 0, 58, 59));
    auto anim7 = SpriteFrame::create("img/lobby/coin_anim7.png", Rect(0, 0, 58, 59));
    
    Vector<SpriteFrame*> animFrames(7);
    animFrames.pushBack(anim1);
    animFrames.pushBack(anim2);
    animFrames.pushBack(anim3);
    animFrames.pushBack(anim4);
    animFrames.pushBack(anim5);
    animFrames.pushBack(anim6);
    animFrames.pushBack(anim7);
    
    auto movie_animation = Animation::createWithSpriteFrames(animFrames, 0.08f);
    auto movie_animator = Animate::create(movie_animation);
    sprCoinAnim->runAction(RepeatForever::create(movie_animator));
}

Layer* LobbyLayer::createAvatarLayer(std::string fileName)
{
    Sprite* sprTop = (Sprite*) getChildByTag(LOT_TOP);
    Size topSize = sprTop->getContentSize();
    
    auto content_spr = Sprite::create(fileName);
    content_spr->setScale(0.5f);
    
    auto stencil = DrawNode::create();
    stencil->drawSolidCircle(Vec2(0.0f, 0.0f), 85.0f / 2, 0.0f, 32.0f, Color4F::GREEN);
    
    auto clipper = ClippingNode::create();
    clipper->setAnchorPoint(Vec2(0.5f, 0.5f));
    clipper->setPosition(Vec2(122.5f, topSize.height - 60.5f));
    clipper->setStencil(stencil);
    clipper->addChild(content_spr);
    
    auto avatar_layer = Layer::create();
    avatar_layer->addChild(clipper);
    return avatar_layer;
}

void LobbyLayer::createLabel()
{
    TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 50.0f * m_fSmallScale);
    TTFConfig ttfConfig1("fonts/ImperialStd-Regular.ttf", 25.0f);
    TTFConfig ttfConfig2("fonts/ImperialStd-Regular.ttf", 50.0f);
    Color3B color = Color3B(255, 255, 255);
    Color3B color1 = Color3B(249, 227, 125);
    Color3B color2 = Color3B(142, 155, 169);
    
    auto labelStatus = Label::createWithTTF(ttfConfig, "", TextHAlignment::LEFT, m_szDevice.width);
    labelStatus->setAnchorPoint(Vec2(0.0f, 0.5f));
    labelStatus->setPosition(Vec2(m_szDevice.width / 2 - 213.0f * m_fSmallScale, m_szDevice.height / 2 - 245.0f * m_fSmallScale));
    labelStatus->setColor(color);
    labelStatus->setTag(LOT_LABEL_STATUS);
    addChild(labelStatus, LAYER_ZORDER1);
    
    Sprite* pTopSprite = (Sprite*) getChildByTag(LOT_TOP);
    Size topSize = pTopSprite->getContentSize();
    
    auto labelName = Label::createWithTTF(ttfConfig2, "", TextHAlignment::LEFT, m_szDevice.width);
    labelName->setAnchorPoint(Vec2(0.0f, 0.5f));
    labelName->setPosition(Vec2(190.0f, topSize.height - 60.5f));
    labelName->setColor(color);
    labelName->setTag(LOT_LABEL_NAME);
    pTopSprite->addChild(labelName, LAYER_ZORDER2);
    
    auto labelCoin = Label::createWithTTF(ttfConfig1, "", TextHAlignment::CENTER, m_szDevice.width);
    labelCoin->setAnchorPoint(Vec2(0.5f, 0.5f));
    labelCoin->setPosition(Vec2(topSize.width / 2 - 122.0f, topSize.height - 45.0f));
    labelCoin->setColor(color1);
    labelCoin->setTag(LOT_LABEL_COIN);
    pTopSprite->addChild(labelCoin, LAYER_ZORDER2);
    
    auto labelLevel = Label::createWithTTF(ttfConfig1, "", TextHAlignment::CENTER, m_szDevice.width);
    labelLevel->setAnchorPoint(Vec2(0.5f, 0.5f));
    labelLevel->setPosition(Vec2(topSize.width / 2 + 155.0f, topSize.height - 45.0f));
    labelLevel->setColor(color1);
    labelLevel->setTag(LOT_LABEL_LEVEL);
    pTopSprite->addChild(labelLevel, LAYER_ZORDER2);
    
    auto labelFriend = Label::createWithTTF(ttfConfig1, "", TextHAlignment::CENTER, m_szDevice.width);
    labelFriend->setAnchorPoint(Vec2(0.5f, 0.5f));
    labelFriend->setPosition(Vec2(topSize.width / 2 + 338.0f, topSize.height - 86.0f));
    labelFriend->setColor(color2);
    labelFriend->setTag(LOT_LABEL_FRIEND);
    pTopSprite->addChild(labelFriend, LAYER_ZORDER4);
    
    auto labelMail = Label::createWithTTF(ttfConfig1, "", TextHAlignment::CENTER, m_szDevice.width);
    labelMail->setAnchorPoint(Vec2(0.5f, 0.5f));
    labelMail->setPosition(Vec2(topSize.width / 2 + 462.0f, topSize.height - 86.0f));
    labelMail->setColor(color2);
    labelMail->setTag(LOT_LABEL_MAIL);
    pTopSprite->addChild(labelMail, LAYER_ZORDER4);
}

void LobbyLayer::createMenu()
{
    auto btnPlay1 = MyMenuItem::create("img/lobby/btn_play_1.png", "img/lobby/btn_play_1.png", "", CC_CALLBACK_1(LobbyLayer::OnPlay1, this));
    btnPlay1->setScale(m_fSmallScale);
    btnPlay1->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPlay1->setPosition(Vec2(m_szDevice.width / 2 - 320.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnPlay6 = MyMenuItem::create("img/lobby/btn_play_6.png", "img/lobby/btn_play_6.png", "", CC_CALLBACK_1(LobbyLayer::OnPlay6, this));
    btnPlay6->setScale(m_fSmallScale);
    btnPlay6->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPlay6->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnPlayFriend = MyMenuItem::create("img/lobby/btn_play_friend.png", "img/lobby/btn_play_friend.png", "", CC_CALLBACK_1(LobbyLayer::OnPlayFriend, this));
    btnPlayFriend->setScale(m_fSmallScale);
    btnPlayFriend->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPlayFriend->setPosition(Vec2(m_szDevice.width / 2 + 320.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto main_menu = Menu::create(btnPlay1, btnPlay6, btnPlayFriend, nullptr);
    main_menu->setPosition(Vec2(0.0f, 0.0f));
    main_menu->setTag(LOT_MENU_MAIN);
    addChild(main_menu, LAYER_ZORDER3);
    
    auto btnDialy = MyMenuItem::create("img/lobby/btn_dialy.png", "img/lobby/btn_dialy.png", "", CC_CALLBACK_1(LobbyLayer::OnFreeCoins, this));
    btnDialy->setScale(m_fSmallScale);
    btnDialy->setAnchorPoint(Vec2(1.0f, 0.5f));
    btnDialy->setPosition(Vec2(m_szDevice.width, m_szDevice.height / 2 + 180.0f * m_fSmallScale));
    
    auto dialy_menu = Menu::create(btnDialy, nullptr);
    dialy_menu->setPosition(Vec2(0.0f, 0.0f));
    dialy_menu->setTag(LOT_MENU_DIALY);
    addChild(dialy_menu, LAYER_ZORDER3);
    
    Sprite* pTopSprite = (Sprite*) getChildByTag(LOT_TOP);
    Size topSize = pTopSprite->getContentSize();
    
    auto btnSetting = MyMenuItem::create("img/lobby/btn_setting_nor.png", "img/lobby/btn_setting_act.png", "", CC_CALLBACK_1(LobbyLayer::OnSetting, this));
    btnSetting->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnSetting->setPosition(Vec2(topSize.width - 53.0f, topSize.height - 64.0f));
    
    auto btnNotification = MyMenuItem::create("img/lobby/btn_email_nor.png", "img/lobby/btn_email_act.png", "", CC_CALLBACK_1(LobbyLayer::OnNotificationList, this));
    btnNotification->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnNotification->setPosition(Vec2(topSize.width - 177.0f, topSize.height - 64.0f));
    
    auto btnFriend = MyMenuItem::create("img/lobby/btn_friend_nor.png", "img/lobby/btn_friend_act.png", "", CC_CALLBACK_1(LobbyLayer::OnFriendList, this));
    btnFriend->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnFriend->setPosition(Vec2(topSize.width - 301.0f, topSize.height - 64.0f));
    
    auto btnAddCoin = MyMenuItem::create("img/lobby/btn_add_nor.png", "img/lobby/btn_add_act.png", "", CC_CALLBACK_1(LobbyLayer::OnAddCoin, this));
    btnAddCoin->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnAddCoin->setPosition(Vec2(topSize.width / 2 - 24.0f, topSize.height - 42.5f));
    
    auto top_menu = Menu::create(btnSetting, btnNotification, btnFriend, btnAddCoin, nullptr);
    top_menu->setPosition(Vec2(0.0f, 0.0f));
    pTopSprite->addChild(top_menu, LAYER_ZORDER2);
}

void LobbyLayer::showMainMenu()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/Bgm1.mp3", true);
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(GameData::getInstance()->getBgmEnable() ? 1.0f : 0.001f);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(GameData::getInstance()->getEffectEnable() ? 1.0f : 0.001f);
    
    Sprite* pSpriteLogo = (Sprite*) getChildByTag(LOT_LOGO);
    Label* pLabelStatus = (Label*) getChildByTag(LOT_LABEL_STATUS);
    Sprite* pSpriteTop = (Sprite*) getChildByTag(LOT_TOP);
    Sprite* pSpriteBottom = (Sprite*) getChildByTag(LOT_BOTTOM);
    Menu* pMenuMain = (Menu*) getChildByTag(LOT_MENU_MAIN);
    Menu* pMenuDialy = (Menu*) getChildByTag(LOT_MENU_DIALY);
    Sprite* pCoinAnimSpr = (Sprite*) getChildByTag(LOT_COIN_ANIM);
    
    pSpriteTop->setVisible(true);
    pSpriteBottom->setVisible(true);
    pMenuMain->setVisible(true);
    pMenuDialy->setVisible(true);
    pCoinAnimSpr->setVisible(true);
    
    pSpriteTop->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height + pSpriteTop->getContentSize().height * m_fScaleX));
    pSpriteBottom->setPosition(Vec2(m_szDevice.width / 2, - pSpriteBottom->getContentSize().height * m_fScaleX));
    pMenuMain->setOpacity(0);
    
    auto anim1 = FadeOut::create(0.5f);
    auto anim2 = DelayTime::create(0.5f);
    auto anim3 = FadeIn::create(0.5f);
    auto anim4 = MoveTo::create(0.5f, Vec2(m_szDevice.width / 2, m_szDevice.height));
    auto anim5 = MoveTo::create(0.5f, Vec2(m_szDevice.width / 2, 0.0f));
    auto anim6 = FadeOut::create(0.5f);
    auto anim7 = FadeIn::create(0.5f);
    auto anim8 = FadeIn::create(0.5f);
    
    pSpriteLogo->runAction(anim1);
    pLabelStatus->runAction(anim6);
    pSpriteTop->runAction(Sequence::create(anim2, anim4, nullptr));
    pSpriteBottom->runAction(Sequence::create(anim2, anim5, nullptr));
    pMenuMain->runAction(Sequence::create(anim2, anim3, nullptr));
    if(GameData::getInstance()->isFreeCoin())
    {
        pMenuDialy->setVisible(true);
        pCoinAnimSpr->setVisible(true);
        pMenuDialy->setOpacity(0);
        pCoinAnimSpr->setOpacity(0);
        pMenuDialy->runAction(Sequence::create(anim2, anim7, nullptr));
        pCoinAnimSpr->runAction(Sequence::create(anim2, anim8, nullptr));
    }
    else
    {
        pMenuDialy->setVisible(false);
        pCoinAnimSpr->setVisible(false);
    }
}

void LobbyLayer::updateUserInfo()
{
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    NotificationParser* pNotificationParser = GameData::getInstance()->getNotificationParser();
    int nNewsCount = pNotificationParser->getNewsCount();
    GameData::getInstance()->setGameRoom(NULL);
    
    Label* pLabelName = (Label*) getChildByTag(LOT_TOP)->getChildByTag(LOT_LABEL_NAME);
    Label* pLabelCoin = (Label*) getChildByTag(LOT_TOP)->getChildByTag(LOT_LABEL_COIN);
    Label* pLabelLevel = (Label*) getChildByTag(LOT_TOP)->getChildByTag(LOT_LABEL_LEVEL);
    Label* pLabelFriend = (Label*) getChildByTag(LOT_TOP)->getChildByTag(LOT_LABEL_FRIEND);
    Label* pLabelMail = (Label*) getChildByTag(LOT_TOP)->getChildByTag(LOT_LABEL_MAIL);
    Sprite* pSprActiveMail = (Sprite*) getChildByTag(LOT_TOP)->getChildByTag(LOT_ACTIVE_MAIL);
    Sprite* pSprActiveFriend = (Sprite*) getChildByTag(LOT_TOP)->getChildByTag(LOT_ACTIVE_FRIEND);
    Sprite* pSpriteTop = (Sprite*) getChildByTag(LOT_TOP);
    
    pLabelName->setString(pUserInfo->m_strName);
    pLabelCoin->setString(getStringFromInt(pUserInfo->m_nCoin));
    pLabelLevel->setString(getStringFromInt(pUserInfo->getDisplayLevel()));
    pLabelFriend->setString(getStringFromInt(GameData::getInstance()->getFriendCount()));
    pLabelMail->setString(getStringFromInt(nNewsCount));
    pSprActiveMail->setVisible(nNewsCount > 0);
    pSprActiveFriend->setVisible(GameData::getInstance()->hasNewFriend());
    
    if(pUserInfo->m_strAvatar != "") {
        Layer* avatarLayer = (Layer*) getChildByTag(LOT_AVATAR);
        if(avatarLayer != nullptr)
        {
            avatarLayer->removeFromParentAndCleanup(true);
            avatarLayer = nullptr;
        }
        
        std::string writablePath = FileUtils::getInstance()->getWritablePath();
        std::string fullPath = writablePath + pUserInfo->m_strAvatar;
        
        if(!FileUtils::getInstance()->isFileExist(fullPath))
        {
            auto avatarLayer = createAvatarLayer("img/setting/profile_default.png");
            avatarLayer->setTag(LOT_AVATAR);
            pSpriteTop->addChild(avatarLayer, LAYER_ZORDER1);
            //download avatar --LeeChanWu
        }
        else {
            auto avatarLayer = createAvatarLayer(fullPath);
            avatarLayer->setTag(LOT_AVATAR);
            pSpriteTop->addChild(avatarLayer, LAYER_ZORDER1);
        }
    }
}
