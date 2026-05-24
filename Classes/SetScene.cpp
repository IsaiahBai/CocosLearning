#include "SetScene.h"

#include "BeginScene.h"

USING_NS_CC;

Scene* SetScene::createScene()
{
    return SetScene::create();
}

bool SetScene::init()
{
    if (!Scene::init()) {
        return false;
    }

    _bindings = KeyBindingConfig::load();
    _activeTab = SettingsTab::Window;

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const auto origin = Director::getInstance()->getVisibleOrigin();
    const float centerX = origin.x + visibleSize.width / 2.0f;
    const float centerY = origin.y + visibleSize.height / 2.0f;

    auto background = LayerColor::create(Color4B(16, 24, 40, 255));
    addChild(background);

    auto title = Label::createWithSystemFont("Settings", "Arial", 42);
    title->setPosition(Vec2(centerX, centerY + 255.0f));
    title->setColor(Color3B::WHITE);
    addChild(title);

    _windowTabBackground = LayerColor::create(Color4B(34, 44, 68, 255), 150.0f, 44.0f);
    _windowTabBackground->setIgnoreAnchorPointForPosition(false);
    _windowTabBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    _windowTabBackground->setPosition(Vec2(centerX - 90.0f, centerY + 205.0f));
    addChild(_windowTabBackground, 2);

    _controlsTabBackground = LayerColor::create(Color4B(34, 44, 68, 255), 150.0f, 44.0f);
    _controlsTabBackground->setIgnoreAnchorPointForPosition(false);
    _controlsTabBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
    _controlsTabBackground->setPosition(Vec2(centerX + 90.0f, centerY + 205.0f));
    addChild(_controlsTabBackground, 2);

    _windowTabUnderline = DrawNode::create();
    addChild(_windowTabUnderline, 2);

    _controlsTabUnderline = DrawNode::create();
    addChild(_controlsTabUnderline, 2);

    auto windowTabText = Label::createWithSystemFont("Window", "Arial", 28);
    auto controlsTabText = Label::createWithSystemFont("Controls", "Arial", 28);
    _windowTabItem = MenuItemLabel::create(windowTabText, CC_CALLBACK_1(SetScene::switchToWindowTab, this));
    _controlsTabItem = MenuItemLabel::create(controlsTabText, CC_CALLBACK_1(SetScene::switchToControlsTab, this));
    _windowTabItem->setPosition(Vec2(centerX - 90.0f, centerY + 205.0f));
    _controlsTabItem->setPosition(Vec2(centerX + 90.0f, centerY + 205.0f));

    auto backText = Label::createWithSystemFont("Back", "Arial", 28);
    backText->setColor(Color3B(255, 130, 130));
    auto backItem = MenuItemLabel::create(backText, CC_CALLBACK_1(SetScene::backToBegin, this));
    backItem->setPosition(Vec2(centerX, centerY - 330.0f));

    auto menu = Menu::create(_windowTabItem, _controlsTabItem, backItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 3);

    _windowPanel = Node::create();
    addChild(_windowPanel, 1);

    _controlsPanel = Node::create();
    addChild(_controlsPanel, 1);

    auto windowTitle = Label::createWithSystemFont("Window Size", "Arial", 28);
    windowTitle->setPosition(Vec2(centerX, centerY + 120.0f));
    windowTitle->setColor(Color3B(200, 210, 235));
    _windowPanel->addChild(windowTitle);

    auto size640Text = Label::createWithSystemFont("800 x 600", "Arial", 30);
    size640Text->setColor(Color3B(100, 220, 255));
    auto size640Item = MenuItemLabel::create(size640Text, CC_CALLBACK_1(SetScene::setWindow800x600, this));
    size640Item->setPosition(Vec2(centerX, centerY + 45.0f));

    auto size720Text = Label::createWithSystemFont("1024 x 768", "Arial", 30);
    size720Text->setColor(Color3B(120, 230, 160));
    auto size720Item = MenuItemLabel::create(size720Text, CC_CALLBACK_1(SetScene::setWindow1024x768, this));
    size720Item->setPosition(Vec2(centerX, centerY - 25.0f));

    auto size1080Text = Label::createWithSystemFont("1280 x 960", "Arial", 30);
    size1080Text->setColor(Color3B(255, 210, 110));
    auto size1080Item = MenuItemLabel::create(size1080Text, CC_CALLBACK_1(SetScene::setWindow1280x960, this));
    size1080Item->setPosition(Vec2(centerX, centerY - 95.0f));

    auto windowMenu = Menu::create(size640Item, size720Item, size1080Item, nullptr);
    windowMenu->setPosition(Vec2::ZERO);
    _windowPanel->addChild(windowMenu);

    auto controlTitle = Label::createWithSystemFont("Key Bindings", "Arial", 28);
    controlTitle->setPosition(Vec2(centerX, centerY + 120.0f));
    controlTitle->setColor(Color3B(200, 210, 235));
    _controlsPanel->addChild(controlTitle);

    _presetLabel = Label::createWithSystemFont("Quick reset:", "Arial", 22);
    _presetLabel->setPosition(Vec2(centerX, centerY + 88.0f));
    _presetLabel->setColor(Color3B(240, 240, 255));
    _controlsPanel->addChild(_presetLabel);

    auto wasdText = Label::createWithSystemFont("Use WASD", "Arial", 24);
    wasdText->setColor(Color3B(100, 220, 255));
    auto wasdItem = MenuItemLabel::create(wasdText, CC_CALLBACK_1(SetScene::useWasdPreset, this));
    wasdItem->setPosition(Vec2(centerX - 120.0f, centerY + 55.0f));

    auto arrowText = Label::createWithSystemFont("Use Arrows", "Arial", 24);
    arrowText->setColor(Color3B(120, 230, 160));
    auto arrowItem = MenuItemLabel::create(arrowText, CC_CALLBACK_1(SetScene::useArrowPreset, this));
    arrowItem->setPosition(Vec2(centerX + 120.0f, centerY + 55.0f));

    auto restoreText = Label::createWithSystemFont("Restore Default", "Arial", 22);
    restoreText->setColor(Color3B(255, 205, 120));
    auto restoreItem = MenuItemLabel::create(restoreText, CC_CALLBACK_1(SetScene::restoreDefaultBindings, this));
    restoreItem->setPosition(Vec2(centerX, centerY + 20.0f));

    _moveLabel = Label::createWithSystemFont("", "Arial", 17);
    _moveLabel->setPosition(Vec2(centerX, centerY - 18.0f));
    _moveLabel->setColor(Color3B(185, 195, 215));
    _controlsPanel->addChild(_moveLabel);

    _fireLabel = Label::createWithSystemFont("", "Arial", 17);
    _fireLabel->setPosition(Vec2(centerX, centerY - 44.0f));
    _fireLabel->setColor(Color3B(185, 195, 215));
    _controlsPanel->addChild(_fireLabel);

    _ultimateLabel = Label::createWithSystemFont("", "Arial", 17);
    _ultimateLabel->setPosition(Vec2(centerX, centerY - 70.0f));
    _ultimateLabel->setColor(Color3B(185, 195, 215));
    _controlsPanel->addChild(_ultimateLabel);

    _bindingHintLabel = Label::createWithSystemFont("Click an action below, then press a key. Esc cancels.", "Arial", 17);
    _bindingHintLabel->setPosition(Vec2(centerX, centerY - 97.0f));
    _bindingHintLabel->setColor(Color3B(255, 220, 150));
    _controlsPanel->addChild(_bindingHintLabel);

    _bindLeft1Item = MenuItemLabel::create(Label::createWithSystemFont("Left 1", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindLeft1Item->setTag(static_cast<int>(PendingBinding::MoveLeftPrimary));
    _bindLeft1Item->setPosition(Vec2(centerX - 180.0f, centerY - 140.0f));
    _left1ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _left1ValueLabel->setPosition(Vec2(centerX - 180.0f, centerY - 160.0f));
    _left1ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_left1ValueLabel);

    _bindLeft2Item = MenuItemLabel::create(Label::createWithSystemFont("Left 2", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindLeft2Item->setTag(static_cast<int>(PendingBinding::MoveLeftSecondary));
    _bindLeft2Item->setPosition(Vec2(centerX - 60.0f, centerY - 140.0f));
    _left2ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _left2ValueLabel->setPosition(Vec2(centerX - 60.0f, centerY - 160.0f));
    _left2ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_left2ValueLabel);

    _bindRight1Item = MenuItemLabel::create(Label::createWithSystemFont("Right 1", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindRight1Item->setTag(static_cast<int>(PendingBinding::MoveRightPrimary));
    _bindRight1Item->setPosition(Vec2(centerX + 60.0f, centerY - 140.0f));
    _right1ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _right1ValueLabel->setPosition(Vec2(centerX + 60.0f, centerY - 160.0f));
    _right1ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_right1ValueLabel);

    _bindRight2Item = MenuItemLabel::create(Label::createWithSystemFont("Right 2", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindRight2Item->setTag(static_cast<int>(PendingBinding::MoveRightSecondary));
    _bindRight2Item->setPosition(Vec2(centerX + 180.0f, centerY - 140.0f));
    _right2ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _right2ValueLabel->setPosition(Vec2(centerX + 180.0f, centerY - 160.0f));
    _right2ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_right2ValueLabel);

    _bindUp1Item = MenuItemLabel::create(Label::createWithSystemFont("Up 1", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindUp1Item->setTag(static_cast<int>(PendingBinding::MoveUpPrimary));
    _bindUp1Item->setPosition(Vec2(centerX - 180.0f, centerY - 178.0f));
    _up1ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _up1ValueLabel->setPosition(Vec2(centerX - 180.0f, centerY - 198.0f));
    _up1ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_up1ValueLabel);

    _bindUp2Item = MenuItemLabel::create(Label::createWithSystemFont("Up 2", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindUp2Item->setTag(static_cast<int>(PendingBinding::MoveUpSecondary));
    _bindUp2Item->setPosition(Vec2(centerX - 60.0f, centerY - 178.0f));
    _up2ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _up2ValueLabel->setPosition(Vec2(centerX - 60.0f, centerY - 198.0f));
    _up2ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_up2ValueLabel);

    _bindDown1Item = MenuItemLabel::create(Label::createWithSystemFont("Down 1", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindDown1Item->setTag(static_cast<int>(PendingBinding::MoveDownPrimary));
    _bindDown1Item->setPosition(Vec2(centerX + 60.0f, centerY - 178.0f));
    _down1ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _down1ValueLabel->setPosition(Vec2(centerX + 60.0f, centerY - 198.0f));
    _down1ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_down1ValueLabel);

    _bindDown2Item = MenuItemLabel::create(Label::createWithSystemFont("Down 2", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindDown2Item->setTag(static_cast<int>(PendingBinding::MoveDownSecondary));
    _bindDown2Item->setPosition(Vec2(centerX + 180.0f, centerY - 178.0f));
    _down2ValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _down2ValueLabel->setPosition(Vec2(centerX + 180.0f, centerY - 198.0f));
    _down2ValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_down2ValueLabel);

    _bindFireItem = MenuItemLabel::create(Label::createWithSystemFont("Fire", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindFireItem->setTag(static_cast<int>(PendingBinding::Fire));
    _bindFireItem->setPosition(Vec2(centerX - 80.0f, centerY - 220.0f));
    _fireValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _fireValueLabel->setPosition(Vec2(centerX - 80.0f, centerY - 240.0f));
    _fireValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_fireValueLabel);

    _bindUltimateItem = MenuItemLabel::create(Label::createWithSystemFont("Ultimate", "Arial", 21), CC_CALLBACK_1(SetScene::handleBindingSelection, this));
    _bindUltimateItem->setTag(static_cast<int>(PendingBinding::Ultimate));
    _bindUltimateItem->setPosition(Vec2(centerX + 80.0f, centerY - 220.0f));
    _ultimateValueLabel = Label::createWithSystemFont("", "Arial", 17);
    _ultimateValueLabel->setPosition(Vec2(centerX + 80.0f, centerY - 240.0f));
    _ultimateValueLabel->setColor(Color3B(120, 220, 255));
    _controlsPanel->addChild(_ultimateValueLabel);

    auto controlsMenu = Menu::create(
        wasdItem, arrowItem, restoreItem,
        _bindLeft1Item, _bindLeft2Item, _bindRight1Item, _bindRight2Item,
        _bindUp1Item, _bindUp2Item, _bindDown1Item, _bindDown2Item,
        _bindFireItem, _bindUltimateItem,
        nullptr);
    controlsMenu->setPosition(Vec2::ZERO);
    _controlsPanel->addChild(controlsMenu);

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (_pendingBinding == PendingBinding::None) {
            return;
        }

        if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
            _pendingBinding = PendingBinding::None;
            if (_bindingHintLabel != nullptr) {
                _bindingHintLabel->setString("Binding cancelled.");
            }
            animateHintLabel(Color3B(255, 210, 130));
            refreshBindingButtonState();
            return;
        }

        if (!KeyBindingConfig::isSupportedKey(keyCode)) {
            if (_bindingHintLabel != nullptr) {
                _bindingHintLabel->setString("Unsupported key. Use A/W/S/D, arrows, Space, J, or K. Esc cancels.");
            }
            animateHintLabel(Color3B(255, 180, 120));
            return;
        }

        const KeyBindingConfig::Action action = toAction(_pendingBinding);
        std::string conflictActionName;
        if (KeyBindingConfig::hasConflict(_bindings, action, keyCode, &conflictActionName)) {
            if (_bindingHintLabel != nullptr) {
                _bindingHintLabel->setString("Conflict: " + KeyBindingConfig::keyName(keyCode) +
                                             " is already used by " + conflictActionName);
            }
            animateHintLabel(Color3B(255, 120, 120));
            return;
        }

        KeyBindingConfig::setBinding(_bindings, action, keyCode);
        KeyBindingConfig::save(_bindings);
        if (_bindingHintLabel != nullptr) {
            _bindingHintLabel->setString("Saved " + KeyBindingConfig::actionName(action) +
                                         " = " + KeyBindingConfig::keyName(keyCode));
        }
        _pendingBinding = PendingBinding::None;
        animateHintLabel(Color3B(120, 255, 160));
        refreshKeyBindingLabels();
        refreshBindingButtonState();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    refreshKeyBindingLabels();
    refreshBindingButtonState();
    refreshTabState();

    return true;
}

void SetScene::refreshKeyBindingLabels()
{
    if (_left1ValueLabel != nullptr) {
        _left1ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveLeftPrimary));
    }
    if (_left2ValueLabel != nullptr) {
        _left2ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveLeftSecondary));
    }
    if (_right1ValueLabel != nullptr) {
        _right1ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveRightPrimary));
    }
    if (_right2ValueLabel != nullptr) {
        _right2ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveRightSecondary));
    }
    if (_up1ValueLabel != nullptr) {
        _up1ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveUpPrimary));
    }
    if (_up2ValueLabel != nullptr) {
        _up2ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveUpSecondary));
    }
    if (_down1ValueLabel != nullptr) {
        _down1ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveDownPrimary));
    }
    if (_down2ValueLabel != nullptr) {
        _down2ValueLabel->setString(KeyBindingConfig::keyName(_bindings.moveDownSecondary));
    }
    if (_fireValueLabel != nullptr) {
        _fireValueLabel->setString(KeyBindingConfig::keyName(_bindings.fire));
    }
    if (_ultimateValueLabel != nullptr) {
        _ultimateValueLabel->setString(KeyBindingConfig::keyName(_bindings.ultimate));
    }

    if (_moveLabel != nullptr) {
        _moveLabel->setString(
            "Move  L:" + KeyBindingConfig::bindingSummary(_bindings.moveLeftPrimary, _bindings.moveLeftSecondary) +
            "  R:" + KeyBindingConfig::bindingSummary(_bindings.moveRightPrimary, _bindings.moveRightSecondary) +
            "  U:" + KeyBindingConfig::bindingSummary(_bindings.moveUpPrimary, _bindings.moveUpSecondary) +
            "  D:" + KeyBindingConfig::bindingSummary(_bindings.moveDownPrimary, _bindings.moveDownSecondary));
    }
    if (_fireLabel != nullptr) {
        _fireLabel->setString("Fire: " + KeyBindingConfig::keyName(_bindings.fire));
    }
    if (_ultimateLabel != nullptr) {
        _ultimateLabel->setString("Ultimate: " + KeyBindingConfig::keyName(_bindings.ultimate));
    }
}

