#include "MessageLayer.h"
#include "LobbyLayer.h"
#include "MatchFriendLayer.h"
#include "SelectGameLayer.h"
#include "../Widget/MyMenuItem.h"

std::string MESSAGE_TEXT[] =
{
    "Could not connect to the server.",
    "Failed to load data from server.",
    "Failed to remove friend.",
    "All friens are not able to match.",
    " requested match with ",
    "All friends declined match.",
    "Coin is not enough to match.",
    "Friend's coin is not enough to match.",
    "Uploading avatar was failed.",
    "Name is using by another player.",
    "Name is able to use.",
    "Name is using by another player.",
    "You can play with 4 friends maximum"
};

Scene* MessageLayer::createScene()
{
    Scene* scene = Scene::create();
    MessageLayer* layer = MessageLayer::create();
    scene->addChild(layer);
    return scene;
}

MessageLayer::~MessageLayer()
{
}

bool MessageLayer::init()
{
    if ( !BaseLayer::init() )
    return false;
    
    return true;
}

bool MessageLayer::init(int message_type, std::string param1, std::string param2, int param3)
{
    m_nType = LAYER_MESSAGE;
    m_nMessageType = message_type;
    m_strParam1 = param1;
    m_strParam2 = param2;
    m_nParam3 = param3;
    
    createBackground();
    createMenu();
    
    return true;
}

void MessageLayer::onEnter()
{
    BaseLayer::onEnter();
    
    m_pTouchListener = EventListenerTouchOneByOne::create();
    m_pTouchListener->onTouchBegan = CC_CALLBACK_2(MessageLayer::onTouchBegan, this);
    m_pTouchListener->onTouchMoved = CC_CALLBACK_2(MessageLayer::onTouchMoved, this);
    m_pTouchListener->onTouchEnded = CC_CALLBACK_2(MessageLayer::onTouchEnded, this);
    m_pTouchListener->onTouchCancelled = CC_CALLBACK_2(MessageLayer::onTouchCancelled, this);
    m_pTouchListener->setSwallowTouches(true);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_pTouchListener, this);
}

bool MessageLayer::onTouchBegan(Touch *touch, Event *event)
{
    return true;
}

void MessageLayer::onTouchMoved(Touch *touch, Event *event)
{
    
}

void MessageLayer::onTouchEnded(Touch *touch, Event *event)
{
    
}

void MessageLayer::onTouchCancelled(Touch *touch, Event *event)
{
    
}

void MessageLayer::OnAction(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    switch(m_nMessageType)
    {
        case MSG_CONNECT_FAILED:
            ((LobbyLayer*) getParent())->startConnecting();
            break;
        case MSG_USERINFO_FAILED:
            ((LobbyLayer*) getParent())->loadUserInfo();
            break;
        case MSG_NOTIFY_FRIEND_MATCH:
            ((BaseLayer*) getParent())->respondFriendMatchReq(m_strParam2, FRIEND_MATCH_REQ_ACCEPT, m_nParam3);
            break;
     /*   case MSG_NOT_ENOUGH_COIN:
            ((SelectGameLayer*) getParent())->showPurchase();
            break;*/
    }
    ((BaseLayer*) getParent())->removeMessageLayer();
}

void MessageLayer::OnCancel(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    switch(m_nMessageType)
    {
        case MSG_FRIEND_MATCH_FAILED:
        case MSG_FRIEND_MATCH_DECLINE:
            ((MatchFriendLayer*) getParent())->close();
            break;
        case MSG_NOTIFY_FRIEND_MATCH:
            ((BaseLayer*) getParent())->respondFriendMatchReq(m_strParam2, FRIEND_MATCH_REQ_DECLINE, m_nParam3);
            break;
        case MSG_NOT_ENOUGH_COIN:
            break;
        case MSG_NOT_ENOUGH_COIN_FRIEND:
            break;
        case MSG_FAILED_UPLOAD_AVATAR:
            break;
        case MSG_FAILED_UPDATE_PROFILE:
            break;
        case MSG_SUCCESS_CHECK_NAME:
            break;
        case MSG_FAILED_CHECK_NAME:
            break;
        case MSG_MAX_FRIENDS:
            break;
    }
    
    ((BaseLayer*) getParent())->removeMessageLayer();
}

