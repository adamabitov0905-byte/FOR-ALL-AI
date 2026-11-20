#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace mh {

// Every hack the mod exposes. Order here is the order in the menu.
enum class Hack {
    Noclip,
    Bot,
    Speedhack,
    Hitbox,
    HitboxPlayer,
    PracticeMusic,
    ShowPerms,
    SafeMode,
    _Count
};

struct HackInfo {
    char const* id;    // setting key in mod.json
    char const* name;  // label in the menu
    char const* note;  // one-line hint under the row
};

HackInfo const* info(Hack hack);

// Reads the setting and applies side effects (like time scale for speedhack).
bool enabled(Hack hack);
void setEnabled(Hack hack, bool value);

// Called once at load: hooks setting changes so speedhack re-applies live.
void initSettings();

// PlayLayer-scoped helpers.
bool sandboxActive();
bool safeModeOn();
bool cheatUsed();
void markCheat();
void markBlocked();

// Applies the speedhack value to the director, or resets it to 1.
void applySpeed();

// Level types where a cheat must never count as a real completion.
bool checksumSafeRun(PlayLayer* layer);

} // namespace mh
