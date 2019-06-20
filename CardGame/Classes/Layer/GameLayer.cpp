#include "GameLayer.h"
#include "LobbyLayer.h"
#include "GameOverLayer.h"
#include "MessageLayer.h"
#include "../Network/Packet.h"
#include "../Network/PacketParser.h"
#include "../Widget/MyMenuItem.h"
#include "../Widget/CardSprite.h"
#include "../Common/GameData.h"
#include "../Data/UserInfo.h"
#include "../Data/GameRoom.h"

#define START_CARD_COUNT    7
#define MAX_CARD_COUNT      52
#define PICKUP_CARD_NUM     52

int MAX_PLAYERS[3] = {2, 5, 5};
int ME_POSITION[3] = {1, 4, 4};

Vec2 g_staticPlayerPos[3][5] = {
    {Vec2(381.0f, 266.0f), Vec2(-387.0f, -165.0f)},
    {Vec2(-500.0f, 43.0f), Vec2(-183.0f, 266.0f), Vec2(173.0f, 266.0f), Vec2(496.0f, 43.0f), Vec2(-387.0f, -165.0f)},
    {Vec2(-500.0f, 43.0f), Vec2(-183.0f, 266.0f), Vec2(173.0f, 266.0f), Vec2(496.0f, 43.0f), Vec2(-387.0f, -165.0f)},
};

Vec2 g_staticCardPos[3][5] = {
    {Vec2(-3.0f, 194.0f), Vec2(-3.0f, -286.5f)},
    {Vec2(-382.0f, 43.0f), Vec2(-183.0f, 147.0f), Vec2(173.0f, 147.0f), Vec2(378.0f, 43.0f), Vec2(-3.0f, -286.5f)},
    {Vec2(-382.0f, 43.0f), Vec2(-183.0f, 147.0f), Vec2(173.0f, 147.0f), Vec2(378.0f, 43.0f), Vec2(-3.0f, -286.5f)},
};

enum GAME_TAG {
    GT_MENU = 1,
    GT_ITEM_MENU_BG = 2,
    GT_ITEM_MENU = 3,
    GT_ACTIVE_DIAMOND = 4,
    GT_ACTIVE_HEART = 5,
    GT_ACTIVE_CLUB = 6,
    GT_ACTIVE_SPADE = 7,
    GT_PICKUP2 = 8,
    GT_PICKUP4 = 9,
    GT_PICKUP6 = 10,
    GT_PICKUP8 = 11,
    GT_PICKUP5 = 12,
    GT_PICKUP10 = 13,
};

enum GAME_MENU_TAG {
    GTM_PICKUP = 2,
    GTM_PLACE = 3,
    GTM_PICKUP5 = 4,
    GTM_PICKUP10 = 5,
    GTM_PICKUP2 = 6,
    GTM_PICKUP4 = 7,
    GTM_PICKUP6 = 8,
    GTM_PICKUP8 = 9,
    GTM_START_MATCH = 10,
    GTM_CLOSE = 11
};

Scene* GameLayer::createScene()
{
    Scene* scene = Scene::create();
    GameLayer* layer = GameLayer::create();
    scene->addChild(layer);
    return scene;
}

GameLayer::~GameLayer()
{
}

bool GameLayer::init()
{
    if ( !BaseLayer::init() )
        return false;
    
    m_nType = LAYER_GAME;
    g_pCurrentLayer = this;
    
    m_pActiveCard = NULL;
    m_pGameOverLayer = NULL;
    m_bStartedGame = false;
    m_bProcessingCards = false;
    m_vecPlayerSprites.clear();
    m_nActiveFlower = CARD_NONE;
    m_nCard2Turn = -1;
    m_strPickState = "";
    m_vecCandidates.clear();
    
    createSprites();
    createLabels();
    createMenu();
    
    return true;
}

void GameLayer::onEnter()
{
    BaseLayer::onEnter();
}

