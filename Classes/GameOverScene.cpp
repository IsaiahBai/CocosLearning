#include "GameOverScene.h"

#include "BeginScene.h"
#include "FlyScene.h"

USING_NS_CC;

Scene* GameOverScene::createScene(int finalScore, int levelId)
{
    return GameOverScene::create(finalScore, levelId);
}

GameOverScene* GameOverScene::create(int finalScore, int levelId)
{
    auto ret = new (std::nothrow) GameOverScene();
    if (ret != nullptr && ret->initWithScore(finalScore, levelId)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool GameOverScene::initWithScore(int finalScore, int levelId)
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

    auto background = LayerColor::create(Color4B(10, 10, 18, 255));
    addChild(background);

    auto titleLabel = Label::createWithSystemFont("Game Over", "Arial", 48);
    titleLabel->setPosition(Vec2(centerX, centerY + 120.0f));
    titleLabel->setColor(Color3B(230, 70, 70));
    addChild(titleLabel);

    auto scoreLabel = Label::createWithSystemFont("Final Score: " + std::to_string(_finalScore), "Arial", 32);
    scoreLabel->setPosition(Vec2(centerX, centerY + 40.0f));
    scoreLabel->setColor(Color3B::WHITE);
    addChild(scoreLabel);

    auto restartText = Label::createWithSystemFont(u8"\u91cd\u65b0\u5f00\u59cb", "Arial", 30);
    restartText->setColor(Color3B(80, 220, 120));
    auto restartItem = MenuItemLabel::create(restartText, CC_CALLBACK_1(GameOverScene::restartGame, this));
    restartItem->setPosition(Vec2(centerX, centerY - 40.0f));

    auto homeText = Label::createWithSystemFont(u8"\u8fd4\u56de\u4e3b\u9875", "Arial", 30);
    homeText->setColor(Color3B(100, 180, 255));
    auto homeItem = MenuItemLabel::create(homeText, CC_CALLBACK_1(GameOverScene::backToHome, this));
    homeItem->setPosition(Vec2(centerX, centerY - 110.0f));

    auto menu = Menu::create(restartItem, homeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu);

    return true;
}

void GameOverScene::restartGame(Ref* sender)
{
    auto scene = FlyScene::createScene(_levelId);
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void GameOverScene::backToHome(Ref* sender)
{
    auto scene = BeginScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}
