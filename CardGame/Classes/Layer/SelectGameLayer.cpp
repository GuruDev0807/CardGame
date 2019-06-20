#include "SelectGameLayer.h"
#include "LobbyLayer.h"
#include "MatchLayer.h"
#include "MatchFriendLayer.h"
#include "MessageLayer.h"
#include "PurchaseLayer.h"
#include "FriendLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Widget/MyMenuItem.h"

Scene* SelectGameLayer::createScene(int nRoomType, Vector<UserInfo*> vecFriends)
{
    Scene* scene = Scene::create();
    SelectGameLayer* layer = SelectGameLayer::create();
    layer->init(nRoomType, vecFriends);
    scene->addChild(layer);
    return scene;
}

SelectGameLayer::~SelectGameLayer()
{
}

bool SelectGameLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    return true;
}

void SelectGameLayer::init(int nRoomType, Vector<UserInfo*> vecFriends) {
    
    m_nType = LAYER_SELECT_GAME;
    m_nRoomType = nRoomType;
    m_vecFriends = vecFriends;
    g_pCurrentLayer = this;
    
    createSprites();
    createMenu();
}

void SelectGameLayer::onEnter()
{
    BaseLayer::onEnter();
}

void SelectGameLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    if(m_vecFriends.size() == 0)
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
    else
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, FriendLayer::createScene(m_nRoomType), Color3B(0, 0, 0)));
}

void SelectGameLayer::OnGame50(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    if(pMe->m_nCoin < 50)
    {
        createMessageLayer(MSG_NOT_ENOUGH_COIN);
        return;
    }
    
    if(m_vecFriends.size() == 0)
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchLayer::createScene(m_nRoomType, 50), Color3B(0, 0, 0)));
    else
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchFriendLayer::createScene(m_vecFriends, 50), Color3B(0, 0, 0)));
}

void SelectGameLayer::OnGame200(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    if(pMe->m_nCoin < 200)
    {
        createMessageLayer(MSG_NOT_ENOUGH_COIN);
        return;
    }
    
    if(m_vecFriends.size() == 0)
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchLayer::createScene(m_nRoomType, 200), Color3B(0, 0, 0)));
    else
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchFriendLayer::createScene(m_vecFriends, 200), Color3B(0, 0, 0)));
}

void SelectGameLayer::OnGame500(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    if(pMe->m_nCoin < 500)
    {
        createMessageLayer(MSG_NOT_ENOUGH_COIN);
        return;
    }
    
    if(m_vecFriends.size() == 0)
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchLayer::createScene(m_nRoomType, 500), Color3B(0, 0, 0)));
    else
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchFriendLayer::createScene(m_vecFriends, 500), Color3B(0, 0, 0)));
}

void SelectGameLayer::OnGame1000(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    UserInfo* pMe = GameData::getInstance()->getUserInfo();
    if(pMe->m_nCoin < 1000)
    {
        createMessageLayer(MSG_NOT_ENOUGH_COIN);
        return;
    }
    
    if(m_vecFriends.size() == 0)
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchLayer::createScene(m_nRoomType, 1000), Color3B(0, 0, 0)));
    else
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MatchFriendLayer::createScene(m_vecFriends, 1000), Color3B(0, 0, 0)));
}

void SelectGameLayer::showPurchase()
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, PurchaseLayer::createScene(), Color3B(0, 0, 0)));
}

void SelectGameLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTitle = Sprite::create("img/chip/title_chip.png");
    sprTitle->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTitle->setScale(m_fSmallScale);
    sprTitle->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height - 135.0f * m_fSmallScale));
    addChild(sprTitle, LAYER_ZORDER2);
    
    auto sprBottom = Sprite::create("img/lobby/lobby_bottom.png");
    sprBottom->setAnchorPoint(Vec2(0.5f, 0.0f));
    sprBottom->setScale(m_fScaleX);
    sprBottom->setPosition(Vec2(m_szDevice.width / 2, 0.0f));
    addChild(sprBottom, LAYER_ZORDER2);
}

void SelectGameLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(SelectGameLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 135.0f * m_fSmallScale));
    
    auto btnChip50 = MyMenuItem::create("img/chip/btn_chip_50.png", "img/chip/btn_chip_50.png", "", CC_CALLBACK_1(SelectGameLayer::OnGame50, this));
    btnChip50->setScale(m_fSmallScale);
    btnChip50->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnChip50->setPosition(Vec2(m_szDevice.width / 2 - 455.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnChip200 = MyMenuItem::create("img/chip/btn_chip_200.png", "img/chip/btn_chip_200.png", "", CC_CALLBACK_1(SelectGameLayer::OnGame200, this));
    btnChip200->setScale(m_fSmallScale);
    btnChip200->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnChip200->setPosition(Vec2(m_szDevice.width / 2 - 152.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnChip500 = MyMenuItem::create("img/chip/btn_chip_500.png", "img/chip/btn_chip_500.png", "", CC_CALLBACK_1(SelectGameLayer::OnGame500, this));
    btnChip500->setScale(m_fSmallScale);
    btnChip500->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnChip500->setPosition(Vec2(m_szDevice.width / 2 + 152.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnChip1000 = MyMenuItem::create("img/chip/btn_chip_1000.png", "img/chip/btn_chip_1000.png", "", CC_CALLBACK_1(SelectGameLayer::OnGame1000, this));
    btnChip1000->setScale(m_fSmallScale);
    btnChip1000->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnChip1000->setPosition(Vec2(m_szDevice.width / 2 + 455.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, btnChip50, btnChip200, btnChip500, btnChip1000, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}
