#include "HackMenu.hpp"
#include "../Hacks.hpp"

using namespace geode::prelude;

namespace mh {

HackMenu* HackMenu::create() {
    auto ret = new HackMenu();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void HackMenu::open() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return;
    if (scene->getChildByID("mh-hack-menu"_spr)) return;

    auto menu = HackMenu::create();
    menu->setID("mh-hack-menu"_spr);
    scene->addChild(menu, 1000);
}

bool HackMenu::init() {
    if (!CCLayer::init()) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto dim = CCLayerColor::create({0, 0, 0, 120});
    this->addChild(dim);

    m_panel = CCLayerColor::create({20, 22, 30, 235});
    m_panel->setContentSize({340.f, 320.f});
    m_panel->setPosition({winSize.width / 2 - 170.f, winSize.height / 2 - 160.f});
    m_panel->setID("mh-panel"_spr);
    this->addChild(m_panel);

    auto border = CCDrawNode::create();
    border->drawRect({0, 0}, {340.f, 320.f}, ccColor4F{0.f, 0.f, 0.f, 0.f}, 2.f, ccColor4F{1.f, 0.45f, 0.2f, 1.f});
    m_panel->addChild(border);

    auto title = CCLabelBMFont::create("MegaHack", "bigFont.fnt");
    title->setScale(0.6f);
    title->setPosition({170.f, 300.f});
    title->setID("mh-title"_spr);
    m_panel->addChild(title);

    auto hint = CCLabelBMFont::create("sandbox only: editor playtest", "chatFont.fnt");
    hint->setScale(0.6f);
    hint->setOpacity(140);
    hint->setPosition({170.f, 284.f});
    hint->setID("mh-hint"_spr);
    m_panel->addChild(hint);

    auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSpr->setScale(0.7f);
    auto closeBtn = CCMenuItemSpriteExtra::create(
        closeSpr, this, menu_selector(HackMenu::onClose)
    );
    closeBtn->setPosition({322.f, 302.f});
    closeBtn->setID("mh-close"_spr);

    auto closeMenu = CCMenu::create();
    closeMenu->addChild(closeBtn);
    closeMenu->setPosition({0.f, 0.f});
    m_panel->addChild(closeMenu);

    this->rebuild();
    return true;
}

void HackMenu::rebuild() {
    if (auto old = m_panel->getChildByID("mh-list"_spr)) old->removeFromParent();

    auto count = static_cast<int>(Hack::_Count);
    constexpr float rowH = 42.f;
    float height = rowH * static_cast<float>(count);

    auto list = CCLayer::create();
    list->setContentSize({320.f, height});
    list->setID("mh-list"_spr);

    auto toggleMenu = CCMenu::create();
    toggleMenu->setPosition({0.f, 0.f});
    list->addChild(toggleMenu);

    for (int i = 0; i < count; ++i) {
        auto hack = static_cast<Hack>(i);
        auto info = mh::info(hack);
        float y = height - rowH * static_cast<float>(i) - rowH / 2.f;

        auto label = CCLabelBMFont::create(info->name, "bigFont.fnt");
        label->setScale(0.42f);
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition({14.f, y + 6.f});
        label->setID(fmt::format("mh-label-{}"_spr.c_str(), info->id));
        list->addChild(label);

        auto note = CCLabelBMFont::create(info->note, "chatFont.fnt");
        note->setScale(0.5f);
        note->setAnchorPoint({0.f, 0.5f});
        note->setOpacity(130);
        note->setPosition({14.f, y - 9.f});
        list->addChild(note);

        auto off = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto on = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        auto toggle = CCMenuItemToggler::create(
            off, on, this, menu_selector(HackMenu::onToggle)
        );
        toggle->toggle(mh::enabled(hack));
        toggle->setTag(i);
        toggle->setPosition({296.f, y});
        toggle->setID(fmt::format("mh-toggle-{}"_spr.c_str(), info->id));
        toggleMenu->addChild(toggle);
    }

    auto scroll = geode::ScrollLayer::create({320.f, 240.f});
    scroll->m_contentLayer->addChild(list);
    list->setPosition({0.f, 0.f});
    m_panel->addChildAtPosition(scroll, Anchor::BottomLeft, {10.f, 14.f});
    scroll->setID("mh-scroll"_spr);
}

void HackMenu::onToggle(CCObject* sender) {
    auto toggle = static_cast<CCMenuItemToggler*>(sender);
    auto hack = static_cast<Hack>(toggle->getTag());

    bool nowOn = !mh::enabled(hack);
    mh::setEnabled(hack, nowOn);

    if (hack == Hack::Noclip || hack == Hack::Bot) {
        if (nowOn && !mh::sandboxActive()) {
            Notification::create("Will apply only in an editor playtest", NotificationIcon::Info)->show();
        }
    }
}

void HackMenu::onClose(CCObject*) {
    this->removeFromParent();
}

} // namespace mh
