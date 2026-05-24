/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "FlyScene.h"
#include "GameOverScene.h"
#include "VictoryScene.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <sstream>

USING_NS_CC;

namespace {
namespace layout {
constexpr float ENEMY_FIRE_START_Y_OFFSET = 40.0f;
constexpr float BOSS_TOP_Y = 520.0f;
constexpr int PLAYER_BLINK_ACTION_TAG = 1001;
constexpr float PLAYER_HEALTH_BAR_OFFSET_Y = -52.0f;
constexpr float BOSS_HEALTH_BAR_OFFSET_Y = 78.0f;
}

constexpr char PLAYER_IMAGE[] = "plane.png";
constexpr char BULLET_IMAGE[] = "bullet.png";
constexpr char ENEMY_IMAGES[][11] = {
    "enemy0.png",
    "enemy1.png",
    "enemy2.png",
    "enemy3.png"
};
constexpr char BOSS_IMAGE[] = "enemy3.png";
constexpr char LEVEL_CONFIG_FILE[] = "level_config.ini";
constexpr char LEGACY_ENEMY_SPAWN_CONFIG_FILE[] = "enemy_spawn.ini";
constexpr int BOSS_SCORE_REWARD = 1000;

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

void transitionToVictoryScene(int finalScore, int levelId)
{
    auto scene = VictoryScene::createScene(finalScore, levelId);
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

Scene* FlyScene::createScene(int levelId)
{
    return FlyScene::create(levelId);
}

FlyScene* FlyScene::create(int levelId)
{
    auto* ret = new (std::nothrow) FlyScene();
    if (ret != nullptr && ret->initWithLevel(levelId)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool FlyScene::init()
{
    return initWithLevel(1);
}

bool FlyScene::initWithLevel(int levelId)
{
    if (!Scene::init()) {
        return false;
    }

    _keyBindings = KeyBindingConfig::load();
    _levelId = std::max(1, levelId);
    _enemySpawnInterval = 1.0f;
    _enemySpawnCount = 1;
    _playerFireInterval = 0.2f;
    _playerInvincibleTime = 1.0f;
    _playerUltimateEnergyMax = 100.0f;
    _playerUltimateEnergyCost = 100.0f;
    _playerUltimateCooldown = 8.0f;
    _playerUltimateHitEnergyGain = 20.0f;
    _playerUltimateDamage = 3;
    _enemySpeed = 100.0f;
    _enemyFireInterval = 1.2f;
    _playerBulletSpeed = 600.0f;
    _enemyBulletSpeed = 320.0f;
    _playerBulletDamage = 1;
    _enemyBulletDamage = 1;
    _bossBulletDamage[0] = 1;
    _bossBulletDamage[1] = 1;
    _bossBulletDamage[2] = 1;
    _bossSpawnScore = 1000;
    _bossMaxHealth = 30;
    _bossPhaseSpeedX[0] = 90.0f;
    _bossPhaseSpeedX[1] = 140.0f;
    _bossPhaseSpeedX[2] = 200.0f;
    _bossPhaseFireInterval[0] = 0.9f;
    _bossPhaseFireInterval[1] = 0.55f;
    _bossPhaseFireInterval[2] = 0.35f;
    _bossPhaseBulletSpeed[0] = 340.0f;
    _bossPhaseBulletSpeed[1] = 380.0f;
    _bossPhaseBulletSpeed[2] = 430.0f;

    loadEnemySpawnConfig();
    initBackground();
    initPlayer();
    initHUD();
    initInput();

    scheduleUpdate();

    return true;
}

void FlyScene::loadEnemySpawnConfig()
{
    std::string configText = FileUtils::getInstance()->getStringFromFile(LEVEL_CONFIG_FILE);
    const bool usingLegacyConfig = configText.empty();
    if (usingLegacyConfig) {
        configText = FileUtils::getInstance()->getStringFromFile(LEGACY_ENEMY_SPAWN_CONFIG_FILE);
    }
    if (configText.empty()) {
        CCLOG("FlyScene: level config not found, using defaults.");
        return;
    }

    const std::string targetSection = "level" + std::to_string(_levelId);
    const auto applyConfigValue = [this](const std::string& section, const std::string& key, const std::string& value) {
        try {
            if (section == "player") {
                if (key == "player_health") {
                    _health = std::max(1, std::stoi(value));
                } else if (key == "player_fire_interval") {
                    _playerFireInterval = std::max(0.05f, std::stof(value));
                } else if (key == "player_invincible_time") {
                    _playerInvincibleTime = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_energy_max") {
                    _playerUltimateEnergyMax = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_energy_cost") {
                    _playerUltimateEnergyCost = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_cooldown") {
                    _playerUltimateCooldown = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_hit_energy_gain") {
                    _playerUltimateHitEnergyGain = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_damage") {
                    _playerUltimateDamage = std::max(1, std::stoi(value));
                } else if (key == "player_bullet_speed") {
                    _playerBulletSpeed = std::max(50.0f, std::stof(value));
                }
            } else if (section == "enemy") {
                if (key == "spawn_interval") {
                    _enemySpawnInterval = std::max(0.1f, std::stof(value));
                } else if (key == "spawn_count") {
                    _enemySpawnCount = std::max(1, std::stoi(value));
                } else if (key == "enemy_speed") {
                    _enemySpeed = std::max(10.0f, std::stof(value));
                } else if (key == "enemy_fire_interval") {
                    _enemyFireInterval = std::max(0.1f, std::stof(value));
                } else if (key == "enemy_bullet_speed") {
                    _enemyBulletSpeed = std::max(50.0f, std::stof(value));
                }
            } else if (section == "boss") {
                if (key == "boss_spawn_score") {
                    _bossSpawnScore = std::max(0, std::stoi(value));
                } else if (key == "boss_health") {
                    _bossMaxHealth = std::max(1, std::stoi(value));
                } else if (key == "boss_phase1_speed_x") {
                    _bossPhaseSpeedX[0] = std::max(10.0f, std::stof(value));
                } else if (key == "boss_phase2_speed_x") {
                    _bossPhaseSpeedX[1] = std::max(10.0f, std::stof(value));
                } else if (key == "boss_phase3_speed_x") {
                    _bossPhaseSpeedX[2] = std::max(10.0f, std::stof(value));
                } else if (key == "boss_phase1_fire_interval") {
                    _bossPhaseFireInterval[0] = std::max(0.05f, std::stof(value));
                } else if (key == "boss_phase2_fire_interval") {
                    _bossPhaseFireInterval[1] = std::max(0.05f, std::stof(value));
                } else if (key == "boss_phase3_fire_interval") {
                    _bossPhaseFireInterval[2] = std::max(0.05f, std::stof(value));
                } else if (key == "boss_phase1_bullet_speed") {
                    _bossPhaseBulletSpeed[0] = std::max(50.0f, std::stof(value));
                } else if (key == "boss_phase2_bullet_speed") {
                    _bossPhaseBulletSpeed[1] = std::max(50.0f, std::stof(value));
                } else if (key == "boss_phase3_bullet_speed") {
                    _bossPhaseBulletSpeed[2] = std::max(50.0f, std::stof(value));
                }
            } else if (section == "default" || section.empty()) {
                if (key == "spawn_interval") {
                    _enemySpawnInterval = std::max(0.1f, std::stof(value));
                } else if (key == "spawn_count") {
                    _enemySpawnCount = std::max(1, std::stoi(value));
                } else if (key == "player_health") {
                    _health = std::max(1, std::stoi(value));
                } else if (key == "player_fire_interval") {
                    _playerFireInterval = std::max(0.05f, std::stof(value));
                } else if (key == "player_invincible_time") {
                    _playerInvincibleTime = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_energy_max") {
                    _playerUltimateEnergyMax = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_energy_cost") {
                    _playerUltimateEnergyCost = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_cooldown") {
                    _playerUltimateCooldown = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_hit_energy_gain") {
                    _playerUltimateHitEnergyGain = std::max(0.0f, std::stof(value));
                } else if (key == "player_ultimate_damage") {
                    _playerUltimateDamage = std::max(1, std::stoi(value));
                } else if (key == "enemy_speed") {
                    _enemySpeed = std::max(10.0f, std::stof(value));
                } else if (key == "enemy_fire_interval") {
                    _enemyFireInterval = std::max(0.1f, std::stof(value));
                } else if (key == "player_bullet_speed") {
                    _playerBulletSpeed = std::max(50.0f, std::stof(value));
                } else if (key == "player_bullet_damage") {
                    _playerBulletDamage = std::max(0, std::stoi(value));
                } else if (key == "enemy_bullet_speed") {
                    _enemyBulletSpeed = std::max(50.0f, std::stof(value));
                } else if (key == "enemy_bullet_damage") {
                    _enemyBulletDamage = std::max(0, std::stoi(value));
                } else if (key == "boss_spawn_score") {
                    _bossSpawnScore = std::max(0, std::stoi(value));
                } else if (key == "boss_health") {
                    _bossMaxHealth = std::max(1, std::stoi(value));
                } else if (key == "boss_phase1_speed_x") {
                    _bossPhaseSpeedX[0] = std::max(10.0f, std::stof(value));
                } else if (key == "boss_phase2_speed_x") {
                    _bossPhaseSpeedX[1] = std::max(10.0f, std::stof(value));
                } else if (key == "boss_phase3_speed_x") {
                    _bossPhaseSpeedX[2] = std::max(10.0f, std::stof(value));
                } else if (key == "boss_phase1_fire_interval") {
                    _bossPhaseFireInterval[0] = std::max(0.05f, std::stof(value));
                } else if (key == "boss_phase2_fire_interval") {
                    _bossPhaseFireInterval[1] = std::max(0.05f, std::stof(value));
                } else if (key == "boss_phase3_fire_interval") {
                    _bossPhaseFireInterval[2] = std::max(0.05f, std::stof(value));
                } else if (key == "boss_phase1_bullet_speed") {
                    _bossPhaseBulletSpeed[0] = std::max(50.0f, std::stof(value));
                } else if (key == "boss_phase2_bullet_speed") {
                    _bossPhaseBulletSpeed[1] = std::max(50.0f, std::stof(value));
                } else if (key == "boss_phase3_bullet_speed") {
                    _bossPhaseBulletSpeed[2] = std::max(50.0f, std::stof(value));
                } else if (key == "boss_phase1_bullet_damage") {
                    _bossBulletDamage[0] = std::max(0, std::stoi(value));
                } else if (key == "boss_phase2_bullet_damage") {
                    _bossBulletDamage[1] = std::max(0, std::stoi(value));
                } else if (key == "boss_phase3_bullet_damage") {
                    _bossBulletDamage[2] = std::max(0, std::stoi(value));
                }
            }
        } catch (const std::exception&) {
            CCLOG("FlyScene: invalid enemy spawn config '%s=%s', using current value.",
                  key.c_str(), value.c_str());
        }
    };

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
        if (equalPos == std::string::npos) {
            continue;
        }

        const std::string key = trimString(trimmedLine.substr(0, equalPos));
        const std::string value = trimString(trimmedLine.substr(equalPos + 1));
        if (key.empty() || value.empty()) {
            continue;
        }

        if (currentSection.empty()) {
            applyConfigValue("default", key, value);
            continue;
        }

        if (currentSection == targetSection) {
            applyConfigValue(currentSection, key, value);
            continue;
        }

        applyConfigValue(currentSection, key, value);
    }
}

void FlyScene::initBackground()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto bgLayer = LayerColor::create(Color4B(8, 12, 28, 255));
    addChild(bgLayer, 0);

    auto stars = DrawNode::create();
    for (int i = 0; i < 50; ++i) {
        const float x = static_cast<float>(std::rand() % static_cast<int>(visibleSize.width));
        const float y = static_cast<float>(std::rand() % static_cast<int>(visibleSize.height));
        const float size = static_cast<float>((std::rand() % 3) + 1);
        stars->drawDot(Vec2(x, y), size, Color4F(1.0f, 1.0f, 1.0f, 0.8f));
    }
    addChild(stars, 0);
}

void FlyScene::initPlayer()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    static Vec2 playerPoints[] = {
        Vec2(0, -40),
        Vec2(-25, 30),
        Vec2(-10, 20),
        Vec2(0, 35),
        Vec2(10, 20),
        Vec2(25, 30)
    };

    _player = FlyObject::createPlayer(PLAYER_IMAGE);
    _player.setNormalDisplayColor(Color3B::WHITE);
    _player.setDamagedDisplayColor(Color3B(255, 120, 120));
    _player.configurePolygonAppearance(
        playerPoints,
        6,
        Color4F(0.0f, 0.8f, 0.0f, 1.0f),
        Color4F(0.0f, 1.0f, 0.0f, 1.0f),
        Color4F(0.9f, 0.3f, 0.3f, 1.0f),
        Color4F(1.0f, 0.6f, 0.6f, 1.0f),
        2.0f);
    _player.setVelocity(Vec2::ZERO);
    _player.setMaxHealth(_health);
    _player.setHealth(_health);
    _player.enableHealthBar(
        56.0f,
        8.0f,
        Vec2(0.0f, layout::PLAYER_HEALTH_BAR_OFFSET_Y),
        Color4F(0.0f, 0.0f, 0.0f, 0.65f),
        Color4F(0.2f, 0.95f, 0.35f, 1.0f),
        Color4F(1.0f, 1.0f, 1.0f, 0.9f));
    _player.setUltimateEnergyMax(_playerUltimateEnergyMax);
    _player.setUltimateEnergyCost(_playerUltimateEnergyCost);
    _player.setUltimateCooldown(_playerUltimateCooldown);
    _player.setUltimateEnergy(_playerUltimateEnergyMax);
    _player.resetUltimateCooldown();
    _player.setPosition(Vec2(visibleSize.width / 2.0f, 100.0f));
    addChild(_player.getNode(), 10);
}

void FlyScene::updatePlayerAppearance(bool damaged)
{
    _player.applyDamageAppearance(damaged);
}

void FlyScene::updateUltimateHUD()
{
    if (_ultimateLabel == nullptr) {
        return;
    }

    const int energy = static_cast<int>(_player.getUltimateEnergy());
    const int energyMax = static_cast<int>(_player.getUltimateEnergyMax());
    std::string text = "Ultimate(J): " + std::to_string(energy) + "/" + std::to_string(energyMax);

    if (_player.canCastUltimate()) {
        text += " READY";
        _ultimateLabel->setColor(Color3B(255, 220, 80));
    } else if (_player.getUltimateCooldownTimer() > 0.0f) {
        text += " CD " + StringUtils::format("%.1f", _player.getUltimateCooldownTimer());
        _ultimateLabel->setColor(Color3B(180, 180, 180));
    } else {
        _ultimateLabel->setColor(Color3B(120, 220, 255));
    }

    _ultimateLabel->setString(text);
}

void FlyScene::updateBossHUD()
{
    if (_bossLabel == nullptr) {
        return;
    }

    if (_boss.getNode() == nullptr || _boss.getHealth() <= 0) {
        if (_bossDefeated) {
            _bossLabel->setString("Boss: defeated");
            _bossLabel->setColor(Color3B(140, 140, 140));
        } else {
            const int remainScore = std::max(0, _bossSpawnScore - _score);
            _bossLabel->setString("Boss in: " + std::to_string(remainScore));
            _bossLabel->setColor(Color3B(255, 220, 120));
        }
        return;
    }

    std::string phaseText = "P" + std::to_string(_bossPhase);
    _bossLabel->setString("Boss HP: " + std::to_string(_boss.getHealth()) + "/" +
                          std::to_string(_bossMaxHealth) + " " + phaseText);

    if (_bossPhase == 1) {
        _bossLabel->setColor(Color3B(255, 210, 160));
    } else if (_bossPhase == 2) {
        _bossLabel->setColor(Color3B(255, 150, 120));
    } else {
        _bossLabel->setColor(Color3B(255, 90, 90));
    }
}

void FlyScene::updateBossPhase()
{
    if (_boss.getNode() == nullptr || _boss.getHealth() <= 0 || _bossMaxHealth <= 0) {
        _bossPhase = 0;
        _bossLabel->setColor(Color3B(140, 140, 140));
        return;
    }

    const float healthRatio = static_cast<float>(_boss.getHealth()) / static_cast<float>(_bossMaxHealth);
    int nextPhase = 1;
    if (healthRatio <= 0.33f) {
        nextPhase = 3;
    } else if (healthRatio <= 0.66f) {
        nextPhase = 2;
    }

    if (_bossPhase != nextPhase) {
        _bossPhase = nextPhase;
    }

    switch (_bossPhase) {
    case 1:
        _boss.setFireInterval(_bossPhaseFireInterval[0]);
        break;
    case 2:
        _boss.setFireInterval(_bossPhaseFireInterval[1]);
        break;
    case 3:
    default:
        _boss.setFireInterval(_bossPhaseFireInterval[2]);
        break;
    }
}

void FlyScene::initHUD()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    _scoreLabel = Label::createWithSystemFont("Score: 0", "Arial", 24);
    _scoreLabel->setPosition(Vec2(80.0f, visibleSize.height - 30.0f));
    _scoreLabel->setColor(Color3B::WHITE);
    addChild(_scoreLabel, 20);

