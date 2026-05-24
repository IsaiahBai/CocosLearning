#ifndef __KEY_BINDING_CONFIG_H__
#define __KEY_BINDING_CONFIG_H__

#include "cocos2d.h"

#include <string>

class KeyBindingConfig
{
public:
    struct Bindings
    {
        cocos2d::EventKeyboard::KeyCode moveLeftPrimary;
        cocos2d::EventKeyboard::KeyCode moveLeftSecondary;
        cocos2d::EventKeyboard::KeyCode moveRightPrimary;
        cocos2d::EventKeyboard::KeyCode moveRightSecondary;
        cocos2d::EventKeyboard::KeyCode moveUpPrimary;
        cocos2d::EventKeyboard::KeyCode moveUpSecondary;
        cocos2d::EventKeyboard::KeyCode moveDownPrimary;
        cocos2d::EventKeyboard::KeyCode moveDownSecondary;
        cocos2d::EventKeyboard::KeyCode fire;
        cocos2d::EventKeyboard::KeyCode ultimate;
    };

    enum class Preset
    {
        WASD = 0,
        ARROWS = 1
    };

    enum class Action
    {
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

    static Bindings load();
    static void save(const Bindings& bindings);
    static void resetToPreset(Preset preset);
    static std::string presetName(Preset preset);
    static std::string keyName(cocos2d::EventKeyboard::KeyCode keyCode);
    static std::string bindingSummary(cocos2d::EventKeyboard::KeyCode primary,
                                      cocos2d::EventKeyboard::KeyCode secondary);
    static std::string exportFilePath();
    static bool isSupportedKey(cocos2d::EventKeyboard::KeyCode keyCode);
    static void setBinding(Bindings& bindings, Action action, cocos2d::EventKeyboard::KeyCode keyCode);
    static cocos2d::EventKeyboard::KeyCode getBinding(const Bindings& bindings, Action action);
    static std::string actionName(Action action);
    static bool hasConflict(const Bindings& bindings, Action action, cocos2d::EventKeyboard::KeyCode keyCode, std::string* conflictActionName);

private:
    static Bindings makeBindings(Preset preset);
};

#endif // __KEY_BINDING_CONFIG_H__
