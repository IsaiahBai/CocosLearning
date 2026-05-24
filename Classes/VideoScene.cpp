#include "VideoScene.h"

#include "BeginScene.h"

USING_NS_CC;

namespace {
constexpr float VIDEO_FRAME_MARGIN_X = 40.0f;
constexpr float VIDEO_FRAME_TOP_MARGIN = 120.0f;
constexpr float VIDEO_FRAME_BOTTOM_MARGIN = 170.0f;
constexpr float BUTTON_Y_OFFSET = 70.0f;
constexpr float BUTTON_X_GAP = 140.0f;
constexpr float RATE_FAST = 2.0f;
constexpr float RATE_NORMAL = 1.0f;
}

Scene* VideoScene::createScene(const std::string& videoPath)
{
    return VideoScene::create(videoPath);
}

VideoScene* VideoScene::create(const std::string& videoPath)
{
    auto* ret = new (std::nothrow) VideoScene();
    if (ret != nullptr && ret->initWithVideoPath(videoPath)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool VideoScene::init()
{
    return initWithVideoPath("");
}

bool VideoScene::initWithVideoPath(const std::string& videoPath)
{
    if (!Scene::init()) {
        return false;
    }

    _videoPath = videoPath;
    _playbackRate = RATE_NORMAL;
    _trackedPlaybackTime = 0.0f;
    _videoStarted = false;
    _leavingScene = false;

    buildBaseUI();
    buildVideoPlayer();
    scheduleUpdate();

    return true;
}

void VideoScene::buildBaseUI()
{
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();
    const float centerX = origin.x + visibleSize.width / 2.0f;

    auto background = LayerColor::create(Color4B(8, 10, 16, 255));
    addChild(background);

    auto titleLabel = Label::createWithSystemFont("Video Player", "Arial", 36);
    titleLabel->setPosition(Vec2(centerX, origin.y + visibleSize.height - 42.0f));
    titleLabel->setColor(Color3B(235, 235, 245));
    addChild(titleLabel, 2);

    auto frame = DrawNode::create();
    const Vec2 frameRect[] = {
        Vec2(origin.x + VIDEO_FRAME_MARGIN_X, origin.y + VIDEO_FRAME_BOTTOM_MARGIN),
        Vec2(origin.x + visibleSize.width - VIDEO_FRAME_MARGIN_X, origin.y + VIDEO_FRAME_BOTTOM_MARGIN),
        Vec2(origin.x + visibleSize.width - VIDEO_FRAME_MARGIN_X, origin.y + visibleSize.height - VIDEO_FRAME_TOP_MARGIN),
        Vec2(origin.x + VIDEO_FRAME_MARGIN_X, origin.y + visibleSize.height - VIDEO_FRAME_TOP_MARGIN)
    };
    frame->drawSolidPoly(frameRect, 4, Color4F(0.02f, 0.03f, 0.05f, 1.0f));
    frame->drawPoly(frameRect, 4, true, Color4F(0.45f, 0.5f, 0.62f, 1.0f));
    addChild(frame, 1);

    auto pathLabel = Label::createWithSystemFont(
        "Path: " + (_videoPath.empty() ? std::string("<empty>") : _videoPath),
        "Arial",
        18);
    pathLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
    pathLabel->setDimensions(visibleSize.width - 80.0f, 50.0f);
    pathLabel->setPosition(Vec2(centerX, origin.y + 112.0f));
    pathLabel->setColor(Color3B(185, 195, 210));
    addChild(pathLabel, 2);

    _statusLabel = Label::createWithSystemFont("", "Arial", 24);
    _statusLabel->setPosition(Vec2(centerX, origin.y + 145.0f));
    addChild(_statusLabel, 2);

    auto skipText = Label::createWithSystemFont("Skip", "Arial", 30);
    skipText->setColor(Color3B(255, 210, 130));
    auto skipItem = MenuItemLabel::create(skipText, CC_CALLBACK_1(VideoScene::skipVideo, this));
    skipItem->setPosition(Vec2(centerX - BUTTON_X_GAP, origin.y + BUTTON_Y_OFFSET));

    _speedLabel = Label::createWithSystemFont("", "Arial", 30);
    _speedLabel->setColor(Color3B(120, 220, 255));
    updatePlaybackRateLabel();
    auto speedItem = MenuItemLabel::create(_speedLabel, CC_CALLBACK_1(VideoScene::togglePlaybackRate, this));
    speedItem->setPosition(Vec2(centerX + BUTTON_X_GAP, origin.y + BUTTON_Y_OFFSET));

    auto menu = Menu::create(skipItem, speedItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 3);
}

void VideoScene::buildVideoPlayer()
{
    if (_videoPath.empty()) {
        updateStatusLabel("Video path is empty", Color3B(255, 120, 120));
        return;
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();

    _videoPlayer = ui::VideoPlayer::create();
    _videoPlayer->setAnchorPoint(Vec2(0.0f, 0.0f));
    _videoPlayer->setPosition(Vec2(origin.x + VIDEO_FRAME_MARGIN_X, origin.y + VIDEO_FRAME_BOTTOM_MARGIN));
    _videoPlayer->setContentSize(Size(
        visibleSize.width - VIDEO_FRAME_MARGIN_X * 2.0f,
        visibleSize.height - VIDEO_FRAME_TOP_MARGIN - VIDEO_FRAME_BOTTOM_MARGIN));
    _videoPlayer->setKeepAspectRatioEnabled(true);
    _videoPlayer->setFullScreenEnabled(false);
    _videoPlayer->setLooping(false);
    _videoPlayer->setUserInputEnabled(true);
    _videoPlayer->setFileName(_videoPath);
    _videoPlayer->addEventListener(CC_CALLBACK_2(VideoScene::handleVideoEvent, this));
    addChild(_videoPlayer, 4);

    _videoPlayer->play();
    _videoStarted = true;
    updateStatusLabel("Playing 1.0x", Color3B(120, 255, 160));
#else
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();
    auto placeholder = Label::createWithSystemFont(
        "VideoPlayer is not implemented on this platform.\nUse Android or iOS to play the video.",
        "Arial",
        28);
    placeholder->setAlignment(TextHAlignment::CENTER);
    placeholder->setPosition(Vec2(origin.x + visibleSize.width / 2.0f, origin.y + visibleSize.height / 2.0f));
    placeholder->setColor(Color3B(220, 220, 235));
    addChild(placeholder, 2);

    updateStatusLabel("Win32 keeps scene and button logic only", Color3B(255, 210, 130));
#endif
}

void VideoScene::updatePlaybackRateLabel()
{
    if (_speedLabel != nullptr) {
        _speedLabel->setString(StringUtils::format("Speed %.1fx", _playbackRate));
    }
}

void VideoScene::updateStatusLabel(const std::string& text, const Color3B& color)
{
    if (_statusLabel != nullptr) {
        _statusLabel->setString(text);
        _statusLabel->setColor(color);
    }
}

void VideoScene::leaveScene()
{
    if (_leavingScene) {
        return;
    }

    _leavingScene = true;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
    if (_videoPlayer != nullptr) {
        _videoPlayer->stop();
    }
#endif

    auto scene = BeginScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.25f, scene));
}

void VideoScene::skipVideo(Ref* sender)
{
    updateStatusLabel("Video skipped", Color3B(255, 210, 130));
    leaveScene();
}

void VideoScene::togglePlaybackRate(Ref* sender)
{
    _playbackRate = (_playbackRate < RATE_FAST) ? RATE_FAST : RATE_NORMAL;
    updatePlaybackRateLabel();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
    if (_videoPlayer != nullptr && _videoPlayer->isPlaying()) {
        const float resumeTime = _trackedPlaybackTime;
        _videoPlayer->stop();
        _videoPlayer->play();
        if (resumeTime > 0.0f) {
            _videoPlayer->seekTo(resumeTime);
        }
    }
    updateStatusLabel(StringUtils::format("Playing %.1fx", _playbackRate), Color3B(120, 220, 255));
#else
    updateStatusLabel(StringUtils::format("Current speed %.1fx on desktop preview", _playbackRate), Color3B(120, 220, 255));
#endif
}

void VideoScene::update(float delta)
{
    if (_leavingScene || !_videoStarted) {
        return;
    }

    _trackedPlaybackTime += delta * _playbackRate;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_TIZEN) && !defined(CC_PLATFORM_OS_TVOS)
void VideoScene::handleVideoEvent(Ref* sender, ui::VideoPlayer::EventType eventType)
{
    switch (eventType) {
    case ui::VideoPlayer::EventType::PLAYING:
        updateStatusLabel(StringUtils::format("Playing %.1fx", _playbackRate), Color3B(120, 255, 160));
        break;
    case ui::VideoPlayer::EventType::PAUSED:
        updateStatusLabel("Video paused", Color3B(255, 220, 120));
        break;
    case ui::VideoPlayer::EventType::STOPPED:
        if (!_leavingScene) {
            updateStatusLabel("Video stopped", Color3B(255, 180, 120));
        }
        break;
    case ui::VideoPlayer::EventType::COMPLETED:
        updateStatusLabel("Video completed", Color3B(120, 255, 160));
        leaveScene();
        break;
    case ui::VideoPlayer::EventType::ERROR:
        updateStatusLabel("Video playback failed", Color3B(255, 120, 120));
        break;
    default:
        break;
    }
}
#endif