void SetScene::beginBinding(PendingBinding binding)
{
    _activeTab = SettingsTab::Controls;
    _pendingBinding = binding;
    refreshTabState();
    if (_bindingHintLabel != nullptr) {
        _bindingHintLabel->setString("Press a key for " + KeyBindingConfig::actionName(toAction(binding)));
    }
    animateHintLabel(Color3B(255, 220, 150));
    refreshBindingButtonState();
}

void SetScene::handleBindingSelection(Ref* sender)
{
    auto* item = dynamic_cast<MenuItem*>(sender);
    if (item == nullptr) {
        return;
    }

    beginBinding(static_cast<PendingBinding>(item->getTag()));
}

KeyBindingConfig::Action SetScene::toAction(PendingBinding binding) const
{
    switch (binding) {
    case PendingBinding::MoveLeftPrimary:
        return KeyBindingConfig::Action::MoveLeftPrimary;
    case PendingBinding::MoveLeftSecondary:
        return KeyBindingConfig::Action::MoveLeftSecondary;
    case PendingBinding::MoveRightPrimary:
        return KeyBindingConfig::Action::MoveRightPrimary;
    case PendingBinding::MoveRightSecondary:
        return KeyBindingConfig::Action::MoveRightSecondary;
    case PendingBinding::MoveUpPrimary:
        return KeyBindingConfig::Action::MoveUpPrimary;
    case PendingBinding::MoveUpSecondary:
        return KeyBindingConfig::Action::MoveUpSecondary;
    case PendingBinding::MoveDownPrimary:
        return KeyBindingConfig::Action::MoveDownPrimary;
    case PendingBinding::MoveDownSecondary:
        return KeyBindingConfig::Action::MoveDownSecondary;
    case PendingBinding::Fire:
        return KeyBindingConfig::Action::Fire;
    case PendingBinding::Ultimate:
        return KeyBindingConfig::Action::Ultimate;
    default:
        return KeyBindingConfig::Action::Fire;
    }
}