void GameLayer::onReceiveMessage(std::string packet_name, std::string message)
{
    BaseLayer::onReceiveMessage(packet_name, message);
    
    if(packet_name == PT_NOTIFY_JOIN_PLAYER)
    {
        GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
        UserInfo* pPlayer = PacketParser::getInstance()->parseJoinedPlayer(message);
        pGameRoom->m_vecPlayers.pushBack(pPlayer);
        addPlayerSprite(pPlayer);
        updatePlayerStates();
    }
    else if(packet_name == PT_NOTIFY_LEAVE_PLAYER)
    {
        LeaveInfo leaveInfo = PacketParser::getInstance()->parseLeavePlayer(message);
        GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
        if(pGameRoom->m_strId != leaveInfo.strRoomId)
            return;
        
        for(int index = 0; index < pGameRoom->m_vecPlayers.size(); index++)
        {
            UserInfo* pPlayer = pGameRoom->m_vecPlayers.at(index);
            if(pPlayer->m_strId == leaveInfo.strUserId)
            {
                removePlayerSprite(pPlayer);
                pGameRoom->m_vecPlayers.eraseObject(pPlayer);
                delete pPlayer;
                pPlayer = NULL;
                break;
            }
        }
    }
    else if(packet_name == PT_NOTIFY_STARTGAME)
    {
        startGame();
    }
    else if(packet_name == PT_NOTIFY_PUT_CARD)
    {
        m_bProcessingCards = true;
        
        for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
        {
            PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
            pPlayerSprite->deactive();
        }

        PutCardInfo putCardInfo = PacketParser::getInstance()->parsePutCard(message);
        GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
        UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
        UserInfo* pPrevPlayer = pGameRoom->findTurnPlayer(putCardInfo.nPrevTune);
        PlayerSprite* pPrevPlayerSprite = findPlayerSprite(pPrevPlayer);
        m_strPickState = putCardInfo.strPickState;
        
        getChildByTag(GT_PICKUP10)->setVisible(false);
        getChildByTag(GT_PICKUP5)->setVisible(false);
        getChildByTag(GT_PICKUP2)->setVisible(false);
        getChildByTag(GT_PICKUP4)->setVisible(false);
        getChildByTag(GT_PICKUP6)->setVisible(false);
        getChildByTag(GT_PICKUP8)->setVisible(false);
        
        if(putCardInfo.strPickState == "pickup2")
            getChildByTag(GT_PICKUP2)->setVisible(true);
        else if(putCardInfo.strPickState == "pickup4")
            getChildByTag(GT_PICKUP4)->setVisible(true);
        else if(putCardInfo.strPickState == "pickup6")
            getChildByTag(GT_PICKUP6)->setVisible(true);
        else if(putCardInfo.strPickState == "pickup8")
            getChildByTag(GT_PICKUP8)->setVisible(true);
        else if(putCardInfo.strPickState == "pickup5")
            getChildByTag(GT_PICKUP5)->setVisible(true);
        else if(putCardInfo.strPickState == "pickup10")
            getChildByTag(GT_PICKUP10)->setVisible(true);
        
        if(putCardInfo.vecCardValues.size() == 1 && putCardInfo.vecCardValues[0] == PICKUP_CARD_NUM)
        {
            pPrevPlayerSprite->showPickup();
            
            if(putCardInfo.bDealNewCard)
            {
                if(putCardInfo.nNewDealCount > 0)
                {
                    for(int index = 0; index < putCardInfo.nNewDealCount; index++) {
                        CardSprite* pCardSprite = pGameRoom->m_vecRemainCards.at(pGameRoom->m_vecRemainCards.size() - 1);
                        pGameRoom->m_vecRemainCards.popBack();
                        pCardSprite->setLocalZOrder(LAYER_ZORDER2 + 252 - index);
                        if(pPrevPlayer->m_strId == pUserInfo->m_strId)
                            pCardSprite->hideBackSprite();
                        
                        float delayStart = 0.25f * index;
                        
                        auto anim1 = DelayTime::create(delayStart);
                        auto anim2 = MoveTo::create(0.25f, Vec2(m_szDevice.width / 2 + g_staticCardPos[pGameRoom->m_nType][pPrevPlayerSprite->m_nDisplayPos].x * m_fSmallScale,
                                                                m_szDevice.height / 2 + g_staticCardPos[pGameRoom->m_nType][pPrevPlayerSprite->m_nDisplayPos].y * m_fSmallScale));
                        auto anim3 = ScaleTo::create(0.25f, 0.443f * m_fSmallScale);
                        auto anim4 = RotateBy::create(0.25f, 90.0f);
                        auto anim5 = RotateBy::create(0.25f, -90.0f);
                        auto anim6 = ScaleTo::create(0.25f, 0.7f * m_fSmallScale);
                        
                        if(pPrevPlayerSprite->m_nDisplayPos == ME_POSITION[pGameRoom->m_nType])
                        {
                            pCardSprite->runAction(Sequence::create(anim1, anim2,
                                                                    CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealJackCard, this, pPrevPlayer, pCardSprite, putCardInfo.nNewDealCount - 1, index, putCardInfo.nCurTune)), nullptr));
                        }
                        else if(pGameRoom->m_nType == ROOM_TYPE_1)
                        {
                            pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim6, NULL),
                                                                    CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealJackCard, this, pPrevPlayer, pCardSprite, putCardInfo.nNewDealCount - 1, index, putCardInfo.nCurTune)), nullptr));
                        }
                        else if(pGameRoom->m_nType == ROOM_TYPE_5 || pGameRoom->m_nType == ROOM_TYPE_FRIEND)
                        {
                            if(pPrevPlayerSprite->m_nDisplayPos == 0)
                            {
                                pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim5, NULL),
                                                                        CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealJackCard, this, pPrevPlayer, pCardSprite, putCardInfo.nNewDealCount - 1, index, putCardInfo.nCurTune)), nullptr));
                            }
                            else if(pPrevPlayerSprite->m_nDisplayPos == 3)
                            {
                                pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim4, NULL),
                                                                        CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealJackCard, this, pPrevPlayer, pCardSprite, putCardInfo.nNewDealCount - 1, index, putCardInfo.nCurTune)), nullptr));
                            }
                            else
                            {
                                pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, NULL),
                                                                        CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealJackCard, this, pPrevPlayer, pCardSprite, putCardInfo.nNewDealCount - 1, index, putCardInfo.nCurTune)), nullptr));
                            }
                        }
                    }
                }
                else
                {
                    int nTurn = putCardInfo.nCurTune;
                    int nDelayIndex = 0;
                    
                    while(true)
                    {
                        UserInfo* pPlayer = pGameRoom->findTurnPlayer(nTurn);
                        PlayerSprite* pPlayerSprite = findPlayerSprite(pPlayer);
                        
                        if(pPlayer != NULL)
                        {
                            CardSprite* pCardSprite = pGameRoom->m_vecRemainCards.at(pGameRoom->m_vecRemainCards.size() - 1);
                            pGameRoom->m_vecRemainCards.popBack();
                            pCardSprite->setLocalZOrder(LAYER_ZORDER2 + 252 - nDelayIndex);
                            if(pPlayer->m_strId == pUserInfo->m_strId)
                                pCardSprite->hideBackSprite();
                            
                            float delayStart = 0.25f * nDelayIndex;
                            
                            auto anim1 = DelayTime::create(delayStart);
                            auto anim2 = MoveTo::create(0.25f, Vec2(m_szDevice.width / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].x * m_fSmallScale,
                                                                    m_szDevice.height / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].y * m_fSmallScale));
                            auto anim3 = ScaleTo::create(0.25f, 0.443f * m_fSmallScale);
                            auto anim4 = RotateBy::create(0.25f, 90.0f);
                            auto anim5 = RotateBy::create(0.25f, -90.0f);
                            auto anim6 = ScaleTo::create(0.25f, 0.7f * m_fSmallScale);
                            
                            if(pPlayerSprite->m_nDisplayPos == ME_POSITION[pGameRoom->m_nType])
                            {
                                pCardSprite->runAction(Sequence::create(anim1, anim2,
                                                                        CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealNewCard, this, pPlayer, pCardSprite, putCardInfo.nPrevTune, putCardInfo.nCurTune)), nullptr));
                            }
                            else if(pGameRoom->m_nType == ROOM_TYPE_1)
                            {
                                pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim6, NULL),
                                                                        CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealNewCard, this, pPlayer, pCardSprite, putCardInfo.nPrevTune, putCardInfo.nCurTune)), nullptr));
                            }
                            else if(pGameRoom->m_nType == ROOM_TYPE_5 || pGameRoom->m_nType == ROOM_TYPE_FRIEND)
                            {
                                if(pPlayerSprite->m_nDisplayPos == 0)
                                {
                                    pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim5, NULL),
                                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealNewCard, this, pPlayer, pCardSprite, putCardInfo.nPrevTune, putCardInfo.nCurTune)), nullptr));
                                }
                                else if(pPlayerSprite->m_nDisplayPos == 3)
                                {
                                    pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim4, NULL),
                                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealNewCard, this, pPlayer, pCardSprite, putCardInfo.nPrevTune, putCardInfo.nCurTune)), nullptr));
                                }
                                else
                                {
                                    pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, NULL),
                                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealNewCard, this, pPlayer, pCardSprite, putCardInfo.nPrevTune, putCardInfo.nCurTune)), nullptr));
                                }
                            }
                            nDelayIndex++;
                        }
                        
                        nTurn++;
                        if(nTurn == pGameRoom->m_nMaxMember)
                            nTurn = 0;
                        
                        if(nTurn == putCardInfo.nCurTune)
                            break;
                    }
                }
            }
            else
            {
                m_bProcessingCards = false;
                pGameRoom->m_nCurTurn = putCardInfo.nCurTune;
                updateTurn(pPrevPlayer);
            }
        }
        else
        {
            pPrevPlayerSprite->hidePickup();
            m_nActiveFlower = putCardInfo.nCardFlower;
            for(int index = 0; index < putCardInfo.vecCardValues.size(); index++)
            {
                CardSprite* pCardSprite = pPrevPlayer->findCard(putCardInfo.vecCardValues[index]);
                if(pCardSprite == NULL)
                    continue;
                
                if(index == putCardInfo.vecCardValues.size() - 1 && pCardSprite->getCardNum() == CARD_2)
                    m_nCard2Turn = putCardInfo.nPrevTune;
                
                if(pPrevPlayer->m_strId != pUserInfo->m_strId)
                    pPrevPlayer->m_vecActiveCards.pushBack(pCardSprite);
                
                auto anim1 = DelayTime::create(0.5f * index);
                auto anim2 = MoveTo::create(0.25f, Vec2(m_szDevice.width / 2 + 53.5f * m_fSmallScale , m_szDevice.height / 2 - 25.5f * m_fSmallScale));
                auto anim3 = ScaleTo::create(0.25f, 1.0f * m_fSmallScale);
                auto anim4 = RotateBy::create(0.25f, -90.0f);
                auto anim5 = RotateBy::create(0.25f, 90.0f);
                
                if(pGameRoom->m_nType == ROOM_TYPE_1)
                {
                    pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, NULL),
                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DonePlaceCard, this, pPrevPlayer, pCardSprite, putCardInfo.nCurTune)), nullptr));
                }
                else if(pGameRoom->m_nType == ROOM_TYPE_5 || pGameRoom->m_nType == ROOM_TYPE_FRIEND)
                {
                    if(pPrevPlayerSprite->m_nDisplayPos == 0)
                    {
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim5, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DonePlaceCard, this, pPrevPlayer, pCardSprite, putCardInfo.nCurTune)), nullptr));
                    }
                    else if(pPrevPlayerSprite->m_nDisplayPos == 3)
                    {
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim4, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DonePlaceCard, this, pPrevPlayer, pCardSprite, putCardInfo.nCurTune)), nullptr));
                    }
                    else
                    {
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DonePlaceCard, this, pPrevPlayer, pCardSprite, putCardInfo.nCurTune)), nullptr));
                    }
                }
            }
        }
    }
    else if(packet_name == PT_NOTIFY_GAME_RESULT)
    {
        m_GameResultInfo = PacketParser::getInstance()->parseGameResult(message);
        GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
        
        if(pGameRoom->m_strId != m_GameResultInfo.strRoomId)
            return;
        
        m_bStartedGame = false;
        if(m_bProcessingCards)
            return;
        
        processGameResult();
    }
}

void GameLayer::onDownloadedAvatar(std::string file_name)
{
    for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
    {
        PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
        if(file_name == pPlayerSprite->m_pPlayer->m_strAvatar)
            pPlayerSprite->updateAvatar();
    }
}

void GameLayer::OnClose(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    GameData::getInstance()->sendPacket(PT_REQ_LEAVEROOM, PacketParser::getInstance()->makeRequestLeaveGame(pUserInfo->m_strId, pGameRoom->m_strId));
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
}

void GameLayer::OnPickup(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    pCurPlayerSprite->deactive();
    
    hideItemMenu();
    
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecCards.begin(); it != pCurPlayer->m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->isActive())
            pCardSprite->setActive(false);
    }
    
    MyMenuItem* pBtnPickup = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    cards.push_back(PICKUP_CARD_NUM);
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
}

void GameLayer::OnPickup5(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    pCurPlayerSprite->deactive();
    
    hideItemMenu();
    
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecCards.begin(); it != pCurPlayer->m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->isActive())
            pCardSprite->setActive(false);
    }
    
    MyMenuItem* pBtnPickup5 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP5);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup5->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    cards.push_back(PICKUP_CARD_NUM);
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
}

void GameLayer::OnPickup10(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    pCurPlayerSprite->deactive();
    
    hideItemMenu();
    
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecCards.begin(); it != pCurPlayer->m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->isActive())
            pCardSprite->setActive(false);
    }
    
    MyMenuItem* pBtnPickup10 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP10);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup10->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    cards.push_back(PICKUP_CARD_NUM);
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
}

void GameLayer::OnPickup2(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    pCurPlayerSprite->deactive();
    
    hideItemMenu();
    
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecCards.begin(); it != pCurPlayer->m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->isActive())
            pCardSprite->setActive(false);
    }
    
    MyMenuItem* pBtnPickup2 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP2);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup2->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    cards.push_back(PICKUP_CARD_NUM);
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
}

