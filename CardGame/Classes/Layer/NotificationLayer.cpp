#include "NotificationLayer.h"
#include "LobbyLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/Notification.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Widget/MyMenuItem.h"
#include "../Xml/NotificationParser.h"

enum NOTIFICATION_TAG
{
    NT_SCROLL = 1,
};

Scene* NotificationLayer::createScene()
{
    Scene* scene = Scene::create();
    NotificationLayer* layer = NotificationLayer::create();
    scene->addChild(layer);
    return scene;
}

NotificationLayer::~NotificationLayer()
{
}

bool NotificationLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_NOTIFICATION;
    g_pCurrentLayer = this;
    
    createSprites();
    createLabels();
    createMenu();
    loadNotifications();
   
    return true;
}

void NotificationLayer::onEnter()
{
    BaseLayer::onEnter();
}

void NotificationLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == RESPONSE_ACCEPT_FRIEND)
    {
        MailActionInfo mailActionInfo = PacketParser::getInstance()->parseMailAction(message);
        hideLoading();
        if(mailActionInfo.nStatus == RES_SUCCESS)
            updateNotification(mailActionInfo.strMailId, mailActionInfo.nAction);
    }
}

void NotificationLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
}

void NotificationLayer::OnDecline(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    NotificationParser* pNotificationParser = GameData::getInstance()->getNotificationParser();
    
    MyMenuItem* pMenuItem = (MyMenuItem*) pSender;
    Menu* pMenu = (Menu*) pMenuItem->getParent();
    int tag = pMenu->getTag();
    
    Notification* pNotification = pNotificationParser->m_vecNotifications.at(tag);
    GameData::getInstance()->sendPacket(REQUEST_ACCEPT_FRIEND, PacketParser::getInstance()->makeRequestAcceptFriend(pMe->m_strId, pNotification->m_strId, EMAIL_ACTION_DECLINE));
    showLoading();
}

void NotificationLayer::OnAccept(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    NotificationParser* pNotificationParser = GameData::getInstance()->getNotificationParser();
    
    MyMenuItem* pMenuItem = (MyMenuItem*) pSender;
    Menu* pMenu = (Menu*) pMenuItem->getParent();
    int tag = pMenu->getTag();
    
    Notification* pNotification = pNotificationParser->m_vecNotifications.at(tag);
    GameData::getInstance()->sendPacket(REQUEST_ACCEPT_FRIEND, PacketParser::getInstance()->makeRequestAcceptFriend(pMe->m_strId, pNotification->m_strId, EMAIL_ACTION_ACCEPT));
    showLoading();
}

void NotificationLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTitle = Sprite::create("img/notification/title_notification.png");
    sprTitle->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTitle->setScale(m_fSmallScale);
    sprTitle->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height - 80.0f * m_fSmallScale));
    addChild(sprTitle, LAYER_ZORDER1);

    auto scrollView = ui::ScrollView::create();
    scrollView->setContentSize(Size(m_szDevice.width, m_szDevice.height - 134.0f * m_fSmallScale));
    scrollView->setPosition(Vec2(0.0f, 0.0f));
    scrollView->setScrollBarEnabled(false);
    scrollView->setCascadeColorEnabled(true);
    scrollView->setTag(NT_SCROLL);
    addChild(scrollView, LAYER_ZORDER1);
}

void NotificationLayer::createLabels()
{
}

void NotificationLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(NotificationLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}

void NotificationLayer::loadNotifications()
{
    NotificationParser* pNotificationParser = GameData::getInstance()->getNotificationParser();
    
    ui::ScrollView* scrollView = (ui::ScrollView*) getChildByTag(NT_SCROLL);
    scrollView->removeAllChildrenWithCleanup(true);
    
    int nNotificationCount = (int)pNotificationParser->m_vecNotifications.size();
    scrollView->setInnerContainerSize(Size(m_szDevice.width, 111.0f * nNotificationCount * m_fScaleX));
    
    for(int index = nNotificationCount - 1; index >= 0; index--)
    {
        Notification* pNotification = pNotificationParser->m_vecNotifications.at(index);
        pNotification->m_bNew = false;
        
        auto sprCellBg = Sprite::create("img/friend/cell_bg.png");
        sprCellBg->setAnchorPoint(Vec2(0.5f, 0.5f));
        sprCellBg->setScale(m_fScaleX);
        sprCellBg->setPosition(Vec2(scrollView->getInnerContainerSize().width / 2, scrollView->getInnerContainerSize().height - 111.0f * (nNotificationCount - 1 - index) * m_fScaleX - 55.0f * m_fScaleX));
        sprCellBg->setTag(index);
        scrollView->addChild(sprCellBg, LAYER_ZORDER1);
        createOneNotification(sprCellBg, pNotification, index);
    }
    
    GameData::getInstance()->saveNotifications();
}

