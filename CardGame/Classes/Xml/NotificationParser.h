#ifndef __CARDGAME_NOTIFICATION_PARSER_H__
#define __CARDGAME_NOTIFICATION_PARSER_H__

#include "XMLParser.h"

class Notification;

class NotificationParser : public XMLParser
{
public:
    NotificationParser();
    virtual ~NotificationParser();
    
    static NotificationParser* formatWithXMLFile(const char* xmlFile);
    static NotificationParser* formatWithPLISTFile(const char* plistFile);
    
    void startElement(void *ctx, const char *name, const char **atts);
    void endElement(void *ctx, const char *name);
    void textHandler(void *ctx, const char *ch, size_t len);
    
    void saveWithPLISTFile(const char* plistFile);
    
    Notification* getNotification(std::string strId);
    int getNewsCount();
    
protected:
    void internalInit(const char* xmlFileName);
    bool initWithPLISTFile(const char* plistFile);
    
public:
    Vector<Notification*>       m_vecNotifications;
};

#endif /* __CARDGAME_NOTIFICATION_PARSER_H__ */