void GameLayer::OnPickup4(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    pCurPlayerSprite->deactive();
    
    hideItemMenu();
    
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecCards.begin(); it != pCurPlayer->m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->isActive())
            pCardSprite->setActive(false);
    }
    
    MyMenuItem* pBtnPickup4 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP4);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup4->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    cards.push_back(PICKUP_CARD_NUM);
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
}

void GameLayer::OnPickup6(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    pCurPlayerSprite->deactive();
    
    hideItemMenu();
    
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecCards.begin(); it != pCurPlayer->m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->isActive())
            pCardSprite->setActive(false);
    }
    
    MyMenuItem* pBtnPickup6 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP6);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup6->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    cards.push_back(PICKUP_CARD_NUM);
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
}

void GameLayer::OnPickup8(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    pCurPlayerSprite->deactive();
    
    hideItemMenu();
    
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecCards.begin(); it != pCurPlayer->m_vecCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        if(pCardSprite->isActive())
            pCardSprite->setActive(false);
    }
    
    MyMenuItem* pBtnPickup8 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP8);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup8->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    cards.push_back(PICKUP_CARD_NUM);
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
}

void GameLayer::OnPlace(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    if(pCurPlayer->m_strId != pUserInfo->m_strId)
        return;
    
    MyMenuItem* pBtnPickup = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP);
    MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    pBtnPickup->setEnabled(false);
    pBtnPlace->setEnabled(false);
    
    std::vector<int> cards;
    bool allAce = true;
    for(Vector<CardSprite*>::iterator it = pCurPlayer->m_vecActiveCards.begin(); it != pCurPlayer->m_vecActiveCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        cards.push_back(pCardSprite->getCardValue());
        
        if(pCardSprite->getCardNum() != CARD_1)
            allAce = false;
    }
    
    if(allAce)
    {
        showItemMenu();
    }
    else
    {
        PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
        pCurPlayerSprite->deactive();
        GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pCurPlayer->m_strId, pCurPlayer->m_strRoomId, cards, CARD_NONE));
    }
}

void GameLayer::OnDiamond(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    putCardWithFlower(CARD_DIAMOND);
}

void GameLayer::OnHeart(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    putCardWithFlower(CARD_HEART);
}

void GameLayer::OnClub(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    putCardWithFlower(CARD_CLUB);
}

void GameLayer::OnSpade(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    putCardWithFlower(CARD_SPADE);
}

void GameLayer::OnStartMatch(Ref* pSender)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sound/Click.wav");
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameData::getInstance()->sendPacket(PT_REQ_START_GAME, PacketParser::getInstance()->makeRequestStart(pUserInfo->m_strId, pGameRoom->m_strId));
    MyMenuItem* pStartMatch = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_START_MATCH);
    pStartMatch->setVisible(false);
}

void GameLayer::DoneDealOneCard(UserInfo* pPlayer, CardSprite* pSprite)
{
    pSprite->setPlayer(pPlayer);
    pPlayer->m_vecCards.pushBack(pSprite);
    pSprite->setLocalZOrder(LAYER_ZORDER2 + 200 + (int)pPlayer->m_vecCards.size());
}

void GameLayer::DoneDealLastCard(UserInfo* pPlayer, CardSprite* pSprite)
{
    pSprite->setPlayer(pPlayer);
    pPlayer->m_vecCards.pushBack(pSprite);
    pSprite->setLocalZOrder(LAYER_ZORDER2 + 200 + (int)pPlayer->m_vecCards.size());
    expandCards();
    flipDeckCard();
}

void GameLayer::DoneFlipDeckCard(CardSprite* pSender)
{
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    pSender->setLocalZOrder(LAYER_ZORDER2 + 100 + (int)pGameRoom->m_vecDeckCards.size());
    m_bStartedGame = true;
    
    UserInfo* pMe = pGameRoom->findPlayer(pUserInfo->m_strId);
    if(pMe != NULL && pMe->m_nInGame == 1)
    {
        MyMenuItem* pMenuPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
        MyMenuItem* pMenuPickup = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP);
        
        pMenuPlace->setVisible(true);
        pMenuPlace->setEnabled(false);
        pMenuPickup->setVisible(true);
        pMenuPickup->setEnabled(false);
    }
    
    updateTurn(NULL);
}

void GameLayer::DoneDealNewCard(UserInfo* pPlayer, CardSprite* pSprite, int nCompleteTurn, int nCurTurn)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    pSprite->setPlayer(pPlayer);
    pSprite->setLocalZOrder(pPlayer->m_vecCards.at(pPlayer->m_vecCards.size() - 1)->getLocalZOrder() + 1);
    pPlayer->m_vecCards.pushBack(pSprite);
    repositionCards(pPlayer);
    
    if(pPlayer->m_nPosition == nCompleteTurn)
    {
        m_bProcessingCards = false;
        if(!m_bStartedGame)
        {
            processGameResult();
        }
        else
        {
            for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
            {
                PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
                pPlayerSprite->hidePickup();
            }
            
            pGameRoom->m_nCurTurn = nCurTurn;
            UserInfo* pPrevPlayer = pGameRoom->findTurnPlayer(nCompleteTurn);
            updateTurn(pPrevPlayer);
        }
    }
}

void GameLayer::DoneDealJackCard(UserInfo* pPlayer, CardSprite* pSprite, int nCompleteIndex, int nCurIndex, int nTurn)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    pSprite->setPlayer(pPlayer);
    pSprite->setLocalZOrder(pPlayer->m_vecCards.at(pPlayer->m_vecCards.size() - 1)->getLocalZOrder() + 1);
    pPlayer->m_vecCards.pushBack(pSprite);
    repositionCards(pPlayer);
    
    if(nCurIndex == nCompleteIndex)
    {
        m_bProcessingCards = false;
        if(!m_bStartedGame)
        {
            processGameResult();
        }
        else
        {
            for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
            {
                PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
                pPlayerSprite->hidePickup();
            }
            
            pGameRoom->m_nCurTurn = nTurn;
            updateTurn(pPlayer);
        }
    }
}

void GameLayer::DonePlaceCard(UserInfo* pPlayer, CardSprite* pSprite, int nNextTurn)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    
    pPlayer->m_vecCards.eraseObject(pSprite);
    pPlayer->m_vecActiveCards.eraseObject(pSprite);
    pSprite->setPlayer(NULL);
    pGameRoom->m_vecDeckCards.pushBack(pSprite);
    m_pActiveCard = pSprite;
    pSprite->setLocalZOrder(LAYER_ZORDER2 + 100 + (int)pGameRoom->m_vecDeckCards.size());
    repositionCards(pPlayer);
    
    if(pPlayer->m_vecActiveCards.size() == 0)
    {
        m_bProcessingCards = false;
        if(!m_bStartedGame)
        {
            processGameResult();
        }
        else
        {
            showActiveFlower();
            pGameRoom->m_nCurTurn = nNextTurn;
            updateTurn(pPlayer);
        }
    }
    
    if(pPlayer->m_strId != pUserInfo->m_strId)
    {
        pSprite->hideBackSprite();
        pSprite->setScaleX(0.0f);
        pSprite->runAction(ScaleTo::create(0.2f, pSprite->getScaleY(), pSprite->getScaleY()));
    }
}

void GameLayer::timeoutPlayer(UserInfo* pPlayer)
{
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    if(pUserInfo->m_strId == pPlayer->m_strId)
    {
        for(Vector<CardSprite*>::iterator it = pPlayer->m_vecCards.begin(); it != pPlayer->m_vecCards.end(); it++)
        {
            CardSprite* pCardSprite = (CardSprite*)(*it);
            if(pCardSprite->isActive())
                pCardSprite->setActive(false);
        }
        
        MyMenuItem* pBtnPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
        MyMenuItem* pBtnPickup = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP);
        
        pBtnPlace->setEnabled(false);
        pBtnPickup->setEnabled(false);
        
        hideItemMenu();
        
        std::vector<int> cards;
        cards.push_back(PICKUP_CARD_NUM);
        GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pPlayer->m_strId, pPlayer->m_strRoomId, cards, CARD_NONE));
    }
}

bool GameLayer::isTimeoutPlayer(UserInfo* pPlayer)
{
    PlayerSprite* pPlayerSprite = findPlayerSprite(pPlayer);
    if(pPlayerSprite == NULL)
        return true;
    
    return pPlayerSprite->isTimeout();
}

void GameLayer::chooseCard(UserInfo* pPlayer)
{
    MyMenuItem* pMenuPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    
    bool bMatchRule = true;
    if(pPlayer->m_vecActiveCards.size() == 0)
        bMatchRule = false;
    
    bool bCheckCard1 = pPlayer->isAllAce();
    bool bCheckCard2 = pPlayer->isAll2();
    
    for(int index = 0; index < pPlayer->m_vecActiveCards.size(); index++)
    {
        CardSprite* pCardSprite = pPlayer->m_vecActiveCards.at(index);
        
        if(index == 0)
        {
            bMatchRule = match1(m_pActiveCard, pCardSprite, bCheckCard1, bCheckCard2);
            if(!bMatchRule)
                break;
        }
        else
        {
            CardSprite* pPrevCardSprite = pPlayer->m_vecActiveCards.at(index - 1);
            bMatchRule = match2(pPrevCardSprite, pCardSprite);
            if(!bMatchRule)
                break;
        }
    }
    
    pMenuPlace->setEnabled(bMatchRule);
}

