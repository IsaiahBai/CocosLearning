#ifndef __BEGIN_SCENE_H__
#define __BEGIN_SCENE_H__

#include "cocos2d.h"

class BeginScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;

    CREATE_FUNC(BeginScene);

private:
    void startGame(cocos2d::Ref* sender);
    void openSettings(cocos2d::Ref* sender);
};

#endif // __BEGIN_SCENE_H__
