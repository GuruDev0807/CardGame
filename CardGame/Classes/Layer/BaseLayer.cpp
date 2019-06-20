#include "BaseLayer.h"
#include "MessageLayer.h"
#include "LoadLayer.h"
#include "GameLayer.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"
#include "../Common/GameData.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"

BaseLayer::~BaseLayer()
{
}

bool BaseLayer::init()
{
    if(!Layer::init())
    return false;
    
    m_pMessageLayer = NULL;
    m_pLoadLayer = NULL;
    
    m_szDevice = Director::getInstance()->getWinSize();
    
    m_fScaleX = (float) (m_szDevice.width / designResolutionSize.width);
    m_fScaleY = (float) (m_szDevice.height / designResolutionSize.height);
    
    if(m_fScaleX < m_fScaleY)
    {
        m_fSmallScale = m_fScaleX;
        m_fLargeScale = m_fScaleY;
    }
    else
    {
        m_fSmallScale = m_fScaleY;
        m_fLargeScale = m_fScaleX;
    }
    
    m_appOrigin.x = (m_szDevice.width - designResolutionSize.width * m_fSmallScale) / 2;
    m_appOrigin.y = (m_szDevice.height - designResolutionSize.height * m_fSmallScale) / 2;
    
    return true;
}

void BaseLayer::onNetClientConnected()
{
    
}

void BaseLayer::onNetClientClose()
{
    
}

void BaseLayer::onNetClientError(std::string error)
{
    createMessageLayer(MSG_CONNECT_FAILED);
}

void BaseLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    if(packet_name == PT_NOTIFY_STARTGAME)
    {
        PacketParser::getInstance()->parseUpdatedGameRoom(message);
    }
    else if(packet_name == RESPONSE_NOTIFICATION)
    {
        PacketParser::getInstance()->parseNotificationList(message);
    }
    else if(packet_name == NOTIFY_NOTIFICATION)
    {
        PacketParser::getInstance()->parseNotifyNotification(message);
    }
    else if(packet_name == NOTIFY_UPDATE_PROFIE)
    {
        UserInfo* pUserInfo = PacketParser::getInstance()->parseUserInfo(message);
        GameData::getInstance()->setUserInfo(pUserInfo);
    }
    else if(packet_name == NOTIFY_FRIEND_MATCH_REQ)
    {
        NotifyFriendMatch result = PacketParser::getInstance()->parseNotifyFriendMatch(message);
        if(result.nAction == FRIEND_MATCH_REQ_SEND)
        {
            createMessageLayer(MSG_NOTIFY_FRIEND_MATCH, result.strFriendName, result.strFriendId, result.nGameCoin);
        }
        else if(result.nAction == FRIEND_MATCH_REQ_CLOSE)
        {
            removeMessageLayer();
        }
    }
    else if(packet_name == PT_RES_JOINROOM)
    {
        GameRoom* pGameRoom = PacketParser::getInstance()->parseGameRoom(message);
        GameData::getInstance()->setGameRoom(pGameRoom);
        
        if(m_nType != LAYER_MATCH && m_nType != LAYER_MATCH_FRIEND)
        {
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameLayer::createScene(), Color3B(0, 0, 0)));
        }
    }
}

void BaseLayer::onDownloadedAvatar(std::string file_name)
{
    
}

void BaseLayer::connectToServer()
{
    GameData::getInstance()->initNetClient();
}

void BaseLayer::createMessageLayer(int message_type, std::string param1, std::string param2, int param3)
{
    if(m_pMessageLayer != NULL)
        return;
    
    m_pMessageLayer = MessageLayer::create();
    m_pMessageLayer->init(message_type, param1, param2, param3);
    m_pMessageLayer->setPosition(Vec2(0.0f, 0.0f));
    m_pMessageLayer->setCascadeOpacityEnabled(true);
    addChild(m_pMessageLayer, LAYER_ZORDER10);
}

void BaseLayer::removeMessageLayer()
{
    if(m_pMessageLayer != NULL)
    {
        m_pMessageLayer->removeFromParentAndCleanup(true);
        m_pMessageLayer = NULL;
    }
}

void BaseLayer::showLoading()
{
    if(m_pLoadLayer != NULL)
        return;
    
    m_pLoadLayer = LoadLayer::create();
    m_pLoadLayer->setPosition(Vec2(0.0f, 0.0f));
    m_pLoadLayer->setCascadeOpacityEnabled(true);
    addChild(m_pLoadLayer, LAYER_ZORDER10);
}

void BaseLayer::hideLoading()
{
    if(m_pLoadLayer != NULL)
    {
        m_pLoadLayer->removeFromParentAndCleanup(true);
        m_pLoadLayer = NULL;
    }
}

void BaseLayer::respondFriendMatchReq(std::string strFriendId, int action, int coin)
{
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(REQUEST_FRIEND_MATCH_RES, PacketParser::getInstance()->makeRequestFriendMatchRes(pUserInfo->m_strId, pUserInfo->m_strName, strFriendId, action, coin));
}
