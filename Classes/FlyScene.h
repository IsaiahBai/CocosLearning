#ifndef __FLYSCENE_SCENE_H__
#define __FLYSCENE_SCENE_H__

#include "Bullet.h"
#include "FlyObject.h"
#include "KeyBindingConfig.h"
#include "cocos2d.h"

#include <unordered_set>
#include <vector>

class FlyScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int levelId = 1);
    static FlyScene* create(int levelId);

    bool init() override;
    bool initWithLevel(int levelId);

    void menuCloseCallback(cocos2d::Ref* pSender);

private:
    void initBackground();
    void initPlayer();
    void initHUD();
    void initInput();
    void loadEnemySpawnConfig();
    void update(float delta) override;
    void handlePlayerMovement(float delta);
    void setPlayerPositionClamped(const cocos2d::Vec2& position);
    void updatePlayerAppearance(bool damaged);
    void updateUltimateHUD();
    void updateEnemies(float delta);
    void updateBullets(float delta);
    void removeOutOfBoundsObjects(const cocos2d::Size& visibleSize);
    void updateBossHUD();
    void updateBossPhase();
    bool isTouchOnPlayer(const cocos2d::Vec2& point) const;
    bool intersectsCircle(const cocos2d::Vec2& firstCenter, float firstRadius,
                          const cocos2d::Vec2& secondCenter, float secondRadius) const;
    bool isActionPressed(cocos2d::EventKeyboard::KeyCode primary,
                         cocos2d::EventKeyboard::KeyCode secondary) const;
    void spawnEnemy();
    void spawnBoss(int health);
    void firePlayerBullet();
    void fireEnemyBullet(const cocos2d::Vec2& startPosition);
    void fireEnemyBullet(const cocos2d::Vec2& startPosition, const cocos2d::Vec2& direction, float speed, int damage = 1);
    void castPlayerUltimate();
    void checkCollisions();
    void damagePlayer(int damage);
    void gameOver();
    bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode) const;

    int _levelId = 1;
    FlyObject _player = FlyObject(true);
    FlyObject _boss = FlyObject(false);
    cocos2d::Label* _scoreLabel = nullptr;
    cocos2d::Label* _healthLabel = nullptr;
    cocos2d::Label* _ultimateLabel = nullptr;
    cocos2d::Label* _bossLabel = nullptr;

    std::vector<Bullet> _bullets;
    std::vector<FlyObject> _enemies;
    std::unordered_set<int> _pressedKeys;
    KeyBindingConfig::Bindings _keyBindings = KeyBindingConfig::load();

    int _score = 0;
    int _health = 3;
    int _bossMaxHealth = 0;
    int _bossPhase = 0;
    int _enemySpawnCount = 1;
    int _playerUltimateDamage = 3;
    int _bossSpawnScore = 1000;
    float _enemySpeed = 100.0f;
    float _enemyFireInterval = 1.2f;
    float _playerBulletSpeed = 600.0f;
    float _enemyBulletSpeed = 320.0f;
    int _playerBulletDamage = 1;
    int _enemyBulletDamage = 1;
    int _bossBulletDamage[3] = { 1, 1, 1 };
    float _enemySpawnTimer = 0.0f;
    float _enemySpawnInterval = 1.0f;
    float _playerFireInterval = 0.2f;
    float _playerInvincibleTime = 1.0f;
    float _playerUltimateEnergyMax = 100.0f;
    float _playerUltimateEnergyCost = 100.0f;
    float _playerUltimateCooldown = 8.0f;
    float _playerUltimateHitEnergyGain = 20.0f;
    float _fireTimer = 0.0f;
    float _invincibleTimer = 0.0f;
    float _bossMoveDirection = 1.0f;
    float _bossPhaseSpeedX[3] = { 90.0f, 140.0f, 200.0f };
    float _bossPhaseFireInterval[3] = { 0.9f, 0.55f, 0.35f };
    float _bossPhaseBulletSpeed[3] = { 340.0f, 380.0f, 430.0f };
    bool _bossSpawned = false;
    bool _bossDefeated = false;
    bool _gameOver = false;
    bool _isDraggingPlayer = false;
    cocos2d::Vec2 _dragOffset;
};

#endif // __FLYSCENE_SCENE_H__