    _healthLabel = Label::createWithSystemFont("HP: 3", "Arial", 24);
    _healthLabel->setPosition(Vec2(visibleSize.width - 60.0f, visibleSize.height - 30.0f));
    _healthLabel->setColor(Color3B::RED);
    addChild(_healthLabel, 20);

    _ultimateLabel = Label::createWithSystemFont("", "Arial", 22);
    _ultimateLabel->setAnchorPoint(Vec2(0.0f, 0.5f));
    _ultimateLabel->setPosition(Vec2(20.0f, visibleSize.height - 60.0f));
    addChild(_ultimateLabel, 20);
    updateUltimateHUD();

    _bossLabel = Label::createWithSystemFont("", "Arial", 22);
    _bossLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
    _bossLabel->setPosition(Vec2(visibleSize.width / 2.0f, visibleSize.height - 30.0f));
    addChild(_bossLabel, 20);
    updateBossHUD();
}

void FlyScene::initInput()
{
    auto keyboardListener = EventListenerKeyboard::create();

    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (_gameOver) {
            return;
        }

        _pressedKeys.insert(static_cast<int>(keyCode));

        if (keyCode == _keyBindings.fire && _fireTimer >= _playerFireInterval) {
            firePlayerBullet();
            _fireTimer = 0.0f;
        } else if (keyCode == _keyBindings.ultimate) {
            castPlayerUltimate();
        }
    };

    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        _pressedKeys.erase(static_cast<int>(keyCode));
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);

    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (_gameOver || _player.getNode() == nullptr) {
            return false;
        }

        const Vec2 touchLocation = touch->getLocation();
        if (!isTouchOnPlayer(touchLocation)) {
            return false;
        }

        _isDraggingPlayer = true;
        _dragOffset = _player.getPosition() - touchLocation;
        return true;
    };

    touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        if (!_isDraggingPlayer || _player.getNode() == nullptr) {
            return;
        }

        setPlayerPositionClamped(touch->getLocation() + _dragOffset);
    };

    touchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        _isDraggingPlayer = false;
    };

    touchListener->onTouchCancelled = [this](Touch* touch, Event* event) {
        _isDraggingPlayer = false;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool FlyScene::isKeyPressed(EventKeyboard::KeyCode keyCode) const
{
    return _pressedKeys.find(static_cast<int>(keyCode)) != _pressedKeys.end();
}

bool FlyScene::isActionPressed(EventKeyboard::KeyCode primary,
                               EventKeyboard::KeyCode secondary) const
{
    return isKeyPressed(primary) || isKeyPressed(secondary);
}

bool FlyScene::isTouchOnPlayer(const Vec2& point) const
{
    if (_player.getNode() == nullptr) {
        return false;
    }

    return _player.getPosition().distance(point) <= 42.0f;
}

bool FlyScene::intersectsCircle(const Vec2& firstCenter, float firstRadius,
                                const Vec2& secondCenter, float secondRadius) const
{
    const float radiusSum = firstRadius + secondRadius;
    return firstCenter.distanceSquared(secondCenter) <= radiusSum * radiusSum;
}

void FlyScene::update(float delta)
{
    if (_gameOver) {
        return;
    }

    if (_invincibleTimer > 0.0f) {
        _invincibleTimer = std::max(0.0f, _invincibleTimer - delta);
        if (_invincibleTimer <= 0.0f) {
            updatePlayerAppearance(false);
        }
    }

    _player.updateUltimateCooldown(delta);

    auto visibleSize = Director::getInstance()->getVisibleSize();

    handlePlayerMovement(delta);

    _enemySpawnTimer += delta;
    if (_enemySpawnTimer >= _enemySpawnInterval) {
        for (int i = 0; i < _enemySpawnCount; ++i) {
            spawnEnemy();
        }
        _enemySpawnTimer = 0.0f;
    }

    if (!_bossSpawned && !_bossDefeated && _score >= _bossSpawnScore) {
        spawnBoss(_bossMaxHealth);
    }

    _fireTimer += delta;
    if (_fireTimer >= _playerFireInterval && isKeyPressed(_keyBindings.fire)) {
        firePlayerBullet();
        _fireTimer = 0.0f;
    }

    updateBullets(delta);
    updateEnemies(delta);
    removeOutOfBoundsObjects(visibleSize);

    checkCollisions();
    updateBossHUD();
    updateUltimateHUD();
}

void FlyScene::updateEnemies(float delta)
{
    const auto visibleSize = Director::getInstance()->getVisibleSize();

    for (auto& enemy : _enemies) {
        enemy.updatePosition(delta);

        if (enemy.getPosition().y > visibleSize.height - layout::ENEMY_FIRE_START_Y_OFFSET) {
            continue;
        }

        enemy.addFireTimer(delta);
        if (enemy.canFire()) {
            fireEnemyBullet(enemy.getPosition() + Vec2(0.0f, -30.0f));
            enemy.resetFireTimer();
        }
    }

    if (_boss.getNode() != nullptr && _boss.getHealth() > 0) {
        updateBossPhase();

        float moveSpeed = _bossPhaseSpeedX[0];
        if (_bossPhase == 2) {
            moveSpeed = _bossPhaseSpeedX[1];
        } else if (_bossPhase == 3) {
            moveSpeed = _bossPhaseSpeedX[2];
        }

        Vec2 bossVelocity(_bossMoveDirection * moveSpeed, 0.0f);
        _boss.setVelocity(bossVelocity);
        _boss.updatePosition(delta);

        Vec2 bossPosition = _boss.getPosition();
        const float minX = 80.0f;
        const float maxX = visibleSize.width - 80.0f;
        if (bossPosition.x <= minX) {
            bossPosition.x = minX;
            _bossMoveDirection = 1.0f;
        } else if (bossPosition.x >= maxX) {
            bossPosition.x = maxX;
            _bossMoveDirection = -1.0f;
        }
        bossPosition.y = std::min(bossPosition.y, layout::BOSS_TOP_Y);
        _boss.setPosition(bossPosition);

        _boss.addFireTimer(delta);
        if (_boss.canFire()) {
            const Vec2 bossMuzzle = _boss.getPosition() + Vec2(0.0f, -45.0f);
            Vec2 forward = _player.getPosition() - bossMuzzle;
            if (forward.isZero()) {
                forward = Vec2(0.0f, -1.0f);
            } else {
                forward.normalize();
            }

            if (_bossPhase == 1) {
                fireEnemyBullet(bossMuzzle, forward, _bossPhaseBulletSpeed[0], _bossBulletDamage[0]);
            } else if (_bossPhase == 2) {
                fireEnemyBullet(bossMuzzle, forward, _bossPhaseBulletSpeed[1], _bossBulletDamage[1]);

                Vec2 left = forward.getRPerp();
                Vec2 right = forward.getPerp();
                fireEnemyBullet(bossMuzzle + Vec2(-18.0f, 0.0f), (forward + left * 0.35f).getNormalized(), _bossPhaseBulletSpeed[1], _bossBulletDamage[1]);
                fireEnemyBullet(bossMuzzle + Vec2(18.0f, 0.0f), (forward + right * 0.35f).getNormalized(), _bossPhaseBulletSpeed[1], _bossBulletDamage[1]);
            } else {
                fireEnemyBullet(bossMuzzle, forward, _bossPhaseBulletSpeed[2], _bossBulletDamage[2]);

                for (float angle : { -0.6f, -0.3f, 0.3f, 0.6f }) {
                    Vec2 spread(
                        forward.x * std::cos(angle) - forward.y * std::sin(angle),
                        forward.x * std::sin(angle) + forward.y * std::cos(angle));
                    fireEnemyBullet(bossMuzzle, spread.getNormalized(), _bossPhaseBulletSpeed[2], _bossBulletDamage[2]);
                }
            }
            _boss.resetFireTimer();
        }
    }
}

void FlyScene::updateBullets(float delta)
{
    for (auto& bullet : _bullets) {
        bullet.updatePosition(delta);
    }
}

void FlyScene::removeOutOfBoundsObjects(const Size& visibleSize)
{
    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(),
        [visibleSize](const Bullet& bullet) {
            const auto position = bullet.getPosition();
            const bool outOfBounds = position.y > visibleSize.height + 50.0f ||
                                     position.y < -50.0f ||
                                     position.x > visibleSize.width + 50.0f ||
                                     position.x < -50.0f;
            if (outOfBounds) {
                if (bullet.getNode() != nullptr) {
                    bullet.getNode()->removeFromParent();
                }
            }
            return outOfBounds;
        }),
        _bullets.end());

    _enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(),
        [](const FlyObject& enemy) {
            const auto position = enemy.getPosition();
            const bool outOfBounds = position.y < -50.0f ||
                                     position.x < -80.0f ||
                                     position.x > Director::getInstance()->getVisibleSize().width + 80.0f;
            if (outOfBounds && enemy.getNode() != nullptr) {
                enemy.getNode()->removeFromParent();
            }
            return outOfBounds;
        }),
        _enemies.end());

    if (_boss.getNode() != nullptr) {
        const auto position = _boss.getPosition();
        const bool outOfBounds = position.y < -100.0f ||
                                 position.x < -120.0f ||
                                 position.x > visibleSize.width + 120.0f;
        if (outOfBounds) {
            _boss.getNode()->removeFromParent();
            _boss = FlyObject(false);
            _bossSpawned = false;
            updateBossHUD();
        }
    }
}

