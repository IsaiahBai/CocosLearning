#ifndef __GAMEOVER_SCENE_H__
#define __GAMEOVER_SCENE_H__

#include "cocos2d.h"

class GameOverScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int finalScore, int levelId);
    static GameOverScene* create(int finalScore, int levelId);

    bool initWithScore(int finalScore, int levelId);

private:
    void restartGame(cocos2d::Ref* sender);
    void backToHome(cocos2d::Ref* sender);

    int _finalScore = 0;
    int _levelId = 1;
};

#endif // __GAMEOVER_SCENE_H__
