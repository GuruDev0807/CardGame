#include "GameOverLayer.h"
#include "GameLayer.h"
#include "../Widget/MyMenuItem.h"

GameOverLayer::~GameOverLayer()
{
}

bool GameOverLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    return true;
}

bool GameOverLayer::init(bool bWinner)
{
    m_nType = LAYER_GAMEOVER;
    
    createBackground(bWinner);
    createMenu();
    return true;
}

void GameOverLayer::onEnter()
{
    BaseLayer::onEnter();
    
    m_pTouchListener = EventListenerTouchOneByOne::create();
    m_pTouchListener->onTouchBegan = CC_CALLBACK_2(GameOverLayer::onTouchBegan, this);
    m_pTouchListener->onTouchMoved = CC_CALLBACK_2(GameOverLayer::onTouchMoved, this);
    m_pTouchListener->onTouchEnded = CC_CALLBACK_2(GameOverLayer::onTouchEnded, this);
    m_pTouchListener->onTouchCancelled = CC_CALLBACK_2(GameOverLayer::onTouchCancelled, this);
    m_pTouchListener->setSwallowTouches(true);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_pTouchListener, this);
}

bool GameOverLayer::onTouchBegan(Touch *touch, Event *event)
{
    return true;
}

void GameOverLayer::onTouchMoved(Touch *touch, Event *event)
{
    
}

void GameOverLayer::onTouchEnded(Touch *touch, Event *event)
{
    
}

void GameOverLayer::onTouchCancelled(Touch *touch, Event *event)
{
    
}

void GameOverLayer::OnCancel(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    ((GameLayer*) getParent())->closeGame();
}

void GameOverLayer::OnTryAgain(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    ((GameLayer*) getParent())->restartGame();
}

void GameOverLayer::createBackground(bool bWinner)
{
    auto pOverLayLayer = LayerColor::create(Color4B(0, 0, 0, 125));
    pOverLayLayer->setPosition(Vec2(0, 0));
    addChild(pOverLayLayer, LAYER_ZORDER1);
    
    auto message_bg = Sprite::create("img/message/message_bg.png");
    message_bg->setAnchorPoint(Vec2(0.5f, 0.5f));
    message_bg->setScale(m_fSmallScale);
    message_bg->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 - 18.0f * m_fSmallScale));
    addChild(message_bg, LAYER_ZORDER1);
    
    if(bWinner)
    {
        auto message_title = Sprite::create("img/message/title_win.png");
        message_title->setAnchorPoint(Vec2(0.5f, 0.5f));
        message_title->setScale(m_fSmallScale);
        message_title->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 155.5f * m_fSmallScale));
        addChild(message_title, LAYER_ZORDER1);
        
        auto message_txt = Sprite::create("img/message/message_win.png");
        message_txt->setAnchorPoint(Vec2(0.5f, 0.5f));
        message_txt->setScale(m_fSmallScale);
        message_txt->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 9.0f * m_fSmallScale));
        addChild(message_txt, LAYER_ZORDER1);
    }
    else
    {
        auto message_title = Sprite::create("img/message/title_lose.png");
        message_title->setAnchorPoint(Vec2(0.5f, 0.5f));
        message_title->setScale(m_fSmallScale);
        message_title->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 153.0f * m_fSmallScale));
        addChild(message_title, LAYER_ZORDER1);
        
        auto message_txt = Sprite::create("img/message/message_lose.png");
        message_txt->setAnchorPoint(Vec2(0.5f, 0.5f));
        message_txt->setScale(m_fSmallScale);
        message_txt->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 9.0f * m_fSmallScale));
        addChild(message_txt, LAYER_ZORDER1);
    }
}

void GameOverLayer::createMenu()
{
    auto btnClose = MyMenuItem::create("img/message/btn_close_nor.png", "img/message/btn_close_act.png", "", CC_CALLBACK_1(GameOverLayer::OnCancel, this));
    btnClose->setScale(m_fSmallScale);
    btnClose->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnClose->setPosition(Vec2(m_szDevice.width /2 + 288.0f * m_fSmallScale, m_szDevice.height / 2 + 150.5f * m_fSmallScale));
    
    auto btnAction = MyMenuItem::create("img/message/btn_tryagain_nor.png", "img/message/btn_tryagain_act.png", "", CC_CALLBACK_1(GameOverLayer::OnTryAgain, this));
    btnAction->setScale(m_fSmallScale);
    btnAction->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnAction->setPosition(Vec2(m_szDevice.width /2, m_szDevice.height / 2 - 106.5f * m_fSmallScale));
    
    auto menu = Menu::create(btnClose, btnAction, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}
