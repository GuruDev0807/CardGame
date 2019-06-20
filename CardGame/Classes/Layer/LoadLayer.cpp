#include "LoadLayer.h"

LoadLayer::~LoadLayer()
{
}

bool LoadLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_LOAD;
    
    auto pOverLayLayer = LayerColor::create(Color4B(0, 0, 0, 125));
    pOverLayLayer->setPosition(Vec2(0, 0));
    addChild(pOverLayLayer, LAYER_ZORDER1);
    
    auto sprLoad = Sprite::create("img/setting/load.png");
    sprLoad->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprLoad->setScale(m_fSmallScale);
    sprLoad->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprLoad, LAYER_ZORDER1);
    
    sprLoad->runAction(RepeatForever::create(RotateBy::create(3.0f, 360.0f)));
    
    return true;
}

void LoadLayer::onEnter()
{
    BaseLayer::onEnter();
    
    m_pTouchListener = EventListenerTouchOneByOne::create();
    m_pTouchListener->onTouchBegan = CC_CALLBACK_2(LoadLayer::onTouchBegan, this);
    m_pTouchListener->onTouchMoved = CC_CALLBACK_2(LoadLayer::onTouchMoved, this);
    m_pTouchListener->onTouchEnded = CC_CALLBACK_2(LoadLayer::onTouchEnded, this);
    m_pTouchListener->onTouchCancelled = CC_CALLBACK_2(LoadLayer::onTouchCancelled, this);
    m_pTouchListener->setSwallowTouches(true);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_pTouchListener, this);
}

bool LoadLayer::onTouchBegan(Touch *touch, Event *event)
{
    return true;
}

void LoadLayer::onTouchMoved(Touch *touch, Event *event)
{
    
}

void LoadLayer::onTouchEnded(Touch *touch, Event *event)
{
    
}

void LoadLayer::onTouchCancelled(Touch *touch, Event *event)
{
    
}