void GameLayer::restartGame()
{
    hideGameOver();
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pMePlayer = pGameRoom->findPlayer(pUserInfo->m_strId);
    if(pMePlayer->m_nCoin < pGameRoom->m_nGameCoin)
    {
        createMessageLayer(MSG_NOT_ENOUGH_COIN);
        return;
    }
    
    GameData::getInstance()->sendPacket(PT_REQ_RESTART_GAME, PacketParser::getInstance()->makeRequestRestart(pUserInfo->m_strId, pGameRoom->m_strId));
}

void GameLayer::closeGame()
{
    hideGameOver();
    
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    GameData::getInstance()->sendPacket(PT_REQ_LEAVEROOM, PacketParser::getInstance()->makeRequestLeaveGame(pUserInfo->m_strId, pGameRoom->m_strId));
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, LobbyLayer::createScene(), Color3B(0, 0, 0)));
}

void GameLayer::createSprites()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    auto sprBackground = Sprite::create("img/game/background.png");
    sprBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprBackground->setScale(m_fSmallScale);
    sprBackground->setPosition(Vec2(m_szDevice.width / 2, m_szDevice.height / 2));
    addChild(sprBackground, LAYER_ZORDER1);
    
    auto sprTable = Sprite::create("img/game/table.png");
    sprTable->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTable->setScale(m_fSmallScale);
    sprTable->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 40.0f * m_fSmallScale));
    addChild(sprTable, LAYER_ZORDER1);
    
    auto sprTableLogo = Sprite::create("img/game/table_logo.png");
    sprTableLogo->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprTableLogo->setScale(m_fSmallScale);
    sprTableLogo->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 40.0f * m_fSmallScale));
    addChild(sprTableLogo, LAYER_ZORDER1);
    
    auto sprPickup2 = Sprite::create("img/game/pickup_2.png");
    sprPickup2->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprPickup2->setScale(m_fSmallScale);
    sprPickup2->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 74.0f * m_fSmallScale));
    sprPickup2->setTag(GT_PICKUP2);
    sprPickup2->setVisible(false);
    addChild(sprPickup2, LAYER_ZORDER1);
    
    auto sprPickup4 = Sprite::create("img/game/pickup_4.png");
    sprPickup4->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprPickup4->setScale(m_fSmallScale);
    sprPickup4->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 74.0f * m_fSmallScale));
    sprPickup4->setTag(GT_PICKUP4);
    sprPickup4->setVisible(false);
    addChild(sprPickup4, LAYER_ZORDER1);
    
    auto sprPickup6 = Sprite::create("img/game/pickup_6.png");
    sprPickup6->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprPickup6->setScale(m_fSmallScale);
    sprPickup6->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 74.0f * m_fSmallScale));
    sprPickup6->setTag(GT_PICKUP6);
    sprPickup6->setVisible(false);
    addChild(sprPickup6, LAYER_ZORDER1);
    
    auto sprPickup8 = Sprite::create("img/game/pickup_8.png");
    sprPickup8->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprPickup8->setScale(m_fSmallScale);
    sprPickup8->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 74.0f * m_fSmallScale));
    sprPickup8->setTag(GT_PICKUP8);
    sprPickup8->setVisible(false);
    addChild(sprPickup8, LAYER_ZORDER1);
    
    auto sprPickup5 = Sprite::create("img/game/pickup_5.png");
    sprPickup5->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprPickup5->setScale(m_fSmallScale);
    sprPickup5->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 74.0f * m_fSmallScale));
    sprPickup5->setTag(GT_PICKUP5);
    sprPickup5->setVisible(false);
    addChild(sprPickup5, LAYER_ZORDER1);
    
    auto sprPickup10 = Sprite::create("img/game/pickup_10.png");
    sprPickup10->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprPickup10->setScale(m_fSmallScale);
    sprPickup10->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 + 74.0f * m_fSmallScale));
    sprPickup10->setTag(GT_PICKUP10);
    sprPickup10->setVisible(false);
    addChild(sprPickup10, LAYER_ZORDER1);
    
    auto sprItemMenuBg = Sprite::create("img/game/item_menu_bg.png");
    sprItemMenuBg->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprItemMenuBg->setScale(m_fSmallScale);
    sprItemMenuBg->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 - 133.5f * m_fSmallScale));
    sprItemMenuBg->setTag(GT_ITEM_MENU_BG);
    sprItemMenuBg->setVisible(false);
    addChild(sprItemMenuBg, LAYER_ZORDER1);
    
    auto sprActiveHeart = Sprite::create("img/game/item_heart.png");
    sprActiveHeart->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprActiveHeart->setScale(m_fSmallScale);
    sprActiveHeart->setPosition(Vec2(m_szDevice.width / 2 + 138.5f * m_fSmallScale, m_szDevice.height / 2 + 24.0f * m_fSmallScale));
    sprActiveHeart->setTag(GT_ACTIVE_HEART);
    sprActiveHeart->setVisible(false);
    addChild(sprActiveHeart, LAYER_ZORDER1);
    
    auto sprActiveDiamond = Sprite::create("img/game/item_diamond.png");
    sprActiveDiamond->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprActiveDiamond->setScale(m_fSmallScale);
    sprActiveDiamond->setPosition(Vec2(m_szDevice.width / 2 + 138.5f * m_fSmallScale, m_szDevice.height / 2 + 24.0f * m_fSmallScale));
    sprActiveDiamond->setTag(GT_ACTIVE_DIAMOND);
    sprActiveDiamond->setVisible(false);
    addChild(sprActiveDiamond, LAYER_ZORDER1);
    
    auto sprActiveClub = Sprite::create("img/game/item_club.png");
    sprActiveClub->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprActiveClub->setScale(m_fSmallScale);
    sprActiveClub->setPosition(Vec2(m_szDevice.width / 2 + 138.5f * m_fSmallScale, m_szDevice.height / 2 + 24.0f * m_fSmallScale));
    sprActiveClub->setTag(GT_ACTIVE_CLUB);
    sprActiveClub->setVisible(false);
    addChild(sprActiveClub, LAYER_ZORDER1);
    
    auto sprActiveSpade = Sprite::create("img/game/item_spade.png");
    sprActiveSpade->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprActiveSpade->setScale(m_fSmallScale);
    sprActiveSpade->setPosition(Vec2(m_szDevice.width / 2 + 138.5f * m_fSmallScale, m_szDevice.height / 2 + 24.0f * m_fSmallScale));
    sprActiveSpade->setTag(GT_ACTIVE_SPADE);
    sprActiveSpade->setVisible(false);
    addChild(sprActiveSpade, LAYER_ZORDER1);
    
    for(int index = 0; index < MAX_PLAYERS[pGameRoom->m_nType]; index++)
    {
        auto sprEmptySuit = Sprite::create("img/game/suit_empty.png");
        sprEmptySuit->setAnchorPoint(Vec2(0.5f, 0.5f));
        sprEmptySuit->setScale(m_fSmallScale);
        sprEmptySuit->setPosition(Vec2(m_szDevice.width / 2 + g_staticPlayerPos[pGameRoom->m_nType][index].x * m_fSmallScale,
                                       m_szDevice.height / 2 + g_staticPlayerPos[pGameRoom->m_nType][index].y * m_fSmallScale));
        addChild(sprEmptySuit, LAYER_ZORDER1);
    }
    
    addPlayerSprites();
}

void GameLayer::createLabels()
{
    
}

