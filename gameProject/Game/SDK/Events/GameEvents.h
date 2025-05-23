// GameEvents.h
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

class IGameEvent {
public:
    virtual ~IGameEvent() = default;
    virtual void Execute() = 0;
    virtual std::string GetName() const = 0;
};

class CallbackEvent : public IGameEvent {
    std::function<void()> callback;
    std::string name;
public:
    CallbackEvent(const std::string& eventName, std::function<void()> cb)
        : name(eventName), callback(cb) {}

    void Execute() override {
        if (callback) callback();
    }
    std::string GetName() const override { return name; }
};
