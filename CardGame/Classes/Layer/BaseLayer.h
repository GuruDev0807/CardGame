#ifndef __CARDGAME_BASELAYER_H__
#define __CARDGAME_BASELAYER_H__

#include "../Common/Common.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "ObjectiveC-Interface.h"
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "../Social/SocialJNI.h"
#endif

enum LAYER_TYPE
{
    LAYER_LANDING       = 0,
    LAYER_LOBBY         = 1,
    LAYER_MESSAGE       = 2,
    LAYER_MATCH         = 3,
    LAYER_GAME          = 4,
    LAYER_GAMEOVER      = 5,
    LAYER_SETTING       = 6,
    LAYER_NOTIFICATION  = 7,
    LAYER_USERLIST      = 8,
    LAYER_FRIENDLIST    = 9,
    LAYER_LOAD          = 10,
    LAYER_MATCH_FRIEND  = 11,
    LAYER_SELECT_GAME   = 12,
    LAYER_PURCHASE      = 13,
    LAYER_HELP          = 14
};

enum LAYER_ZORDER
{
    LAYER_ZORDER1 = 1,
    LAYER_ZORDER2 = 2,
    LAYER_ZORDER3 = 3,
    LAYER_ZORDER4 = 4,
    LAYER_ZORDER5 = 5,
    LAYER_ZORDER6 = 6,
    LAYER_ZORDER7 = 7,
    LAYER_ZORDER8 = 8,
    LAYER_ZORDER9 = 9,
    LAYER_ZORDER10 = 10,
};

class MessageLayer;
class LoadLayer;

class BaseLayer : public Layer
{
public:
    virtual ~BaseLayer();
    virtual bool init();
    
    virtual void onNetClientConnected();
    virtual void onNetClientClose();
    virtual void onNetClientError(std::string error);
    virtual void onReceiveMessage(std::string packet_name, std::string message);
    virtual void onDownloadedAvatar(std::string file_name);
    
    virtual void connectToServer();
    
    void    createMessageLayer(int message_type, std::string param1 = "", std::string param2 = "", int param3 = 0);
    void    removeMessageLayer();
    void    showLoading();
    void    hideLoading();
    void    respondFriendMatchReq(std::string strFriendId, int action, int coin);
    
public:
    int     m_nType;
    Size    m_szDevice;
    Vec2    m_appOrigin;
    float   m_fScaleX;
    float   m_fScaleY;
    float   m_fSmallScale;
    float   m_fLargeScale;
    
private:
    MessageLayer*   m_pMessageLayer;
    LoadLayer*      m_pLoadLayer;
    
public:
    CREATE_FUNC(BaseLayer);
};

#endif /* __CARDGAME_BASELAYER_H__ */
