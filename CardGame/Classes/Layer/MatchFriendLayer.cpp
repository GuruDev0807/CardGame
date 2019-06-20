#include "MatchFriendLayer.h"
#include "FriendLayer.h"
#include "GameLayer.h"
#include "MessageLayer.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"
#include "../Widget/MyMenuItem.h"

enum MATCH_FRIEND_TAG
{
    MFT_LABEL_STATUS = 1,
};

Scene* MatchFriendLayer::createScene(Vector<UserInfo*> vecFriends, int nGameCoin)
{
    Scene* scene = Scene::create();
    MatchFriendLayer* layer = MatchFriendLayer::create();
    layer->init(vecFriends, nGameCoin);
    scene->addChild(layer);
    return scene;
}

MatchFriendLayer::~MatchFriendLayer()
{
}

bool MatchFriendLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    return true;
}

void MatchFriendLayer::init(Vector<UserInfo*> vecFriends, int nGameCoin) {
    
    m_nType = LAYER_MATCH_FRIEND;
    m_vecFriends = vecFriends;
    m_nTimeIndex = 0;
    m_nGameCoin = nGameCoin;
    g_pCurrentLayer = this;
    
    createSprites();
    createLabels();
    createMenu();
}

void MatchFriendLayer::onEnter()
{
    BaseLayer::onEnter();
    schedule(schedule_selector(MatchFriendLayer::updateStatus), 0.5f);
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    std::vector<std::string> strFriends;
    for(int index = 0; index < m_vecFriends.size(); index++)
    {
        UserInfo* pUser = m_vecFriends.at(index);
        strFriends.push_back(pUser->m_strId);
    }
    GameData::getInstance()->sendPacket(REQUEST_FRIEND_MATCH_REQ, PacketParser::getInstance()->makeRequestFriendMatchReq(pUserInfo->m_strId, pUserInfo->m_strName, strFriends, FRIEND_MATCH_REQ_SEND, m_nGameCoin));
}

void MatchFriendLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == RESPONSE_FRIEND_MATCH_REQ)
    {
        FriendMatchReqResult result = PacketParser::getInstance()->parseFriendMatchReq(message);
        
        if(result.nAction == FRIEND_MATCH_REQ_CLOSE)
        {
            close();
        }
        else if(result.nAction == FRIEND_MATCH_REQ_SEND)
        {
            if(result.nStatus == RES_FAILED)
            {
                unschedule(schedule_selector(MatchFriendLayer::updateStatus));
                Label* pLabelStatus = (Label*) getChildByTag(MFT_LABEL_STATUS);
                pLabelStatus->setString("");
                createMessageLayer(MSG_FRIEND_MATCH_FAILED);
            }
        }
    /*    else if(result.nAction == FRIEND_MATCH_REQ_COIN)
        {
            if(result.nStatus == RES_FAILED)
            {
                unschedule(schedule_selector(MatchFriendLayer::updateStatus));
                Label* pLabelStatus = (Label*) getChildByTag(MFT_LABEL_STATUS);
                pLabelStatus->setString("");
                createMessageLayer(MSG_NOT_ENOUGH_COIN_FRIEND);
            }
        }*/
    }
    else if(packet_name == RESPONSE_FRIEND_MATCH_RES)
    {
        NotifyFriendMatch result = PacketParser::getInstance()->parseNotifyFriendMatch(message);
        if(result.nAction == FRIEND_MATCH_REQ_DECLINE)
        {
            unschedule(schedule_selector(MatchFriendLayer::updateStatus));
            Label* pLabelStatus = (Label*) getChildByTag(MFT_LABEL_STATUS);
            pLabelStatus->setString("");
            createMessageLayer(MSG_FRIEND_MATCH_DECLINE);
        }
        else if(result.nAction == FRIEND_MATCH_REQ_ACCEPT)
        {
        }
    }
    else if(packet_name == PT_RES_JOINROOM)
    {
        unschedule(schedule_selector(MatchFriendLayer::updateStatus));
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameLayer::createScene(), Color3B(0, 0, 0)));
    }
}

void MatchFriendLayer::OnBack(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    unschedule(schedule_selector(MatchFriendLayer::updateStatus));
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    std::vector<std::string> strFriends;
    for(int index = 0; index < m_vecFriends.size(); index++)
    {
        UserInfo* pUser = m_vecFriends.at(index);
        strFriends.push_back(pUser->m_strId);
    }
    GameData::getInstance()->sendPacket(REQUEST_FRIEND_MATCH_REQ, PacketParser::getInstance()->makeRequestFriendMatchReq(pUserInfo->m_strId, pUserInfo->m_strName, strFriends, FRIEND_MATCH_REQ_CLOSE, m_nGameCoin));
}

void MatchFriendLayer::updateStatus(float dt)
{
    std::string strStatus = "Waiting response from friends";
    
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
    
    Label* pLabelStatus = (Label*) getChildByTag(MFT_LABEL_STATUS);
    pLabelStatus->setString(strStatus);
}

void MatchFriendLayer::createSprites()
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

void MatchFriendLayer::createLabels()
{
    TTFConfig ttfConfig("fonts/ImperialStd-Regular.ttf", 50.0f * m_fSmallScale);
    Color3B color = Color3B(255, 255, 255);
    
    auto labelStatus = Label::createWithTTF(ttfConfig, "", TextHAlignment::LEFT, m_szDevice.width);
    labelStatus->setAnchorPoint(Vec2(0.0f, 0.5f));
    labelStatus->setPosition(Vec2(m_szDevice.width / 2 - 310.0f * m_fSmallScale, m_szDevice.height / 2 - 245.0f * m_fSmallScale));
    labelStatus->setColor(color);
    labelStatus->setTag(MFT_LABEL_STATUS);
    addChild(labelStatus, LAYER_ZORDER1);
}

void MatchFriendLayer::createMenu()
{
    auto btnBack = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(MatchFriendLayer::OnBack, this));
    btnBack->setScale(m_fSmallScale);
    btnBack->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnBack->setPosition(Vec2(108.0f * m_fSmallScale, m_szDevice.height - 80.0f * m_fSmallScale));
    
    auto menu = Menu::create(btnBack, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    addChild(menu, LAYER_ZORDER2);
}

void MatchFriendLayer::close()
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, FriendLayer::createScene(FST_PLAY), Color3B(0, 0, 0)));
}
