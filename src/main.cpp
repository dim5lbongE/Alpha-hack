#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

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
    struct Fields { bool nativeSync = false; };
    void applyMusicSetting() {
        m_practiceMusicSync = Mod::get()->getSettingValue<bool>("enabled") || m_fields->nativeSync;
    }
    bool init(GJGameLevel* level, bool dontCreate) {
        if (!PlayLayer::init(level, dontCreate)) return false;
        m_fields->nativeSync = m_practiceMusicSync;
        applyMusicSetting();
        return true;
    }
    void togglePracticeMode(bool practice) {
        applyMusicSetting();
        PlayLayer::togglePracticeMode(practice);
        applyMusicSetting();
    }
    void toggleMusicInPractice() {
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

$on_mod(Loaded) {
    listenForSettingChanges<bool>("enabled", [](bool) {
        if (auto play = PlayLayer::get()) static_cast<LitePractice*>(play)->applyMusicSetting();
    });
}
