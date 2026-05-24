#include "FlyObject.h"
#include "Bullet.h"

#include <algorithm>
#include <cmath>

USING_NS_CC;

FlyObject::FlyObject(bool isPlayer)
    : _isPlayer(isPlayer)
    , _health(isPlayer ? 3 : 1)
    , _maxHealth(isPlayer ? 3 : 1) {
}

FlyObject::FlyObject(bool isPlayer, const std::string& imagePath)
    : FlyObject(isPlayer)
{
    loadSprite(imagePath);
}

FlyObject FlyObject::createPlayer(const std::string& imagePath, float visualScale)
{
    FlyObject player(true, imagePath);
    float baseScale = 1.0f;
    if (player.getNode() == nullptr) {
        static Vec2 playerPoints[] = {
            Vec2(0, -40),
            Vec2(-25, 30),
            Vec2(-10, 20),
            Vec2(0, 35),
            Vec2(10, 20),
            Vec2(25, 30)
        };
        player.createPolygon(
            playerPoints,
            6,
            Color4F(0.0f, 0.8f, 0.0f, 1.0f),
            2.0f,
            Color4F(0.0f, 1.0f, 0.0f, 1.0f));
    } else {
        baseScale = 0.7f;
    }

    player.setVisualScale(baseScale * visualScale);
    player.setCollisionRadius(24.0f * visualScale);
    return player;
}

FlyObject FlyObject::createEnemy(const std::string& imagePath, float visualScale)
{
    FlyObject enemy(false, imagePath);
    float baseScale = 1.0f;
    if (enemy.getNode() == nullptr) {
        static Vec2 enemyPoints[] = {
            Vec2(0, 30),
            Vec2(-20, -25),
            Vec2(0, -15),
            Vec2(20, -25)
        };
        enemy.createPolygon(
            enemyPoints,
            4,
            Color4F(0.8f, 0.2f, 0.2f, 1.0f),
            2.0f,
            Color4F(1.0f, 0.0f, 0.0f, 1.0f));
    } else {
        baseScale = 0.6f;
    }

    enemy.setVisualScale(baseScale * visualScale);
    enemy.setCollisionRadius(22.0f * visualScale);
    enemy.setNormalDisplayColor(Color3B::WHITE);
    enemy.setDamagedDisplayColor(Color3B(255, 150, 150));
    return enemy;
}

Node* FlyObject::createBulletNode(BulletVisualType visualType,
                                  const std::string& imagePath,
                                  float scale,
                                  const Color3B& color,
                                  float fallbackRadius,
                                  const Color4F& fallbackColor)
{
    if (visualType == BulletVisualType::Sprite) {
        auto bulletSprite = Sprite::create(imagePath);
        if (bulletSprite != nullptr) {
            bulletSprite->setScale(scale);
            bulletSprite->setColor(color);
            return bulletSprite;
        }
    }

    auto bulletShape = DrawNode::create();
    const float radius = fallbackRadius * scale;
    switch (visualType) {
    case BulletVisualType::Dot:
    case BulletVisualType::Sprite:
        bulletShape->drawDot(Vec2::ZERO, radius, fallbackColor);
        break;
    case BulletVisualType::Diamond: {
        Vec2 points[] = {
            Vec2(0.0f, radius * 1.6f),
            Vec2(radius, 0.0f),
            Vec2(0.0f, -radius * 1.6f),
            Vec2(-radius, 0.0f)
        };
        bulletShape->drawPolygon(points, 4, fallbackColor, 1.0f, fallbackColor);
        break;
    }
    case BulletVisualType::Triangle: {
        Vec2 points[] = {
            Vec2(0.0f, radius * 1.8f),
            Vec2(radius * 1.1f, -radius),
            Vec2(-radius * 1.1f, -radius)
        };
        bulletShape->drawPolygon(points, 3, fallbackColor, 1.0f, fallbackColor);
        break;
    }
    case BulletVisualType::Laser: {
        Vec2 points[] = {
            Vec2(-radius * 0.45f, radius * 2.6f),
            Vec2(radius * 0.45f, radius * 2.6f),
            Vec2(radius * 0.45f, -radius * 2.6f),
            Vec2(-radius * 0.45f, -radius * 2.6f)
        };
        bulletShape->drawPolygon(points, 4, fallbackColor, 1.0f, fallbackColor);
        break;
    }
    }
    return bulletShape;
}

void FlyObject::setNode(Node* node)
{
    _node = node;
    updateHealthBar();
}

