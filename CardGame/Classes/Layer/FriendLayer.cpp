#include "FriendLayer.h"
#include "LobbyLayer.h"
#include "MessageLayer.h"
#include "MatchFriendLayer.h"
#include "SelectGameLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Widget/MyMenuItem.h"

enum FRIEND_TAG
{
    FT_SCROLL = 1,
};

Scene* FriendLayer::createScene(int nShowType)
{
    Scene* scene = Scene::create();
    FriendLayer* layer = FriendLayer::create();
    layer->init(nShowType);
    scene->addChild(layer);
    return scene;
}

FriendLayer::~FriendLayer()
{
    m_vecFriends.clear();
}

bool FriendLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    return true;
}

void FriendLayer::init(int nShowType) {
    
    m_nType = LAYER_FRIENDLIST;
    m_nShowType = nShowType;
    g_pCurrentLayer = this;
    m_vecFriends.clear();
    
    createSprites();
    createLabels();
    createMenu();
    
    GameData::getInstance()->clearNewFriend();
}

void FriendLayer::onEnter()
{
    BaseLayer::onEnter();
    
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(REQUEST_FRIEND_LIST, PacketParser::getInstance()->makeRequestFriendList(pMe->m_strId, pMe->m_vecFriends));
    showLoading();
}

void FriendLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == RESPONSE_FRIEND_LIST)
    {
        hideLoading();
        m_vecFriends.clear();
        m_vecFriends = PacketParser::getInstance()->parseUserList(message);
        updateScrollView();
    }
    else if(packet_name == RESPONSE_REMOVE_FRIEND)
    {
        hideLoading();
        
        RemoveFriendResult removeFriendResult = PacketParser::getInstance()->parseRemoveFriendResult(message);
        if(removeFriendResult.nStatus == RES_FAILED)
        {
            createMessageLayer(MSG_REMOVE_FRIEND_FAILED);
            return;
        }
        
        removeFriend(removeFriendResult.strFriendId);
    }
}

void FriendLayer::onDownloadedAvatar(std::string file_name)
{
    ui::ScrollView* scrollView = (ui::ScrollView*) getChildByTag(FT_SCROLL);
    Vector<Node*> cells = scrollView->getChildren();
    for(Vector<Node*>::iterator it = cells.begin(); it != cells.end(); it++)
    {
        Sprite* pCellSpr = (Sprite*)(*it);
        Layer* pAvatarLayer = (Layer*) pCellSpr->getChildByTag(2);
        if(pAvatarLayer->getName() == file_name) {
            createAvatarLayer(pCellSpr, file_name, "");
        }
    }
}

void FriendLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
}

void FriendLayer::OnRemove(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    
    MyMenuItem* pMenuItem = (MyMenuItem*) pSender;
    Menu* pMenu = (Menu*) pMenuItem->getParent();
    int tag = pMenu->getTag();
    
    GameData::getInstance()->sendPacket(REQUEST_REMOVE_FRIEND, PacketParser::getInstance()->makeRequestRemoveFriend(pMe->m_strId, m_vecFriends.at(tag)->m_strId));
    showLoading();
}

void FriendLayer::OnPlay(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
  
    Vector<UserInfo*> choosenFriends;
    for(int index = 0; index < m_vecFriends.size(); index++)
    {
        UserInfo* pUser = m_vecFriends.at(index);
        if(pUser->m_bChecked)
            choosenFriends.pushBack(pUser);
    }
    
    if(choosenFriends.size() > 4) {
        createMessageLayer(MSG_MAX_FRIENDS);
        return;
    }
    
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectGameLayer::createScene(ROOM_TYPE_FRIEND, choosenFriends), Color3B(0, 0, 0)));
}

void FriendLayer::OnCheck(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    MyMenuItem* pMenuItem = (MyMenuItem*) pSender;
    Menu* pMenu = (Menu*) pMenuItem->getParent();
    int tag = pMenu->getTag();
    m_vecFriends.at(tag)->m_bChecked = true;
    pMenuItem->setVisible(false);
    ((MyMenuItem*)pMenu->getChildByTag(0))->setVisible(true);
    updateMainMenu();
}

void FriendLayer::OnUnCheck(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    MyMenuItem* pMenuItem = (MyMenuItem*) pSender;
    Menu* pMenu = (Menu*) pMenuItem->getParent();
    int tag = pMenu->getTag();
    m_vecFriends.at(tag)->m_bChecked = false;
    pMenuItem->setVisible(false);
    ((MyMenuItem*)pMenu->getChildByTag(1))->setVisible(true);
    updateMainMenu();
}

void FriendLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTitle = Sprite::create("img/friend/title_friends.png");
    sprTitle->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTitle->setScale(m_fSmallScale);
    sprTitle->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height - 80.0f * m_fSmallScale));
    addChild(sprTitle, LAYER_ZORDER1);
    
    auto scrollView = ui::ScrollView::create();
    scrollView->setContentSize(Size(m_szDevice.width, m_szDevice.height - 134.0f * m_fSmallScale));
    scrollView->setPosition(Vec2(0.0f, 0.0f));
    scrollView->setScrollBarEnabled(false);
    scrollView->setCascadeColorEnabled(true);
    scrollView->setTag(FT_SCROLL);
    addChild(scrollView, LAYER_ZORDER1);
}

void FriendLayer::createLabels()
{
}

void FriendLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(FriendLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    menu->setTag(0);
    addChild(menu, LAYER_ZORDER2);
    
    if(m_nShowType == FST_PLAY) {
        auto btnPlay = MyMenuItem::create("img/friend/btn_play_nor.png", "img/friend/btn_play_act.png", "img/friend/btn_play_dis.png", CC_CALLBACK_1(FriendLayer::OnPlay, this));
        btnPlay->setScale(m_fSmallScale);
        btnPlay->setAnchorPoint(Vec2(0.5f, 0.5f));
        btnPlay->setPosition(Vec2(m_szDevice.width - 130.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
        btnPlay->setTag(0);
        btnPlay->setEnabled(false);
        menu->addChild(btnPlay);
    }
}

void FriendLayer::updateScrollView()
{
    ui::ScrollView* scrollView = (ui::ScrollView*) getChildByTag(FT_SCROLL);
    scrollView->removeAllChildrenWithCleanup(true);
    
    int nFriendCount = (int)m_vecFriends.size();
    scrollView->setInnerContainerSize(Size(m_szDevice.width, 111.0f * nFriendCount * m_fScaleX));
    
    for(int index = 0; index < m_vecFriends.size(); index++)
    {
        UserInfo* pUser = m_vecFriends.at(index);
        
        auto sprCellBg = Sprite::create("img/friend/cell_bg.png");
        sprCellBg->setAnchorPoint(Vec2(0.5f, 0.5f));
        sprCellBg->setScale(m_fScaleX);
        sprCellBg->setPosition(Vec2(scrollView->getInnerContainerSize().width / 2, scrollView->getInnerContainerSize().height - 111.0f * index * m_fScaleX - 55.0f * m_fScaleX));
        scrollView->addChild(sprCellBg, LAYER_ZORDER1);
        
        createOneFriend(sprCellBg, pUser, index);
    }
}

void FriendLayer::createOneFriend(Sprite* pSprBg, UserInfo* pUser, int tag)
{
    Size bgSize = pSprBg->getContentSize();
    
    createAvatarLayer(pSprBg, pUser->m_strAvatar, pUser->m_strAvatarUrl);
    
    auto sprAvatarBorder = Sprite::create("img/lobby/lobby_avatar_border.png");
    sprAvatarBorder->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprAvatarBorder->setPosition(Vec2(90.0f, bgSize.height / 2));
    pSprBg->addChild(sprAvatarBorder);
    
    auto sprOnline = Sprite::create("img/friend/online.png");
    sprOnline->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprOnline->setPosition(Vec2(bgSize.width - 250.0f, bgSize.height / 2));
    sprOnline->setVisible(m_nShowType == FST_PLAY && pUser->m_nOnline == 1);
    pSprBg->addChild(sprOnline);
    
    auto sprOffline = Sprite::create("img/friend/offline.png");
    sprOffline->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprOffline->setPosition(Vec2(bgSize.width - 250.0f, bgSize.height / 2));
    sprOffline->setVisible(m_nShowType == FST_PLAY && pUser->m_nOnline == 0);
    pSprBg->addChild(sprOffline);
    
    auto sprStar = Sprite::create("img/friend/star.png");
    sprStar->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprStar->setPosition(Vec2(bgSize.width - 500.0f, bgSize.height / 2));
    pSprBg->addChild(sprStar);
    
    TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 50.0f);
    Color3B color = Color3B(255, 255, 255);
    Color3B color1 = Color3B(249, 227, 125);
    
    auto labelName = Label::createWithTTF(ttfConfig, pUser->m_strName, TextHAlignment::LEFT, m_szDevice.width);
    labelName->setAnchorPoint(Vec2(0.0f, 0.5f));
    labelName->setPosition(Vec2(160.0f, bgSize.height / 2));
    labelName->setColor(color);
    pSprBg->addChild(labelName);
    
    auto labelLevel = Label::createWithTTF(ttfConfig, getStringFromInt(pUser->getDisplayLevel()), TextHAlignment::LEFT, m_szDevice.width);
    labelLevel->setAnchorPoint(Vec2(0.0f, 0.5f));
    labelLevel->setPosition(Vec2(bgSize.width - 460.0f, bgSize.height / 2 - 5.0f));
    labelLevel->setColor(color1);
    pSprBg->addChild(labelLevel);
    
    if(m_nShowType == FST_REMOVE)
    {
        auto btnRemove = MyMenuItem::create("img/friend/btn_remove_nor.png", "img/friend/btn_remove_act.png", "", CC_CALLBACK_1(FriendLayer::OnRemove, this));
        btnRemove->setAnchorPoint(Vec2(0.5f, 0.5f));
        btnRemove->setPosition(Vec2(bgSize.width - 130.0f, bgSize.height / 2));
        btnRemove->setTag(0);
        
        auto menu = Menu::create(btnRemove, nullptr);
        menu->setPosition(Vec2(0.0f, 0.0f));
        menu->setTag(tag);
        pSprBg->addChild(menu);
    }
    else if(m_nShowType == FST_PLAY)
    {
        auto btnCheck = MyMenuItem::create("img/friend/check.png", "img/friend/check.png", "", CC_CALLBACK_1(FriendLayer::OnUnCheck, this));
        btnCheck->setAnchorPoint(Vec2(0.5f, 0.5f));
        btnCheck->setPosition(Vec2(bgSize.width - 100.0f, bgSize.height / 2));
        btnCheck->setTag(0);
        btnCheck->setVisible(pUser->m_bChecked);
        
        auto btnUncheck = MyMenuItem::create("img/friend/uncheck.png", "img/friend/uncheck.png", "", CC_CALLBACK_1(FriendLayer::OnCheck, this));
        btnUncheck->setAnchorPoint(Vec2(0.5f, 0.5f));
        btnUncheck->setPosition(Vec2(bgSize.width - 100.0f, bgSize.height / 2));
        btnUncheck->setTag(1);
        btnUncheck->setVisible(!pUser->m_bChecked);
        
        auto menu = Menu::create(btnCheck, btnUncheck, nullptr);
        menu->setPosition(Vec2(0.0f, 0.0f));
        menu->setTag(tag);
        pSprBg->addChild(menu);
    }
}

void FriendLayer::createAvatarLayer(Sprite* pParentSpr, std::string fileName, std::string downloadLink)
{
    Size bgSize = pParentSpr->getContentSize();
    Layer* avatarLayer = (Layer*) pParentSpr->getChildByTag(2);
    
    if(avatarLayer != nullptr)
    {
        avatarLayer->removeFromParentAndCleanup(true);
        avatarLayer = nullptr;
    }
    
    bool bDownloadNeed = false;
    std::string file_name = "img/setting/profile_default.png";
    if(fileName != "")
    {
        std::string writablePath = FileUtils::getInstance()->getWritablePath();
        std::string fullPath = writablePath + fileName;
        
        if(FileUtils::getInstance()->isFileExist(fullPath))
            file_name = fullPath;
        else
            bDownloadNeed = true;
    }
    
    auto content_spr = Sprite::create(file_name);
    content_spr->setScale(0.5f);
    
    auto stencil = DrawNode::create();
    stencil->drawSolidCircle(Vec2(0.0f, 0.0f), 85.0f / 2, 0.0f, 32.0f, Color4F::GREEN);
    
    auto clipper = ClippingNode::create();
    clipper->setAnchorPoint(Vec2(0.5f, 0.5f));
    clipper->setPosition(Vec2(90.0f, bgSize.height / 2));
    clipper->setStencil(stencil);
    clipper->addChild(content_spr);
    
    avatarLayer = Layer::create();
    avatarLayer->addChild(clipper);
    avatarLayer->setTag(2);
    avatarLayer->setName(fileName);
    pParentSpr->addChild(avatarLayer);
    
    if(bDownloadNeed) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        downloadAvatar(fileName, downloadLink);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
    }
}

void FriendLayer::removeFriend(std::string strFriendId)
{
    for(int index = 0; index < m_vecFriends.size(); index++)
    {
        UserInfo* pFriend = m_vecFriends.at(index);
        if(pFriend->m_strId == strFriendId)
        {
            m_vecFriends.eraseObject(pFriend);
            break;
        }
    }
    
    updateScrollView();
}

void FriendLayer::updateMainMenu()
{
    if(m_nShowType == FST_REMOVE)
        return;
    
    bool bPlayable = false;
    for(int index = 0; index < m_vecFriends.size(); index++)
    {
        UserInfo* pUser = m_vecFriends.at(index);
        if(pUser->m_bChecked)
        {
            bPlayable = true;
            break;
        }
    }
    
    MyMenuItem* pPlayItem = (MyMenuItem*) getChildByTag(0)->getChildByTag(0);
    pPlayItem->setEnabled(bPlayable);
}
