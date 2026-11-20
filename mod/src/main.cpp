#include "Hacks.hpp"
#include "ui/HackMenu.hpp"
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// ------------------------------------------------------------------ pause

class $modify(MHPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        this->addHackButton();
    }

    void addHackButton() {
        auto menu = this->getChildByID("right-button-menu");
        if (!menu) menu = this->getChildByID("left-button-menu");
        if (!menu) return;

        auto spr = ButtonSprite::create("HACKS", "bigFont.fnt", "GJ_button_04.png", 0.6f);
        spr->setScale(0.75f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MHPauseLayer::onHackMenu)
        );
        btn->setID("mh-open-button"_spr);
        menu->addChild(btn);
        menu->updateLayout();
    }

    void onHackMenu(CCObject*) {
        mh::HackMenu::open();
    }
};

// ------------------------------------------------------------------- menu

class $modify(MHMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto menu = this->getChildByID("bottom-menu");
        if (!menu) return true;

        auto spr = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_starsIcon_001.png", 1.f, CircleBaseColor::Orange, CircleBaseSize::Medium
        );
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MHMenuLayer::onHackMenu)
        );
        btn->setID("mh-menu-button"_spr);
        menu->addChild(btn);
        menu->updateLayout();
        return true;
    }

    void onHackMenu(CCObject*) {
        mh::HackMenu::open();
    }
};

// ------------------------------------------------------------------ entry

$on_mod(Loaded) {
    mh::initSettings();

    listenForKeybindSettingPresses("open-menu", [](Keybind const&, bool down, bool repeat, double) {
        if (!down || repeat) return;
        mh::HackMenu::open();
    });

    log::info("MegaHack (Geode) loaded");
}