Node* FlyObject::getNode() const
{
    return _node;
}

DrawNode* FlyObject::getDrawNode() const
{
    return dynamic_cast<DrawNode*>(_node);
}

Sprite* FlyObject::getSprite() const
{
    return dynamic_cast<Sprite*>(_node);
}

bool FlyObject::loadSprite(const std::string& imagePath)
{
    auto sprite = Sprite::create(imagePath);
    if (sprite == nullptr) {
        return false;
    }

    _node = sprite;
    updateHealthBar();
    return true;
}

bool FlyObject::createPolygon(const Vec2* points, int count,
                              const Color4F& fillColor,
                              float borderWidth,
                              const Color4F& borderColor)
{
    auto drawNode = DrawNode::create();
    if (drawNode == nullptr) {
        return false;
    }

    _node = drawNode;
    updatePolygonAppearance(points, count, fillColor, borderWidth, borderColor);
    updateHealthBar();
    return true;
}

void FlyObject::configurePolygonAppearance(const Vec2* points, int count,
                                           const Color4F& normalFillColor,
                                           const Color4F& normalBorderColor,
                                           const Color4F& damagedFillColor,
                                           const Color4F& damagedBorderColor,
                                           float borderWidth)
{
    _polygonPoints.assign(points, points + count);
    _normalFillColor = normalFillColor;
    _normalBorderColor = normalBorderColor;
    _damagedFillColor = damagedFillColor;
    _damagedBorderColor = damagedBorderColor;
    _polygonBorderWidth = borderWidth;

    auto* drawNode = getDrawNode();
    if (drawNode == nullptr) {
        createPolygon(points, count, normalFillColor, borderWidth, normalBorderColor);
    } else {
        updatePolygonAppearance(points, count, normalFillColor, borderWidth, normalBorderColor);
    }
}

void FlyObject::updatePolygonAppearance(const Vec2* points, int count,
                                        const Color4F& fillColor,
                                        float borderWidth,
                                        const Color4F& borderColor)
{
    auto* drawNode = getDrawNode();
    if (drawNode == nullptr) {
        return;
    }

    drawNode->clear();
    drawNode->drawPolygon(points, count, fillColor, borderWidth, borderColor);
}

void FlyObject::setSpriteScale(float scale)
{
    auto* sprite = getSprite();
    if (sprite != nullptr) {
        sprite->setScale(scale);
    }
}

float FlyObject::getSpriteScale() const
{
    auto* sprite = getSprite();
    return sprite != nullptr ? sprite->getScale() : 1.0f;
}

void FlyObject::setVisualScale(float scale)
{
    if (_node != nullptr) {
        _node->setScale(scale);
    }
}

float FlyObject::getVisualScale() const
{
    return _node != nullptr ? _node->getScale() : 1.0f;
}

void FlyObject::setDisplayColor(const Color3B& color)
{
    if (_node != nullptr) {
        _node->setColor(color);
    }
}

Color3B FlyObject::getDisplayColor() const
{
    return _node != nullptr ? _node->getColor() : Color3B::WHITE;
}

void FlyObject::setNormalDisplayColor(const Color3B& color)
{
    _normalDisplayColor = color;
}

const Color3B& FlyObject::getNormalDisplayColor() const
{
    return _normalDisplayColor;
}

void FlyObject::setDamagedDisplayColor(const Color3B& color)
{
    _damagedDisplayColor = color;
}

const Color3B& FlyObject::getDamagedDisplayColor() const
{
    return _damagedDisplayColor;
}

void FlyObject::applyDamageAppearance(bool damaged)
{
    auto* drawNode = getDrawNode();
    if (drawNode != nullptr && !_polygonPoints.empty()) {
        const auto& fillColor = damaged ? _damagedFillColor : _normalFillColor;
        const auto& borderColor = damaged ? _damagedBorderColor : _normalBorderColor;
        updatePolygonAppearance(_polygonPoints.data(),
                                static_cast<int>(_polygonPoints.size()),
                                fillColor,
                                _polygonBorderWidth,
                                borderColor);
        return;
    }

    setDisplayColor(damaged ? _damagedDisplayColor : _normalDisplayColor);
}

void FlyObject::setPosition(const Vec2& position)
{
    if (_node != nullptr) {
        _node->setPosition(position);
    }
}

Vec2 FlyObject::getPosition() const
{
    return _node != nullptr ? _node->getPosition() : Vec2::ZERO;
}

bool FlyObject::hasNode() const
{
    return _node != nullptr;
}