void FlyScene::handlePlayerMovement(float delta)
{
    if (_player.getNode() == nullptr) {
        return;
    }

    const float moveSpeed = 260.0f * delta;

    if (isActionPressed(_keyBindings.moveLeftPrimary, _keyBindings.moveLeftSecondary)) {
        setPlayerPositionClamped(_player.getPosition() + Vec2(-moveSpeed, 0.0f));
    }
    if (isActionPressed(_keyBindings.moveRightPrimary, _keyBindings.moveRightSecondary)) {
        setPlayerPositionClamped(_player.getPosition() + Vec2(moveSpeed, 0.0f));
    }
    if (isActionPressed(_keyBindings.moveUpPrimary, _keyBindings.moveUpSecondary)) {
        setPlayerPositionClamped(_player.getPosition() + Vec2(0.0f, moveSpeed));
    }
    if (isActionPressed(_keyBindings.moveDownPrimary, _keyBindings.moveDownSecondary)) {
        setPlayerPositionClamped(_player.getPosition() + Vec2(0.0f, -moveSpeed));
    }
}

void FlyScene::setPlayerPositionClamped(const Vec2& position)
{
    if (_player.getNode() == nullptr) {
        return;
    }

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const float x = clampf(position.x, 30.0f, visibleSize.width - 30.0f);
    const float y = clampf(position.y, 40.0f, visibleSize.height - 40.0f);
    _player.setPosition(Vec2(x, y));
}

