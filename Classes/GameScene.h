#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;
class GameScene : public Layer
{
public:
	//创建场景 
	static Scene* createGameScene();
	//初始化
	virtual bool init();

	//触摸事件
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);

	//定时器
	void createPlane(float t);
	void createBullet(float t);
	void move(float t);

	//碰撞检测
	void CollisionCheck();

	//游戏结束
	void GameOver();

	CREATE_FUNC(GameScene);
private:
	//背景图
	Sprite* spriteBack1;
	Sprite* spriteBack2;
	//主飞机
	Sprite* spriteMain;
	//分数label
	Label* scoreLabel;
	unsigned curScore;

	//飞机
	Vector<Sprite*> enemyVec;	//保存敌机
	Vector<Sprite*> bulletVec;	//保存子弹
};

#endif // !__GAME_SCENE_H__
