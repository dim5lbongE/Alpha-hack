#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/utils/file.hpp>
#include <cctype>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <cwctype>
#include <unordered_map>
#include <fmod.hpp>
#include "core.hpp"
#include "physics.hpp"
using namespace geode::prelude;
namespace fs=std::filesystem;
namespace nova {
struct Config {
 bool music=true,noclip=false,speedhack=false,bypass=false,tpsBypass=false,showFPS=true,clock=true,indicator=true,accuracy=true,icons=false,levels=false,hitboxes=false,audio=true,vsync=false,smooth=false;
 double speed=1,fps=240,tps=240;
} c;
struct Run { PlayLayer* layer=nullptr; bool dirty=false,nativeTest=false,hit=false; uint64_t total=0,bad=0;uint32_t reasons=0;float appliedSpeed=1.f;uint64_t updates=0; } run;
class Menu;
struct TPSContext { GJBaseGameLayer* layer=nullptr;bool enabled=false; };
TPSContext tpsContext;
Menu* menu=nullptr;
void openMenu();
bool picker=false,fpsApplied=false;
double originalInterval=1./60.; bool originalVsync=true; float lastPitch=-1;
void load() {
 auto m=Mod::get();
 c.vsync=m->getSettingValue<bool>("vertical-sync");c.smooth=m->getSettingValue<bool>("smooth-fix");
 c.music=m->getSettingValue<bool>("enabled"); c.noclip=m->getSettingValue<bool>("noclip"); c.speedhack=m->getSettingValue<bool>("speedhack"); c.bypass=m->getSettingValue<bool>("fps-bypass"); c.tpsBypass=m->getSettingValue<bool>("tps-bypass");
 c.showFPS=m->getSettingValue<bool>("show-fps"); c.clock=m->getSettingValue<bool>("show-clock"); c.indicator=m->getSettingValue<bool>("cheat-indicator"); c.accuracy=m->getSettingValue<bool>("show-accuracy");
 c.icons=m->getSettingValue<bool>("unlock-icons"); c.levels=m->getSettingValue<bool>("unlock-levels"); c.hitboxes=m->getSettingValue<bool>("hitboxes"); c.audio=m->getSettingValue<bool>("audio-speed");
 c.speed=core::speed(m->getSettingValue<double>("speed")); c.fps=core::fps(m->getSettingValue<int64_t>("fps-target")); c.tps=core::tps(m->getSettingValue<int64_t>("tps-target"));
}
uint32_t currentReasons(){return core::reasons(c.noclip,c.speedhack,c.speed,c.tpsBypass&&physics::ready,c.tps,c.hitboxes,c.smooth);}
bool cheats(){return currentReasons()!=0;}
std::string reasonNames(uint32_t mask){std::string s;for(auto [bit,name]:{std::pair{1u,"Noclip"},{2u,"Speed"},{4u,"TPS"},{8u,"Hitboxes"},{16u,"Smooth Fix"}})if(mask&bit){if(!s.empty())s+=" + ";s+=name;}return s.empty()?"None":s;}
void mark(){if(run.layer&&!run.layer->m_hasCompletedLevel&&cheats()){run.reasons|=currentReasons();run.dirty=true;run.layer->m_isTestMode=true;}}
void sync(PlayLayer* p) { if(p&&c.music) p->m_practiceMusicSync=true; }
void pitch(float n) {
 auto a=FMODAudioEngine::sharedEngine();
 if(a&&a->m_backgroundMusicChannel) { a->m_backgroundMusicChannel->setPitch(n); lastPitch=n; }
}
void applyFPS() {
 static bool busy=false; if(busy) return; busy=true;
 CCDirector::sharedDirector()->setSmoothFix(c.smooth);
#ifdef GEODE_IS_WINDOWS
 CCApplication::sharedApplication()->toggleVerticalSync(c.vsync);
#endif
 auto app=CCApplication::sharedApplication();
 if(c.bypass) {
  if(!fpsApplied) {
   originalInterval=CCDirector::sharedDirector()->getAnimationInterval();
   if(!std::isfinite(originalInterval)||originalInterval<=0) originalInterval=CCDirector::sharedDirector()->getAnimationInterval();
   if(!std::isfinite(originalInterval)||originalInterval<=0) originalInterval=1./60.;
   
  }
  fpsApplied=true;  app->setAnimationInterval(1./c.fps);
 } else if(fpsApplied) { fpsApplied=false; app->setAnimationInterval(originalInterval); }
 busy=false;
}
std::string utf8(fs::path const& p) { auto s=p.u8string(); return std::string(s.begin(),s.end()); }
std::string songKey(bool custom,int id) { return fmt::format("song-{}-{}",custom?"ng":"main",id); }
std::unordered_map<std::string,std::string> songs;
std::string replacement(bool custom,int id) {
 auto key=songKey(custom,id); if(auto i=songs.find(key);i!=songs.end()) return i->second;
 auto path=Mod::get()->getSavedValue<std::string>(key,""); std::error_code ec;
 if(!path.empty()&&!fs::is_regular_file(fs::path(std::u8string(path.begin(),path.end())),ec)) path.clear();
 songs[key]=path; return path;
}
// A neutral, text-free mobile launcher with a circular 40-point touch target.
CCNode* launcherSprite(){
 auto node=CCNode::create();node->setContentSize({40,40});
 auto circle=CCDrawNode::create();circle->drawDot({20,20},20,{.08f,.13f,.20f,1});circle->drawDot({20,20},18,{.22f,.32f,.44f,1});node->addChild(circle);
 for(float y:{13.f,19.f,25.f}){auto bar=CCLayerColor::create({255,255,255,255},18,2);bar->setPosition({11,y});node->addChild(bar);}
 return node;
}
class HUD:public CCNode {
 CCLabelBMFont* text=nullptr;CCMenu* buttons=nullptr;CCDrawNode* dot=nullptr;
 std::chrono::steady_clock::time_point last=std::chrono::steady_clock::now();
 double elapsed=0;unsigned frames=0;
public:
 static HUD* create(){auto p=new HUD();if(p->init()){p->autorelease();return p;}delete p;return nullptr;}
 bool init() override {
  if(!CCNode::init())return false;setID("nova-hud"_spr);
  buttons=CCMenu::create();buttons->setPosition({0,0});addChild(buttons,100);
#ifdef GEODE_IS_ANDROID
  auto button=CCMenuItemSpriteExtra::create(launcherSprite(),this,menu_selector(HUD::onMenu));
  button->setPosition({CCDirector::sharedDirector()->getWinSize().width-26,CCDirector::sharedDirector()->getWinSize().height-74});buttons->addChild(button);
#endif
  dot=CCDrawNode::create();addChild(dot,2);
  text=CCLabelBMFont::create("","chatFont.fnt");text->setAnchorPoint({0,1});text->setScale(.65f);text->setColor({255,255,255});addChild(text);return true;
 }
 void onMenu(CCObject*){openMenu();}
 void update(float) override {
  auto p=PlayLayer::get();setVisible(p!=nullptr);buttons->setVisible(p&&!menu&&!p->m_hasCompletedLevel);
  dot->clear();if(p&&(c.indicator||p->m_hasCompletedLevel)){
   bool red=run.layer==p&&run.dirty;
   dot->drawDot({10,10},4,{0,0,0,.85f});dot->drawDot({10,10},2.7f,red?ccColor4F{1,.20f,.20f,1}:ccColor4F{.20f,1,.35f,1});
  }
  auto now=std::chrono::steady_clock::now();elapsed+=std::chrono::duration<double>(now-last).count();last=now;++frames;
  if(elapsed<.25)return;double fps=frames/elapsed;frames=0;elapsed=0;
  text->setPosition({8,CCDirector::sharedDirector()->getWinSize().height-6});std::string s;
  if(c.showFPS)s+=fmt::format("FPS {:.0f}\n",fps);
  if(c.clock){auto t=std::time(nullptr);std::tm tm{};
#ifdef GEODE_IS_WINDOWS
   localtime_s(&tm,&t);
#else
   localtime_r(&t,&tm);
#endif
   s+=fmt::format("{:02}:{:02}:{:02}\n",tm.tm_hour,tm.tm_min,tm.tm_sec);
  }
  if(c.accuracy&&p&&c.noclip)s+=fmt::format("ACC {:.2f}%",core::accuracy(run.total,run.bad));
  text->setString(s.c_str());
 }
};
void hud() {
 auto scene=CCDirector::sharedDirector()->getRunningScene();if(!scene)return;
 auto overlay=typeinfo_cast<HUD*>(scene->getChildByID("nova-hud"_spr));
 if(!overlay){overlay=HUD::create();scene->addChild(overlay,9998);}
 overlay->update(0);
}
}
