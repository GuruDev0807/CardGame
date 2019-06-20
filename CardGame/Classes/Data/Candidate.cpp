
#include "Candidate.h"
#include "../Widget/CardSprite.h"

Move::Move()
{
    
}

Move::~Move()
{
    
}

Candidate::Candidate()
{
    m_vecMoves.clear();
}

Candidate::~Candidate()
{
    m_vecMoves.clear();
}

bool Candidate::isAllAce()
{
    for(int index = 0; index < m_vecMoves.size(); index++)
    {
        Move* pMove = m_vecMoves.at(index);
        if(pMove->m_nCardNum != CARD_1)
            return false;
    }
    
    return true;
}

bool Candidate::isAll2()
{
    for(int index = 0; index < m_vecMoves.size(); index++)
    {
        Move* pMove = m_vecMoves.at(index);
        if(pMove->m_nCardNum != CARD_2)
            return false;
    }
    
    return true;
}
