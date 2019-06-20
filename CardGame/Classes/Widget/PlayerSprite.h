#ifndef __CARDGAME_PLAYER_SPRITE_H__
#define __CARDGAME_PLAYER_SPRITE_H__

#include "../Common/Common.h"

class UserInfo;

class PlayerSprite : public Sprite
{
public:
    static PlayerSprite* create(UserInfo* pPlayer, int nDisplayPos);
    
    void active();
    void deactive();
    bool isTimeout();
    void showPickup();
    void hidePickup();
    void showKnock();
    void hideKnock();
    
    void updateClock(float dt);
    void updateInfo();
    void updateAvatar();
    
private:
    PlayerSprite();
    void init(UserInfo* pPlayer, int nDisplayPos);
    void createAvatarLayer(std::string fileName, std::string downloadLink);
    
public:
    UserInfo*   m_pPlayer;
    int         m_nDisplayPos;
    float       m_fRemainTime;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(PlayerSprite);
};

#endif /* __CARDGAME_PLAYER_SPRITE_H__ */