void FlyScene::spawnEnemy()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    const int imageIndex = std::rand() % 4;
    FlyObject enemy = FlyObject::createEnemy(ENEMY_IMAGES[imageIndex], 2.0f);
    const float x = static_cast<float>((std::rand() % static_cast<int>(visibleSize.width - 100.0f)) + 50);
    enemy.setPosition(Vec2(x, visibleSize.height + 50.0f));
    enemy.setVelocity(Vec2(0.0f, -_enemySpeed - static_cast<float>(std::rand() % 100)));
    enemy.setFireInterval(_enemyFireInterval);
    enemy.resetFireTimer();

    addChild(enemy.getNode(), 5);
    _enemies.push_back(enemy);
}

void FlyScene::spawnBoss(int health)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    FlyObject boss = FlyObject::createEnemy(BOSS_IMAGE, 3.5f);
    boss.setPosition(Vec2(visibleSize.width / 2.0f, std::min(layout::BOSS_TOP_Y, visibleSize.height - 100.0f)));
    boss.setVelocity(Vec2(_bossPhaseSpeedX[0], 0.0f));
    boss.setFireInterval(_bossPhaseFireInterval[0]);
    boss.resetFireTimer();
    boss.setMaxHealth(health);
    boss.setHealth(health);
    boss.setCollisionRadius(48.0f);
    boss.setNormalDisplayColor(Color3B(255, 220, 220));
    boss.setDamagedDisplayColor(Color3B(255, 90, 90));
    boss.enableHealthBar(
        110.0f,
        10.0f,
        Vec2(0.0f, layout::BOSS_HEALTH_BAR_OFFSET_Y),
        Color4F(0.15f, 0.0f, 0.0f, 0.7f),
        Color4F(0.95f, 0.2f, 0.2f, 1.0f),
        Color4F(1.0f, 0.85f, 0.85f, 1.0f));

    _boss = boss;
    _bossMaxHealth = health;
    _bossPhase = 1;
    _bossMoveDirection = 1.0f;
    _bossSpawned = true;
    _bossDefeated = false;
    addChild(_boss.getNode(), 6);
    updateBossHUD();
}

