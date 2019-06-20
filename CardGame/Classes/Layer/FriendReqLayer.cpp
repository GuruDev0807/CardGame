#include "FriendReqLayer.h"
#include "SettingLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Widget/MyMenuItem.h"

enum FRIENDREQ_TAG
{
    FRT_NAME = 1,
    FRT_SCROLL = 2,
};

Scene* FriendReqLayer::createScene()
{
    Scene* scene = Scene::create();
    FriendReqLayer* layer = FriendReqLayer::create();
    scene->addChild(layer);
    return scene;
}

FriendReqLayer::~FriendReqLayer()
{
    m_vecUsers.clear();
}

bool FriendReqLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_USERLIST;
    g_pCurrentLayer = this;
    m_fAdjustVert = 0.0f;
    m_vecUsers.clear();
    
    createSprites();
    createLabels();
    createMenu();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(FriendReqLayer::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(FriendReqLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void FriendReqLayer::onEnter()
{
    BaseLayer::onEnter();
}

void FriendReqLayer::keyboardWillShow(cocos2d::IMEKeyboardNotificationInfo& info)
{
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(FRT_NAME);
    
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

void FriendReqLayer::keyboardWillHide(IMEKeyboardNotificationInfo& info)
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

void FriendReqLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == RESPONSE_USERLIST)
    {
        m_vecUsers.clear();
        m_vecUsers = PacketParser::getInstance()->parseUserList(message);
        hideLoading();
        updateScrollView();
    }
    else if(packet_name == RESPONSE_ADD_FRIEND)
    {
        hideLoading();
    }
}

void FriendReqLayer::onDownloadedAvatar(std::string file_name)
{
    ui::ScrollView* scrollView = (ui::ScrollView*) getChildByTag(FRT_SCROLL);
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

bool FriendReqLayer::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    m_posTouch = touch->getLocation();
    return true;
}

void FriendReqLayer::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(FRT_NAME);
    
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

void FriendReqLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SettingLayer::createScene(), Color3B(0, 0, 0)));
}

void FriendReqLayer::OnSend(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    
    std::vector<std::string> friends;
    for(int index = 0; index < m_vecUsers.size(); index++)
    {
        UserInfo* pUser = m_vecUsers.at(index);
        if(pUser->m_bChecked)
            friends.push_back(pUser->m_strId);
    }
    
    if(friends.size() == 0)
        return;
    
    GameData::getInstance()->sendPacket(REQUEST_ADD_FRIEND, PacketParser::getInstance()->makeRequestAddFriend(pMe->m_strId, friends));
    showLoading();
}

void FriendReqLayer::OnSearch(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(FRT_NAME);
    std::string strSearch = pLabelName->getString();

    onClickTrackNode(false, Vec2(0.0f, 0.0f));
    GameData::getInstance()->sendPacket(REQUEST_USERLIST, PacketParser::getInstance()->makeRequestUserList(strSearch));
    showLoading();
}

void FriendReqLayer::OnCheck(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    MyMenuItem* pMenuItem = (MyMenuItem*) pSender;
    Menu* pMenu = (Menu*) pMenuItem->getParent();
    int tag = pMenu->getTag();
    m_vecUsers.at(tag)->m_bChecked = true;
    pMenuItem->setVisible(false);
    ((MyMenuItem*)pMenu->getChildByTag(0))->setVisible(true);
}

void FriendReqLayer::OnUnCheck(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    MyMenuItem* pMenuItem = (MyMenuItem*) pSender;
    Menu* pMenu = (Menu*) pMenuItem->getParent();
    int tag = pMenu->getTag();
    m_vecUsers.at(tag)->m_bChecked = false;
    pMenuItem->setVisible(false);
    ((MyMenuItem*)pMenu->getChildByTag(1))->setVisible(true);
}

void FriendReqLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTitle = Sprite::create("img/friend/title_friend_req.png");
    sprTitle->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTitle->setScale(m_fSmallScale);
    sprTitle->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height - 80.0f * m_fSmallScale));
    addChild(sprTitle, LAYER_ZORDER1);
    
    auto sprSearch = Sprite::create("img/friend/search_bg.png");
    sprSearch->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprSearch->setScale(m_fSmallScale);
    sprSearch->setPosition(Vec2(m_szDevice.width / 2 - 68.0f * m_fSmallScale, m_szDevice.height - 212.0f * m_fSmallScale));
    addChild(sprSearch, LAYER_ZORDER1);
    
    auto scrollView = ui::ScrollView::create();
    scrollView->setContentSize(Size(m_szDevice.width, m_szDevice.height - 258.0f * m_fSmallScale));
    scrollView->setPosition(Vec2(0.0f, 0.0f));
    scrollView->setScrollBarEnabled(false);
    scrollView->setCascadeColorEnabled(true);
    scrollView->setTag(FRT_SCROLL);
    addChild(scrollView, LAYER_ZORDER1);
}

void FriendReqLayer::createLabels()
{
    auto searchTTF = TextFieldTTF::textFieldWithPlaceHolder("Search Player", Size(480.0f * m_fSmallScale, 60.0f * m_fSmallScale), TextHAlignment::LEFT, "fonts/ImperialStd-Regular.ttf", 50.0f * m_fSmallScale);
    searchTTF->setAnchorPoint(Vec2(0.0f, 0.5f));
    searchTTF->setPosition(Vec2(m_szDevice.width / 2 - 300.0f * m_fSmallScale, m_szDevice.height - 212.0f * m_fSmallScale));
    searchTTF->setColorSpaceHolder(Color3B(127, 127, 127));
    searchTTF->setTextColor(Color4B(0, 0, 0, 255));
    searchTTF->setTag(FRT_NAME);
    addChild(searchTTF, LAYER_ZORDER1);
}

void FriendReqLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(FriendReqLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto btnSend = MyMenuItem::create("img/friend/btn_send_nor.png", "img/friend/btn_send_act.png", "", CC_CALLBACK_1(FriendReqLayer::OnSend, this));
    btnSend->setScale(m_fSmallScale);
    btnSend->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnSend->setPosition(Vec2(m_szDevice.width - 113.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto btnSearch = MyMenuItem::create("img/friend/btn_search_nor.png", "img/friend/btn_search_act.png", "", CC_CALLBACK_1(FriendReqLayer::OnSearch, this));
    btnSearch->setScale(m_fSmallScale);
    btnSearch->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnSearch->setPosition(Vec2(m_szDevice.width / 2 + 260.0f * m_fSmallScale, m_szDevice.height - 212.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, btnSend, btnSearch, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}

void FriendReqLayer::onClickTrackNode(bool bClicked, const cocos2d::Vec2& touchPos)
{
    TextFieldTTF* pLabelName = (TextFieldTTF*) getChildByTag(FRT_NAME);
    if(bClicked)
        pLabelName->attachWithIME();
    else
        pLabelName->detachWithIME();
}

void FriendReqLayer::updateScrollView()
{
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    
    ui::ScrollView* scrollView = (ui::ScrollView*) getChildByTag(FRT_SCROLL);
    scrollView->removeAllChildrenWithCleanup(true);
    
    int nUserCount = 0;
    for(int index = 0; index < m_vecUsers.size(); index++)
    {
        UserInfo* pUser = m_vecUsers.at(index);
        if(pUser->m_strId == pMe->m_strId)
            continue;
        
        if(pMe->isFriend(pUser->m_strId))
            continue;
        
        nUserCount++;
    }
    scrollView->setInnerContainerSize(Size(m_szDevice.width, 111.0f * nUserCount * m_fScaleX));
    
    int nRealIndex = 0;
    for(int index = 0; index < m_vecUsers.size(); index++)
    {
        UserInfo* pUser = m_vecUsers.at(index);
        if(pUser->m_strId == pMe->m_strId)
            continue;
        
        if(pMe->isFriend(pUser->m_strId))
            continue;
        
        auto sprCellBg = Sprite::create("img/friend/cell_bg.png");
        sprCellBg->setAnchorPoint(Vec2(0.5f, 0.5f));
        sprCellBg->setScale(m_fScaleX);
        sprCellBg->setPosition(Vec2(scrollView->getInnerContainerSize().width / 2, scrollView->getInnerContainerSize().height - 111.0f * nRealIndex * m_fScaleX - 55.0f * m_fScaleX));
        sprCellBg->setTag(index);
        scrollView->addChild(sprCellBg, LAYER_ZORDER1);
        
        createOneUser(sprCellBg, m_vecUsers.at(index), index);
        nRealIndex++;
    }
}

void FriendReqLayer::createOneUser(Sprite* pSprBg, UserInfo* pUser, int tag)
{
    Size bgSize = pSprBg->getContentSize();
    
    createAvatarLayer(pSprBg, pUser->m_strAvatar, pUser->m_strAvatarUrl);
    
    auto sprAvatarBorder = Sprite::create("img/lobby/lobby_avatar_border.png");
    sprAvatarBorder->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprAvatarBorder->setPosition(Vec2(90.0f, bgSize.height / 2));
    pSprBg->addChild(sprAvatarBorder);
    
    auto sprOnline = Sprite::create("img/friend/online.png");
    sprOnline->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprOnline->setPosition(Vec2(bgSize.width - 200.0f, bgSize.height / 2));
    sprOnline->setVisible(pUser->m_nOnline == 1);
    pSprBg->addChild(sprOnline);
    
    auto sprOffline = Sprite::create("img/friend/offline.png");
    sprOffline->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprOffline->setPosition(Vec2(bgSize.width - 200.0f, bgSize.height / 2));
    sprOffline->setVisible(pUser->m_nOnline == 0);
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
    
    auto btnCheck = MyMenuItem::create("img/friend/check.png", "img/friend/check.png", "", CC_CALLBACK_1(FriendReqLayer::OnUnCheck, this));
    btnCheck->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnCheck->setPosition(Vec2(bgSize.width - 100.0f, bgSize.height / 2));
    btnCheck->setTag(0);
    btnCheck->setVisible(pUser->m_bChecked);
    
    auto btnUncheck = MyMenuItem::create("img/friend/uncheck.png", "img/friend/uncheck.png", "", CC_CALLBACK_1(FriendReqLayer::OnCheck, this));
    btnUncheck->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnUncheck->setPosition(Vec2(bgSize.width - 100.0f, bgSize.height / 2));
    btnUncheck->setTag(1);
    btnUncheck->setVisible(!pUser->m_bChecked);
    
    auto menu = Menu::create(btnCheck, btnUncheck, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    menu->setTag(tag);
    pSprBg->addChild(menu);
}

void FriendReqLayer::createAvatarLayer(Sprite* pParentSpr, std::string fileName, std::string downloadLink)
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