void GameLayer::createMenu()
{
    auto btnClose = MyMenuItem::create("img/match/btn_back_nor.png", "img/match/btn_back_act.png", "", CC_CALLBACK_1(GameLayer::OnClose, this));
    btnClose->setScale(m_fSmallScale);
    btnClose->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnClose->setPosition(Vec2(101.0f * m_fSmallScale, m_szDevice.height - 44.5f * m_fSmallScale));
    btnClose->setTag(GTM_CLOSE);
    
    auto btnPickup = MyMenuItem::create("img/game/btn_pickup_nor.png", "img/game/btn_pickup_act.png", "img/game/btn_pickup_dis.png", CC_CALLBACK_1(GameLayer::OnPickup, this));
    btnPickup->setScale(m_fSmallScale);
    btnPickup->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPickup->setPosition(Vec2(149.0f * m_fSmallScale, 47.5f * m_fSmallScale));
    btnPickup->setTag(GTM_PICKUP);
    btnPickup->setVisible(false);
    
    auto btnPickup5 = MyMenuItem::create("img/game/btn_pickup5_nor.png", "img/game/btn_pickup5_act.png", "img/game/btn_pickup5_dis.png", CC_CALLBACK_1(GameLayer::OnPickup5, this));
    btnPickup5->setScale(m_fSmallScale);
    btnPickup5->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPickup5->setPosition(Vec2(149.0f * m_fSmallScale, 47.5f * m_fSmallScale));
    btnPickup5->setTag(GTM_PICKUP5);
    btnPickup5->setVisible(false);
    
    auto btnPickup10 = MyMenuItem::create("img/game/btn_pickup10_nor.png", "img/game/btn_pickup10_act.png", "img/game/btn_pickup10_dis.png", CC_CALLBACK_1(GameLayer::OnPickup10, this));
    btnPickup10->setScale(m_fSmallScale);
    btnPickup10->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPickup10->setPosition(Vec2(149.0f * m_fSmallScale, 47.5f * m_fSmallScale));
    btnPickup10->setTag(GTM_PICKUP10);
    btnPickup10->setVisible(false);
    
    auto btnPickup2 = MyMenuItem::create("img/game/btn_pickup2_nor.png", "img/game/btn_pickup2_act.png", "img/game/btn_pickup2_dis.png", CC_CALLBACK_1(GameLayer::OnPickup2, this));
    btnPickup2->setScale(m_fSmallScale);
    btnPickup2->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPickup2->setPosition(Vec2(149.0f * m_fSmallScale, 47.5f * m_fSmallScale));
    btnPickup2->setTag(GTM_PICKUP2);
    btnPickup2->setVisible(false);
    
    auto btnPickup4 = MyMenuItem::create("img/game/btn_pickup4_nor.png", "img/game/btn_pickup4_act.png", "img/game/btn_pickup4_dis.png", CC_CALLBACK_1(GameLayer::OnPickup4, this));
    btnPickup4->setScale(m_fSmallScale);
    btnPickup4->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPickup4->setPosition(Vec2(149.0f * m_fSmallScale, 47.5f * m_fSmallScale));
    btnPickup4->setTag(GTM_PICKUP4);
    btnPickup4->setVisible(false);
    
    auto btnPickup6 = MyMenuItem::create("img/game/btn_pickup6_nor.png", "img/game/btn_pickup6_act.png", "img/game/btn_pickup6_dis.png", CC_CALLBACK_1(GameLayer::OnPickup6, this));
    btnPickup6->setScale(m_fSmallScale);
    btnPickup6->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPickup6->setPosition(Vec2(149.0f * m_fSmallScale, 47.5f * m_fSmallScale));
    btnPickup6->setTag(GTM_PICKUP6);
    btnPickup6->setVisible(false);
    
    auto btnPickup8 = MyMenuItem::create("img/game/btn_pickup8_nor.png", "img/game/btn_pickup8_act.png", "img/game/btn_pickup8_dis.png", CC_CALLBACK_1(GameLayer::OnPickup8, this));
    btnPickup8->setScale(m_fSmallScale);
    btnPickup8->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPickup8->setPosition(Vec2(149.0f * m_fSmallScale, 47.5f * m_fSmallScale));
    btnPickup8->setTag(GTM_PICKUP8);
    btnPickup8->setVisible(false);
    
    auto btnPlace = MyMenuItem::create("img/game/btn_place_nor.png", "img/game/btn_place_act.png", "img/game/btn_place_dis.png", CC_CALLBACK_1(GameLayer::OnPlace, this));
    btnPlace->setScale(m_fSmallScale);
    btnPlace->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnPlace->setPosition(Vec2(m_szDevice.width - 148.5f * m_fSmallScale, 54.5f * m_fSmallScale));
    btnPlace->setTag(GTM_PLACE);
    btnPlace->setVisible(false);
    
    auto btnStartMatch = MyMenuItem::create("img/game/btn_start_nor.png", "img/game/btn_start_act.png", "", CC_CALLBACK_1(GameLayer::OnStartMatch, this));
    btnStartMatch->setScale(m_fSmallScale);
    btnStartMatch->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnStartMatch->setPosition(Vec2(m_szDevice.width / 2 - 3.0f * m_fSmallScale, m_szDevice.height / 2 - 54.0f * m_fSmallScale));
    btnStartMatch->setTag(GTM_START_MATCH);
    
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    if(pGameRoom->m_nType == ROOM_TYPE_FRIEND && pUserInfo->m_strId == pGameRoom->m_strCreaterId)
        btnStartMatch->setVisible(true);
    else
        btnStartMatch->setVisible(false);
    
    auto menu = Menu::create(btnClose, btnPickup, btnPickup5, btnPickup10, btnPickup2, btnPickup4, btnPickup6, btnPickup8, btnPlace, btnStartMatch, nullptr);
    menu->setPosition(Vec2(0.0f, 0.0f));
    menu->setTag(GT_MENU);
    addChild(menu, LAYER_ZORDER3);
    
    auto btnSpade = MyMenuItem::create("img/game/item_spade.png", "img/game/item_spade.png", "", CC_CALLBACK_1(GameLayer::OnSpade, this));
    btnSpade->setScale(m_fSmallScale);
    btnSpade->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnSpade->setPosition(Vec2(m_szDevice.width / 2 - 176.5f * m_fSmallScale, m_szDevice.height / 2 - 133.5f * m_fSmallScale));
    
    auto btnDiamond = MyMenuItem::create("img/game/item_diamond.png", "img/game/item_diamond.png", "", CC_CALLBACK_1(GameLayer::OnDiamond, this));
    btnDiamond->setScale(m_fSmallScale);
    btnDiamond->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnDiamond->setPosition(Vec2(m_szDevice.width / 2 - 70.0f * m_fSmallScale, m_szDevice.height / 2 - 133.5f * m_fSmallScale));
    
    auto btnClub = MyMenuItem::create("img/game/item_club.png", "img/game/item_club.png", "", CC_CALLBACK_1(GameLayer::OnClub, this));
    btnClub->setScale(m_fSmallScale);
    btnClub->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnClub->setPosition(Vec2(m_szDevice.width / 2 + 42.0f * m_fSmallScale, m_szDevice.height / 2 - 133.5f * m_fSmallScale));
    
    auto btnHeart = MyMenuItem::create("img/game/item_heart.png", "img/game/item_heart.png", "", CC_CALLBACK_1(GameLayer::OnHeart, this));
    btnHeart->setScale(m_fSmallScale);
    btnHeart->setAnchorPoint(Vec2(0.5f, 0.5f));
    btnHeart->setPosition(Vec2(m_szDevice.width / 2 + 160.5f * m_fSmallScale, m_szDevice.height / 2 - 133.5f * m_fSmallScale));
    
    auto menu1 = Menu::create(btnSpade, btnDiamond, btnClub, btnHeart, nullptr);
    menu1->setPosition(Vec2(0.0f, 0.0f));
    menu1->setTag(GT_ITEM_MENU);
    menu1->setVisible(false);
    addChild(menu1, LAYER_ZORDER3);
}

void GameLayer::startGame()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    if(pGameRoom->m_nStarted != 1)
        return;
    
    updatePlayerStates();
    updatePlayerSprites();
    initCards();
    drawCards();
    dealCards();
}

void GameLayer::initCards()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    pGameRoom->m_vecRemainCards.pushBack(pGameRoom->m_vecDeckCards.at(0));
    pGameRoom->m_vecDeckCards.erase(0);
    
    for(int nCardIndex = START_CARD_COUNT - 1; nCardIndex >= 0; nCardIndex--)
    {
        int nTurn = pGameRoom->m_nStartTurn - 1;
        if(nTurn < 0)
            nTurn = pGameRoom->m_nMaxMember - 1;
        
        while(true)
        {
            UserInfo* pPlayer = pGameRoom->findTurnPlayer(nTurn);
            if(pPlayer != NULL)
            {
                CardSprite* pCardSprite = pPlayer->m_vecCards.at(nCardIndex);
                pCardSprite->setPlayer(NULL);
                pGameRoom->m_vecRemainCards.pushBack(pPlayer->m_vecCards.at(nCardIndex));
            }
            
            if(nTurn == pGameRoom->m_nStartTurn)
                break;
            
            nTurn--;
            if(nTurn < 0)
                nTurn = pGameRoom->m_nMaxMember - 1;
        }
    }
    
    for(Vector<UserInfo*>::iterator it = pGameRoom->m_vecPlayers.begin(); it != pGameRoom->m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        pPlayer->m_vecCards.clear();
    }
}

void GameLayer::drawCards()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    auto sprRemainCard = Sprite::create("img/card/card_deck.png");
    sprRemainCard->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprRemainCard->setScale(m_fSmallScale);
    sprRemainCard->setPosition(Vec2(m_szDevice.width / 2 - 62.5f * m_fSmallScale , m_szDevice.height / 2 - 25.5f * m_fSmallScale));
    addChild(sprRemainCard, LAYER_ZORDER2);
    
    auto sprDeckCard = Sprite::create("img/card/card_deck.png");
    sprDeckCard->setAnchorPoint(Vec2(0.5f, 0.5f));
    sprDeckCard->setScale(m_fSmallScale);
    sprDeckCard->setPosition(Vec2(m_szDevice.width / 2 + 53.5f * m_fSmallScale , m_szDevice.height / 2 - 25.5f * m_fSmallScale));
    addChild(sprDeckCard, LAYER_ZORDER2);
    
    int zorder = 0;
    for(Vector<CardSprite*>::iterator it = pGameRoom->m_vecRemainCards.begin(); it != pGameRoom->m_vecRemainCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        pCardSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
        pCardSprite->setScale(m_fSmallScale);
        pCardSprite->setPosition(Vec2(m_szDevice.width / 2 - 62.5f * m_fSmallScale , m_szDevice.height / 2 - 25.5f * m_fSmallScale));
        addChild(pCardSprite, LAYER_ZORDER2 + zorder);
        zorder++;
    }
}

