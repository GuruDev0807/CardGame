#include "SettingLayer.h"
#include "LobbyLayer.h"
#include "FriendReqLayer.h"
#include "MessageLayer.h"
#include "HelpLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Widget/MyMenuItem.h"

enum SETTING_TAG
{
    ST_MENU = 1,
    ST_NAME = 2,
    ST_PROFILE = 3,
};

enum SETTING_MENU_TAG
{
    STM_SOUNDON = 1,
    STM_SOUNDOFF = 2
};

Scene* SettingLayer::createScene()
{
    Scene* scene = Scene::create();
    SettingLayer* layer = SettingLayer::create();
    scene->addChild(layer);
    return scene;
}

SettingLayer::~SettingLayer()
{
}

bool SettingLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_SETTING;
    g_pCurrentLayer = this;
    m_fAdjustVert = 0.0f;
    m_bChangedAvatar = false;
    
    createSprites();
    createLabels();
    createMenu();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(SettingLayer::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(SettingLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void SettingLayer::onEnter()
{
    BaseLayer::onEnter();
}

void SettingLayer::keyboardWillShow(cocos2d::IMEKeyboardNotificationInfo& info)
{
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(ST_NAME);
    
    Rect rect;
    rect.origin = pLabelName->getPosition();
    rect.size = pLabelName->getContentSize();
    rect.origin.y -= rect.size.height / 2;

    if (! rect.intersectsRect(info.end))
        return;
    
    m_fAdjustVert = info.end.getMaxY() - rect.getMinY();
    
    auto& children = getChildren();
    Node * node = 0;
    ssize_t count = children.size();
    Vec2 pos;
    for (int i = 0; i < count; ++i)
    {
        node = children.at(i);
        pos = node->getPosition();
        pos.y += m_fAdjustVert;
        node->setPosition(pos);
    }
}

void SettingLayer::keyboardWillHide(IMEKeyboardNotificationInfo& info)
{
    auto& children = getChildren();
    Node * node = 0;
    ssize_t count = children.size();
    Vec2 pos;
    for (int i = 0; i < count; ++i)
    {
        node = children.at(i);
        pos = node->getPosition();
        pos.y -= m_fAdjustVert;
        node->setPosition(pos);
    }
    
    m_fAdjustVert = 0.0f;
}

void SettingLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == PT_RES_UPDATE_PROFILE)
    {
        hideLoading();
        UserInfo* pUserInfo = PacketParser::getInstance()->parseUserInfo(message);
        if(pUserInfo != NULL)
        {
            GameData::getInstance()->setUserInfo(pUserInfo);
            m_bChangedAvatar = false;
        }
        else
            createMessageLayer(MSG_FAILED_UPDATE_PROFILE);
    }
    else if(packet_name == RESPONSE_CHECK_NAME)
    {
        hideLoading();
        bool value = PacketParser::getInstance()->parseCheckNameResponse(message);
        if(value)
            createMessageLayer(MSG_SUCCESS_CHECK_NAME);
        else
            createMessageLayer(MSG_FAILED_CHECK_NAME);
    }
}

bool SettingLayer::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    m_posTouch = touch->getLocation();
    return true;
}

void SettingLayer::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(ST_NAME);
    
    auto endPos = touch->getLocation();
    
    float delta = 5.0f;
    if(std::abs(endPos.x - m_posTouch.x) > delta || std::abs(endPos.y - m_posTouch.y) > delta)
    {
        m_posTouch.x = m_posTouch.y = -1;
        return;
    }
    
    Rect rect;
    rect.size = pLabelName->getContentSize();
    auto clicked = isScreenPointInRect(endPos, Camera::getVisitingCamera(), pLabelName->getWorldToNodeTransform(), rect, nullptr);
    onClickTrackNode(clicked, endPos);
}

void SettingLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
}

void SettingLayer::OnSoundOn(Ref* pSender)
{
    GameData::getInstance()->setBgmEnable(!GameData::getInstance()->getBgmEnable());
    GameData::getInstance()->setEffectEnable(!GameData::getInstance()->getEffectEnable());
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    updateMenu();
}

void SettingLayer::OnSoundOff(Ref* pSender)
{
    GameData::getInstance()->setBgmEnable(!GameData::getInstance()->getBgmEnable());
    GameData::getInstance()->setEffectEnable(!GameData::getInstance()->getEffectEnable());
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    updateMenu();
}

void SettingLayer::OnFriendRequest(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, FriendReqLayer::createScene(), Color3B(0, 0, 0)));
}

void SettingLayer::OnProfileUpdate(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    bool bShowLoading = true;
    if(m_bChangedAvatar)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        uploadAvatar();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
    }
    else
    {
        UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
        TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(ST_NAME);
        std::string strNewName = pLabelName->getString();
        if(strNewName != pUserInfo->m_strName)
            uploadUserInfo(pUserInfo->m_strAvatar);
        else
            bShowLoading = false;
    }

    if(bShowLoading)
        showLoading();
}

