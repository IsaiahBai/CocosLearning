#include "VictoryScene.h"

#include "BeginScene.h"
#include "FlyScene.h"
#include "LevelSelectScene.h"

USING_NS_CC;

Scene* VictoryScene::createScene(int finalScore, int levelId)
{
    return VictoryScene::create(finalScore, levelId);
}

VictoryScene* VictoryScene::create(int finalScore, int levelId)
{
    auto* ret = new (std::nothrow) VictoryScene();
    if (ret != nullptr && ret->initWithScore(finalScore, levelId)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool VictoryScene::initWithScore(int finalScore, int levelId)
{
    if (!Scene::init()) {
        return false;
    }

    _finalScore = finalScore;
    _levelId = std::max(1, levelId);

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();
    const float centerX = origin.x + visibleSize.width / 2.0f;
    const float centerY = origin.y + visibleSize.height / 2.0f;

    auto background = LayerColor::create(Color4B(14, 26, 22, 255));
    addChild(background);

    auto titleLabel = Label::createWithSystemFont("Victory", "Arial", 48);
    titleLabel->setPosition(Vec2(centerX, centerY + 130.0f));
    titleLabel->setColor(Color3B(255, 230, 120));
    addChild(titleLabel);

    auto scoreLabel = Label::createWithSystemFont("Final Score: " + std::to_string(_finalScore), "Arial", 30);
    scoreLabel->setPosition(Vec2(centerX, centerY + 55.0f));
    scoreLabel->setColor(Color3B::WHITE);
    addChild(scoreLabel);

    auto levelSelectText = Label::createWithSystemFont(u8"\u9009\u62e9\u5173\u5361", "Arial", 30);
    levelSelectText->setColor(Color3B(120, 220, 255));
    auto levelSelectItem = MenuItemLabel::create(levelSelectText, CC_CALLBACK_1(VictoryScene::openLevelSelect, this));
    levelSelectItem->setPosition(Vec2(centerX, centerY - 30.0f));

    auto retryText = Label::createWithSystemFont(u8"\u518d\u6b21\u6311\u6218", "Arial", 30);
    retryText->setColor(Color3B(120, 255, 150));
    auto retryItem = MenuItemLabel::create(retryText, CC_CALLBACK_1(VictoryScene::retryGame, this));
    retryItem->setPosition(Vec2(centerX, centerY - 95.0f));

    auto homeText = Label::createWithSystemFont(u8"\u8fd4\u56de\u4e3b\u9875", "Arial", 30);
    homeText->setColor(Color3B(255, 190, 120));
    auto homeItem = MenuItemLabel::create(homeText, CC_CALLBACK_1(VictoryScene::backToHome, this));
    homeItem->setPosition(Vec2(centerX, centerY - 160.0f));

    auto menu = Menu::create(levelSelectItem, retryItem, homeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu);

    return true;
}

void VictoryScene::openLevelSelect(Ref* sender)
{
    auto scene = LevelSelectScene::createScene(LevelSelectScene::loadLevelConfigs());
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void VictoryScene::retryGame(Ref* sender)
{
    auto scene = FlyScene::createScene(_levelId);
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void VictoryScene::backToHome(Ref* sender)
{
    auto scene = BeginScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}
