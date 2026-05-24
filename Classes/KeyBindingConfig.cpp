#include "KeyBindingConfig.h"

#include <sstream>

USING_NS_CC;

namespace {
constexpr char KEY_LEFT_PRIMARY[] = "bind_left_primary";
constexpr char KEY_LEFT_SECONDARY[] = "bind_left_secondary";
constexpr char KEY_RIGHT_PRIMARY[] = "bind_right_primary";
constexpr char KEY_RIGHT_SECONDARY[] = "bind_right_secondary";
constexpr char KEY_UP_PRIMARY[] = "bind_up_primary";
constexpr char KEY_UP_SECONDARY[] = "bind_up_secondary";
constexpr char KEY_DOWN_PRIMARY[] = "bind_down_primary";
constexpr char KEY_DOWN_SECONDARY[] = "bind_down_secondary";
constexpr char KEY_FIRE[] = "bind_fire";
constexpr char KEY_ULTIMATE[] = "bind_ultimate";
}

KeyBindingConfig::Bindings KeyBindingConfig::load()
{
    const Bindings defaults = makeBindings(Preset::WASD);
    auto* userDefault = UserDefault::getInstance();

    return {
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_LEFT_PRIMARY, static_cast<int>(defaults.moveLeftPrimary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_LEFT_SECONDARY, static_cast<int>(defaults.moveLeftSecondary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_RIGHT_PRIMARY, static_cast<int>(defaults.moveRightPrimary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_RIGHT_SECONDARY, static_cast<int>(defaults.moveRightSecondary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_UP_PRIMARY, static_cast<int>(defaults.moveUpPrimary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_UP_SECONDARY, static_cast<int>(defaults.moveUpSecondary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_DOWN_PRIMARY, static_cast<int>(defaults.moveDownPrimary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_DOWN_SECONDARY, static_cast<int>(defaults.moveDownSecondary))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_FIRE, static_cast<int>(defaults.fire))),
        static_cast<EventKeyboard::KeyCode>(userDefault->getIntegerForKey(KEY_ULTIMATE, static_cast<int>(defaults.ultimate)))
    };
}

void KeyBindingConfig::save(const Bindings& bindings)
{
    auto* userDefault = UserDefault::getInstance();
    userDefault->setIntegerForKey(KEY_LEFT_PRIMARY, static_cast<int>(bindings.moveLeftPrimary));
    userDefault->setIntegerForKey(KEY_LEFT_SECONDARY, static_cast<int>(bindings.moveLeftSecondary));
    userDefault->setIntegerForKey(KEY_RIGHT_PRIMARY, static_cast<int>(bindings.moveRightPrimary));
    userDefault->setIntegerForKey(KEY_RIGHT_SECONDARY, static_cast<int>(bindings.moveRightSecondary));
    userDefault->setIntegerForKey(KEY_UP_PRIMARY, static_cast<int>(bindings.moveUpPrimary));
    userDefault->setIntegerForKey(KEY_UP_SECONDARY, static_cast<int>(bindings.moveUpSecondary));
    userDefault->setIntegerForKey(KEY_DOWN_PRIMARY, static_cast<int>(bindings.moveDownPrimary));
    userDefault->setIntegerForKey(KEY_DOWN_SECONDARY, static_cast<int>(bindings.moveDownSecondary));
    userDefault->setIntegerForKey(KEY_FIRE, static_cast<int>(bindings.fire));
    userDefault->setIntegerForKey(KEY_ULTIMATE, static_cast<int>(bindings.ultimate));
    userDefault->flush();

    std::ostringstream output;
    output << "move_left_primary=" << keyName(bindings.moveLeftPrimary) << "\n";
    output << "move_left_secondary=" << keyName(bindings.moveLeftSecondary) << "\n";
    output << "move_right_primary=" << keyName(bindings.moveRightPrimary) << "\n";
    output << "move_right_secondary=" << keyName(bindings.moveRightSecondary) << "\n";
    output << "move_up_primary=" << keyName(bindings.moveUpPrimary) << "\n";
    output << "move_up_secondary=" << keyName(bindings.moveUpSecondary) << "\n";
    output << "move_down_primary=" << keyName(bindings.moveDownPrimary) << "\n";
    output << "move_down_secondary=" << keyName(bindings.moveDownSecondary) << "\n";
    output << "fire=" << keyName(bindings.fire) << "\n";
    output << "ultimate=" << keyName(bindings.ultimate) << "\n";

    FileUtils::getInstance()->writeStringToFile(output.str(), exportFilePath());
}

void KeyBindingConfig::resetToPreset(Preset preset)
{
    save(makeBindings(preset));
}

std::string KeyBindingConfig::presetName(Preset preset)
{
    return preset == Preset::ARROWS ? "Arrow Keys" : "WASD";
}

std::string KeyBindingConfig::keyName(EventKeyboard::KeyCode keyCode)
{
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_A:
        return "A";
    case EventKeyboard::KeyCode::KEY_D:
        return "D";
    case EventKeyboard::KeyCode::KEY_W:
        return "W";
    case EventKeyboard::KeyCode::KEY_S:
        return "S";
    case EventKeyboard::KeyCode::KEY_J:
        return "J";
    case EventKeyboard::KeyCode::KEY_K:
        return "K";
    case EventKeyboard::KeyCode::KEY_SPACE:
        return "Space";
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        return "Left";
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        return "Right";
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
        return "Up";
    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        return "Down";
    default:
        return "Unknown";
    }
}

std::string KeyBindingConfig::bindingSummary(EventKeyboard::KeyCode primary,
                                             EventKeyboard::KeyCode secondary)
{
    if (primary == secondary) {
        return keyName(primary);
    }

    return keyName(primary) + " / " + keyName(secondary);
}

std::string KeyBindingConfig::exportFilePath()
{
    return FileUtils::getInstance()->getWritablePath() + "keybindings.txt";
}

bool KeyBindingConfig::isSupportedKey(EventKeyboard::KeyCode keyCode)
{
    switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_A:
    case EventKeyboard::KeyCode::KEY_D:
    case EventKeyboard::KeyCode::KEY_W:
    case EventKeyboard::KeyCode::KEY_S:
    case EventKeyboard::KeyCode::KEY_J:
    case EventKeyboard::KeyCode::KEY_K:
    case EventKeyboard::KeyCode::KEY_SPACE:
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        return true;
    default:
        return false;
    }
}

void KeyBindingConfig::setBinding(Bindings& bindings, Action action, EventKeyboard::KeyCode keyCode)
{
    switch (action) {
    case Action::MoveLeftPrimary:
        bindings.moveLeftPrimary = keyCode;
        break;
    case Action::MoveLeftSecondary:
        bindings.moveLeftSecondary = keyCode;
        break;
    case Action::MoveRightPrimary:
        bindings.moveRightPrimary = keyCode;
        break;
    case Action::MoveRightSecondary:
        bindings.moveRightSecondary = keyCode;
        break;
    case Action::MoveUpPrimary:
        bindings.moveUpPrimary = keyCode;
        break;
    case Action::MoveUpSecondary:
        bindings.moveUpSecondary = keyCode;
        break;
    case Action::MoveDownPrimary:
        bindings.moveDownPrimary = keyCode;
        break;
    case Action::MoveDownSecondary:
        bindings.moveDownSecondary = keyCode;
        break;
    case Action::Fire:
        bindings.fire = keyCode;
        break;
    case Action::Ultimate:
        bindings.ultimate = keyCode;
        break;
    }
}

EventKeyboard::KeyCode KeyBindingConfig::getBinding(const Bindings& bindings, Action action)
{
    switch (action) {
    case Action::MoveLeftPrimary:
        return bindings.moveLeftPrimary;
    case Action::MoveLeftSecondary:
        return bindings.moveLeftSecondary;
    case Action::MoveRightPrimary:
        return bindings.moveRightPrimary;
    case Action::MoveRightSecondary:
        return bindings.moveRightSecondary;
    case Action::MoveUpPrimary:
        return bindings.moveUpPrimary;
    case Action::MoveUpSecondary:
        return bindings.moveUpSecondary;
    case Action::MoveDownPrimary:
        return bindings.moveDownPrimary;
    case Action::MoveDownSecondary:
        return bindings.moveDownSecondary;
    case Action::Fire:
        return bindings.fire;
    case Action::Ultimate:
        return bindings.ultimate;
    default:
        return EventKeyboard::KeyCode::KEY_NONE;
    }
}

std::string KeyBindingConfig::actionName(Action action)
{
    switch (action) {
    case Action::MoveLeftPrimary:
        return "Move Left 1";
    case Action::MoveLeftSecondary:
        return "Move Left 2";
    case Action::MoveRightPrimary:
        return "Move Right 1";
    case Action::MoveRightSecondary:
        return "Move Right 2";
    case Action::MoveUpPrimary:
        return "Move Up 1";
    case Action::MoveUpSecondary:
        return "Move Up 2";
    case Action::MoveDownPrimary:
        return "Move Down 1";
    case Action::MoveDownSecondary:
        return "Move Down 2";
    case Action::Fire:
        return "Fire";
    case Action::Ultimate:
        return "Ultimate";
    default:
        return "Unknown";
    }
}

bool KeyBindingConfig::hasConflict(const Bindings& bindings,
                                   Action action,
                                   EventKeyboard::KeyCode keyCode,
                                   std::string* conflictActionName)
{
    const Action actions[] = {
        Action::MoveLeftPrimary,
        Action::MoveLeftSecondary,
        Action::MoveRightPrimary,
        Action::MoveRightSecondary,
        Action::MoveUpPrimary,
        Action::MoveUpSecondary,
        Action::MoveDownPrimary,
        Action::MoveDownSecondary,
        Action::Fire,
        Action::Ultimate
    };

    for (Action candidate : actions) {
        if (candidate == action) {
            continue;
        }

        if (getBinding(bindings, candidate) == keyCode) {
            if (conflictActionName != nullptr) {
                *conflictActionName = actionName(candidate);
            }
            return true;
        }
    }

    return false;
}

KeyBindingConfig::Bindings KeyBindingConfig::makeBindings(Preset preset)
{
    if (preset == Preset::ARROWS) {
        return {
            EventKeyboard::KeyCode::KEY_LEFT_ARROW,
            EventKeyboard::KeyCode::KEY_A,
            EventKeyboard::KeyCode::KEY_RIGHT_ARROW,
            EventKeyboard::KeyCode::KEY_D,
            EventKeyboard::KeyCode::KEY_UP_ARROW,
            EventKeyboard::KeyCode::KEY_W,
            EventKeyboard::KeyCode::KEY_DOWN_ARROW,
            EventKeyboard::KeyCode::KEY_S,
            EventKeyboard::KeyCode::KEY_SPACE,
            EventKeyboard::KeyCode::KEY_J
        };
    }

    return {
        EventKeyboard::KeyCode::KEY_A,
        EventKeyboard::KeyCode::KEY_LEFT_ARROW,
        EventKeyboard::KeyCode::KEY_D,
        EventKeyboard::KeyCode::KEY_RIGHT_ARROW,
        EventKeyboard::KeyCode::KEY_W,
        EventKeyboard::KeyCode::KEY_UP_ARROW,
        EventKeyboard::KeyCode::KEY_S,
        EventKeyboard::KeyCode::KEY_DOWN_ARROW,
        EventKeyboard::KeyCode::KEY_SPACE,
        EventKeyboard::KeyCode::KEY_J
    };
}
