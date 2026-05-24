#ifndef __LEVEL_SELECT_SCENE_H__
#define __LEVEL_SELECT_SCENE_H__

#include "cocos2d.h"

#include <string>
#include <vector>

struct LevelSelectConfig
{
    int levelId = 0;
    std::string title;
    std::string normalImage;
    std::string selectedImage;
};

class LevelSelectScene : public cocos2d::Scene
{
public:
    static std::vector<LevelSelectConfig> loadLevelConfigs();
    static cocos2d::Scene* createScene(const std::vector<LevelSelectConfig>& levelConfigs);

    bool initWithLevels(const std::vector<LevelSelectConfig>& levelConfigs);

private:
    void buildLevelButtons(const std::vector<LevelSelectConfig>& levelConfigs);
    void startLevel(cocos2d::Ref* sender);
    void backToHome(cocos2d::Ref* sender);

    std::vector<LevelSelectConfig> _levelConfigs;
};

#endif // __LEVEL_SELECT_SCENE_H__
