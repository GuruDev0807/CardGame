#include "PlayerSprite.h"
#include "../Layer/GameLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"

#define LIMIT_TIME          30

Vec2 g_activePos = Vec2(0.0f, 0.0f);
Vec2 g_AvatarPos = Vec2(1.0f, 0.0f);
Vec2 g_BorderPos = Vec2(0.0f, 0.0f);
Vec2 g_InfoPos = Vec2(1.0f, -46.0f);
Vec2 g_TimerBgPos = Vec2(-2.0f, 25.0f);
//Vec2 g_TimerPos = Vec2(0.0f, 22.5f);
Vec2 g_TimerPos = Vec2(-3.5f, 6.0f);
Vec2 g_NamePos = Vec2(1.0f, -60.0f);
Vec2 g_CoinPos = Vec2(-19.0f, -38.0f);
Vec2 g_LevelPos = Vec2(51.0f, -38.0f);
Vec2 g_PickupPos = Vec2(60.0f, 51.0f);
Vec2 g_KnockPos = Vec2(0.0f, 71.0f);

enum PLAYER_TAG {
    PT_ACTIVE = 1,
    PT_AVATAR = 2,
    PT_BORDER = 3,
    PT_INFO = 4,
    PT_TIMER_BG = 5,
    PT_TIMER = 6,
    PT_NAME = 7,
    PT_COIN = 8,
    PT_LEVEL = 9,
    PT_PICKUP = 10,
    PT_KNOCK = 11,
};

PlayerSprite* PlayerSprite::create(UserInfo* pPlayer, int nDisplayPos)
{
    PlayerSprite *ret = new (std::nothrow) PlayerSprite();
    ret->init(pPlayer, nDisplayPos);
    ret->autorelease();
    return ret;
}

void PlayerSprite::active()
{
    m_fRemainTime = LIMIT_TIME;
    
    Sprite* pActive = (Sprite*) getChildByTag(PT_ACTIVE);
    Sprite* pTimerBg = (Sprite*) getChildByTag(PT_TIMER_BG);
    ProgressTimer* pTimer = (ProgressTimer*) getChildByTag(PT_TIMER);
    
    pActive->setVisible(true);
    pTimerBg->setVisible(true);
    pTimer->setVisible(true);
    pTimer->setPercentage(m_fRemainTime / LIMIT_TIME * 100.0f);
    
    schedule(schedule_selector(PlayerSprite::updateClock), 0.1f);
}

void PlayerSprite::deactive()
{
    Sprite* pActive = (Sprite*) getChildByTag(PT_ACTIVE);
    Sprite* pTimerBg = (Sprite*) getChildByTag(PT_TIMER_BG);
    ProgressTimer* pTimer = (ProgressTimer*) getChildByTag(PT_TIMER);
    
    pActive->setVisible(false);
    pTimerBg->setVisible(false);
    pTimer->setVisible(false);
    
    unschedule(schedule_selector(PlayerSprite::updateClock));
}

bool PlayerSprite::isTimeout()
{
    return m_fRemainTime == 0;
}

void PlayerSprite::showPickup()
{
    Sprite* pPickup = (Sprite*) getChildByTag(PT_PICKUP);
    pPickup->setVisible(false);
}

void PlayerSprite::hidePickup()
{
    Sprite* pPickup = (Sprite*) getChildByTag(PT_PICKUP);
    pPickup->setVisible(false);
}

void PlayerSprite::showKnock()
{
    Sprite* pKnock = (Sprite*) getChildByTag(PT_KNOCK);
    pKnock->setVisible(true);
}

void PlayerSprite::hideKnock()
{
    Sprite* pKnock = (Sprite*) getChildByTag(PT_KNOCK);
    pKnock->setVisible(false);
}

void PlayerSprite::updateClock(float dt)
{
    m_fRemainTime -= dt;
    if(m_fRemainTime < 0)
        m_fRemainTime = 0;
    
    ProgressTimer* pTimer = (ProgressTimer*) getChildByTag(PT_TIMER);
    float percent = m_fRemainTime / LIMIT_TIME * 100.0f;
    pTimer->setPercentage(percent);
    
    if(percent <= 25.0f)
    {
        m_fRemainTime = 0;
        unschedule(schedule_selector(PlayerSprite::updateClock));
        
        UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
        if(pUserInfo->m_strId == m_pPlayer->m_strId)
        {
            showPickup();
            ((GameLayer*)g_pCurrentLayer)->timeoutPlayer(m_pPlayer);
        }
    }
}

void PlayerSprite::updateInfo()
{
    Label* pLabelCoin = (Label*) getChildByTag(PT_COIN);
    Label* pLabelLevel = (Label*) getChildByTag(PT_LEVEL);
    
    pLabelCoin->setString(getStringFromInt(m_pPlayer->m_nCoin));
    pLabelLevel->setString(getStringFromInt(m_pPlayer->getDisplayLevel()));
}

void PlayerSprite::updateAvatar()
{
    createAvatarLayer(m_pPlayer->m_strAvatar, m_pPlayer->m_strAvatarUrl);
}

PlayerSprite::PlayerSprite()
{
}

