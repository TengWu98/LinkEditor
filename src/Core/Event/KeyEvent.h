#pragma once

#include "pch.h"
#include "Event.h"

MESH_EDITOR_NAMESPACE_BEGIN

class KeyEvent : public Event
{
public:
    KeyCode GetKeyCode() const { return EventKeyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
    KeyEvent(const KeyCode InKeycode)
        : EventKeyCode(InKeycode) {}

    KeyCode EventKeyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(const KeyCode InKeycode, bool bIsRepeat = false)
        : KeyEvent(InKeycode), bIsRepeat(bIsRepeat) {}

    bool IsRepeat() const { return bIsRepeat; }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << EventKeyCode << " (repeat = " << bIsRepeat << ")";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    bool bIsRepeat;
};

class KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(const KeyCode InKeycode)
        : KeyEvent(InKeycode) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << EventKeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent
{
public:
    KeyTypedEvent(const KeyCode keycode)
        : KeyEvent(keycode) {}

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << EventKeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
};

MESH_EDITOR_NAMESPACE_END