
#include "Notification.h"

Notification::Notification()
{
    m_strId = "";
    m_nType = EMAIL_TYPE_NONE;
    m_strSenderId = "";
    m_strSenderName = "";
    m_nSenderLevel = 0;
    m_nSenderOnline = 0;
    m_strReceiverId = "";
    m_nReceiverAction = EMAIL_ACTION_NONE;
    m_bNew = true;
}

Notification::~Notification()
{
}

ValueVector Notification::getValue()
{
    ValueVector value;
    value.push_back(Value(m_strId));
    value.push_back(Value(getStringFromInt(m_nType)));
    value.push_back(Value(m_strSenderId));
    value.push_back(Value(m_strSenderName));
    value.push_back(Value(getStringFromInt(m_nSenderLevel)));
    value.push_back(Value(getStringFromInt(m_nSenderOnline)));
    value.push_back(Value(m_strReceiverId));
    value.push_back(Value(getStringFromInt(m_nReceiverAction)));
    value.push_back(Value(getStringFromBool(m_bNew)));
    
    return value;
}

void Notification::setValue(ValueVector value)
{
    m_strId = value.at(0).asString();
    m_nType = value.at(1).asInt();
    m_strSenderId = value.at(2).asString();
    m_strSenderName = value.at(3).asString();
    m_nSenderLevel = value.at(4).asInt();
    m_nSenderOnline = value.at(5).asInt();
    m_strReceiverId = value.at(6).asString();
    m_nReceiverAction = value.at(7).asInt();
    m_bNew = value.at(8).asBool();
}
