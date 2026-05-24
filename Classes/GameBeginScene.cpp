#include "GameBeginScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "GameScene.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* GameBegin::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	auto layer = GameBegin::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameBegin::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
	Size wndSize = this->getContentSize();
	//背景
	auto spriteBack = Sprite::create("mainScene.png");
	spriteBack->setAnchorPoint(Vec2::ZERO);
	spriteBack->setPosition(Vec2::ZERO);
	this->addChild(spriteBack);

	//开始 按纽
	auto menuStart = MenuItemImage::create("start_normal.png",
		"start_select.png",
		CC_CALLBACK_1(GameBegin::startGame, this));
	menuStart->setPosition(wndSize.width / 2, 300);
	//退出按纽
	auto menuEnd = MenuItemImage::create("exit_normal.png",
		"exit_select.png",
		CC_CALLBACK_1(GameBegin::endGame, this));
	menuEnd->setPosition(wndSize.width / 2, 240);
	auto menu = Menu::create(menuStart, menuEnd, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);

    return true;
}

void GameBegin::startGame(Ref* obj)
{
	//游戏开始 ，切换到游戏场景 
	auto scene = GameScene::createGameScene();
	Director::getInstance()->replaceScene(TransitionCrossFade::create(1, scene));
}

void GameBegin::endGame(Ref* obj)
{
	//退出游戏，关闭
	Director::getInstance()->end();
}