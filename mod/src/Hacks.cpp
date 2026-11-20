#include "../Hacks.hpp"

using namespace geode::prelude;

namespace mh {

namespace {
    HackInfo const kInfo[] = {
        { "noclip",        "Noclip",            "Ignore death. Sandbox only." },
        { "bot",           "Bot (auto jump)",   "Jumps on ground contact. Sandbox only." },
        { "speedhack",     "Speed",             "Game speed multiplier. Sandbox only." },
        { "hitbox",        "Hitboxes",          "Draw collision boxes instead of sprites." },
        { "hitbox-player", "Player hitbox",     "Box around the player." },
        { "practice-music","Practice music",    "Keep the level song when dying, no slowdown." },
        { "show-perms",    "Show permissions",  "Print the loaded mod list to the console." },
        { "safe-mode",     "Safe Mode",         "Cheat runs are never recorded." },
    };
    static_assert(sizeof(kInfo) / sizeof(kInfo[0]) == static_cast<size_t>(Hack::_Count));

    struct State {
        bool cheat = false;
        bool blocked = false;
    };

    State& state() {
        static State s;
        return s;
    }

    bool settingBool(char const* id) {
        return Mod::get()->getSettingValue<bool>(id);
    }
}

HackInfo const* info(Hack hack) {
    return &kInfo[static_cast<size_t>(hack)];
}

bool enabled(Hack hack) {
    return settingBool(info(hack)->id);
}

bool safeModeOn() {
    return settingBool("safe-mode");
}

bool checksumSafeRun(PlayLayer* layer) {
    if (!layer) return false;
    auto level = layer->m_level;
    if (!level) return false;
    return level->m_levelType == GJLevelType::Editor;
}

bool sandboxActive() {
    auto layer = PlayLayer::get();
    if (!layer) return false;
    return checksumSafeRun(layer);
}

bool cheatUsed() {
    return state().cheat;
}

void markCheat() {
    state().cheat = true;
    if (safeModeOn()) state().blocked = true;
}

void markBlocked() {
    state().blocked = true;
}

void applySpeed() {
    auto dir = CCDirector::sharedDirector();
    if (!dir) return;
    float speed = 1.f;
    if (settingBool("speedhack")) {
        speed = static_cast<float>(Mod::get()->getSettingValue<double>("speed-value"));
    }
    dir->getScheduler()->setTimeScale(speed);
}

void setEnabled(Hack hack, bool value) {
    Mod::get()->setSettingValue(info(hack)->id, value);
    if (hack == Hack::Speedhack) {
        if (auto dir = CCDirector::sharedDirector()) {
            dir->getScheduler()->setTimeScale(
                value ? static_cast<float>(Mod::get()->getSettingValue<double>("speed-value")) : 1.f
            );
        }
    }
}

void initSettings() {
    listenForSettingChanges<bool>("speedhack", [](bool on) {
        if (auto dir = CCDirector::sharedDirector()) {
            auto speed = static_cast<float>(Mod::get()->getSettingValue<double>("speed-value"));
            dir->getScheduler()->setTimeScale(on ? speed : 1.f);
        }
    });
    listenForSettingChanges<double>("speed-value", [](double v) {
        if (!settingBool("speedhack")) return;
        if (auto dir = CCDirector::sharedDirector()) {
            dir->getScheduler()->setTimeScale(static_cast<float>(v));
        }
    });
}

} // namespace mh
