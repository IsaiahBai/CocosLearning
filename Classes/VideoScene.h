#ifndef __VIDEO_SCENE_H__
#define __VIDEO_SCENE_H__

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
#include "ui/CocosGUI.h"
#endif

class VideoScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(const std::string& videoPath);
    static VideoScene* create(const std::string& videoPath);

    bool init() override;
    bool initWithVideoPath(const std::string& videoPath);

private:
    void buildBaseUI();
    void buildVideoPlayer();
    void updatePlaybackRateLabel();
    void updateStatusLabel(const std::string& text, const cocos2d::Color3B& color = cocos2d::Color3B::WHITE);
    void leaveScene();
    void skipVideo(cocos2d::Ref* sender);
    void togglePlaybackRate(cocos2d::Ref* sender);
    void update(float delta) override;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
    void handleVideoEvent(cocos2d::Ref* sender, cocos2d::ui::VideoPlayer::EventType eventType);
#endif

    std::string _videoPath;
    cocos2d::Label* _statusLabel = nullptr;
    cocos2d::Label* _speedLabel = nullptr;

    float _playbackRate = 1.0f;
    float _trackedPlaybackTime = 0.0f;
    bool _videoStarted = false;
    bool _leavingScene = false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
    cocos2d::ui::VideoPlayer* _videoPlayer = nullptr;
#endif
};

#endif // __VIDEO_SCENE_H__
