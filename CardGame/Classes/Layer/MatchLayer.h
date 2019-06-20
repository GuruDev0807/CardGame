#ifndef __CARDGAME_MATCH_LAYER_H__
#define __CARDGAME_MATCH_LAYER_H__

#include "BaseLayer.h"

class MatchLayer : public BaseLayer
{
public:
    static Scene* createScene(int nRoomType, int nCoin);
    
    virtual ~MatchLayer();
    virtual bool init()override;
    void init(int nRoomType, int nCoin);
    virtual void onEnter()override;
    virtual void onReceiveMessage(std::string packet_name, std::string message)override;
    
    void OnBack(Ref* pSender);
    
    void startMatching(float dt);
    void updateStatus(float dt);
    
private:
    void createSprites();
    void createLabels();
    void createMenu();
    void startJoining(std::string strRoomId);
    
private:
    int m_nRoomType;
    int m_nCoin;
    int m_nStep;
    int m_nTimeIndex;
    
public:
    CREATE_FUNC(MatchLayer);
};

#endif // __CARDGAME_MATCH_LAYER_H__
