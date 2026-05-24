#ifndef __SET_SCENE_H__
#define __SET_SCENE_H__

#include "cocos2d.h"
#include "KeyBindingConfig.h"

class SetScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;

    CREATE_FUNC(SetScene);

private:
    enum class SettingsTab
    {
        Window,
        Controls
    };

    enum class PendingBinding
    {
        None,
        MoveLeftPrimary,
        MoveLeftSecondary,
        MoveRightPrimary,
        MoveRightSecondary,
        MoveUpPrimary,
        MoveUpSecondary,
        MoveDownPrimary,
        MoveDownSecondary,
        Fire,
        Ultimate
    };

    void refreshKeyBindingLabels();
    void beginBinding(PendingBinding binding);
    void handleBindingSelection(cocos2d::Ref* sender);
    KeyBindingConfig::Action toAction(PendingBinding binding) const;
    void refreshBindingButtonState();
    void animateHintLabel(const cocos2d::Color3B& color);
    void animateTabPanels(bool showWindow);
    void switchToWindowTab(cocos2d::Ref* sender);
    void switchToControlsTab(cocos2d::Ref* sender);
    void refreshTabState();
    void applyWindowSize(float width, float height);
    void useWasdPreset(cocos2d::Ref* sender);
    void useArrowPreset(cocos2d::Ref* sender);
    void restoreDefaultBindings(cocos2d::Ref* sender);
    void setWindow800x600(cocos2d::Ref* sender);
    void setWindow1024x768(cocos2d::Ref* sender);
    void setWindow1280x960(cocos2d::Ref* sender);
    void backToBegin(cocos2d::Ref* sender);

    cocos2d::Label* _presetLabel = nullptr;
    cocos2d::Label* _moveLabel = nullptr;
    cocos2d::Label* _fireLabel = nullptr;
    cocos2d::Label* _ultimateLabel = nullptr;
    cocos2d::Label* _bindingHintLabel = nullptr;
    cocos2d::Node* _windowPanel = nullptr;
    cocos2d::Node* _controlsPanel = nullptr;
    cocos2d::LayerColor* _windowTabBackground = nullptr;
    cocos2d::LayerColor* _controlsTabBackground = nullptr;
    cocos2d::DrawNode* _windowTabUnderline = nullptr;
    cocos2d::DrawNode* _controlsTabUnderline = nullptr;
    cocos2d::MenuItemLabel* _windowTabItem = nullptr;
    cocos2d::MenuItemLabel* _controlsTabItem = nullptr;
    cocos2d::MenuItemLabel* _bindLeft1Item = nullptr;
    cocos2d::MenuItemLabel* _bindLeft2Item = nullptr;
    cocos2d::MenuItemLabel* _bindRight1Item = nullptr;
    cocos2d::MenuItemLabel* _bindRight2Item = nullptr;
    cocos2d::MenuItemLabel* _bindUp1Item = nullptr;
    cocos2d::MenuItemLabel* _bindUp2Item = nullptr;
    cocos2d::MenuItemLabel* _bindDown1Item = nullptr;
    cocos2d::MenuItemLabel* _bindDown2Item = nullptr;
    cocos2d::MenuItemLabel* _bindFireItem = nullptr;
    cocos2d::MenuItemLabel* _bindUltimateItem = nullptr;
    cocos2d::Label* _left1ValueLabel = nullptr;
    cocos2d::Label* _left2ValueLabel = nullptr;
    cocos2d::Label* _right1ValueLabel = nullptr;
    cocos2d::Label* _right2ValueLabel = nullptr;
    cocos2d::Label* _up1ValueLabel = nullptr;
    cocos2d::Label* _up2ValueLabel = nullptr;
    cocos2d::Label* _down1ValueLabel = nullptr;
    cocos2d::Label* _down2ValueLabel = nullptr;
    cocos2d::Label* _fireValueLabel = nullptr;
    cocos2d::Label* _ultimateValueLabel = nullptr;
    KeyBindingConfig::Bindings _bindings = KeyBindingConfig::load();
    SettingsTab _activeTab = SettingsTab::Window;
    PendingBinding _pendingBinding = PendingBinding::None;
};

#endif // __SET_SCENE_H__
