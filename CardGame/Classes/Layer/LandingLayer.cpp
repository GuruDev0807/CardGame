#include "LandingLayer.h"
#include "LobbyLayer.h"

Scene* LandingLayer::createScene()
{
    Scene* scene = Scene::create();
    LandingLayer* layer = LandingLayer::create();
    scene->addChild(layer);
    return scene;
}

LandingLayer::~LandingLayer()
{
}

bool LandingLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_LANDING;
    g_pCurrentLayer = this;
    
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 255));
    addChild(colorLayer, LAYER_ZORDER1);
    
    auto sprLogo = Sprite::create("img/landing/landing_bg.png");
    sprLogo->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprLogo->setScale(m_fSmallScale);
    sprLogo->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprLogo, LAYER_ZORDER1);
    
    auto callback = CallFunc::create(std::bind( &LandingLayer::completedLanding, this));
    sprLogo->runAction(Sequence::create(DelayTime::create(3.0f), callback, nullptr));
    
    return true;
}

void LandingLayer::completedLanding()
{
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
}
