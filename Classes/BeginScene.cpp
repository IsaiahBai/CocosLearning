#include "BeginScene.h"

#include "FlyScene.h"
#include "LevelSelectScene.h"
#include "SetScene.h"

USING_NS_CC;

Scene* BeginScene::createScene()
{
    return BeginScene::create();
}

bool BeginScene::init()
{
    if (!Scene::init()) {
        return false;
    }

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();
    const float centerX = origin.x + visibleSize.width / 2.0f;
    const float centerY = origin.y + visibleSize.height / 2.0f;

    auto background = LayerColor::create(Color4B(12, 18, 36, 255));
    addChild(background);

    auto title = Label::createWithSystemFont("Plane Battle", "Arial", 48);
    title->setPosition(Vec2(centerX, centerY + 160.0f));
    title->setColor(Color3B(235, 235, 255));
    addChild(title);

    auto startText = Label::createWithSystemFont(u8"\u5f00\u59cb\u6e38\u620f", "Arial", 34);
    startText->setColor(Color3B(90, 220, 120));
    auto startItem = MenuItemLabel::create(startText, CC_CALLBACK_1(BeginScene::startGame, this));
    startItem->setPosition(Vec2(centerX, centerY + 20.0f));

    auto settingsText = Label::createWithSystemFont(u8"\u8bbe\u7f6e", "Arial", 34);
    settingsText->setColor(Color3B(100, 180, 255));
    auto settingsItem = MenuItemLabel::create(settingsText, CC_CALLBACK_1(BeginScene::openSettings, this));
    settingsItem->setPosition(Vec2(centerX, centerY - 60.0f));

    auto menu = Menu::create(startItem, settingsItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu);

    return true;
}

void BeginScene::startGame(Ref* sender)
{
    auto scene = LevelSelectScene::createScene(LevelSelectScene::loadLevelConfigs());
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void BeginScene::openSettings(Ref* sender)
{
    auto scene = SetScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}