void FlyScene::firePlayerBullet()
{
    if (_player.getNode() == nullptr) {
        return;
    }

    Bullet bullet;
    bullet = Bullet::createBullet(
        true,
        BulletVisualType::Diamond,
        BULLET_IMAGE,
        0.7f,
        Color3B(255, 240, 120),
        5.0f,
        Color4F(1.0f, 1.0f, 0.0f, 1.0f),
        _playerBulletDamage);

    const Vec2 playerPos = _player.getPosition();
    bullet.setPosition(Vec2(playerPos.x, playerPos.y + 35.0f));
    bullet.setVelocity(Vec2(0.0f, _playerBulletSpeed));

    addChild(bullet.getNode(), 8);
    _bullets.push_back(bullet);
}

void FlyScene::fireEnemyBullet(const Vec2& startPosition)
{
    if (_player.getNode() == nullptr) {
        return;
    }

    Bullet bullet;
    bullet = Bullet::createBullet(
        false,
        BulletVisualType::Laser,
        BULLET_IMAGE,
        0.7f,
        Color3B(255, 120, 80),
        5.0f,
        Color4F(1.0f, 0.4f, 0.2f, 1.0f),
        _enemyBulletDamage);
    bullet.setPosition(startPosition);

    Vec2 direction = _player.getPosition() - startPosition;
    if (direction.isZero()) {
        direction = Vec2(0.0f, -1.0f);
    } else {
        direction.normalize();
    }
    bullet.setVelocity(direction * _enemyBulletSpeed);

    addChild(bullet.getNode(), 7);
    _bullets.push_back(bullet);
}