void GameLayer::dealCards()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    int nRemainCardCounts = MAX_CARD_COUNT - START_CARD_COUNT * pGameRoom->getInGamePlayerCount();
    int nCardIndex = 0;
    int nTurn = pGameRoom->m_nStartTurn;
    while(true)
    {
        if(nCardIndex == START_CARD_COUNT * pGameRoom->getInGamePlayerCount())
            break;
        
        UserInfo* pPlayer = pGameRoom->findTurnPlayer(nTurn);
        PlayerSprite* pPlayerSprite = findPlayerSprite(pPlayer);
        if(pPlayer != NULL)
        {
            CardSprite* pCardSprite = pGameRoom->m_vecRemainCards.at(pGameRoom->m_vecRemainCards.size() - 1);
            pGameRoom->m_vecRemainCards.popBack();
            pCardSprite->setLocalZOrder(LAYER_ZORDER2 + 252 + (int)pGameRoom->m_vecRemainCards.size());
            
            float delayInterval = 0.25f;
            
            auto anim1 = DelayTime::create(0.5f + delayInterval * nCardIndex);
            auto anim2 = MoveTo::create(0.25f, Vec2(m_szDevice.width / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].x * m_fSmallScale,
                                                    m_szDevice.height / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].y * m_fSmallScale));
            auto anim3 = ScaleTo::create(0.25f, 0.443f * m_fSmallScale);
            auto anim4 = RotateBy::create(0.25f, 90.0f);
            auto anim5 = RotateBy::create(0.25f, -90.0f);
            auto anim6 = ScaleTo::create(0.25f, 0.7f * m_fSmallScale);
            
            if(pPlayerSprite->m_nDisplayPos == ME_POSITION[pGameRoom->m_nType])
            {
                if(pGameRoom->m_vecRemainCards.size() == nRemainCardCounts)
                    pCardSprite->runAction(Sequence::create(anim1, anim2,
                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealLastCard, this, pPlayer, pCardSprite)), nullptr));
                else
                    pCardSprite->runAction(Sequence::create(anim1, anim2,
                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealOneCard, this, pPlayer, pCardSprite)), nullptr));
            }
            else if(pGameRoom->m_nType == ROOM_TYPE_1)
            {
                if(pGameRoom->m_vecRemainCards.size() == nRemainCardCounts)
                    pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim6, NULL),
                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealLastCard, this, pPlayer, pCardSprite)), nullptr));
                else
                    pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim6, NULL),
                                                            CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealOneCard, this, pPlayer, pCardSprite)), nullptr));
            }
            else if(pGameRoom->m_nType == ROOM_TYPE_5 || pGameRoom->m_nType == ROOM_TYPE_FRIEND)
            {
                if(pPlayerSprite->m_nDisplayPos == 0)
                {
                    if(pGameRoom->m_vecRemainCards.size() == nRemainCardCounts)
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim5, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealLastCard, this, pPlayer, pCardSprite)), nullptr));
                    else
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim5, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealOneCard, this, pPlayer, pCardSprite)), nullptr));
                }
                else if(pPlayerSprite->m_nDisplayPos == 3)
                {
                    if(pGameRoom->m_vecRemainCards.size() == nRemainCardCounts)
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim4, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealLastCard, this, pPlayer, pCardSprite)), nullptr));
                    else
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, anim4, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealOneCard, this, pPlayer, pCardSprite)), nullptr));
                }
                else
                {
                    if(pGameRoom->m_vecRemainCards.size() == nRemainCardCounts)
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealLastCard, this, pPlayer, pCardSprite)), nullptr));
                    else
                        pCardSprite->runAction(Sequence::create(anim1, Spawn::create(anim2, anim3, NULL),
                                                                CallFunc::create(CC_CALLBACK_0(GameLayer::DoneDealOneCard, this, pPlayer, pCardSprite)), nullptr));
                }
            }
            
            nCardIndex++;
        }
        nTurn++;
        if(nTurn == pGameRoom->m_nMaxMember)
            nTurn = 0;
    }
}

void GameLayer::expandCards()
{
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    for(Vector<UserInfo*>::iterator it = pGameRoom->m_vecPlayers.begin(); it != pGameRoom->m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        if(pPlayer->m_strId == pUserInfo->m_strId)
        {
            for(Vector<CardSprite*>::iterator it = pPlayer->m_vecCards.begin(); it != pPlayer->m_vecCards.end(); it++)
            {
                CardSprite* pCardSprite = (CardSprite*)(*it);
                pCardSprite->hideBackSprite();
            }
        }
        
        if(pPlayer->m_nInGame == 1)
            repositionCards(pPlayer, true);
    }
}

void GameLayer::flipDeckCard()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    m_pActiveCard = pGameRoom->m_vecRemainCards.at(pGameRoom->m_vecRemainCards.size() - 1);
    pGameRoom->m_vecDeckCards.pushBack(m_pActiveCard);
    pGameRoom->m_vecRemainCards.eraseObject(m_pActiveCard);
    m_pActiveCard->hideBackSprite();
    m_pActiveCard->setScaleX(0.0f);
    
    auto anim1 = ScaleTo::create(0.3f, m_pActiveCard->getScaleY(), m_pActiveCard->getScaleY());
    auto anim2 = MoveTo::create(0.3f, Vec2(m_szDevice.width / 2 + 53.5f * m_fSmallScale , m_szDevice.height / 2 - 25.5f * m_fSmallScale));
    auto anim3 = DelayTime::create(0.5f);
    
    m_pActiveCard->runAction(Sequence::create(Spawn::create(anim1, anim2, NULL), anim3,
                                              CallFunc::create(CC_CALLBACK_0(GameLayer::DoneFlipDeckCard, this, m_pActiveCard)), nullptr));
}

void GameLayer::repositionCards(UserInfo* pPlayer, bool bAnim)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    PlayerSprite* pPlayerSprite = findPlayerSprite(pPlayer);
    
    int nCardCount = (int) pPlayer->m_vecCards.size();
    
    //float value1 = 25.5f;
    //float value2 = 51.0f;
    
    if(pPlayerSprite->m_nDisplayPos == ME_POSITION[pGameRoom->m_nType])
    {
        float value1 = 60.0f;
        float value2 = 97.0f;
        int nTotalWidth = (value1 * (nCardCount - 1) + value2) * m_fSmallScale;
        for(int index = 0; index < nCardCount; index++)
        {
            int posX = m_szDevice.width / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].x * m_fSmallScale -
            nTotalWidth / 2 + value1 * m_fSmallScale * index + value2 * m_fSmallScale / 2 ;
            CardSprite* pCardSprite = pPlayer->m_vecCards.at(index);
            
            if(bAnim)
                pCardSprite->runAction(MoveTo::create(0.3f, Vec2(posX, pCardSprite->getPositionY())));
            else
                pCardSprite->setPositionX(posX);
        }
    }
    else if(pGameRoom->m_nType == ROOM_TYPE_1)
    {
        float value1 = 33.0f;
        float value2 = 67.0f;
        int nTotalWidth = (value1 * (nCardCount - 1) + value2) * m_fSmallScale;
        for(int index = 0; index < nCardCount; index++)
        {
            int posX = m_szDevice.width / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].x * m_fSmallScale -
            nTotalWidth / 2 + value1 * m_fSmallScale * index + value2 * m_fSmallScale / 2 ;
            CardSprite* pCardSprite = pPlayer->m_vecCards.at(index);
            
            if(bAnim)
                pCardSprite->runAction(MoveTo::create(0.3f, Vec2(posX, pCardSprite->getPositionY())));
            else
                pCardSprite->setPositionX(posX);
        }
    }
    else if((pGameRoom->m_nType == ROOM_TYPE_5 || pGameRoom->m_nType == ROOM_TYPE_FRIEND) && (pPlayerSprite->m_nDisplayPos == 0 || pPlayerSprite->m_nDisplayPos == 3))
    {
        float value1 = 21.0f;
        float value2 = 43.0f;
        int nTotalHeight = (value1 * (nCardCount - 1) + value2) * m_fSmallScale;
        for(int index = 0; index < nCardCount; index++)
        {
            int posY = m_szDevice.height / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].y * m_fSmallScale -
            nTotalHeight / 2 + value1 * m_fSmallScale * index + value2 * m_fSmallScale / 2;
            CardSprite* pCardSprite = pPlayer->m_vecCards.at(index);
            
            if(bAnim)
                pCardSprite->runAction(MoveTo::create(0.3f, Vec2(pCardSprite->getPositionX(), posY)));
            else
                pCardSprite->setPositionY(posY);
        }
    }
    else
    {
        float value1 = 21.0f;
        float value2 = 43.0f;
        int nTotalWidth = (value1 * (nCardCount - 1) + value2) * m_fSmallScale;
        for(int index = 0; index < nCardCount; index++)
        {
            int posX = m_szDevice.width / 2 + g_staticCardPos[pGameRoom->m_nType][pPlayerSprite->m_nDisplayPos].x * m_fSmallScale -
            nTotalWidth / 2 + value1 * m_fSmallScale * index + value2 * m_fSmallScale / 2 ;
            CardSprite* pCardSprite = pPlayer->m_vecCards.at(index);
            
            if(bAnim)
                pCardSprite->runAction(MoveTo::create(0.3f, Vec2(posX, pCardSprite->getPositionY())));
            else
                pCardSprite->setPositionX(posX);
        }
    }
}

