#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace mh {

// Floating cheat menu: a scrollable list of toggles, opened from the
// pause menu button or the configured keybind.
class HackMenu : public CCLayer {
public:
    static HackMenu* create();
    static void open();

protected:
    bool init();
    void rebuild();

    void onToggle(CCObject* sender);
    void onClose(CCObject* sender);

    CCLayerColor* m_panel = nullptr;
    CCScrollLayerExt* m_scroll = nullptr;
};

} // namespace mh
