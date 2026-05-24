#ifndef __BULLET_H__
#define __BULLET_H__

#include "cocos2d.h"

enum class BulletVisualType
{
    Sprite,
    Dot,
    Diamond,
    Triangle,
    Laser
};

class Bullet
{
public:
    explicit Bullet(bool isPlayerBullet = true);

    static Bullet createBullet(bool isPlayerBullet,
                               BulletVisualType visualType,
                               const std::string& imagePath,
                               float scale,
                               const cocos2d::Color3B& color,
                               float fallbackRadius,
                               const cocos2d::Color4F& fallbackColor,
                               int damage = 1);

    void setNode(cocos2d::Node* node);
    cocos2d::Node* getNode() const;

    void setPosition(const cocos2d::Vec2& position);
    cocos2d::Vec2 getPosition() const;

    void setVelocity(const cocos2d::Vec2& velocity);
    const cocos2d::Vec2& getVelocity() const;
    void updatePosition(float delta);

    void setPlayerBullet(bool isPlayerBullet);
    bool isPlayerOwned() const;
    void setDamage(int damage);
    int getDamage() const;

private:
    cocos2d::Node* _node = nullptr;
    cocos2d::Vec2 _velocity;
    bool _isPlayerBullet = true;
    int _damage = 1;
};

#endif // __BULLET_H__