void SettingLayer::OnProfileEdit(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    captureAvatar(pUserInfo->m_strId);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    captureAvatar(pUserInfo->m_strId);
#endif
}

void SettingLayer::OnNameCheck(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    
    showLoading();
    
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(ST_NAME);
    std::string strNewName = pLabelName->getString();
    
    onClickTrackNode(false, Vec2(0.0f, 0.0f));
    
    GameData::getInstance()->sendPacket(REQUEST_CHECK_NAME, PacketParser::getInstance()->makeRequestCheckNameReq(pUserInfo->m_strId, strNewName));
}

void SettingLayer::OnHelp(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, HelpLayer::createScene(), Color3B(0, 0, 0)));
}

void SettingLayer::updateProfile(std::string file_name)
{
    Layer* avatarLayer = (Layer*) getChildByTag(ST_PROFILE);
    if(avatarLayer != nullptr)
    {
        avatarLayer->removeFromParentAndCleanup(true);
        avatarLayer = nullptr;
    }
    
    avatarLayer = createAvatarLayer(file_name);
    avatarLayer->setTag(ST_PROFILE);
    addChild(avatarLayer, LAYER_ZORDER1);
    m_bChangedAvatar = true;
}

void SettingLayer::uploadUserInfo(std::string avatarName)
{
    if(avatarName == "failed")
    {
        hideLoading();
        createMessageLayer(MSG_FAILED_UPLOAD_AVATAR);
        return;
    }
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(ST_NAME);
    std::string strNewName = pLabelName->getString();
     
    onClickTrackNode(false, Vec2(0.0f, 0.0f));

    GameData::getInstance()->sendPacket(PT_REQ_UPDATE_PROFILE, PacketParser::getInstance()->makeRequestUpdateProfile(pUserInfo->m_strId, strNewName, avatarName, "http://193.34.145.83/blackjack/upload/" + avatarName));
}

void SettingLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTitle = Sprite::create("img/setting/title_setting.png");
    sprTitle->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTitle->setScale(m_fSmallScale);
    sprTitle->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height - 80.0f * m_fSmallScale));
    addChild(sprTitle, LAYER_ZORDER1);

    std::string avatarName = GameData::getInstance()->getUserInfo()->m_strAvatar;
    
    if(avatarName != "") {
        std::string writablePath = FileUtils::getInstance()->getWritablePath();
        std::string fullPath = writablePath + avatarName;
        
        if(!FileUtils::getInstance()->isFileExist(fullPath))
        {
            auto avatarLayer = createAvatarLayer("img/setting/profile_default.png");
            avatarLayer->setTag(ST_PROFILE);
            addChild(avatarLayer, LAYER_ZORDER1);
            //download avatar --LeeChanWu
        }
        else {
            auto avatarLayer = createAvatarLayer(fullPath);
            avatarLayer->setTag(ST_PROFILE);
            addChild(avatarLayer, LAYER_ZORDER1);
        }
    }
    else {
        auto avatarLayer = createAvatarLayer("img/setting/profile_default.png");
        avatarLayer->setTag(ST_PROFILE);
        addChild(avatarLayer, LAYER_ZORDER1);
    }
    
    auto sprProfileBorder = Sprite::create("img/setting/profile_border.png");
    sprProfileBorder->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprProfileBorder->setScale(m_fSmallScale);
    sprProfileBorder->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height/ 2 + 66.0f * m_fSmallScale));
    addChild(sprProfileBorder, LAYER_ZORDER3);
    
    auto sprInputBg = Sprite::create("img/setting/input_bg.png");
    sprInputBg->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprInputBg->setScale(m_fSmallScale);
    sprInputBg->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height/ 2 - 120.5f * m_fSmallScale));
    addChild(sprInputBg, LAYER_ZORDER1);
}

void SettingLayer::createLabels()
{
    auto labelStatus = TextFieldTTF::textFieldWithPlaceHolder("", Size(300.0f * m_fSmallScale, 80.0f * m_fSmallScale), TextHAlignment::LEFT, "fonts/ImperialStd-Regular.ttf", 50.0f * m_fSmallScale);
    labelStatus->setString(GameData::getInstance()->getUserInfo()->m_strName);
    labelStatus->setAnchorPoint(Vec2(0.0f, 0.5f));
    labelStatus->setPosition(Vec2(m_szDevice.width / 2 - 243.0f * m_fSmallScale, m_szDevice.height / 2 - 120.5f * m_fSmallScale));
    labelStatus->setColor(Color3B(255, 255, 255));
    labelStatus->setTag(ST_NAME);
    addChild(labelStatus, LAYER_ZORDER1);
}

void SettingLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(SettingLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto btnSoundOn = MyMenuItem::create("img/setting/btn_sound_on_nor.png", "img/setting/btn_sound_on_act.png", "", CC_CALLBACK_1(SettingLayer::OnSoundOn, this));
    btnSoundOn->setScale(m_fSmallScale);
    btnSoundOn->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnSoundOn->setPosition(Vec2(m_szDevice.width / 2 - 211.5f * m_fSmallScale, 98.0f * m_fSmallScale));
    btnSoundOn->setTag(STM_SOUNDON);
    
    auto btnSoundOff = MyMenuItem::create("img/setting/btn_sound_off_nor.png", "img/setting/btn_sound_off_act.png", "", CC_CALLBACK_1(SettingLayer::OnSoundOff, this));
    btnSoundOff->setScale(m_fSmallScale);
    btnSoundOff->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnSoundOff->setPosition(Vec2(m_szDevice.width / 2 - 211.5f * m_fSmallScale, 98.0f * m_fSmallScale));
    btnSoundOff->setTag(STM_SOUNDOFF);
    
    auto btnFriendRequest = MyMenuItem::create("img/setting/btn_friend_nor.png", "img/setting/btn_friend_act.png", "", CC_CALLBACK_1(SettingLayer::OnFriendRequest, this));
    btnFriendRequest->setScale(m_fSmallScale);
    btnFriendRequest->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnFriendRequest->setPosition(Vec2(m_szDevice.width / 2 + 211.5f * m_fSmallScale, 98.0f * m_fSmallScale));
    
    auto btnEditProfile = MyMenuItem::create("img/setting/mask_edit.png", "img/setting/mask_edit.png", "", CC_CALLBACK_1(SettingLayer::OnProfileEdit, this));
    btnEditProfile->setScale(m_fSmallScale);
    btnEditProfile->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnEditProfile->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 9.5f * m_fSmallScale));
    
    auto btnCheckName = MyMenuItem::create("img/setting/btn_check_nor.png", "img/setting/btn_check_act.png", "", CC_CALLBACK_1(SettingLayer::OnNameCheck, this));
    btnCheckName->setScale(m_fSmallScale);
    btnCheckName->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnCheckName->setPosition(Vec2(m_szDevice.width / 2 + 173.5f * m_fSmallScale, m_szDevice.height / 2 - 120.0f * m_fSmallScale));
    
    auto btnUpdateProfile = MyMenuItem::create("img/setting/btn_update_nor.png", "img/setting/btn_update_act.png", "", CC_CALLBACK_1(SettingLayer::OnProfileUpdate, this));
    btnUpdateProfile->setScale(m_fSmallScale);
    btnUpdateProfile->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnUpdateProfile->setPosition(Vec2(m_szDevice.width - 133.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto btnHelp = MyMenuItem::create("img/setting/btn_help.png", "img/setting/btn_help.png", "", CC_CALLBACK_1(SettingLayer::OnHelp, this));
    btnHelp->setScale(m_fSmallScale);
    btnHelp->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnHelp->setPosition(Vec2(m_szDevice.width / 2 + 487.0f * m_fSmallScale, 98.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, btnSoundOn, btnSoundOff, btnFriendRequest, btnEditProfile, btnCheckName, btnUpdateProfile, btnHelp, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    menu->setTag(ST_MENU);
    addChild(menu, LAYER_ZORDER2);
    
    updateMenu();
}

Layer* SettingLayer::createAvatarLayer(std::string fileName)
{
    auto content_spr = Sprite::create(fileName);
    content_spr->setScale(m_fSmallScale);
    
    auto stencil = DrawNode::create();
    stencil->drawSolidCircle(Vec2(0.0f, 0.0f), 170.0f * m_fSmallScale / 2, 0.0f, 32.0f, Color4F::GREEN);
    
    auto clipper = ClippingNode::create();
    clipper->setAnchorPoint(Vec2(0.5f, 0.5f));
    clipper->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height/ 2 + 66.0f * m_fSmallScale));
    clipper->setStencil(stencil);
    clipper->addChild(content_spr);
    
    auto avatar_layer = Layer::create();
    avatar_layer->addChild(clipper);
    return avatar_layer;
}

void SettingLayer::updateMenu()
{
    MyMenuItem* pBtnSoundOn = (MyMenuItem*) getChildByTag(ST_MENU)->getChildByTag(STM_SOUNDON);
    MyMenuItem* pBtnSoundOff = (MyMenuItem*) getChildByTag(ST_MENU)->getChildByTag(STM_SOUNDOFF);
    
    pBtnSoundOn->setVisible(GameData::getInstance()->getBgmEnable());
    pBtnSoundOff->setVisible(!GameData::getInstance()->getBgmEnable());
}

void SettingLayer::onClickTrackNode(bool bClicked, const cocos2d::Vec2& touchPos)
{
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(ST_NAME);
    if(bClicked)
        pLabelName->attachWithIME();
    else
        pLabelName->detachWithIME();
}
