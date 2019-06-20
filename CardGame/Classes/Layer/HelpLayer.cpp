#include "HelpLayer.h"
#include "SettingLayer.h"
#include "../Common/GameData.h"
#include "../Widget/MyMenuItem.h"

std::string help = "CrazyBlackjack is about getting rid of all of your cards in order to be the last person left. It is called crazy blackjack as this game is commonly referred to as crazy 8s and blackjack.\n\nDue to blackjack varying rules this form of blackjack has a fixed set of rules so that there is no more confusion when CrazyBlackjack is played and one set of rules can be established.\n\nA - Changes the suit and can be placed on anything\n\n2 - Pickup two cards (unless you have a 2 to put on it which will therefore make the next person pickup\n\n8 - Miss a turn, misses the next person in rotation\n\n10 - Reverses the direction, this reverses the direction the game is going, if it's between two people then it is similar properties to an 8\n\nBlackjack - This means pickup 5 cards\n\nRedjack - This stops the blackjack and stops the person from picking up 5 if they have been made to do so\n\nKnocking - This will appear when a player is able to go out on their next turn\n\nruns - Runs are aloud and must follow a consistency, there is no number of how many your aloud in a run as long as the numbers and suits all follow EG: 5 of spades, 6 of spades and 6 of hearts";

Scene* HelpLayer::createScene()
{
    Scene* scene = Scene::create();
    HelpLayer* layer = HelpLayer::create();
    scene->addChild(layer);
    return scene;
}

HelpLayer::~HelpLayer()
{
}

bool HelpLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_HELP;
    g_pCurrentLayer = this;
    
    createSprites();
    createLabels();
    createMenu();

    return true;
}

void HelpLayer::onEnter()
{
    BaseLayer::onEnter();
}

void HelpLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SettingLayer::createScene(), Color3B(0, 0, 0)));
}

void HelpLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTitle = Sprite::create("img/setting/title_how_to_play.png");
    sprTitle->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTitle->setScale(m_fSmallScale);
    sprTitle->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height - 80.0f * m_fSmallScale));
    addChild(sprTitle, LAYER_ZORDER1);
}

void HelpLayer::createLabels()
{
    TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 40.0f * m_fSmallScale);
    Color3B color = Color3B(255, 255, 255);
    
    auto pHelpScrollView = ui::ScrollView::create();
    pHelpScrollView->setContentSize(Size(1200.0f * m_fSmallScale, m_szDevice.height - 197.0f * m_fSmallScale));
    pHelpScrollView->setPosition(Vec2(m_szDevice.width / 2 - 600.0f * m_fSmallScale, 40.0f * m_fSmallScale));
    pHelpScrollView->setScrollBarAutoHideTime(0.0);
    pHelpScrollView->setScrollBarColor(Color3B::WHITE);
    pHelpScrollView->setScrollBarWidth(8 * m_fSmallScale);
    pHelpScrollView->setScrollBarPositionFromCorner(Vec2(12 * m_fSmallScale, 3 * m_fSmallScale));
    pHelpScrollView->setScrollBarOpacity(230);
    pHelpScrollView->setBounceEnabled(true);
    pHelpScrollView->setCascadeOpacityEnabled(true);
    pHelpScrollView->setInnerContainerSize(Size(1200.0f * m_fSmallScale, 1470.0f * m_fSmallScale));
    addChild(pHelpScrollView, LAYER_ZORDER2);
    
    auto description = Label::createWithTTF(ttfConfig, help, TextHAlignment::LEFT, 1160.0f * m_fSmallScale);
    description->setAnchorPoint(Vec2(0.0f, 1.0f));
    description->setPosition(Vec2(20.0f * m_fSmallScale, pHelpScrollView->getInnerContainerSize().height - 8.0f * m_fSmallScale));
    description->setColor(color);
    pHelpScrollView->addChild(description);
}

void HelpLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(HelpLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}