void FlyScene::fireEnemyBullet(const Vec2& startPosition, const Vec2& direction, float speed, int damage)
{
    Bullet bullet;
    bullet = Bullet::createBullet(
        false,
        BulletVisualType::Laser,
        BULLET_IMAGE,
        0.7f,
        Color3B(255, 120, 80),
        5.0f,
        Color4F(1.0f, 0.4f, 0.2f, 1.0f),
        damage);
    bullet.setPosition(startPosition);
    bullet.setVelocity(direction.getNormalized() * speed);

    addChild(bullet.getNode(), 7);
    _bullets.push_back(bullet);
}

void FlyScene::castPlayerUltimate()
{
    if (_player.getNode() == nullptr || !_player.consumeUltimateEnergy()) {
        return;
    }

    auto* flash = LayerColor::create(Color4B(255, 240, 180, 120));
    flash->setContentSize(Director::getInstance()->getVisibleSize());
    flash->setOpacity(0);
    addChild(flash, 50);
    flash->runAction(Sequence::create(
        FadeTo::create(0.08f, 180),
        FadeOut::create(0.2f),
        RemoveSelf::create(),
        nullptr));

    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(),
        [](const Bullet& bullet) {
            if (!bullet.isPlayerOwned()) {
                if (bullet.getNode() != nullptr) {
                    bullet.getNode()->removeFromParent();
                }
                return true;
            }
            return false;
        }),
        _bullets.end());

    for (auto& enemy : _enemies) {
        enemy.applyDamage(_playerUltimateDamage);
    }

    if (_boss.getNode() != nullptr && _boss.getHealth() > 0) {
        _boss.applyDamage(_playerUltimateDamage);
        updateBossPhase();
    }

    _enemies.erase(std::remove_if(_enemies.begin(), _enemies.end(),
        [this](const FlyObject& enemy) {
            if (enemy.getHealth() <= 0) {
                if (enemy.getNode() != nullptr) {
                    enemy.getNode()->removeFromParent();
                }
                _score += 100;
                return true;
            }
            return false;
        }),
        _enemies.end());

    if (_boss.getNode() != nullptr && _boss.getHealth() <= 0) {
        _boss.getNode()->removeFromParent();
        _boss = FlyObject(false);
        _bossSpawned = false;
        _bossDefeated = true;
        _bossPhase = 0;
        _score += BOSS_SCORE_REWARD;
        _scoreLabel->setString("Score: " + std::to_string(_score));
        updateUltimateHUD();
        updateBossHUD();
        transitionToVictoryScene(_score, _levelId);
        return;
    }

    _scoreLabel->setString("Score: " + std::to_string(_score));
    updateUltimateHUD();
    updateBossHUD();
}

