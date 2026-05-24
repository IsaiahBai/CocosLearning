#include "LevelSelectScene.h"

#include "BeginScene.h"
#include "FlyScene.h"

#include <algorithm>
#include <sstream>

USING_NS_CC;

namespace {
constexpr float BUTTON_WIDTH = 180.0f;
constexpr float BUTTON_HEIGHT = 120.0f;
constexpr int BUTTONS_PER_ROW = 2;
constexpr float BUTTON_X_PADDING = 70.0f;
constexpr float BUTTON_Y_PADDING = 60.0f;
constexpr char LEVEL_CONFIG_FILE[] = "level_config.ini";

std::string trimString(const std::string& text)
{
    const std::string whitespace = " \t\r\n";
    const std::size_t begin = text.find_first_not_of(whitespace);
    if (begin == std::string::npos) {
        return "";
    }

    const std::size_t end = text.find_last_not_of(whitespace);
    return text.substr(begin, end - begin + 1);
}
}

std::vector<LevelSelectConfig> LevelSelectScene::loadLevelConfigs()
{
    std::vector<LevelSelectConfig> configs = {
        {1, u8"\u7b2c1\u5173", "level1.png", "level1.png"},
        {2, u8"\u7b2c2\u5173", "level2.png", "level2.png"},
        {3, u8"\u7b2c3\u5173", "level3.png", "level3.png"},
        {4, u8"\u7b2c4\u5173", "level4.png", "level4.png"}
    };

    const std::string configText = FileUtils::getInstance()->getStringFromFile(LEVEL_CONFIG_FILE);
    if (configText.empty()) {
        return configs;
    }

    std::istringstream configStream(configText);
    std::string line;
    std::string currentSection;

    while (std::getline(configStream, line)) {
        const std::string trimmedLine = trimString(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == ';') {
            continue;
        }

        if (trimmedLine.front() == '[' && trimmedLine.back() == ']') {
            currentSection = trimString(trimmedLine.substr(1, trimmedLine.size() - 2));
            continue;
        }

        const std::size_t equalPos = trimmedLine.find('=');
        if (equalPos == std::string::npos || currentSection.size() <= 5 || currentSection.find("level") != 0) {
            continue;
        }

        const int levelId = std::atoi(currentSection.substr(5).c_str());
        if (levelId <= 0) {
            continue;
        }

        const std::string key = trimString(trimmedLine.substr(0, equalPos));
        const std::string value = trimString(trimmedLine.substr(equalPos + 1));
        if (key.empty() || value.empty()) {
            continue;
        }

        auto it = std::find_if(configs.begin(), configs.end(),
            [levelId](const LevelSelectConfig& config) {
                return config.levelId == levelId;
            });

        if (it == configs.end()) {
            configs.push_back({ levelId, "Level " + std::to_string(levelId), "", "" });
            it = configs.end() - 1;
        }

        if (key == "title") {
            it->title = value;
        } else if (key == "normal_image") {
            it->normalImage = value;
        } else if (key == "selected_image") {
            it->selectedImage = value;
        }
    }

    std::sort(configs.begin(), configs.end(),
        [](const LevelSelectConfig& left, const LevelSelectConfig& right) {
            return left.levelId < right.levelId;
        });

    return configs;
}

Scene* LevelSelectScene::createScene(const std::vector<LevelSelectConfig>& levelConfigs)
{
    auto* scene = new (std::nothrow) LevelSelectScene();
    if (scene != nullptr && scene->initWithLevels(levelConfigs)) {
        scene->autorelease();
        return scene;
    }

    delete scene;
    return nullptr;
}

bool LevelSelectScene::initWithLevels(const std::vector<LevelSelectConfig>& levelConfigs)
{
    if (!Scene::init()) {
        return false;
    }

    _levelConfigs = levelConfigs;

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();
    const float centerX = origin.x + visibleSize.width / 2.0f;
    const float topY = origin.y + visibleSize.height;

    auto background = LayerColor::create(Color4B(18, 24, 42, 255));
    addChild(background);

    auto title = Label::createWithSystemFont(u8"\u5173\u5361\u9009\u62e9", "Arial", 40);
    title->setPosition(Vec2(centerX, topY - 60.0f));
    title->setColor(Color3B(240, 240, 255));
    addChild(title, 2);

    auto hint = Label::createWithSystemFont(u8"\u6309\u94ae\u6570\u91cf\u7531\u5173\u5361\u53c2\u6570\u5217\u8868\u51b3\u5b9a", "Arial", 22);
    hint->setPosition(Vec2(centerX, topY - 105.0f));
    hint->setColor(Color3B(180, 190, 220));
    addChild(hint, 2);

    buildLevelButtons(_levelConfigs);

    auto backText = Label::createWithSystemFont(u8"\u8fd4\u56de\u9996\u9875", "Arial", 28);
    backText->setColor(Color3B(120, 190, 255));
    auto backItem = MenuItemLabel::create(backText, CC_CALLBACK_1(LevelSelectScene::backToHome, this));
    backItem->setPosition(Vec2(centerX, origin.y + 50.0f));

    auto menu = Menu::create(backItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 3);

    return true;
}

