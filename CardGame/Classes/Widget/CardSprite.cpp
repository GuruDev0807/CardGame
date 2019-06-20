#include "CardSprite.h"
#include "../Layer/GameLayer.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"

CardSprite* CardSprite::create(const int nValue, const int nCardNum, const int nCardFlower, UserInfo* pPlayer, const std::string& frontFileName, const std::string& backFileName)
{
    CardSprite *ret = new (std::nothrow) CardSprite();
    ret->init(nValue, nCardNum, nCardFlower, pPlayer, frontFileName, backFileName);
    ret->autorelease();
    return ret;
}

CardSprite::CardSprite()
{
    m_pFrontSprite = NULL;
    m_pBackSprite = NULL;
    m_pOverlaySprite = NULL;
    m_bActived = false;
}

void CardSprite::init(const int nValue, const int nCardNum, const int nCardFlower, UserInfo* pPlayer, const std::string& frontFileName, const std::string& backFileName)
{
    m_nValue = nValue;
    m_nCardNum = nCardNum;
    m_nCardFlower = nCardFlower;
    
    m_pPlayer = pPlayer;
    
    m_pFrontSprite = Sprite::create(frontFileName);
    m_pBackSprite = Sprite::create(backFileName);
    m_pOverlaySprite = Sprite::create(frontFileName);
    m_pOverlaySprite->setColor(Color3B(200, 200, 200));
    m_pOverlaySprite->setVisible(false);
    
    m_pFrontSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pBackSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pOverlaySprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    this->addChild(m_pFrontSprite);
    this->addChild(m_pBackSprite);
    this->addChild(m_pOverlaySprite);
    
    m_pTouchListener = EventListenerTouchOneByOne::create();
    m_pTouchListener->onTouchBegan = CC_CALLBACK_2(CardSprite::onTouchBegan, this);
    m_pTouchListener->onTouchMoved = CC_CALLBACK_2(CardSprite::onTouchMoved, this);
    m_pTouchListener->onTouchEnded = CC_CALLBACK_2(CardSprite::onTouchEnded, this);
    m_pTouchListener->onTouchCancelled = CC_CALLBACK_2(CardSprite::onTouchCancelled, this);
    m_pTouchListener->setSwallowTouches(true);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_pTouchListener, this);
}

bool CardSprite::onTouchBegan(Touch *touch, Event *event)
{
    if(m_pPlayer == NULL)
        return false;
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pTurnPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pTurnPlayer == NULL)
       return false;
    
    if(pUserInfo->m_strId != pTurnPlayer->m_strId)
        return false;
    
    if(m_pPlayer->m_strId != pTurnPlayer->m_strId)
       return false;
    
    if(((GameLayer*)g_pCurrentLayer)->isTimeoutPlayer(m_pPlayer))
        return false;
    
    Vec2 curTouchPosition = Director::getInstance()->convertToGL(touch->getLocationInView());
    Size contentSize = m_pFrontSprite->getContentSize();
    Vec2 position = this->getPosition();
    Rect rect = Rect(position.x - contentSize.width / 2 * g_pCurrentLayer->m_fSmallScale, position.y - contentSize.height / 2 * g_pCurrentLayer->m_fSmallScale,
                     contentSize.width * g_pCurrentLayer->m_fSmallScale, contentSize.height * g_pCurrentLayer->m_fSmallScale);
    
    if(!rect.containsPoint(curTouchPosition))
        return false;
    
    m_pOverlaySprite->setVisible(true);
    return true;
}

void CardSprite::onTouchMoved(Touch *touch, Event *event)
{
    
}

void CardSprite::onTouchEnded(Touch *touch, Event *event)
{
    m_pOverlaySprite->setVisible(false);
    setActive(!m_bActived);
        ((GameLayer*)g_pCurrentLayer)->chooseCard(m_pPlayer);
}

void CardSprite::onTouchCancelled(Touch *touch, Event *event)
{
    m_pOverlaySprite->setVisible(false);
}

void CardSprite::showFrontSprite()
{
    m_pFrontSprite->setVisible(true);
}

void CardSprite::hideFrontSprite()
{
    m_pFrontSprite->setVisible(false);
}

void CardSprite::showBackSprite()
{
    m_pBackSprite->setVisible(true);
}

void CardSprite::hideBackSprite()
{
    m_pBackSprite->setVisible(false);
}

void CardSprite::setPlayer(UserInfo* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CardSprite::setActive(bool bActive)
{
    m_bActived = bActive;
    updatePosition();
    
    if(m_pPlayer != NULL)
    {
        if(m_bActived)
            m_pPlayer->m_vecActiveCards.pushBack(this);
        else
            m_pPlayer->m_vecActiveCards.eraseObject(this);
    }
}

bool CardSprite::isActive()
{
    return m_bActived;
}

int CardSprite::getCardValue()
{
    return m_nValue;
}

int CardSprite::getCardNum()
{
    return m_nCardNum;
}

int CardSprite::getCardFlower()
{
    return m_nCardFlower;
}

void CardSprite::updatePosition()
{
    float deltaY = m_bActived ? 35.0f * g_pCurrentLayer->m_fSmallScale : -35.0f * g_pCurrentLayer->m_fSmallScale;
    this->setPositionY(this->getPositionY() + deltaY);
}
