#pragma once
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <algorithm>
#include <vector>
using namespace geode::prelude;

class $modify(LiteStartPositions, PlayLayer) {
    struct Original {
        StartPosObject* object;
        int mode;
        Speed speed;
        bool mini, dual;
    };
    struct Fields {
        std::vector<StartPosObject*> starts;
        std::vector<GameObject*> portals;
        std::vector<Original> originals;
        CCMenu* controls = nullptr;
        CCLabelBMFont* counter = nullptr;
        bool ready = false;
        bool switching = false;
    };
    bool enabled() {
        return Mod::get()->getSettingValue<bool>("startpos-switcher");
    }
    void remember(StartPosObject* object) {
        auto& saved = m_fields->originals;
        if (!object->m_startSettings || std::any_of(saved.begin(), saved.end(), [object](auto const& x){return x.object == object;})) return;
        auto s = object->m_startSettings;
        saved.push_back({object, s->m_startMode, s->m_startSpeed, s->m_startMini, s->m_startDual});
    }
    void prepareStarts() {
        if (!m_levelSettings) return;
        for (auto start : m_fields->starts) remember(start);
        bool smart = Mod::get()->getSettingValue<bool>("smart-startpos") && !m_levelSettings->m_platformerMode;
        std::stable_sort(m_fields->portals.begin(),m_fields->portals.end(),[](auto a,auto b){return a->getPositionX()<b->getPositionX();});
        // Rebuild from saved settings on every reset so disabling Smart restores manual values.
        for (auto const& old : m_fields->originals) {
            auto s = old.object->m_startSettings;
            s->m_startMode=old.mode; s->m_startSpeed=old.speed;
            s->m_startMini=old.mini; s->m_startDual=old.dual;
            if (!smart) continue;
            s->m_startMode=m_levelSettings->m_startMode;
            s->m_startSpeed=m_levelSettings->m_startSpeed;
            s->m_startMini=m_levelSettings->m_startMini;
            s->m_startDual=m_levelSettings->m_startDual;
            for (auto portal : m_fields->portals) {
                if (portal->getPositionX()>old.object->getPositionX()) break;
                switch (portal->m_objectID) {
                    case 12:s->m_startMode=0;break;
                    case 13:s->m_startMode=1;break;
                    case 47:s->m_startMode=2;break;
                    case 111:s->m_startMode=3;break;
                    case 660:s->m_startMode=4;break;
                    case 745:s->m_startMode=5;break;
                    case 1331:s->m_startMode=6;break;
                    case 1933:s->m_startMode=7;break;
                    case 99:s->m_startMini=false;break;
                    case 101:s->m_startMini=true;break;
                    case 286:s->m_startDual=true;break;
                    case 287:s->m_startDual=false;break;
                    case 200:s->m_startSpeed=Speed::Slow;break;
                    case 201:s->m_startSpeed=Speed::Normal;break;
                    case 202:s->m_startSpeed=Speed::Fast;break;
                    case 203:s->m_startSpeed=Speed::Faster;break;
                    case 1334:s->m_startSpeed=Speed::Fastest;break;
                }
            }
        }
    }
    void addObject(GameObject* object) {
        PlayLayer::addObject(object);
        if (!object) return;
        switch(object->m_objectID) {
            case 31:m_fields->starts.push_back(static_cast<StartPosObject*>(object));break;
            case 12:case 13:case 47:case 111:case 660:case 745:case 1331:case 1933:
            case 99:case 101:case 286:case 287:case 200:case 201:case 202:case 203:case 1334:
                m_fields->portals.push_back(object);break;
        }
    }
    void resetLevel() {
        prepareStarts();
        PlayLayer::resetLevel();
        refreshControls();
    }
    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        auto& starts=m_fields->starts;
        std::stable_sort(starts.begin(),starts.end(),[](auto a,auto b){return a->getPositionX()<b->getPositionX();});
        m_fields->ready=true;
        if (starts.empty() || !m_uiLayer || m_fields->controls) return;
        prepareStarts();
        auto menu=CCMenu::create();menu->setPosition({CCDirector::sharedDirector()->getWinSize().width/2,28});
        m_uiLayer->addChild(menu,100);m_fields->controls=menu;
        for (int direction : {-1,1}) {
            auto sprite=CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");sprite->setScale(.65f);sprite->setFlipX(direction>0);
            auto item=CCMenuItemSpriteExtra::create(sprite,this,menu_selector(LiteStartPositions::onArrow));
            item->setTag(direction);item->setPosition({direction*60.f,0});menu->addChild(item);
        }
        auto label=CCLabelBMFont::create("","bigFont.fnt");label->setScale(.45f);menu->addChild(label);m_fields->counter=label;
        refreshControls();
    }
    void refreshControls() {
        if (!m_fields->controls) return;
        m_fields->controls->setVisible(enabled()&&!m_hasCompletedLevel);
        auto const& starts=m_fields->starts;
        auto it=std::find(starts.begin(),starts.end(),m_startPosObject);
        size_t index=it==starts.end()?0:static_cast<size_t>(it-starts.begin())+1;
        m_fields->counter->setString(fmt::format("{}/{}",index,starts.size()).c_str());
    }
    bool selectRelative(int direction) {
        if (!enabled() || !m_fields->ready || m_fields->switching || m_isPaused || m_hasCompletedLevel || m_fields->starts.empty()) return false;
        auto const& starts=m_fields->starts;
        auto it=std::find(starts.begin(),starts.end(),m_startPosObject);
        int current=it==starts.end()?0:static_cast<int>(it-starts.begin())+1;
        int total=static_cast<int>(starts.size())+1;
        int next=(current+direction+total)%total;
        m_fields->switching=true;
        removeAllCheckpoints();
        prepareStarts();
        // Every switched run remains a test run, including switching back to 0%.
        m_isTestMode=true;
        switchToStartPosObject(next?starts[next-1]:nullptr);
        m_isTestMode=true;
        updateTestModeLabel();refreshControls();
        m_fields->switching=false;
        return true;
    }
    void onArrow(CCObject* sender) { selectRelative(sender->getTag()); }
    void levelComplete() {
        PlayLayer::levelComplete();
        refreshControls();
    }
};
class $modify(LiteStartKeys, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key,bool down,bool repeat,double time) {
        if (down&&!repeat&&(key==KEY_Q||key==KEY_E)) {
            if (auto p=PlayLayer::get();p&&static_cast<LiteStartPositions*>(p)->selectRelative(key==KEY_Q?-1:1)) return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key,down,repeat,time);
    }
};
