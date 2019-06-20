#ifndef __CARDGAME_NOTIFICATION_H__
#define __CARDGAME_NOTIFICATION_H__

#include "../Common/Common.h"

class Notification : public Ref
{
public:
    Notification();
    ~Notification();
    
    ValueVector getValue();
    void setValue(ValueVector value);
    
public:
    std::string             m_strId;
    int                     m_nType;
    std::string             m_strSenderId;
    std::string             m_strSenderName;
    int                     m_nSenderLevel;
    int                     m_nSenderOnline;
    std::string             m_strReceiverId;
    int                     m_nReceiverAction;
    bool                    m_bNew;
};

#endif /* __CARDGAME_NOTIFICATION_H__ */
