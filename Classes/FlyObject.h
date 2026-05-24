#ifndef __FLY_OBJECT_H__
#define __FLY_OBJECT_H__

#include "cocos2d.h"
#include <vector>

enum class BulletVisualType;

class FlyObject
{
public:
    explicit FlyObject(bool isPlayer = false);
    FlyObject(bool isPlayer, const std::string& imagePath);

    static FlyObject createPlayer(const std::string& imagePath, float visualScale = 1.0f);
    static FlyObject createEnemy(const std::string& imagePath, float visualScale = 1.0f);
    static cocos2d::Node* createBulletNode(BulletVisualType visualType,
                                           const std::string& imagePath,
                                           float scale,
                                           const cocos2d::Color3B& color,
                                           float fallbackRadius,
                                           const cocos2d::Color4F& fallbackColor);

    void setNode(cocos2d::Node* node);
    cocos2d::Node* getNode() const;
    cocos2d::DrawNode* getDrawNode() const;
    cocos2d::Sprite* getSprite() const;
    bool loadSprite(const std::string& imagePath);
    bool createPolygon(const cocos2d::Vec2* points, int count,
                       const cocos2d::Color4F& fillColor,
                       float borderWidth,
                       const cocos2d::Color4F& borderColor);
    void configurePolygonAppearance(const cocos2d::Vec2* points, int count,
                                    const cocos2d::Color4F& normalFillColor,
                                    const cocos2d::Color4F& normalBorderColor,
                                    const cocos2d::Color4F& damagedFillColor,
                                    const cocos2d::Color4F& damagedBorderColor,
                                    float borderWidth);
    void updatePolygonAppearance(const cocos2d::Vec2* points, int count,
                                 const cocos2d::Color4F& fillColor,
                                 float borderWidth,
                                 const cocos2d::Color4F& borderColor);
    void setSpriteScale(float scale);
    float getSpriteScale() const;
    void setVisualScale(float scale);
    float getVisualScale() const;
    void setDisplayColor(const cocos2d::Color3B& color);
    cocos2d::Color3B getDisplayColor() const;
    void setNormalDisplayColor(const cocos2d::Color3B& color);
    const cocos2d::Color3B& getNormalDisplayColor() const;
    void setDamagedDisplayColor(const cocos2d::Color3B& color);
    const cocos2d::Color3B& getDamagedDisplayColor() const;
    void applyDamageAppearance(bool damaged);

    void setPosition(const cocos2d::Vec2& position);
    cocos2d::Vec2 getPosition() const;
    bool hasNode() const;
    void removeFromParent();
    void setVisible(bool visible);
    bool isVisible() const;
    void stopActionByTag(int actionTag);
    void runAction(cocos2d::FiniteTimeAction* action);
    void playBlink(float duration, unsigned int blinks, int actionTag);

    void setVelocity(const cocos2d::Vec2& velocity);
    const cocos2d::Vec2& getVelocity() const;
    void updatePosition(float delta);

    bool isPlayerObject() const;

    void setMaxHealth(int maxHealth);
    int getMaxHealth() const;
    void setHealth(int health);
    int getHealth() const;
    void applyDamage(int damage);
    void enableHealthBar(float width,
                         float height,
                         const cocos2d::Vec2& offset,
                         const cocos2d::Color4F& backgroundColor = cocos2d::Color4F(0.0f, 0.0f, 0.0f, 0.6f),
                         const cocos2d::Color4F& fillColor = cocos2d::Color4F(0.2f, 0.9f, 0.2f, 1.0f),
                         const cocos2d::Color4F& borderColor = cocos2d::Color4F::WHITE);
    void disableHealthBar();
    void updateHealthBar();
    void setCollisionRadius(float collisionRadius);
    float getCollisionRadius() const;

    void setFireTimer(float fireTimer);
    float getFireTimer() const;
    void addFireTimer(float delta);
    void setFireInterval(float fireInterval);
    float getFireInterval() const;
    bool canFire() const;
    void resetFireTimer();

    void setUltimateEnergy(float ultimateEnergy);
    float getUltimateEnergy() const;
    void addUltimateEnergy(float delta);
    void setUltimateEnergyMax(float ultimateEnergyMax);
    float getUltimateEnergyMax() const;
    void setUltimateEnergyCost(float ultimateEnergyCost);
    float getUltimateEnergyCost() const;
    bool canCastUltimate() const;
    bool consumeUltimateEnergy();
    void setUltimateCooldown(float ultimateCooldown);
    float getUltimateCooldown() const;
    float getUltimateCooldownTimer() const;
    void updateUltimateCooldown(float delta);
    void resetUltimateCooldown();

private:
    cocos2d::Node* _node = nullptr;
    cocos2d::DrawNode* _healthBarBackground = nullptr;
    cocos2d::DrawNode* _healthBarFill = nullptr;
    cocos2d::Label* _healthBarPercentLabel = nullptr;
    cocos2d::Vec2 _velocity;
    bool _isPlayer = false;
    int _health = 1;
    int _maxHealth = 1;
    bool _healthBarEnabled = false;
    float _healthBarWidth = 0.0f;
    float _healthBarHeight = 0.0f;
    cocos2d::Vec2 _healthBarOffset;
    cocos2d::Color4F _healthBarBackgroundColor = cocos2d::Color4F(0.0f, 0.0f, 0.0f, 0.6f);
    cocos2d::Color4F _healthBarFillColor = cocos2d::Color4F(0.2f, 0.9f, 0.2f, 1.0f);
    cocos2d::Color4F _healthBarBorderColor = cocos2d::Color4F::WHITE;
    float _collisionRadius = 20.0f;
    float _fireTimer = 0.0f;
    float _fireInterval = 0.0f;
    float _ultimateEnergy = 0.0f;
    float _ultimateEnergyMax = 100.0f;
    float _ultimateEnergyCost = 100.0f;
    float _ultimateCooldown = 8.0f;
    float _ultimateCooldownTimer = 0.0f;
    cocos2d::Color3B _normalDisplayColor = cocos2d::Color3B::WHITE;
    cocos2d::Color3B _damagedDisplayColor = cocos2d::Color3B(255, 120, 120);
    std::vector<cocos2d::Vec2> _polygonPoints;
    cocos2d::Color4F _normalFillColor;
    cocos2d::Color4F _normalBorderColor;
    cocos2d::Color4F _damagedFillColor;
    cocos2d::Color4F _damagedBorderColor;
    float _polygonBorderWidth = 0.0f;
};

#endif // __FLY_OBJECT_H__
