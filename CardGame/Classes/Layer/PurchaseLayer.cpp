#include "PurchaseLayer.h"
#include "LobbyLayer.h"
#include "MessageLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Widget/MyMenuItem.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"

Scene* PurchaseLayer::createScene()
{
    Scene* scene = Scene::create();
    PurchaseLayer* layer = PurchaseLayer::create();
    scene->addChild(layer);
    return scene;
}

PurchaseLayer::~PurchaseLayer()
{
}

bool PurchaseLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_PURCHASE;
    g_pCurrentLayer = this;
    
    createSprites();
    createMenu();
    
    return true;
}

void PurchaseLayer::onEnter()
{
    BaseLayer::onEnter();
}

void PurchaseLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == RESPONSE_UPDATE_COIN)
    {
        hideLoading();
        UserInfo* pUserInfo = PacketParser::getInstance()->parseUserInfo(message);
        GameData::getInstance()->setUserInfo(pUserInfo);
    }
}

void PurchaseLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
}

void PurchaseLayer::OnPurchase500(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    purchase500();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
}

void PurchaseLayer::OnPurchase1250(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    purchase1250();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
}

void PurchaseLayer::OnPurchase2500(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    purchase2500();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
}

void PurchaseLayer::OnPurchase5000(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    purchase5000();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
}

void PurchaseLayer::updateCoin(int plus_coin)
{
    showLoading();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(REQUEST_UPDATE_COIN, PacketParser::getInstance()->makeRequestUpdateCoinReq(pUserInfo->m_strId, plus_coin));
}

void PurchaseLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTitle = Sprite::create("img/purchase/title_purchase.png");
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

void PurchaseLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(PurchaseLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 135.0f * m_fSmallScale));
    
    auto btnPurchase500 = MyMenuItem::create("img/purchase/btn_500.png", "img/purchase/btn_500.png", "", CC_CALLBACK_1(PurchaseLayer::OnPurchase500, this));
    btnPurchase500->setScale(m_fSmallScale);
    btnPurchase500->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPurchase500->setPosition(Vec2(m_szDevice.width / 2 - 455.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnPurchase1250 = MyMenuItem::create("img/purchase/btn_1250.png", "img/purchase/btn_1250.png", "", CC_CALLBACK_1(PurchaseLayer::OnPurchase1250, this));
    btnPurchase1250->setScale(m_fSmallScale);
    btnPurchase1250->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPurchase1250->setPosition(Vec2(m_szDevice.width / 2 - 152.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnPurchase2500 = MyMenuItem::create("img/purchase/btn_2500.png", "img/purchase/btn_2500.png", "", CC_CALLBACK_1(PurchaseLayer::OnPurchase2500, this));
    btnPurchase2500->setScale(m_fSmallScale);
    btnPurchase2500->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPurchase2500->setPosition(Vec2(m_szDevice.width / 2 + 152.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto btnPurchase5000 = MyMenuItem::create("img/purchase/btn_5000.png", "img/purchase/btn_5000.png", "", CC_CALLBACK_1(PurchaseLayer::OnPurchase5000, this));
    btnPurchase5000->setScale(m_fSmallScale);
    btnPurchase5000->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPurchase5000->setPosition(Vec2(m_szDevice.width / 2 + 455.0f * m_fSmallScale, m_szDevice.height / 2 - 10.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, btnPurchase500, btnPurchase1250, btnPurchase2500, btnPurchase5000, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}