void NotificationLayer::createOneNotification(Sprite* pSprBg, Notification* pNotification, int tag)
{
    Size bgSize = pSprBg->getContentSize();
    
    auto sprAvatar = Sprite::create("img/lobby/lobby_avatar.png");
    sprAvatar->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprAvatar->setPosition(Vec2(90.0f, bgSize.height / 2));
    pSprBg->addChild(sprAvatar);
    
    auto sprAvatarBorder = Sprite::create("img/lobby/lobby_avatar_border.png");
    sprAvatarBorder->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprAvatarBorder->setPosition(Vec2(90.0f, bgSize.height / 2));
    pSprBg->addChild(sprAvatarBorder);
    
    if(pNotification->m_nType == EMAIL_TYPE_ADD_FRIEND)
    {
        TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 45.0f);
        TTFConfig ttfConfig1("fonts/ImperialStd-Regular.ttf", 35.0f);
        Color3B color = Color3B(255, 255, 255);
        Color3B color1 = Color3B(80, 176, 40);
        Color3B color2 = Color3B(185, 27, 28);
        
        auto labelMessage = Label::createWithTTF(ttfConfig, pNotification->m_strSenderName + " sent friend request to you", TextHAlignment::LEFT, m_szDevice.width);
        labelMessage->setAnchorPoint(Vec2(0.0f, 0.5f));
        labelMessage->setPosition(Vec2(160.0f, bgSize.height / 2));
        labelMessage->setColor(color);
        pSprBg->addChild(labelMessage);
        
        if(pNotification->m_nReceiverAction == EMAIL_ACTION_NONE)
        {
            auto btnDecline = MyMenuItem::create("img/notification/btn_decline_nor.png", "img/notification/btn_decline_act.png", "", CC_CALLBACK_1(NotificationLayer::OnDecline, this));
            btnDecline->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnDecline->setPosition(Vec2(bgSize.width - 300.0f, bgSize.height / 2));
            btnDecline->setTag(0);
            
            auto btnAccept = MyMenuItem::create("img/notification/btn_accept_nor.png", "img/notification/btn_accept_act.png", "", CC_CALLBACK_1(NotificationLayer::OnAccept, this));
            btnAccept->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnAccept->setPosition(Vec2(bgSize.width - 125.0f, bgSize.height / 2));
            btnAccept->setTag(1);
            
            auto menu = Menu::create(btnDecline, btnAccept, nullptr);
            menu->setPosition(Vec2(0.0f, 0.0f));
            menu->setTag(tag);
            pSprBg->addChild(menu);
        }
        else if(pNotification->m_nReceiverAction == EMAIL_ACTION_ACCEPT)
        {
            auto labelDecline = Label::createWithTTF(ttfConfig1, "ACCEPTED", TextHAlignment::CENTER, m_szDevice.width);
            labelDecline->setAnchorPoint(Vec2(0.0f, 0.5f));
            labelDecline->setPosition(Vec2(bgSize.width - 293.0f, bgSize.height / 2));
            labelDecline->setColor(color1);
            pSprBg->addChild(labelDecline);
        }
        else if(pNotification->m_nReceiverAction == EMAIL_ACTION_DECLINE)
        {
            auto labelDecline = Label::createWithTTF(ttfConfig1, "DECLINED", TextHAlignment::CENTER, m_szDevice.width);
            labelDecline->setAnchorPoint(Vec2(0.0f, 0.5f));
            labelDecline->setPosition(Vec2(bgSize.width - 293.0f, bgSize.height / 2));
            labelDecline->setColor(color2);
            pSprBg->addChild(labelDecline);
        }
    }
    else if(pNotification->m_nType == EMAIL_TYPE_ACCEPT_FRIEND)
    {
        TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 45.0f);
        Color3B color = Color3B(80, 176, 40);
        auto labelMessage = Label::createWithTTF(ttfConfig, pNotification->m_strSenderName + " accepted your friend request", TextHAlignment::LEFT, m_szDevice.width);
        labelMessage->setAnchorPoint(Vec2(0.0f, 0.5f));
        labelMessage->setPosition(Vec2(160.0f, bgSize.height / 2));
        labelMessage->setColor(color);
        pSprBg->addChild(labelMessage);
    }
    else if(pNotification->m_nType == EMAIL_TYPE_DECLINE_FRIEND)
    {
        TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 45.0f);
        Color3B color = Color3B(185, 27, 28);
        auto labelMessage = Label::createWithTTF(ttfConfig, pNotification->m_strSenderName + " declined your friend request", TextHAlignment::LEFT, m_szDevice.width);
        labelMessage->setAnchorPoint(Vec2(0.0f, 0.5f));
        labelMessage->setPosition(Vec2(160.0f, bgSize.height / 2));
        labelMessage->setColor(color);
        pSprBg->addChild(labelMessage);
    }
    else if(pNotification->m_nType == EMAIL_TYPE_REMOVE_FRIEND)
    {
        TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 45.0f);
        Color3B color = Color3B(119, 131, 148);
        auto labelMessage = Label::createWithTTF(ttfConfig, pNotification->m_strSenderName + " removed you from friend", TextHAlignment::LEFT, m_szDevice.width);
        labelMessage->setAnchorPoint(Vec2(0.0f, 0.5f));
        labelMessage->setPosition(Vec2(160.0f, bgSize.height / 2));
        labelMessage->setColor(color);
        pSprBg->addChild(labelMessage);
    }
}

void NotificationLayer::updateNotification(std::string strNotificationId, int nAction)
{
    NotificationParser* pNotificationParser = GameData::getInstance()->getNotificationParser();
    Notification* pNotification = pNotificationParser->getNotification(strNotificationId);
    pNotification->m_nReceiverAction = nAction;
    GameData::getInstance()->saveNotifications();
    
    int index = (int)pNotificationParser->m_vecNotifications.getIndex(pNotification);
    Sprite* bgCell = (Sprite*) getChildByTag(NT_SCROLL)->getChildByTag(index);
    bgCell->removeAllChildrenWithCleanup(true);
    createOneNotification(bgCell, pNotification, index);
}