void SetScene::refreshBindingButtonState()
{
    struct ItemState {
        MenuItemLabel* item;
        PendingBinding binding;
    };

    const ItemState items[] = {
        { _bindLeft1Item, PendingBinding::MoveLeftPrimary },
        { _bindLeft2Item, PendingBinding::MoveLeftSecondary },
        { _bindRight1Item, PendingBinding::MoveRightPrimary },
        { _bindRight2Item, PendingBinding::MoveRightSecondary },
        { _bindUp1Item, PendingBinding::MoveUpPrimary },
        { _bindUp2Item, PendingBinding::MoveUpSecondary },
        { _bindDown1Item, PendingBinding::MoveDownPrimary },
        { _bindDown2Item, PendingBinding::MoveDownSecondary },
        { _bindFireItem, PendingBinding::Fire },
        { _bindUltimateItem, PendingBinding::Ultimate }
    };

    for (const ItemState& itemState : items) {
        if (itemState.item == nullptr) {
            continue;
        }

        if (itemState.binding == _pendingBinding) {
            itemState.item->setColor(Color3B(255, 230, 120));
            itemState.item->setScale(1.08f);
        } else {
            itemState.item->setColor(Color3B::WHITE);
            itemState.item->setScale(1.0f);
        }
    }
}

void SetScene::animateHintLabel(const Color3B& color)
{
    if (_bindingHintLabel == nullptr) {
        return;
    }

    _bindingHintLabel->stopAllActions();
    _bindingHintLabel->setColor(color);
    _bindingHintLabel->setOpacity(180);
    _bindingHintLabel->runAction(Sequence::create(
        FadeTo::create(0.08f, 255),
        DelayTime::create(0.25f),
        FadeTo::create(0.18f, 210),
        nullptr));
}

