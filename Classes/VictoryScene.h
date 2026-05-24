#ifndef __VICTORY_SCENE_H__
#define __VICTORY_SCENE_H__

#include "cocos2d.h"

class VictoryScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int finalScore, int levelId);
    static VictoryScene* create(int finalScore, int levelId);

    bool initWithScore(int finalScore, int levelId);

private:
    void openLevelSelect(cocos2d::Ref* sender);
    void retryGame(cocos2d::Ref* sender);
    void backToHome(cocos2d::Ref* sender);

    int _finalScore = 0;
    int _levelId = 1;
};

#endif // __VICTORY_SCENE_H__