void GameLayer::updateTurn(UserInfo* pPrevPlayer)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    UserInfo* pCurPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    PlayerSprite* pCurPlayerSprite = findPlayerSprite(pCurPlayer);
    
    pCurPlayerSprite->active();
    pCurPlayerSprite->hidePickup();
    
    MyMenuItem* pMenuPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    MyMenuItem* pMenuPickup = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP);
    MyMenuItem* pMenuPickup5 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP5);
    MyMenuItem* pMenuPickup10 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP10);
    MyMenuItem* pMenuPickup2 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP2);
    MyMenuItem* pMenuPickup4 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP4);
    MyMenuItem* pMenuPickup6 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP6);
    MyMenuItem* pMenuPickup8 = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP8);
    
    pMenuPickup->setVisible(false);
    pMenuPickup5->setVisible(false);
    pMenuPickup10->setVisible(false);
    pMenuPickup2->setVisible(false);
    pMenuPickup4->setVisible(false);
    pMenuPickup6->setVisible(false);
    pMenuPickup8->setVisible(false);
    
    pMenuPlace->setEnabled(false);
    if(pUserInfo->m_strId == pCurPlayer->m_strId)
    {
        if(m_strPickState == "pickup2") {
            pMenuPickup2->setVisible(true);
            pMenuPickup2->setEnabled(true);
        }
        else if(m_strPickState == "pickup4") {
            pMenuPickup4->setVisible(true);
            pMenuPickup4->setEnabled(true);
        }
        else if(m_strPickState == "pickup6") {
            pMenuPickup6->setVisible(true);
            pMenuPickup6->setEnabled(true);
        }
        else if(m_strPickState == "pickup8") {
            pMenuPickup8->setVisible(true);
            pMenuPickup8->setEnabled(true);
        }
        else if(m_strPickState == "pickup5") {
            pMenuPickup5->setVisible(true);
            pMenuPickup5->setEnabled(true);
        }
        else if(m_strPickState == "pickup10") {
            pMenuPickup10->setVisible(true);
            pMenuPickup10->setEnabled(true);
        }
        else {
            pMenuPickup->setVisible(true);
            pMenuPickup->setEnabled(true);
        }
    }
    else
    {
        pMenuPickup->setVisible(true);
        pMenuPickup->setEnabled(false);
    }
    
    if(pPrevPlayer != NULL)
    {
        PlayerSprite* pPrevPlayerSprite = findPlayerSprite(pPrevPlayer);
        if(hasKnockCandidate(pPrevPlayer))
            pPrevPlayerSprite->showKnock();
        else
            pPrevPlayerSprite->hideKnock();
    }
}

void GameLayer::addPlayerSprites()
{
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    int delta_position = 0;
    for(Vector<UserInfo*>::iterator it = pGameRoom->m_vecPlayers.begin(); it != pGameRoom->m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        if(pPlayer->m_strId == pUserInfo->m_strId)
        {
            delta_position = ME_POSITION[pGameRoom->m_nType] - pPlayer->m_nPosition;
            break;
        }
    }
    
    for(Vector<UserInfo*>::iterator it = pGameRoom->m_vecPlayers.begin(); it != pGameRoom->m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        int display_pos = pPlayer->m_nPosition + delta_position;
        if(display_pos < 0)
            display_pos += MAX_PLAYERS[pGameRoom->m_nType];
        else if(display_pos >= MAX_PLAYERS[pGameRoom->m_nType])
            display_pos -= MAX_PLAYERS[pGameRoom->m_nType];
        
        auto playerSprite = PlayerSprite::create(pPlayer, display_pos);
        playerSprite->setCascadeOpacityEnabled(true);
        playerSprite->setScale(m_fSmallScale);
        playerSprite->setPosition(Vec2(m_szDevice.width / 2 + g_staticPlayerPos[pGameRoom->m_nType][display_pos].x * m_fSmallScale,
                                       m_szDevice.height / 2 + g_staticPlayerPos[pGameRoom->m_nType][display_pos].y * m_fSmallScale));
        addChild(playerSprite, LAYER_ZORDER2);
        m_vecPlayerSprites.pushBack(playerSprite);
    }
}

void GameLayer::addPlayerSprite(UserInfo* pPlayer)
{
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    int delta_position = 0;
    for(Vector<UserInfo*>::iterator it = pGameRoom->m_vecPlayers.begin(); it != pGameRoom->m_vecPlayers.end(); it++)
    {
        UserInfo* pPlayer = (UserInfo*)(*it);
        if(pPlayer->m_strId == pUserInfo->m_strId)
        {
            delta_position = ME_POSITION[pGameRoom->m_nType] - pPlayer->m_nPosition;
            break;
        }
    }
    
    int display_pos = pPlayer->m_nPosition + delta_position;
    if(display_pos < 0)
        display_pos += MAX_PLAYERS[pGameRoom->m_nType];
    else if(display_pos >= MAX_PLAYERS[pGameRoom->m_nType])
        display_pos -= MAX_PLAYERS[pGameRoom->m_nType];
    
    auto playerSprite = PlayerSprite::create(pPlayer, display_pos);
    playerSprite->setCascadeOpacityEnabled(true);
    playerSprite->setScale(m_fSmallScale);
    playerSprite->setPosition(Vec2(m_szDevice.width / 2 + g_staticPlayerPos[pGameRoom->m_nType][display_pos].x * m_fSmallScale,
                                   m_szDevice.height / 2 + g_staticPlayerPos[pGameRoom->m_nType][display_pos].y * m_fSmallScale));
    addChild(playerSprite, LAYER_ZORDER2);
    m_vecPlayerSprites.pushBack(playerSprite);
}

void GameLayer::removePlayerSprite(UserInfo* pPlayer)
{
    PlayerSprite* pPlayerSprite = findPlayerSprite(pPlayer);
    if(pPlayerSprite == NULL)
        return;
    
    pPlayerSprite->removeFromParentAndCleanup(true);
    m_vecPlayerSprites.eraseObject(pPlayerSprite);
}

PlayerSprite* GameLayer::findPlayerSprite(UserInfo* pPlayer)
{
    if(pPlayer == NULL)
        return NULL;
    
    for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
    {
        PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
        if(pPlayer->m_strId == pPlayerSprite->m_pPlayer->m_strId)
            return pPlayerSprite;
    }
    return NULL;
}

void GameLayer::updatePlayerStates()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    if(pGameRoom->m_nStarted != 1)
        return;
    
    for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
    {
        PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
        pPlayerSprite->setOpacity((pPlayerSprite->m_pPlayer->m_nInGame == 1) ? 255 : 125);
    }
}

void GameLayer::updatePlayerSprites()
{
    for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
    {
        PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
        pPlayerSprite->updateInfo();
    }
}

bool GameLayer::match1(CardSprite* pFirst, CardSprite* pSecond, bool bCheckCard1, bool bCheckCard2)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    if(m_strPickState == "pickup5" || m_strPickState == "pickup10")
        return pSecond->getCardNum() == CARD_11;
    
    if(m_strPickState == "pickup2" || m_strPickState == "pickup4" || m_strPickState == "pickup6" || m_strPickState == "pickup8")
    {
        if(bCheckCard2)
        {
            if(m_nCard2Turn == pGameRoom->m_nCurTurn)
                return pFirst->getCardNum() == pSecond->getCardNum() || pFirst->getCardFlower() == pSecond->getCardFlower();
            else
                return pSecond->getCardNum() == CARD_2;
        }
        else
        {
            return false;
        }
    }
    
    if(bCheckCard1 && pSecond->getCardNum() == CARD_1)
        return true;
    
    if(m_nActiveFlower == CARD_NONE)
        return pFirst->getCardNum() == pSecond->getCardNum() || pFirst->getCardFlower() == pSecond->getCardFlower();
    else
        return pSecond->getCardFlower() == m_nActiveFlower || pFirst->getCardNum() == pSecond->getCardNum();
    
    return false;
}

bool GameLayer::match2(CardSprite* pFirst, CardSprite* pSecond)
{
    if(pFirst->getCardNum() == pSecond->getCardNum())
        return true;
    
    if(((pFirst->getCardNum() + 1) == pSecond->getCardNum() || (pFirst->getCardNum() - 1) == pSecond->getCardNum()) && pFirst->getCardFlower() == pSecond->getCardFlower())
        return true;
    
    return false;
}