void SetScene::switchToWindowTab(Ref* sender)
{
    _activeTab = SettingsTab::Window;
    refreshTabState();
    animateTabPanels(true);
}

void SetScene::switchToControlsTab(Ref* sender)
{
    _activeTab = SettingsTab::Controls;
    refreshTabState();
    animateTabPanels(false);
}

void SetScene::refreshTabState()
{
    const bool showWindow = _activeTab == SettingsTab::Window;

    if (_windowPanel != nullptr) {
        _windowPanel->setVisible(showWindow);
    }
    if (_controlsPanel != nullptr) {
        _controlsPanel->setVisible(!showWindow);
    }
    if (_windowTabItem != nullptr) {
        _windowTabItem->setColor(showWindow ? Color3B(255, 230, 120) : Color3B(200, 210, 235));
        _windowTabItem->setScale(showWindow ? 1.08f : 1.0f);
    }
    if (_controlsTabItem != nullptr) {
        _controlsTabItem->setColor(showWindow ? Color3B(200, 210, 235) : Color3B(255, 230, 120));
        _controlsTabItem->setScale(showWindow ? 1.0f : 1.08f);
    }
    if (_windowTabBackground != nullptr) {
        _windowTabBackground->setColor(showWindow ? Color3B(62, 78, 112) : Color3B(34, 44, 68));
    }
    if (_controlsTabBackground != nullptr) {
        _controlsTabBackground->setColor(showWindow ? Color3B(34, 44, 68) : Color3B(62, 78, 112));
    }
    if (_windowTabUnderline != nullptr) {
        _windowTabUnderline->clear();
        if (showWindow) {
            const Vec2 line[] = {
                Vec2(_windowTabBackground->getPositionX() - 75.0f, _windowTabBackground->getPositionY() - 28.0f),
                Vec2(_windowTabBackground->getPositionX() + 75.0f, _windowTabBackground->getPositionY() - 28.0f)
            };
            _windowTabUnderline->drawSegment(line[0], line[1], 3.0f, Color4F(1.0f, 0.88f, 0.45f, 1.0f));
        }
    }
    if (_controlsTabUnderline != nullptr) {
        _controlsTabUnderline->clear();
        if (!showWindow) {
            const Vec2 line[] = {
                Vec2(_controlsTabBackground->getPositionX() - 75.0f, _controlsTabBackground->getPositionY() - 28.0f),
                Vec2(_controlsTabBackground->getPositionX() + 75.0f, _controlsTabBackground->getPositionY() - 28.0f)
            };
            _controlsTabUnderline->drawSegment(line[0], line[1], 3.0f, Color4F(1.0f, 0.88f, 0.45f, 1.0f));
        }
    }
}