void FlyObject::removeFromParent()
{
    if (_node != nullptr) {
        _node->removeFromParent();
    }
}

void FlyObject::setVisible(bool visible)
{
    if (_node != nullptr) {
        _node->setVisible(visible);
    }
}

bool FlyObject::isVisible() const
{
    return _node != nullptr ? _node->isVisible() : false;
}

void FlyObject::stopActionByTag(int actionTag)
{
    if (_node != nullptr) {
        _node->stopActionByTag(actionTag);
    }
}

void FlyObject::runAction(FiniteTimeAction* action)
{
    if (_node != nullptr && action != nullptr) {
        _node->runAction(action);
    }
}

void FlyObject::playBlink(float duration, unsigned int blinks, int actionTag)
{
    if (_node == nullptr) {
        return;
    }

    stopActionByTag(actionTag);
    setVisible(true);

    auto resetVisible = CallFunc::create([this]() {
        setVisible(true);
    });
    auto blinkSequence = Sequence::create(Blink::create(duration, blinks), resetVisible, nullptr);
    blinkSequence->setTag(actionTag);
    runAction(blinkSequence);
}

void FlyObject::setVelocity(const Vec2& velocity)
{
    _velocity = velocity;
}

const Vec2& FlyObject::getVelocity() const
{
    return _velocity;
}

void FlyObject::updatePosition(float delta)
{
    if (_node != nullptr) {
        _node->setPosition(_node->getPosition() + _velocity * delta);
    }
}

bool FlyObject::isPlayerObject() const
{
    return _isPlayer;
}

void FlyObject::setMaxHealth(int maxHealth)
{
    _maxHealth = std::max(1, maxHealth);
    _health = std::min(_health, _maxHealth);
    updateHealthBar();
}

int FlyObject::getMaxHealth() const
{
    return _maxHealth;
}

void FlyObject::setHealth(int health)
{
    _health = std::max(0, std::min(health, _maxHealth));
    updateHealthBar();
}

int FlyObject::getHealth() const
{
    return _health;
}

void FlyObject::applyDamage(int damage)
{
    setHealth(_health - damage);
}

void FlyObject::enableHealthBar(float width,
                                float height,
                                const Vec2& offset,
                                const Color4F& backgroundColor,
                                const Color4F& fillColor,
                                const Color4F& borderColor)
{
    _healthBarEnabled = true;
    _healthBarWidth = width;
    _healthBarHeight = height;
    _healthBarOffset = offset;
    _healthBarBackgroundColor = backgroundColor;
    _healthBarFillColor = fillColor;
    _healthBarBorderColor = borderColor;
    updateHealthBar();
}

void FlyObject::disableHealthBar()
{
    _healthBarEnabled = false;
    if (_healthBarBackground != nullptr) {
        _healthBarBackground->removeFromParent();
        _healthBarBackground = nullptr;
    }
    if (_healthBarFill != nullptr) {
        _healthBarFill->removeFromParent();
        _healthBarFill = nullptr;
    }
    if (_healthBarPercentLabel != nullptr) {
        _healthBarPercentLabel->removeFromParent();
        _healthBarPercentLabel = nullptr;
    }
}

void FlyObject::updateHealthBar()
{
    if (!_healthBarEnabled || _node == nullptr) {
        return;
    }

    if (_healthBarBackground == nullptr) {
        _healthBarBackground = DrawNode::create();
        _healthBarBackground->setPosition(_healthBarOffset);
        _node->addChild(_healthBarBackground, 5);
    }

    if (_healthBarFill == nullptr) {
        _healthBarFill = DrawNode::create();
        _healthBarFill->setPosition(_healthBarOffset);
        _node->addChild(_healthBarFill, 6);
    }

    if (_healthBarPercentLabel == nullptr) {
        _healthBarPercentLabel = Label::createWithSystemFont("", "Arial", _healthBarHeight + 6.0f);
        _healthBarPercentLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
        _healthBarPercentLabel->enableOutline(Color4B::BLACK, 1);
        _node->addChild(_healthBarPercentLabel, 7);
    }

    _healthBarBackground->setPosition(_healthBarOffset);
    _healthBarFill->setPosition(_healthBarOffset);
    _healthBarPercentLabel->setPosition(_healthBarOffset);

    const float halfWidth = _healthBarWidth * 0.5f;
    const float halfHeight = _healthBarHeight * 0.5f;
    Vec2 backgroundPoints[] = {
        Vec2(-halfWidth, halfHeight),
        Vec2(halfWidth, halfHeight),
        Vec2(halfWidth, -halfHeight),
        Vec2(-halfWidth, -halfHeight)
    };

    _healthBarBackground->clear();
    _healthBarBackground->drawPolygon(backgroundPoints, 4, _healthBarBackgroundColor, 1.0f, _healthBarBorderColor);

    const float healthRatio = _maxHealth > 0 ? static_cast<float>(_health) / static_cast<float>(_maxHealth) : 0.0f;
    const float fillWidth = _healthBarWidth * clampf(healthRatio, 0.0f, 1.0f);
    const int healthPercent = static_cast<int>(std::round(clampf(healthRatio, 0.0f, 1.0f) * 100.0f));
    _healthBarFill->clear();
    _healthBarPercentLabel->setString(StringUtils::format("%d%%", healthPercent));
    _healthBarPercentLabel->setTextColor(Color4B::WHITE);

    if (fillWidth <= 0.0f) {
        return;
    }

    Vec2 fillPoints[] = {
        Vec2(-halfWidth, halfHeight),
        Vec2(-halfWidth + fillWidth, halfHeight),
        Vec2(-halfWidth + fillWidth, -halfHeight),
        Vec2(-halfWidth, -halfHeight)
    };
    _healthBarFill->drawPolygon(fillPoints, 4, _healthBarFillColor, 0.0f, _healthBarFillColor);
}

