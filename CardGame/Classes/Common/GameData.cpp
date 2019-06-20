#include "GameData.h"
#include "../Network/NetClient.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"
#include "../Xml/NotificationParser.h"

#define KEY_BGM_ENABLE          "cardgame_bgm_enable"
#define KEY_EFFECT_ENABLE       "cardgame_effect_enable"
#define KEY_FRIEND_COUNT        "friend_count"
#define KEY_LAST_DAILY_TIME     "last_dialy_time"

GameData* GameData::_instance = NULL;

GameData* GameData::getInstance()
{
    if(_instance == NULL)
    _instance = new GameData();
    
    return _instance;
}

GameData::GameData()
{
    init();
}

GameData::~GameData()
{
    if(m_pUserInfo != NULL)
    {
        delete m_pUserInfo;
        m_pUserInfo = NULL;
    }
    
    if(m_pGameRoom != NULL)
    {
        delete m_pGameRoom;
        m_pGameRoom = NULL;
    }
    
    if(m_pNetClient != NULL)
    {
        delete m_pNetClient;
        m_pNetClient = NULL;
    }
}

void GameData::init()
{
    m_pNetClient = NULL;
    m_pUserInfo = NULL;
    m_pGameRoom = NULL;
    m_pNotificationParser = NULL;
    
    m_bBgmEnable = UserDefault::getInstance()->getBoolForKey(KEY_BGM_ENABLE, true);
    m_bEffectEnable = UserDefault::getInstance()->getBoolForKey(KEY_EFFECT_ENABLE, true);
    m_nLastDialyTime = UserDefault::getInstance()->getIntegerForKey(KEY_LAST_DAILY_TIME, 0);
    m_nFriendCount = UserDefault::getInstance()->getIntegerForKey(KEY_FRIEND_COUNT, 0);
    m_nPrevFriendCount = m_nFriendCount;
    m_bConnected = false;
    
    loadNotificationsFromPLIST();
}

bool GameData::loadNotificationsFromPLIST()
{
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + "Notifications.plist";
    
    if(!FileUtils::getInstance()->isFileExist(fullPath))
    {
        m_pNotificationParser = new NotificationParser();
        return false;
    }
    
    m_pNotificationParser = NotificationParser::formatWithPLISTFile(fullPath.c_str());
    return true;
}

void GameData::setNetClient(NetClient* pNetClient)
{
    m_pNetClient = pNetClient;
}

NetClient* GameData::getNetClient()
{
    return m_pNetClient;
}

void GameData::initNetClient()
{
    if(m_pNetClient == NULL)
        m_pNetClient = new NetClient();
    m_pNetClient->SIOClientConnect();
}

void GameData::sendPacket(std::string packet_name, std::string packet_body)
{
    if(m_pNetClient != NULL)
        m_pNetClient->sendPacket(packet_name, packet_body);
}

void GameData::setUserInfo(UserInfo* pUserInfo)
{
    if(m_pUserInfo != NULL)
    {
        delete m_pUserInfo;
        m_pUserInfo = NULL;
    }
    
    m_pUserInfo = pUserInfo;
    if(m_pUserInfo != NULL)
    {
        m_nPrevFriendCount = m_nFriendCount;
        m_nFriendCount = (int)m_pUserInfo->m_vecFriends.size();
        UserDefault::getInstance()->setIntegerForKey(KEY_FRIEND_COUNT, m_nFriendCount);
    }
}

UserInfo* GameData::getUserInfo()
{
    return m_pUserInfo;
}

void GameData::setGameRoom(GameRoom* pGameRoom)
{
    if(m_pGameRoom != NULL)
    {
        delete m_pGameRoom;
        m_pGameRoom = NULL;
    }
    
    m_pGameRoom = pGameRoom;
}

GameRoom* GameData::getGameRoom()
{
    return m_pGameRoom;
}

NotificationParser* GameData::getNotificationParser()
{
    return m_pNotificationParser;
}

void GameData::saveNotifications()
{
    std::string writablePath = FileUtils::getInstance()->getWritablePath();
    std::string fullPath = writablePath + "Notifications.plist";
    m_pNotificationParser->saveWithPLISTFile(fullPath.c_str());
}

bool GameData::getBgmEnable()
{
    return m_bBgmEnable;
}

void GameData::setBgmEnable(bool enable)
{
    m_bBgmEnable = enable;
    UserDefault::getInstance()->setBoolForKey(KEY_BGM_ENABLE, m_bBgmEnable);
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(m_bBgmEnable ? 1.0f : 0.001f);
}

bool GameData::getEffectEnable()
{
    return m_bEffectEnable;
}

void GameData::setEffectEnable(bool enable)
{
    m_bEffectEnable = enable;
    UserDefault::getInstance()->setBoolForKey(KEY_EFFECT_ENABLE, m_bEffectEnable);
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(m_bEffectEnable ? 1.0f : 0.001f);
}

bool GameData::isConnected()
{
    return m_bConnected;
}

void GameData::setConnected(bool value)
{
    m_bConnected = value;
}

int GameData::getFriendCount()
{
    return m_nFriendCount;
}

bool GameData::hasNewFriend()
{
    return (m_nFriendCount - m_nPrevFriendCount) > 0;
}

void GameData::clearNewFriend()
{
    m_nPrevFriendCount = m_nFriendCount;
}

bool GameData::isFreeCoin()
{
    if(m_nLastDialyTime == 0)
        return true;
    
    int curTime = (int) std::time(NULL);
    int delta_last = (curTime - m_nLastDialyTime) / (60 * 60 * 24);
    if(delta_last >= 1)
        return true;
    
    return false;
}

void GameData::updateDialyTime()
{
    m_nLastDialyTime = (int) std::time(NULL);
    UserDefault::getInstance()->setIntegerForKey(KEY_LAST_DAILY_TIME, m_nLastDialyTime);
}