void SetScene::animateTabPanels(bool showWindow)
{
    if (_windowPanel == nullptr || _controlsPanel == nullptr) {
        return;
    }

    _windowPanel->stopAllActions();
    _controlsPanel->stopAllActions();
    _windowPanel->setPosition(Vec2::ZERO);
    _controlsPanel->setPosition(Vec2::ZERO);

    if (showWindow) {
        _windowPanel->setVisible(true);
        _windowPanel->runAction(MoveBy::create(0.16f, Vec2(-10.0f, 0.0f)));

        _controlsPanel->setVisible(true);
        _controlsPanel->runAction(Sequence::create(
            MoveBy::create(0.12f, Vec2(12.0f, 0.0f)),
            Hide::create(),
            nullptr));
    } else {
        _controlsPanel->setVisible(true);
        _controlsPanel->runAction(MoveBy::create(0.16f, Vec2(10.0f, 0.0f)));

        _windowPanel->setVisible(true);
        _windowPanel->runAction(Sequence::create(
            MoveBy::create(0.12f, Vec2(-12.0f, 0.0f)),
            Hide::create(),
            nullptr));
    }
}

void SetScene::applyWindowSize(float width, float height)
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (glview == nullptr) {
        glview = GLViewImpl::createWithRect("planeGame", Rect(0, 0, width, height));
        director->setOpenGLView(glview);
    } else {
        glview->setFrameSize(width, height);
    }

    glview->setDesignResolutionSize(width, height, ResolutionPolicy::SHOW_ALL);

    auto scene = SetScene::createScene();
    director->replaceScene(TransitionFade::create(0.2f, scene));
}

