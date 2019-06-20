#ifndef __CARDGAME_MYMENU_ITEM_H__
#define __CARDGAME_MYMENU_ITEM_H__

#include "../Common/Common.h"

class MyMenuItem : public MenuItemSprite
{
public:
    static MyMenuItem* create(const std::string& norImageName, const std::string& actImageName, const std::string& disImageName, const ccMenuCallback& callback);
    static MyMenuItem* create(const std::string& norImageName, const std::string& actImageName,
                              const std::string& labelName, TTFConfig labelConfig, Vec2 labelPosition, Color3B labelNorColor, Color3B labelActColor, const ccMenuCallback& callback);
    void updateLabel(const std::string& labelName);
    void updateColor(Color3B color, Color3B colorAct);
    
private:
    MyMenuItem();
    void init(const std::string& norImageName, const std::string& actImageName, const std::string& disImageName, const ccMenuCallback& callback);
    void init(const std::string& norImageName, const std::string& actImageName,
              const std::string& labelName, TTFConfig labelConfig, Vec2 labelPosition, Color3B labelNorColor, Color3B labelActColor, const ccMenuCallback& callback);
    
private:
    Label*  m_pNorLabel;
    Label*  m_pActLabel;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(MyMenuItem);
};

#endif /* __CARDGAME_MYMENU_ITEM_H__ */