void PlayerSprite::init(UserInfo* pPlayer, int nDisplayPos)
{
    m_pPlayer = pPlayer;
    m_nDisplayPos = nDisplayPos;
    
    createAvatarLayer(pPlayer->m_strAvatar, pPlayer->m_strAvatarUrl);
    
   /* auto sprAvatar = Sprite::create("img/game/default_avatar.png");
    sprAvatar->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprAvatar->setPosition(g_AvatarPos);
    sprAvatar->setTag(PT_AVATAR);
    addChild(sprAvatar);*/
    
    auto sprAvatarBorder = Sprite::create("img/game/avatar_border.png");
    sprAvatarBorder->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprAvatarBorder->setPosition(g_BorderPos);
    sprAvatarBorder->setTag(PT_BORDER);
    addChild(sprAvatarBorder, LAYER_ZORDER2);
    
    auto sprInfo = Sprite::create("img/game/player_info.png");
    sprInfo->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprInfo->setPosition(g_InfoPos);
    sprInfo->setTag(PT_INFO);
    addChild(sprInfo, LAYER_ZORDER3);
    
    auto sprTimerBg = Sprite::create("img/game/player_timer_bg.png");
    sprTimerBg->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTimerBg->setPosition(Vec2(g_TimerBgPos));
    sprTimerBg->setTag(PT_TIMER_BG);
    sprTimerBg->setVisible(false);
    addChild(sprTimerBg, LAYER_ZORDER4);
    
    auto sprTimer = Sprite::create("img/game/player_timer1.png");
    auto progressTimer = ProgressTimer::create(sprTimer);
    progressTimer->setAnchorPoint(Vec2(0.5f, 0.5f));
    progressTimer->setPosition(g_TimerPos);
    progressTimer->setType(ProgressTimer::Type::RADIAL);
    progressTimer->setMidpoint(Vec2(0.5f, 0.5f));
    progressTimer->setPercentage(50.0f);
    progressTimer->setRotation(145);
    progressTimer->setTag(PT_TIMER);
    progressTimer->setVisible(false);
    addChild(progressTimer, LAYER_ZORDER5);
    
    auto sprActive = Sprite::create("img/game/active_player.png");
    sprActive->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprActive->setPosition(g_activePos);
    sprActive->setTag(PT_ACTIVE);
    sprActive->setVisible(false);
    addChild(sprActive, LAYER_ZORDER6);
    
    TTFConfig ttfConfig1("fonts/ImperialStd-Bold.ttf", 13.0f);
    Color3B color1 = Color3B(255, 255, 255);
    Color3B color2 = Color3B(249, 227, 125);
    
    Size size = Director::getInstance()->getWinSize();
    
    auto lblName = Label::createWithTTF(ttfConfig1, pPlayer->m_strName, TextHAlignment::CENTER, size.width);
    lblName->setAnchorPoint(Vec2(0.5f, 0.5f));
    lblName->setPosition(g_NamePos);
    lblName->setColor(color1);
    lblName->setTag(PT_NAME);
    addChild(lblName, LAYER_ZORDER7);
    
    auto lblCoin = Label::createWithTTF(ttfConfig1, getStringFromInt(pPlayer->m_nCoin), TextHAlignment::CENTER, size.width);
    lblCoin->setAnchorPoint(Vec2(0.5f, 0.5f));
    lblCoin->setPosition(g_CoinPos);
    lblCoin->setColor(color2);
    lblCoin->setTag(PT_COIN);
    addChild(lblCoin, LAYER_ZORDER8);
    
    auto lblLevel = Label::createWithTTF(ttfConfig1, getStringFromInt(pPlayer->getDisplayLevel()), TextHAlignment::CENTER, size.width);
    lblLevel->setAnchorPoint(Vec2(0.5f, 0.5f));
    lblLevel->setPosition(g_LevelPos);
    lblLevel->setColor(color2);
    lblLevel->setTag(PT_LEVEL);
    addChild(lblLevel, LAYER_ZORDER9);
    
    auto sprPickup = Sprite::create("img/game/player_pickup.png");
    sprPickup->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprPickup->setPosition(g_PickupPos);
    sprPickup->setTag(PT_PICKUP);
    sprPickup->setVisible(false);
    addChild(sprPickup, LAYER_ZORDER10);
    
    auto sprKnock = Sprite::create("img/game/player_knock.png");
    sprKnock->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprKnock->setPosition(g_KnockPos);
    sprKnock->setTag(PT_KNOCK);
    sprKnock->setVisible(false);
    addChild(sprKnock, LAYER_ZORDER10);
}

void PlayerSprite::createAvatarLayer(std::string fileName, std::string downloadLink)
{
    Layer* avatarLayer = (Layer*) getChildByTag(PT_AVATAR);
    if(avatarLayer != nullptr)
    {
        avatarLayer->removeFromParentAndCleanup(true);
        avatarLayer = nullptr;
    }
    
    bool bDownloadNeed = false;
    std::string file_name = "img/setting/profile_default.png";
    if(fileName != "")
    {
        std::string writablePath = FileUtils::getInstance()->getWritablePath();
        std::string fullPath = writablePath + fileName;
        
        if(FileUtils::getInstance()->isFileExist(fullPath))
            file_name = fullPath;
        else
            bDownloadNeed = true;
    }
    
    auto content_spr = Sprite::create(file_name);
    content_spr->setScale(0.835f);
    
    auto stencil = DrawNode::create();
    stencil->drawSolidCircle(Vec2(0.0f, 0.0f), 142.0f / 2, 0.0f, 32.0f, Color4F::GREEN);
    
    auto clipper = ClippingNode::create();
    clipper->setAnchorPoint(Vec2(0.5f, 0.5f));
    clipper->setPosition(g_AvatarPos);
    clipper->setStencil(stencil);
    clipper->addChild(content_spr);
    
    avatarLayer = Layer::create();
    avatarLayer->addChild(clipper);
    avatarLayer->setTag(PT_AVATAR);
    avatarLayer->setName(fileName);
    addChild(avatarLayer, LAYER_ZORDER1);
    
    if(bDownloadNeed) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        downloadAvatar(fileName, downloadLink);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#endif
    }
}