void SetScene::useWasdPreset(Ref* sender)
{
    KeyBindingConfig::resetToPreset(KeyBindingConfig::Preset::WASD);
    _bindings = KeyBindingConfig::load();
    _pendingBinding = PendingBinding::None;
    if (_bindingHintLabel != nullptr) {
        _bindingHintLabel->setString("Reset to WASD preset.");
    }
    animateHintLabel(Color3B(120, 220, 255));
    refreshKeyBindingLabels();
    refreshBindingButtonState();
}

void SetScene::useArrowPreset(Ref* sender)
{
    KeyBindingConfig::resetToPreset(KeyBindingConfig::Preset::ARROWS);
    _bindings = KeyBindingConfig::load();
    _pendingBinding = PendingBinding::None;
    if (_bindingHintLabel != nullptr) {
        _bindingHintLabel->setString("Reset to Arrow Keys preset.");
    }
    animateHintLabel(Color3B(120, 230, 160));
    refreshKeyBindingLabels();
    refreshBindingButtonState();
}

void SetScene::restoreDefaultBindings(Ref* sender)
{
    KeyBindingConfig::resetToPreset(KeyBindingConfig::Preset::WASD);
    _bindings = KeyBindingConfig::load();
    _pendingBinding = PendingBinding::None;
    if (_bindingHintLabel != nullptr) {
        _bindingHintLabel->setString("Restored default bindings.");
    }
    animateHintLabel(Color3B(255, 205, 120));
    refreshKeyBindingLabels();
    refreshBindingButtonState();
}

void SetScene::setWindow800x600(Ref* sender)
{
    applyWindowSize(800.0f, 600.0f);
}

void SetScene::setWindow1024x768(Ref* sender)
{
    applyWindowSize(1024.0f, 768.0f);
}

void SetScene::setWindow1280x960(Ref* sender)
{
    applyWindowSize(1280.0f, 960.0f);
}

void SetScene::backToBegin(Ref* sender)
{
    auto scene = BeginScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, scene));
}
