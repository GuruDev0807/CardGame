#include "MatchLayer.h"
#include "SelectGameLayer.h"
#include "GameLayer.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"
#include "../Widget/MyMenuItem.h"

enum MATCH_STEP
{
    MS_FIND = 0,
    MS_JOIN = 1,
};

enum MATCH_TAG
{
    MT_LABEL_STATUS = 1,
};

Scene* MatchLayer::createScene(int nRoomType, int nCoin)
{
    Scene* scene = Scene::create();
    MatchLayer* layer = MatchLayer::create();
    layer->init(nRoomType, nCoin);
    scene->addChild(layer);
    return scene;
}

MatchLayer::~MatchLayer()
{
}

bool MatchLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    return true;
}

void MatchLayer::init(int nRoomType, int nCoin) {
    
    m_nType = LAYER_MATCH;
    m_nRoomType = nRoomType;
    m_nCoin = nCoin;
    m_nStep = MS_FIND;
    m_nTimeIndex = 0;
    g_pCurrentLayer = this;
    
    createSprites();
    createLabels();
    createMenu();
}

void MatchLayer::onEnter()
{
    BaseLayer::onEnter();
    schedule(schedule_selector(MatchLayer::startMatching), 1.0f);
}

void MatchLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == PT_RES_MATCH)
    {
        std::string strRoomId = PacketParser::getInstance()->parseMatchResponse(message);
        startJoining(strRoomId);
    }
    else if(packet_name == PT_RES_JOINROOM)
    {
        unschedule(schedule_selector(MatchLayer::updateStatus));
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameLayer::createScene(), Color3B(0, 0, 0)));
    }
}

void MatchLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    unschedule(schedule_selector(MatchLayer::updateStatus));
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(PT_REQ_CLOSEMATCH, PacketParser::getInstance()->makeRequestCloseMatch(pUserInfo->m_strId));
    Vector<UserInfo*> vecFriends;
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, SelectGameLayer::createScene(m_nRoomType, vecFriends), Color3B(0, 0, 0)));
}

void MatchLayer::startMatching(float dt)
{
    unschedule(schedule_selector(MatchLayer::startMatching));
    
    m_nStep = MS_FIND;
    m_nTimeIndex = 0;
    
    Label* pLabelStatus = (Label*) getChildByTag(MT_LABEL_STATUS);
    pLabelStatus->setPosition(Vec2(m_szDevice.width / 2 - 247.0f * m_fSmallScale, m_szDevice.height / 2 - 245.0f * m_fSmallScale));
    
    schedule(schedule_selector(MatchLayer::updateStatus), 0.5f);
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(PT_REQ_MATCH, PacketParser::getInstance()->makeRequestMatch(pUserInfo->m_strId, m_nRoomType, m_nCoin));
}

void MatchLayer::updateStatus(float dt)
{
    std::string strStatus = "";
    if(m_nStep == MS_FIND)
        strStatus = "Finding opponent player";
    else if(m_nStep == MS_JOIN)
        strStatus = "Joinging to game room";
    
    m_nTimeIndex ++;
    if(m_nTimeIndex > 3)
        m_nTimeIndex = 1;
    
    switch (m_nTimeIndex) {
        case 1:
            strStatus += " .";
            break;
        case 2:
            strStatus += " ..";
            break;
        case 3:
            strStatus += " ...";
            break;
        default:
            break;
    }
    
    Label* pLabelStatus = (Label*) getChildByTag(MT_LABEL_STATUS);
    pLabelStatus->setString(strStatus);
}

void MatchLayer::createSprites()
{
    auto sprBackground = Sprite::create("img/lobby/lobby_bg.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fLargeScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprLogo = Sprite::create("img/lobby/logo.png");
    sprLogo->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprLogo->setScale(m_fSmallScale);
    sprLogo->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2 + 40.0f * m_fSmallScale));
    addChild(sprLogo, LAYER_ZORDER1);
}

void MatchLayer::createLabels()
{
    TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 50.0f * m_fSmallScale);
    Color3B color = Color3B(255, 255, 255);
    
    auto labelStatus = Label::createWithTTF(ttfConfig, "", TextHAlignment::LEFT, m_szDevice.width);
    labelStatus->setAnchorPoint(Vec2(0.0f, 0.5f));
    labelStatus->setPosition(Vec2(m_szDevice.width / 2 - 247.0f * m_fSmallScale, m_szDevice.height / 2 - 245.0f * m_fSmallScale));
    labelStatus->setColor(color);
    labelStatus->setTag(MT_LABEL_STATUS);
    addChild(labelStatus, LAYER_ZORDER1);
}

void MatchLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(MatchLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}

void MatchLayer::startJoining(std::string strRoomId)
{
    m_nStep = MS_JOIN;
    m_nTimeIndex = 0;
    
    Label* pLabelStatus = (Label*) getChildByTag(MT_LABEL_STATUS);
    pLabelStatus->setPosition(Vec2(m_szDevice.width / 2 - 230.0f * m_fSmallScale, m_szDevice.height / 2 - 245.0f * m_fSmallScale));
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(PT_REQ_JOINROOM, PacketParser::getInstance()->makeRequestJoinRoom(pUserInfo->m_strId, strRoomId));
}