void FlyScene::checkCollisions()
{
    for (auto bulletIt = _bullets.begin(); bulletIt != _bullets.end();) {
        bool bulletRemoved = false;

        if (bulletIt->isPlayerOwned()) {
            for (auto enemyIt = _enemies.begin(); enemyIt != _enemies.end();) {
                if (intersectsCircle(bulletIt->getPosition(), 8.0f,
                                     enemyIt->getPosition(), enemyIt->getCollisionRadius())) {
                    enemyIt->applyDamage(bulletIt->getDamage());
                    _player.addUltimateEnergy(_playerUltimateHitEnergyGain);
                    bulletIt->getNode()->removeFromParent();
                    bulletIt = _bullets.erase(bulletIt);
                    bulletRemoved = true;

                    if (enemyIt->getHealth() <= 0) {
                        if (enemyIt->getNode() != nullptr) {
                            enemyIt->getNode()->removeFromParent();
                        }
                        enemyIt = _enemies.erase(enemyIt);
                        _score += 100;
                        _scoreLabel->setString("Score: " + std::to_string(_score));
                    } else {
                        ++enemyIt;
                    }
                    break;
                }

                ++enemyIt;
            }

            if (!bulletRemoved &&
                _boss.getNode() != nullptr &&
                _boss.getHealth() > 0 &&
                intersectsCircle(bulletIt->getPosition(), 8.0f,
                                 _boss.getPosition(), _boss.getCollisionRadius())) {
                _boss.applyDamage(bulletIt->getDamage());
                _player.addUltimateEnergy(_playerUltimateHitEnergyGain);
                updateBossPhase();
                bulletIt->getNode()->removeFromParent();
                bulletIt = _bullets.erase(bulletIt);
                bulletRemoved = true;

                if (_boss.getHealth() <= 0) {
                    _boss.getNode()->removeFromParent();
                    _boss = FlyObject(false);
                    _bossSpawned = false;
                    _bossDefeated = true;
                    _bossPhase = 0;
                    _score += BOSS_SCORE_REWARD;
                    _scoreLabel->setString("Score: " + std::to_string(_score));
                    updateBossHUD();
                    updateUltimateHUD();
                    transitionToVictoryScene(_score, _levelId);
                    return;
                }

                updateBossHUD();
            }
        } else if (_player.getNode() != nullptr &&
                   _invincibleTimer <= 0.0f &&
                   intersectsCircle(bulletIt->getPosition(), 8.0f,
                                    _player.getPosition(), _player.getCollisionRadius())) {
            const int damage = bulletIt->getDamage();
            bulletIt->getNode()->removeFromParent();
            bulletIt = _bullets.erase(bulletIt);
            bulletRemoved = true;
            damagePlayer(damage);
        }

        if (!bulletRemoved) {
            ++bulletIt;
        }
    }

    if (_player.getNode() == nullptr) {
        return;
    }

    for (auto enemyIt = _enemies.begin(); enemyIt != _enemies.end();) {
        if (_invincibleTimer <= 0.0f &&
            intersectsCircle(_player.getPosition(), _player.getCollisionRadius(),
                             enemyIt->getPosition(), enemyIt->getCollisionRadius())) {
            damagePlayer(1);
            if (_gameOver) {
                break;
            }
            ++enemyIt;
        } else {
            ++enemyIt;
        }
    }

    if (_player.getNode() != nullptr &&
        _boss.getNode() != nullptr &&
        _boss.getHealth() > 0 &&
        _invincibleTimer <= 0.0f &&
        intersectsCircle(_player.getPosition(), _player.getCollisionRadius(),
                         _boss.getPosition(), _boss.getCollisionRadius())) {
        damagePlayer(1);
    }
}

void FlyScene::damagePlayer(int damage)
{
    _health -= damage;
    _player.setHealth(_health);
    _invincibleTimer = _playerInvincibleTime;
    _healthLabel->setString("HP: " + std::to_string(_health));
    updatePlayerAppearance(true);

    auto* playerNode = _player.getNode();
    if (playerNode != nullptr) {
        _player.playBlink(_playerInvincibleTime, 5, layout::PLAYER_BLINK_ACTION_TAG);
    }

    if (_health <= 0) {
        gameOver();
    }
}

void FlyScene::gameOver()
{
    _gameOver = true;
    _pressedKeys.clear();
    _isDraggingPlayer = false;

    auto* playerNode = _player.getNode();
    if (playerNode != nullptr) {
        _player.stopActionByTag(layout::PLAYER_BLINK_ACTION_TAG);
        updatePlayerAppearance(false);
        _player.setVisible(true);
    }

    auto scene = GameOverScene::createScene(_score, _levelId);
    Director::getInstance()->replaceScene(TransitionFade::create(0.3f, scene));
}

void FlyScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
