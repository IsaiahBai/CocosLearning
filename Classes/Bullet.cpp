#include "Bullet.h"

#include "FlyObject.h"

USING_NS_CC;

Bullet::Bullet(bool isPlayerBullet)
    : _isPlayerBullet(isPlayerBullet) {
}

Bullet Bullet::createBullet(bool isPlayerBullet,
                            BulletVisualType visualType,
                            const std::string& imagePath,
                            float scale,
                            const Color3B& color,
                            float fallbackRadius,
                            const Color4F& fallbackColor,
                            int damage)
{
    Bullet bullet(isPlayerBullet);
    bullet.setNode(FlyObject::createBulletNode(visualType, imagePath, scale, color, fallbackRadius, fallbackColor));
    bullet.setDamage(damage);
    return bullet;
}

void Bullet::setNode(Node* node)
{
    _node = node;
}

Node* Bullet::getNode() const
{
    return _node;
}

void Bullet::setPosition(const Vec2& position)
{
    if (_node != nullptr) {
        _node->setPosition(position);
    }
}

Vec2 Bullet::getPosition() const
{
    return _node != nullptr ? _node->getPosition() : Vec2::ZERO;
}

void Bullet::setVelocity(const Vec2& velocity)
{
    _velocity = velocity;
}

const Vec2& Bullet::getVelocity() const
{
    return _velocity;
}

void Bullet::updatePosition(float delta)
{
    if (_node != nullptr) {
        _node->setPosition(_node->getPosition() + _velocity * delta);
    }
}

void Bullet::setPlayerBullet(bool isPlayerBullet)
{
    _isPlayerBullet = isPlayerBullet;
}

bool Bullet::isPlayerOwned() const
{
    return _isPlayerBullet;
}

void Bullet::setDamage(int damage)
{
    _damage = std::max(0, damage);
}

int Bullet::getDamage() const
{
    return _damage;
}
