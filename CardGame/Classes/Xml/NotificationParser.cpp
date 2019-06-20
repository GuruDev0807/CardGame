
#include "NotificationParser.h"
#include "../Data/Notification.h"

NotificationParser::NotificationParser() : XMLParser()
{
    m_vecNotifications.clear();
}

NotificationParser::~NotificationParser()
{
    m_vecNotifications.clear();
}

NotificationParser* NotificationParser::formatWithXMLFile(const char *xmlFile)
{
    NotificationParser* notificationParser = new NotificationParser();
    if(notificationParser->initWithXMLFile(xmlFile))
        return notificationParser;
    
    CC_SAFE_DELETE(notificationParser);
    return NULL;
}

NotificationParser* NotificationParser::formatWithPLISTFile(const char* plistFile)
{
    NotificationParser* notificationParser = new NotificationParser();
    if(notificationParser->initWithPLISTFile(plistFile))
        return notificationParser;
    
    CC_SAFE_DELETE(notificationParser);
    return NULL;
}

void NotificationParser::internalInit(const char *xmlFileName)
{
    XMLParser::internalInit(xmlFileName);
}

bool NotificationParser::initWithPLISTFile(const char* plistFile)
{
    ValueVector readArray = FileUtils::getInstance()->getValueVectorFromFile(plistFile);
    for(int i = 0; i < readArray.size(); i++)
    {
        ValueVector value = readArray.at(i).asValueVector();
        Notification* pNotification = new Notification();
        pNotification->setValue(value);
        m_vecNotifications.pushBack(pNotification);
    }
    
    return true;
}

void NotificationParser::startElement(void *ctx, const char *name, const char **atts)
{
    CC_UNUSED_PARAM(ctx);
    
    std::string elementName = (char*)name;
    DictionaryMap* attributeDict = parseAttributes(atts);
    
    if(elementName == "notification")
    {
        std::string strId = valueForKey("id", attributeDict);
        std::string strType = valueForKey("type", attributeDict);
        std::string strSenderId = valueForKey("sender_id", attributeDict);
        std::string strSenderName = valueForKey("sender_name", attributeDict);
        std::string strSenderLevel = valueForKey("sender_level", attributeDict);
        std::string strSenderOnline = valueForKey("sender_online", attributeDict);
        std::string strReceiverId = valueForKey("receiver_id", attributeDict);
        std::string strReceiverAction = valueForKey("receive_action", attributeDict);
        
        Notification* pNotification = new Notification();
        pNotification->m_strId = strId;
        pNotification->m_nType = getIntFromString(strType);
        pNotification->m_strSenderId = strSenderId;
        pNotification->m_strSenderName = strSenderName;
        pNotification->m_nSenderLevel = getIntFromString(strSenderLevel);
        pNotification->m_nSenderOnline = getIntFromString(strSenderOnline);
        pNotification->m_strReceiverId = strReceiverId;
        pNotification->m_nReceiverAction = getIntFromString(strReceiverAction);
        
        m_vecNotifications.pushBack(pNotification);
    }
    
    deleteAttributes(attributeDict);
}

void NotificationParser::endElement(void *ctx, const char *name)
{
    
}

void NotificationParser::textHandler(void *ctx, const char *ch, size_t len)
{
    
}

void NotificationParser::saveWithPLISTFile(const char* plistFile)
{
    ValueVector array;
    for(Vector<Notification*>::iterator it = m_vecNotifications.begin(); it != m_vecNotifications.end(); it++)
    {
        Notification* pNotification = (Notification*)(*it);
        array.push_back(Value(pNotification->getValue()));
    }
    
    if (FileUtils::getInstance()->writeValueVectorToFile(array, plistFile))
        log("see the plist file at %s", plistFile);
    else
        log("write plist file failed");
}

Notification* NotificationParser::getNotification(std::string strId)
{
    for(int index = 0; index < m_vecNotifications.size(); index++)
    {
        Notification* pNotification = m_vecNotifications.at(index);
        if(pNotification->m_strId == strId)
            return pNotification;
    }
    
    return nullptr;
}

int NotificationParser::getNewsCount()
{
    int count = 0;
    for(int index = 0; index < m_vecNotifications.size(); index++)
    {
        Notification* pNotification = m_vecNotifications.at(index);
        if(pNotification->m_bNew)
            count++;
    }
    return count;
}