void LevelSelectScene::buildLevelButtons(const std::vector<LevelSelectConfig>& levelConfigs)
{
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();
    const float totalWidth = BUTTONS_PER_ROW * BUTTON_WIDTH + (BUTTONS_PER_ROW - 1) * BUTTON_X_PADDING;
    const float startX = origin.x + (visibleSize.width - totalWidth) / 2.0f + BUTTON_WIDTH / 2.0f;
    const float startY = origin.y + visibleSize.height - 220.0f;

    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 2);

    for (std::size_t i = 0; i < levelConfigs.size(); ++i) {
        const int row = static_cast<int>(i) / BUTTONS_PER_ROW;
        const int col = static_cast<int>(i) % BUTTONS_PER_ROW;

        const float x = startX + col * (BUTTON_WIDTH + BUTTON_X_PADDING);
        const float y = startY - row * (BUTTON_HEIGHT + BUTTON_Y_PADDING);

        Sprite* normalSprite = nullptr;
        Sprite* selectedSprite = nullptr;
        const bool hasNormalImage = !levelConfigs[i].normalImage.empty() &&
            FileUtils::getInstance()->isFileExist(levelConfigs[i].normalImage);
        const bool hasSelectedImage = !levelConfigs[i].selectedImage.empty() &&
            FileUtils::getInstance()->isFileExist(levelConfigs[i].selectedImage);

        if (hasNormalImage) {
            normalSprite = Sprite::create(levelConfigs[i].normalImage);
        }

        if (hasSelectedImage) {
            selectedSprite = Sprite::create(levelConfigs[i].selectedImage);
        } else if (hasNormalImage) {
            selectedSprite = Sprite::create(levelConfigs[i].normalImage);
            if (selectedSprite != nullptr) {
                selectedSprite->setColor(Color3B(210, 210, 210));
            }
        }

        if (normalSprite == nullptr || selectedSprite == nullptr) {
            auto normalNode = LayerColor::create(Color4B::WHITE, BUTTON_WIDTH, BUTTON_HEIGHT);
            auto selectedNode = LayerColor::create(Color4B(210, 210, 210, 255), BUTTON_WIDTH, BUTTON_HEIGHT);

            Vec2 rectPoints[] = {
                Vec2::ZERO,
                Vec2(BUTTON_WIDTH, 0.0f),
                Vec2(BUTTON_WIDTH, BUTTON_HEIGHT),
                Vec2(0.0f, BUTTON_HEIGHT)
            };

            auto normalBorder = DrawNode::create();
            normalBorder->drawPoly(rectPoints, 4, true, Color4F(0.15f, 0.2f, 0.35f, 1.0f));
            normalNode->addChild(normalBorder);

            auto selectedBorder = DrawNode::create();
            selectedBorder->drawPoly(rectPoints, 4, true, Color4F(0.2f, 0.25f, 0.4f, 1.0f));
            selectedNode->addChild(selectedBorder);

            auto iconLabel = Label::createWithSystemFont(StringUtils::format("%02d", levelConfigs[i].levelId), "Arial", 36);
            iconLabel->setPosition(Vec2(BUTTON_WIDTH / 2.0f, BUTTON_HEIGHT / 2.0f + 10.0f));
            iconLabel->setColor(Color3B(45, 60, 95));
            normalNode->addChild(iconLabel);

            auto selectedIconLabel = Label::createWithSystemFont(StringUtils::format("%02d", levelConfigs[i].levelId), "Arial", 36);
            selectedIconLabel->setPosition(Vec2(BUTTON_WIDTH / 2.0f, BUTTON_HEIGHT / 2.0f + 10.0f));
            selectedIconLabel->setColor(Color3B(45, 60, 95));
            selectedNode->addChild(selectedIconLabel);

            auto item = MenuItemSprite::create(normalNode, selectedNode, CC_CALLBACK_1(LevelSelectScene::startLevel, this));
            item->setTag(levelConfigs[i].levelId);
            item->setPosition(Vec2(x, y));
            menu->addChild(item);
        } else {
            normalSprite->setScaleX(BUTTON_WIDTH / normalSprite->getContentSize().width);
            normalSprite->setScaleY(BUTTON_HEIGHT / normalSprite->getContentSize().height);
            selectedSprite->setScaleX(BUTTON_WIDTH / selectedSprite->getContentSize().width);
            selectedSprite->setScaleY(BUTTON_HEIGHT / selectedSprite->getContentSize().height);

            auto item = MenuItemSprite::create(normalSprite, selectedSprite, CC_CALLBACK_1(LevelSelectScene::startLevel, this));
            item->setTag(levelConfigs[i].levelId);
            item->setPosition(Vec2(x, y));
            menu->addChild(item);
        }

        auto titleLabel = Label::createWithSystemFont(levelConfigs[i].title, "Arial", 24);
        titleLabel->setPosition(Vec2(x, y - BUTTON_HEIGHT / 2.0f - 22.0f));
        titleLabel->setColor(Color3B(245, 245, 255));
        addChild(titleLabel, 2);
    }
}

void LevelSelectScene::startLevel(Ref* sender)
{
    auto* item = dynamic_cast<MenuItem*>(sender);
    const int levelId = item != nullptr ? item->getTag() : 1;
    auto scene = FlyScene::createScene(levelId);
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void LevelSelectScene::backToHome(Ref* sender)
{
    auto scene = BeginScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}
