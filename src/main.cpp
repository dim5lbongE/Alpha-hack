#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
using namespace geode::prelude;
#include "startpos.hpp"

class $modify(LiteIcons, GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        if (Mod::get()->getSettingValue<bool>("unlock-icons") && id > 0 && type != IconType::Item)
            return true;
        return GameManager::isIconUnlocked(id, type);
    }
    bool isColorUnlocked(int id, UnlockType type) {
        if (Mod::get()->getSettingValue<bool>("unlock-icons") && id >= 0 &&
            (type == UnlockType::Col1 || type == UnlockType::Col2)) return true;
        return GameManager::isColorUnlocked(id, type);
    }
};

class $modify(LitePractice, PlayLayer) {
    struct Fields { bool nativeSync = false; bool ready = false; };
    void applyMusicSetting() {
        if (!m_fields->ready) return;
        m_practiceMusicSync = Mod::get()->getSettingValue<bool>("enabled") || m_fields->nativeSync;
    }
    bool init(GJGameLevel* level, bool dontCreate) {
        if (!PlayLayer::init(level, dontCreate)) return false;
        m_fields->nativeSync = m_practiceMusicSync;
        m_fields->ready = true;
        applyMusicSetting();
        return true;
    }
    void togglePracticeMode(bool practice) {
        applyMusicSetting();
        PlayLayer::togglePracticeMode(practice);
        applyMusicSetting();
    }
    void toggleMusicInPractice() {
        if (!m_fields->ready) { PlayLayer::toggleMusicInPractice(); return; }
        // Let the game's own option retain its value under the override.
        m_practiceMusicSync = m_fields->nativeSync;
        PlayLayer::toggleMusicInPractice();
        m_fields->nativeSync = m_practiceMusicSync;
        applyMusicSetting();
    }
    void resetLevel() {
        PlayLayer::resetLevel();
        applyMusicSetting();
    }
};

// Keep downloaded level metadata unchanged outside the copy UI operation.
namespace {
struct CopyPermission {
    GJGameLevel* level;
    int password;
    explicit CopyPermission(GJGameLevel* value) : level(value), password(value ? value->m_password : 0) {
        if (level) level->m_password = 1;
    }
    ~CopyPermission() { if (level) level->m_password = password; }
};
}
class $modify(LiteCopy, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!level || !Mod::get()->getSettingValue<bool>("copy-bypass"))
            return LevelInfoLayer::init(level, challenge);
        CopyPermission permission(level);
        return LevelInfoLayer::init(level, challenge);
    }
    void tryCloneLevel(CCObject* sender) {
        if (!m_level || !Mod::get()->getSettingValue<bool>("copy-bypass")) {
            LevelInfoLayer::tryCloneLevel(sender);
            return;
        }
        CopyPermission permission(m_level);
        LevelInfoLayer::tryCloneLevel(sender);
    }
};

$on_mod(Loaded) {
    listenForSettingChanges<bool>("startpos-switcher", [](bool) {
        if(auto p=PlayLayer::get())static_cast<LiteStartPositions*>(p)->refreshControls();
    });
    listenForSettingChanges<bool>("enabled", [](bool) {
        if (auto play = PlayLayer::get()) static_cast<LitePractice*>(play)->applyMusicSetting();
    });
}