void MessageLayer::createBackground()
{
    auto pOverLayLayer = LayerColor::create(Color4B(0, 0, 0, 125));
    pOverLayLayer->setPosition(Vec2(0, 0));
    addChild(pOverLayLayer, LAYER_ZORDER1);
    
    auto message_bg = Sprite::create("img/message/message_bg.png");
    message_bg->setAnchorPoint(Vec2(0.5f, 0.5f));
    message_bg->setScale(m_fSmallScale);
    message_bg->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(message_bg, LAYER_ZORDER1);
    
    TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 50.0f * m_fSmallScale);
    Color3B color = Color3B(255, 255, 255);
    
    std::string strText = MESSAGE_TEXT[m_nMessageType];
    if(m_nMessageType == MSG_NOTIFY_FRIEND_MATCH)
        strText = m_strParam1 + strText + getStringFromInt(m_nParam3) + "chip";

    auto text = Label::createWithTTF(ttfConfig, strText, TextHAlignment::CENTER, 450.0f * m_fSmallScale);
    text->setAnchorPoint(Vec2(0.5f, 0.5f));
    text->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 60.0f * m_fSmallScale));
    text->setColor(color);
    addChild(text, LAYER_ZORDER2);
}

void MessageLayer::createMenu()
{
    auto menu = Menu::create();
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER3);
    
    switch (m_nMessageType) {
        case MSG_CONNECT_FAILED:
        case MSG_USERINFO_FAILED:
        {
            auto btnAction = MyMenuItem::create("img/message/btn_reconnect_nor.png", "img/message/btn_reconnect_act.png", "", CC_CALLBACK_1(MessageLayer::OnAction, this));
            btnAction->setScale(m_fSmallScale);
            btnAction->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnAction->setPosition(Vec2(m_szDevice.width /2, m_szDevice.height / 2 - 85.0f * m_fSmallScale));
            menu->addChild(btnAction);
            break;
        }
        case MSG_REMOVE_FRIEND_FAILED:
        case MSG_FRIEND_MATCH_FAILED:
        case MSG_FRIEND_MATCH_DECLINE:
        case MSG_NOT_ENOUGH_COIN_FRIEND:
        case MSG_FAILED_UPLOAD_AVATAR:
        case MSG_FAILED_UPDATE_PROFILE:
        case MSG_SUCCESS_CHECK_NAME:
        case MSG_FAILED_CHECK_NAME:
        case MSG_NOT_ENOUGH_COIN:
        case MSG_MAX_FRIENDS:
        {
            auto btnClose = MyMenuItem::create("img/message/btn_close_nor.png", "img/message/btn_close_act.png", "", CC_CALLBACK_1(MessageLayer::OnCancel, this));
            btnClose->setScale(m_fSmallScale);
            btnClose->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnClose->setPosition(Vec2(m_szDevice.width /2 + 288.0f * m_fSmallScale, m_szDevice.height / 2 + 150.5f * m_fSmallScale));
            menu->addChild(btnClose);
            break;
        }
        case MSG_NOTIFY_FRIEND_MATCH:
        {
            auto btnAction = MyMenuItem::create("img/message/btn_play_nor.png", "img/message/btn_play_act.png", "", CC_CALLBACK_1(MessageLayer::OnAction, this));
            btnAction->setScale(m_fSmallScale);
            btnAction->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnAction->setPosition(Vec2(m_szDevice.width /2, m_szDevice.height / 2 - 85.0f * m_fSmallScale));
            menu->addChild(btnAction);
            
            auto btnClose = MyMenuItem::create("img/message/btn_close_nor.png", "img/message/btn_close_act.png", "", CC_CALLBACK_1(MessageLayer::OnCancel, this));
            btnClose->setScale(m_fSmallScale);
            btnClose->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnClose->setPosition(Vec2(m_szDevice.width /2 + 288.0f * m_fSmallScale, m_szDevice.height / 2 + 150.5f * m_fSmallScale));
            menu->addChild(btnClose);
            break;
        }
     /*   case MSG_NOT_ENOUGH_COIN:
        {
            auto btnAction = MyMenuItem::create("img/message/btn_purchase_nor.png", "img/message/btn_purchase_act.png", "", CC_CALLBACK_1(MessageLayer::OnAction, this));
            btnAction->setScale(m_fSmallScale);
            btnAction->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnAction->setPosition(Vec2(m_szDevice.width /2, m_szDevice.height / 2 - 85.0f * m_fSmallScale));
            menu->addChild(btnAction);
            
            auto btnClose = MyMenuItem::create("img/message/btn_close_nor.png", "img/message/btn_close_act.png", "", CC_CALLBACK_1(MessageLayer::OnCancel, this));
            btnClose->setScale(m_fSmallScale);
            btnClose->setAnchorPoint(Vec2(0.5f, 0.5f));
            btnClose->setPosition(Vec2(m_szDevice.width /2 + 288.0f * m_fSmallScale, m_szDevice.height / 2 + 150.5f * m_fSmallScale));
            menu->addChild(btnClose);
            break;
        }*/
        default:
            break;
    }
}
