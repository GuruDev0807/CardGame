#ifndef __CARDGAME_CANDIDATE_H__
#define __CARDGAME_CANDIDATE_H__

#include "../Common/Common.h"

class Move : public Ref
{
public:
    Move();
    ~Move();
    
public:
    int         m_nValue;
    int         m_nCardNum;
    int         m_nCardFlower;
};

class Candidate : public Ref
{
public:
    Candidate();
    ~Candidate();
    
    bool isAllAce();
    bool isAll2();
    
public:
    Vector<Move*>     m_vecMoves;
};

#endif /* __CARDGAME_CANDIDATE_H__ */
