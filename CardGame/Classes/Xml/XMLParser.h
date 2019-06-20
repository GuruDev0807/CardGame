#ifndef __CARDGAME_XMLPARSER_H__
#define __CARDGAME_XMLPARSER_H__

#include "../Common/Common.h"

typedef std::map<std::string, std::string> DictionaryMap;

class XMLParser : public SAXDelegator
{
public:
    bool            initWithXMLFile(const char* xmlFile);
    bool            parseXMLFile(const char* xmlFileName);
    DictionaryMap*  parseAttributes(const char **atts);
    void            deleteAttributes(DictionaryMap* attributeDict);
    const char*     valueForKey(const char *key, DictionaryMap* dict);
    
protected:
    XMLParser();
    virtual ~XMLParser();
    
    virtual void internalInit(const char* xmlFileName);
    virtual bool initWithPLISTFile(const char* plistFile);
    
protected:
    std::string     m_strXMLFileName;
};

#endif /* __CARDGAME_XMLPARSER_H__ */


