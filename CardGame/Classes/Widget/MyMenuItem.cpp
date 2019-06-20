#include "MyMenuItem.h"

MyMenuItem* MyMenuItem::create(const std::string& norImageName, const std::string& actImageName, const std::string& disImageName, const ccMenuCallback& callback)
{
    MyMenuItem *ret = new (std::nothrow) MyMenuItem();
    ret->init(norImageName, actImageName, disImageName, callback);
    ret->autorelease();
    return ret;
}

MyMenuItem* MyMenuItem::create(const std::string& norImageName, const std::string& actImageName,
                               const std::string& labelName, TTFConfig labelConfig, Vec2 labelPosition, Color3B labelNorColor, Color3B labelActColor, const ccMenuCallback& callback)
{
    MyMenuItem *ret = new (std::nothrow) MyMenuItem();
    ret->init(norImageName, actImageName, labelName, labelConfig, labelPosition, labelNorColor, labelActColor, callback);
    ret->autorelease();
    return ret;
}

MyMenuItem::MyMenuItem()
{
    m_pNorLabel = NULL;
    m_pActLabel = NULL;
}

void MyMenuItem::init(const std::string& norImageName, const std::string& actImageName, const std::string& disImageName, const ccMenuCallback& callback)
{
    auto nor_spr = Sprite::create(norImageName);
    auto act_spr = Sprite::create((actImageName == "") ? norImageName : actImageName);
    auto dis_spr = (disImageName == "") ? NULL : Sprite::create(disImageName);
    act_spr->setColor(Color3B(200, 200, 200));
    
    initWithNormalSprite(nor_spr, act_spr, dis_spr, callback);
}

void MyMenuItem::init(const std::string& norImageName, const std::string& actImageName,
                      const std::string& labelName, TTFConfig labelConfig, Vec2 labelPosition, Color3B labelNorColor, Color3B labelActColor, const ccMenuCallback& callback)
{
    auto nor_spr = Sprite::create(norImageName);
    auto act_spr = Sprite::create(actImageName);
    act_spr->setColor(Color3B(200, 200, 200));
    
    Size deviceSize = Director::getInstance()->getWinSize();
    Size itemSize = nor_spr->getContentSize();
    
    m_pNorLabel = Label::createWithTTF(labelConfig, labelName, TextHAlignment::CENTER, deviceSize.width);
    m_pActLabel = Label::createWithTTF(labelConfig, labelName, TextHAlignment::CENTER, deviceSize.width);
    m_pNorLabel->setAnchorPoint(Vec2(0.5, 0.5));
    m_pActLabel->setAnchorPoint(Vec2(0.5, 0.5));
    m_pNorLabel->setColor(labelNorColor);
    m_pActLabel->setColor(labelActColor);
    m_pNorLabel->setPosition(Vec2(itemSize.width / 2 + labelPosition.x, itemSize.height / 2 + labelPosition.y));
    m_pActLabel->setPosition(Vec2(itemSize.width / 2 + labelPosition.x, itemSize.height / 2 + labelPosition.y));
    
    nor_spr->addChild(m_pNorLabel);
    act_spr->addChild(m_pActLabel);
    
    initWithNormalSprite(nor_spr, act_spr, nullptr, callback);
}

void MyMenuItem::updateLabel(const std::string& labelName)
{
    if(m_pNorLabel != NULL)
    m_pNorLabel->setString(labelName);
    
    if(m_pActLabel != NULL)
    m_pActLabel->setString(labelName);
}

void MyMenuItem::updateColor(Color3B colorNor, Color3B colorAct)
{
    if(m_pNorLabel != NULL)
    m_pNorLabel->setColor(colorNor);
    
    if(m_pActLabel != NULL)
    m_pActLabel->setColor(colorAct);
}

