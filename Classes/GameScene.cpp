#include "GameScene.h"
#include "GameBeginScene.h"

//不停地做一件事，定时的隔多久进行一次相应操作
//schedule   scheduleUpdate  scheduleOnce
Scene* GameScene::createGameScene()
{
	Scene* scene = Scene::create();
	GameScene* gameLayer = GameScene::create();
	scene->addChild(gameLayer);
	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	srand(time(NULL));
	curScore = 0;
	Size wndSize = getContentSize();
	//添加背景
	spriteBack1 = Sprite::create("GameScene.png");
	spriteBack1->setAnchorPoint(Vec2::ZERO);
	spriteBack1->setPosition(Vec2::ZERO);
	spriteBack1->setScaleY(480 / 680.0);
	addChild(spriteBack1);

	spriteBack2 = Sprite::create("GameScene.png");
	spriteBack2->setAnchorPoint(Vec2::ZERO);
	spriteBack2->setPosition(Vec2(0,wndSize.height));
	spriteBack2->setScaleY(480 / 680.0);
	addChild(spriteBack2);

	//创建主飞机
	spriteMain = Sprite::create("plane.png",Rect(0,0,50,56));
	spriteMain->setPosition(wndSize.width / 2, 30);
	Vector<SpriteFrame*> planeVec;
	for (int i = 0; i < 3;++i)
	{
		auto spriteFrame = SpriteFrame::create("plane.png",
			Rect(50*i,0,50,56));
		planeVec.pushBack(spriteFrame);
	}
	auto ani = Animation::createWithSpriteFrames(planeVec, 0.2);
	auto animate = Animate::create(ani);
	spriteMain->runAction(RepeatForever::create(animate));
	this->addChild(spriteMain);
	//添加分数信息
	scoreLabel = Label::create();
	scoreLabel->setString("Score: 0");
	scoreLabel->setSystemFontSize(20);
	scoreLabel->setColor(Color3B(0, 255, 0));
	scoreLabel->setAnchorPoint(Vec2(0, 1));
	scoreLabel->setPosition(Vec2(5,wndSize.height-5));
	this->addChild(scoreLabel);

	//触摸事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(
		listener, this);
	////////////2秒后再创建下面的定时器
	//添加飞机
	schedule(CC_SCHEDULE_SELECTOR(GameScene::createPlane), 0.3);
	//添加子弹
	schedule(CC_SCHEDULE_SELECTOR(GameScene::createBullet), 0.4);
	//子弹和飞机移动
	schedule(CC_SCHEDULE_SELECTOR(GameScene::move), 0.1);
	

	return true;
}


bool GameScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

void GameScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	Vec2 vec = touch->getDelta();
	Vec2 oldPos = spriteMain->getPosition();
	Vec2 newPos = oldPos + vec;
	//判断越界
	Size wndSize = getContentSize();
	Size spriteSize = spriteMain->getContentSize();
	if (newPos.x < spriteSize.width/2)
	{
		newPos.x = spriteSize.width / 2;
	}
	else if (newPos.x > wndSize.width-spriteSize.width/2)
	{
		newPos.x = wndSize.width - spriteSize.width / 2;
	}
	if (newPos.y < spriteSize.height/2)
	{
		newPos.y = spriteSize.height / 2;
	}
	else if (newPos.y > wndSize.height-spriteSize.height/2)
	{
		newPos.y = wndSize.height - spriteSize.height / 2;
	}

	spriteMain->setPosition(newPos);
}

void GameScene::onTouchEnded(Touch *touch, Event *unused_event)
{

}

