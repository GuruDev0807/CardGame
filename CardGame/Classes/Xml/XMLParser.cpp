
#include "XMLParser.h"

XMLParser::XMLParser()
{
    m_strXMLFileName = "";
}

XMLParser::~XMLParser()
{
    
}

bool XMLParser::initWithXMLFile(const char* xmlFile)
{
    internalInit(xmlFile);
    return parseXMLFile(m_strXMLFileName.c_str());
}

void XMLParser::internalInit(const char *xmlFileName)
{
    if(xmlFileName != NULL)
        m_strXMLFileName = FileUtils::getInstance()->fullPathForFilename(xmlFileName);
}

bool XMLParser::initWithPLISTFile(const char* plistFile)
{
    return false;
}

bool XMLParser::parseXMLFile(const char* xmlFileName)
{
    SAXParser parser;
    if(false == parser.init("UTF-8"))
        return false;
    
    parser.setDelegator(this);
    return parser.parse(xmlFileName);
}

DictionaryMap* XMLParser::parseAttributes(const char **atts)
{
    DictionaryMap *attributeDict = new DictionaryMap();
    if(atts && atts[0])
    {
        for(int i = 0; atts[i]; i += 2)
        {
            std::string key = (char*)atts[i];
            std::string value = (char*)atts[i+1];
            attributeDict->insert(std::pair<std::string, std::string>(key, value));
        }
    }
    
    return attributeDict;
}

void XMLParser::deleteAttributes(DictionaryMap *attributeDict)
{
    if(attributeDict)
    {
        attributeDict->clear();
        delete attributeDict;
    }
}

const char* XMLParser::valueForKey(const char *key, DictionaryMap *dict)
{
    if(dict)
    {
        DictionaryMap::iterator it = dict->find(key);
        return it != dict->end() ? it->second.c_str() : "";
    }
    return "";
}