void FlyObject::setCollisionRadius(float collisionRadius)
{
    _collisionRadius = collisionRadius;
}

float FlyObject::getCollisionRadius() const
{
    return _collisionRadius;
}

void FlyObject::setFireTimer(float fireTimer)
{
    _fireTimer = fireTimer;
}

float FlyObject::getFireTimer() const
{
    return _fireTimer;
}

void FlyObject::addFireTimer(float delta)
{
    _fireTimer += delta;
}

void FlyObject::setFireInterval(float fireInterval)
{
    _fireInterval = fireInterval;
}

float FlyObject::getFireInterval() const
{
    return _fireInterval;
}

bool FlyObject::canFire() const
{
    return _fireInterval > 0.0f && _fireTimer >= _fireInterval;
}

void FlyObject::resetFireTimer()
{
    _fireTimer = 0.0f;
}

void FlyObject::setUltimateEnergy(float ultimateEnergy)
{
    _ultimateEnergy = clampf(ultimateEnergy, 0.0f, _ultimateEnergyMax);
}

float FlyObject::getUltimateEnergy() const
{
    return _ultimateEnergy;
}

void FlyObject::addUltimateEnergy(float delta)
{
    setUltimateEnergy(_ultimateEnergy + delta);
}

void FlyObject::setUltimateEnergyMax(float ultimateEnergyMax)
{
    _ultimateEnergyMax = std::max(0.0f, ultimateEnergyMax);
    _ultimateEnergy = clampf(_ultimateEnergy, 0.0f, _ultimateEnergyMax);
    _ultimateEnergyCost = clampf(_ultimateEnergyCost, 0.0f, _ultimateEnergyMax);
}

float FlyObject::getUltimateEnergyMax() const
{
    return _ultimateEnergyMax;
}

void FlyObject::setUltimateEnergyCost(float ultimateEnergyCost)
{
    _ultimateEnergyCost = clampf(ultimateEnergyCost, 0.0f, _ultimateEnergyMax);
}

float FlyObject::getUltimateEnergyCost() const
{
    return _ultimateEnergyCost;
}

bool FlyObject::canCastUltimate() const
{
    return _ultimateEnergy >= _ultimateEnergyCost && _ultimateCooldownTimer <= 0.0f;
}

bool FlyObject::consumeUltimateEnergy()
{
    if (!canCastUltimate()) {
        return false;
    }

    _ultimateEnergy -= _ultimateEnergyCost;
    _ultimateCooldownTimer = _ultimateCooldown;
    return true;
}

void FlyObject::setUltimateCooldown(float ultimateCooldown)
{
    _ultimateCooldown = std::max(0.0f, ultimateCooldown);
}

float FlyObject::getUltimateCooldown() const
{
    return _ultimateCooldown;
}

float FlyObject::getUltimateCooldownTimer() const
{
    return _ultimateCooldownTimer;
}

void FlyObject::updateUltimateCooldown(float delta)
{
    _ultimateCooldownTimer = std::max(0.0f, _ultimateCooldownTimer - delta);
}

void FlyObject::resetUltimateCooldown()
{
    _ultimateCooldownTimer = 0.0f;
}
