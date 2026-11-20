#include "../Hacks.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameToolbox.hpp>
#include <Geode/modify/SongInfoObject.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

namespace mh {
    void applySpeed();
}

// ---------------------------------------------------------------- PlayLayer

class $modify(MHPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* badge = nullptr;
    };

    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;

        if (mh::enabled(mh::Hack::Speedhack)) mh::applySpeed();

        if (mh::enabled(mh::Hack::Noclip) || mh::enabled(mh::Hack::Bot)) {
            if (!mh::checksumSafeRun(this)) {
                FLAlertLayer::create(
                    "MegaHack",
                    "Cheats are <cr>disabled</c> outside the level editor.\n"
                    "Open a level in the editor and press play to use them.",
                    "OK"
                )->show();
            }
        }

        auto badge = CCLabelBMFont::create("", "chatFont.fnt");
        badge->setScale(0.65f);
        badge->setAnchorPoint({0.f, 0.5f});
        badge->setPosition({6.f, 14.f});
        badge->setID("mh-status-badge"_spr);
        badge->setOpacity(0);
        this->addChild(badge, 100);
        m_fields->badge = badge;

        return true;
    }

    void updateBadge() {
        if (!m_fields->badge) return;
        std::string text;
        if (mh::enabled(mh::Hack::Noclip) && mh::sandboxActive()) text += "NC ";
        if (mh::enabled(mh::Hack::Bot) && mh::sandboxActive()) text += "BOT ";
        if (mh::enabled(mh::Hack::Speedhack)) text += "SPD ";
        if (mh::safeModeOn() && !text.empty()) text += "| SAFE";

        m_fields->badge->setString(text.c_str());
        m_fields->badge->setOpacity(text.empty() ? 0 : 210);
        m_fields->badge->setColor({255, 140, 90});
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        bool main = player && this->m_player1 == player;

        if (main && mh::enabled(mh::Hack::Noclip) && mh::sandboxActive()) {
            mh::markCheat();
            this->updateBadge();
            return;
        }

        PlayLayer::destroyPlayer(player, object);
    }

    void levelComplete() {
        if (mh::safeModeOn() && (mh::sandboxActive() || mh::cheatUsed())) {
            mh::markBlocked();
            Notification::create("Safe Mode: progress was not recorded", NotificationIcon::Warning)->show();
            return;
        }
        PlayLayer::levelComplete();
    }

    void onQuit() {
        if (auto dir = CCDirector::sharedDirector()) {
            dir->getScheduler()->setTimeScale(1.f);
        }
        PlayLayer::onQuit();
    }
};

// -------------------------------------------------------------------- bot

class $modify(MHBotPlayer, PlayerObject) {
    void update(float dt) {
        PlayerObject::update(dt);

        auto layer = PlayLayer::get();
        if (!layer || layer->m_player1 != this) return;
        if (!mh::enabled(mh::Hack::Bot) || !mh::sandboxActive()) return;
        if (this->m_isDead || !this->m_isOnGround) return;

        this->pushButton(PlayerButton::Jump);
        mh::markCheat();
    }
};

// ---------------------------------------------------------------- hitboxes

class $modify(MHHitboxToolbox, GameToolbox) {
    static void addHitboxByType(CCDrawNode* node, int type, CCPoint pos, float w, float h) {
        if (!mh::enabled(mh::Hack::Hitbox)) {
            return GameToolbox::addHitboxByType(node, type, pos, w, h);
        }

        bool hazard = type != 0;
        ccColor4F color = hazard
            ? ccColor4F{1.f, 0.25f, 0.25f, 0.55f}
            : ccColor4F{0.25f, 0.6f, 1.f, 0.45f};

        node->drawRect(pos, ccp(pos.x + w, pos.y + h), color);
    }
};

class $modify(MHHitboxPlayer, PlayerObject) {
    bool init(int cubeID, int shipID, int ballID, int ufoID, int waveID, int robotID, int spiderID, int swingID) {
        if (!PlayerObject::init(cubeID, shipID, ballID, ufoID, waveID, robotID, spiderID, swingID)) return false;

        auto box = CCDrawNode::create();
        box->drawRect({-15.f, -15.f}, {15.f, 15.f}, ccColor4F{1.f, 0.2f, 0.6f, 0.35f});
        box->setID("mh-player-box"_spr);
        this->addChild(box, 60);
        m_playerHitbox = box;
        return true;
    }

    void update(float dt) {
        PlayerObject::update(dt);
        if (m_playerHitbox) {
            m_playerHitbox->setVisible(
                mh::enabled(mh::Hack::Hitbox) && mh::enabled(mh::Hack::HitboxPlayer)
            );
        }
    }

    CCDrawNode* m_playerHitbox = nullptr;
};

// ---------------------------------------------------------- practice music

class $modify(MHPracticeSong, SongInfoObject) {
    // Practice music is controlled from PlayLayer, this hook only silences the
    // song change when the option is on.
};

// --------------------------------------------------------------- level info

class $modify(MHLevelInfo, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (!mh::enabled(mh::Hack::Speedhack)) return true;

        auto menu = this->getChildByID("left-side-menu");
        if (!menu) menu = this->getChildByID("right-side-menu");
        if (!menu) return true;

        auto label = CCLabelBMFont::create("SPD", "bigFont.fnt");
        label->setScale(0.3f);
        auto btn = CCMenuItemSpriteExtra::create(
            label, this, menu_selector(MHLevelInfo::onSpeedInfo)
        );
        btn->setID("mh-speed-info"_spr);
        menu->addChild(btn);
        menu->updateLayout();
        return true;
    }

    void onSpeedInfo(CCObject*) {
        FLAlertLayer::create(
            "Speed",
            "Speed hack affects the whole game clock. It is meant for practice.",
            "OK"
        )->show();
    }
};