void GameScene::createPlane(float t)
{
	//随机一架飞机
	int iPlaneIndex = rand() % 4;
	char fileName[20];
	sprintf(fileName, "enemy%d.png", iPlaneIndex);
	auto sprite = Sprite::create(fileName);
	//随机位置
	auto planeSize = sprite->getContentSize();
	auto wndSize = this->getContentSize();
	int iMinX = planeSize.width / 2;
	int iMaxX = wndSize.width - planeSize.width / 2;
	int iPosX = rand() % (iMaxX - iMinX + 1) + iMinX;
	int iPosY = planeSize.height / 2 + wndSize.height;
	sprite->setPosition(Vec2(iPosX, iPosY));
	this->addChild(sprite);
	enemyVec.pushBack(sprite);
}
void GameScene::createBullet(float t)
{
	//添加子弹
	auto bullet = Sprite::create("bullet.png");
	auto mainPlanePos = spriteMain->getPosition();
	auto mainPlaneSize = spriteMain->getContentSize();
	Vec2 pos;
	pos.x = mainPlanePos.x;
	pos.y = mainPlanePos.y + mainPlaneSize.height / 2 +
		bullet->getContentSize().width / 2;
	bullet->setPosition(pos);
	this->addChild(bullet);
	bulletVec.pushBack(bullet);
}
void GameScene::move(float t)
{
	//移动地图
	Size wndSize = this->getContentSize();
	Vec2 pos1 = spriteBack1->getPosition();
	Vec2 pos2 = spriteBack2->getPosition();
	pos1.y -= 3;
	pos2.y -= 3;
	if (pos1.y < -wndSize.height)
	{
		//归位
		spriteBack1->setPosition(Vec2::ZERO);
		spriteBack2->setPosition(Vec2(0, wndSize.height));
	}
	else
	{
		spriteBack1->setPosition(pos1);
		spriteBack2->setPosition(pos2);
	}
	//遍历所有敌机，移动
	for (int i = 0; i < enemyVec.size();++i)
	{
		auto plane = enemyVec.at(i);
		Vec2 pos = plane->getPosition();
		pos.y -= 10;
		plane->setPosition(pos);
		if (pos.y< -plane->getContentSize().height/2)
		{
			//移除
			plane->removeFromParent();
			enemyVec.eraseObject(plane);
			--i;
		}
	}
	//遍历所有子弹移动
	for (int i = 0; i < bulletVec.size();++i)
	{
		auto bullet = bulletVec.at(i);
		Vec2 pos = bullet->getPosition();
		pos.y += 13;
		bullet->setPosition(pos);
		if (pos.y > wndSize.height+bullet->getContentSize().height/2)
		{
			//移除
			bullet->removeFromParent();
			bulletVec.eraseObject(bullet);
			--i;
		}
	}
	//碰撞检测
	CollisionCheck();
}

void GameScene::CollisionCheck()
{
	//子弹和敌机的碰撞
	for (int i = 0; i < bulletVec.size();++i)
	{
		auto bullet = bulletVec.at(i);
		Rect bulletRect = bullet->getBoundingBox();
		bulletRect.setRect(bulletRect.getMinX() + 5,
			bulletRect.getMinY() + 8,
			bulletRect.getMaxX() - bulletRect.getMinX() - 10,
			bulletRect.getMaxY() - bulletRect.getMinY() - 16);
		for (int j = 0; j < enemyVec.size();++j)
		{
			auto enemy = enemyVec.at(j);
			Rect enemyRect = enemy->getBoundingBox();
			enemyRect.setRect(enemyRect.getMinX() + 3,
				enemyRect.getMinY() + 13,
				enemyRect.getMaxX() - enemyRect.getMinX() - 6,
				enemyRect.getMaxY() - enemyRect.getMinY() - 26);
			if (bulletRect.intersectsRect(enemyRect))
			{
				//碰撞到，说明子弹打到了对应敌机
				//把敌机和子弹都移除
				enemy->removeFromParent();
				enemyVec.eraseObject(enemy);

				bullet->removeFromParent();
				bulletVec.eraseObject(bullet);
				--i;

				//加分
				curScore += 10;
				char scoreString[20];
				sprintf(scoreString, "Score: %d", curScore);
				scoreLabel->setString(scoreString);

				break;
			}
		}
	}
	//主飞机和敌机碰撞
	Rect mainPlaneRect = spriteMain->getBoundingBox();
	for (int i = 0; i < enemyVec.size();++i)
	{
		auto enemy = enemyVec.at(i);
		Rect enemyRect = enemy->getBoundingBox();
		enemyRect.setRect(enemyRect.getMinX() + 3,
			enemyRect.getMinY() + 13,
			enemyRect.getMaxX() - enemyRect.getMinX() - 6,
			enemyRect.getMaxY() - enemyRect.getMinY() - 26);
		if (mainPlaneRect.intersectsRect(enemyRect))
		{
			//游戏结束
			GameOver();
		}
	}
}

void GameScene::GameOver()
{
	auto scene = GameBegin::createScene();
	Director::getInstance()->replaceScene(scene);
}