bool GameLayer::check1(Move* pMove1, Move* pMove2, bool bCheckCard1, bool bCheckCard2)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    
    if(m_strPickState == "pickup5" || m_strPickState == "pickup10")
        return pMove2->m_nCardNum == CARD_11;
    
    if(m_strPickState == "pickup2" || m_strPickState == "pickup4" || m_strPickState == "pickup6" || m_strPickState == "pickup8")
    {
        if(bCheckCard2)
        {
            if(m_nCard2Turn == pGameRoom->m_nCurTurn)
                return pMove1->m_nCardNum == pMove2->m_nCardNum || pMove1->m_nCardFlower == pMove2->m_nCardFlower;
            else
                return pMove2->m_nCardNum == CARD_2;
        }
        else
        {
            return false;
        }
    }
    
    if(bCheckCard1 && pMove2->m_nCardNum == CARD_1)
        return true;
    
    if(m_nActiveFlower == CARD_NONE)
        return pMove1->m_nCardNum == pMove2->m_nCardNum || pMove1->m_nCardFlower == pMove2->m_nCardFlower;
    else
        return pMove2->m_nCardFlower == m_nActiveFlower || pMove1->m_nCardNum == pMove2->m_nCardNum;
    
    return false;
}

bool GameLayer::check2(Move* pMove1, Move* pMove2)
{
    if(pMove1->m_nCardNum == pMove2->m_nCardNum)
        return true;
    
    if(((pMove1->m_nCardNum + 1) == pMove2->m_nCardNum || (pMove1->m_nCardNum - 1) == pMove2->m_nCardNum) && pMove1->m_nCardFlower == pMove2->m_nCardFlower)
        return true;
    
    return false;
}

void GameLayer::processGameResult()
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pUserInfo = GameData::getInstance()->getUserInfo();
    
    for(int index = 0; index < pGameRoom->m_vecPlayers.size(); index++)
    {
        UserInfo* pPlayer = pGameRoom->m_vecPlayers.at(index);
        
        if(pPlayer->m_strId == m_GameResultInfo.strWinnerId)
        {
            pPlayer->levelup();
            pPlayer->m_nCoin += m_GameResultInfo.nWinnerCoin;
        }
        
        if(pPlayer->m_nInGame == 1)
        {
            if(pPlayer->m_strId == m_GameResultInfo.strWinnerId)
            {
                if(pPlayer->m_strId == pUserInfo->m_strId)
                    showGameOver(true);
                else
                    showGameOver(false);
                break;
            }
        }
    }
    
    updatePlayerSprites();
}

void GameLayer::showGameOver(bool bWinner)
{
    for(Vector<PlayerSprite*>::iterator it = m_vecPlayerSprites.begin(); it != m_vecPlayerSprites.end(); it++)
    {
        PlayerSprite* pPlayerSprite = (PlayerSprite*)(*it);
        pPlayerSprite->deactive();
        pPlayerSprite->hidePickup();
    }
    
    MyMenuItem* pMenuPlace = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PLACE);
    MyMenuItem* pMenuPickup = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_PICKUP);
    
    pMenuPlace->setVisible(false);
    pMenuPickup->setVisible(false);
   
    if(m_pGameOverLayer != NULL)
        return;
    
    m_pGameOverLayer = GameOverLayer::create();
    m_pGameOverLayer->init(bWinner);
    m_pGameOverLayer->setCascadeOpacityEnabled(true);
    addChild(m_pGameOverLayer, LAYER_ZORDER2 + 350);
}

void GameLayer::hideGameOver()
{
    if(m_pGameOverLayer != NULL)
    {
        m_pGameOverLayer->removeFromParentAndCleanup(true);
        m_pGameOverLayer = NULL;
    }
    
    removeAllChildrenWithCleanup(true);
    
    m_pActiveCard = NULL;
    m_bStartedGame = false;
    m_bProcessingCards = false;
    m_vecPlayerSprites.clear();
    m_nActiveFlower = CARD_NONE;
    m_nCard2Turn = -1;
    m_strPickState = "";
    m_vecCandidates.clear();
    
    createSprites();
    createLabels();
    createMenu();
    
    MyMenuItem* pStartMatch = (MyMenuItem*) getChildByTag(GT_MENU)->getChildByTag(GTM_START_MATCH);
    pStartMatch->setVisible(false);
}

void GameLayer::showItemMenu()
{
    Sprite* pMenuBg = (Sprite*) getChildByTag(GT_ITEM_MENU_BG);
    Menu* pMenu = (Menu*) getChildByTag(GT_ITEM_MENU);
    
    pMenuBg->setVisible(true);
    pMenu->setVisible(true);
}

void GameLayer::hideItemMenu()
{
    Sprite* pMenuBg = (Sprite*) getChildByTag(GT_ITEM_MENU_BG);
    Menu* pMenu = (Menu*) getChildByTag(GT_ITEM_MENU);
    
    pMenuBg->setVisible(false);
    pMenu->setVisible(false);
}

void GameLayer::putCardWithFlower(int flower)
{
    GameRoom* pGameRoom = GameData::getInstance()->getGameRoom();
    UserInfo* pPlayer = pGameRoom->findTurnPlayer(pGameRoom->m_nCurTurn);
    
    hideItemMenu();
    
    PlayerSprite* pPlayerSprite = findPlayerSprite(pPlayer);
    pPlayerSprite->deactive();
    
    std::vector<int> cards;
    for(Vector<CardSprite*>::iterator it = pPlayer->m_vecActiveCards.begin(); it != pPlayer->m_vecActiveCards.end(); it++)
    {
        CardSprite* pCardSprite = (CardSprite*)(*it);
        cards.push_back(pCardSprite->getCardValue());
    }
    
    GameData::getInstance()->sendPacket(PT_REQ_PUT_CARD, PacketParser::getInstance()->makeRequestPutCard(pPlayer->m_strId, pPlayer->m_strRoomId, cards, flower));
}

void GameLayer::showActiveFlower()
{
    Sprite* pSprHeart = (Sprite*) getChildByTag(GT_ACTIVE_HEART);
    Sprite* pSprDiamond = (Sprite*) getChildByTag(GT_ACTIVE_DIAMOND);
    Sprite* pSprClub = (Sprite*) getChildByTag(GT_ACTIVE_CLUB);
    Sprite* pSprSpade = (Sprite*) getChildByTag(GT_ACTIVE_SPADE);
    
    pSprHeart->setVisible(false);
    pSprDiamond->setVisible(false);
    pSprClub->setVisible(false);
    pSprSpade->setVisible(false);
    
    switch (m_nActiveFlower) {
        case CARD_HEART:
            pSprHeart->setVisible(true);
            break;
        case CARD_DIAMOND:
            pSprDiamond->setVisible(true);
            break;
        case CARD_SPADE:
            pSprSpade->setVisible(true);
            break;
        case CARD_CLUB:
            pSprClub->setVisible(true);
            break;
        default:
            break;
    }
}

void GameLayer::permute(Vector<Move*> vecMoves, int nStartIndex, int nEndIndex)
{
    int i;
    if (nStartIndex == nEndIndex)
    {
        Candidate* pCandidate = new Candidate();
        pCandidate->m_vecMoves.clear();
        
        for(int index = 0; index < vecMoves.size(); index++)
        {
            Move* pMove = vecMoves.at(index);
            pCandidate->m_vecMoves.pushBack(pMove);
        }
        m_vecCandidates.pushBack(pCandidate);
    }
    else
    {
        for (i = nStartIndex; i <= nEndIndex; i++)
        {
            vecMoves.swap(nStartIndex, i);
            permute(vecMoves, nStartIndex + 1, nEndIndex);
            vecMoves.swap(nStartIndex, i);
        }
    }
}

bool GameLayer::hasKnockCandidate(UserInfo* pPlayer)
{
    if(pPlayer->m_vecCards.size() >= 8)
        return false;
    
    Vector<Move*> vecMoves;
    for(int index = 0; index < pPlayer->m_vecCards.size(); index++)
    {
        CardSprite* pCardSprite = pPlayer->m_vecCards.at(index);
        Move* pMove = new Move();
        pMove->m_nValue = pCardSprite->getCardValue();
        pMove->m_nCardNum = pCardSprite->getCardNum();
        pMove->m_nCardFlower = pCardSprite->getCardFlower();
        vecMoves.pushBack(pMove);
    }
    
    Move activeMove;
    activeMove.m_nValue = m_pActiveCard->getCardValue();
    activeMove.m_nCardNum = m_pActiveCard->getCardNum();
    activeMove.m_nCardFlower = m_pActiveCard->getCardFlower();
    
    m_vecCandidates.clear();
    permute(vecMoves, 0, (int)vecMoves.size() - 1);
    
    for(int index = 0; index < m_vecCandidates.size(); index++) {
        Candidate* pCandidate = m_vecCandidates.at(index);
        
        bool bMatchRule = true;
        
        bool bCheckCard1 = pCandidate->isAllAce();
        bool bCheckCard2 = pCandidate->isAll2();
        
        for(int index1 = 0; index1 < pCandidate->m_vecMoves.size(); index1++)
        {
            Move* pMove = pCandidate->m_vecMoves.at(index1);
            
            if(index1 == 0)
            {
                bMatchRule = check1(&activeMove, pMove, bCheckCard1, bCheckCard2);
                if(!bMatchRule)
                    break;
            }
            else
            {
                Move* pPrevMove = pCandidate->m_vecMoves.at(index1 - 1);
                bMatchRule = check2(pPrevMove, pMove);
                if(!bMatchRule)
                    break;
            }
        }
        
        if(bMatchRule)
            return true;
    }
    
    return false;
}
