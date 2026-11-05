#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include "menu.hpp"
class $modify(AlphaFrame,CCDirector){
 void drawScene(){nova::hud();CCDirector::drawScene();}
};
// Scale the scheduler delta exactly once; do not also scale the physics delta
// or CCDirector's wall clock. Menu-only scenes remain at normal speed.
class $modify(AlphaSpeedClock,CCScheduler){
 void update(float dt){
  auto p=PlayLayer::get();bool active=p&&!p->m_isPaused&&!p->m_hasCompletedLevel;
  if(active&&nova::run.layer!=p){nova::run={};nova::run.layer=p;nova::run.nativeTest=p->m_isTestMode;}
  float scale=active&&nova::c.speedhack?static_cast<float>(nova::c.speed):1.f;
  if(active){nova::mark();nova::run.appliedSpeed=scale;nova::pitch(nova::c.audio?scale:1.f);}
  CCScheduler::update(dt*scale);
 }
};
class $modify(NovaKeys,CCKeyboardDispatcher){
 bool dispatchKeyboardMSG(enumKeyCodes k,bool down,bool repeat,double timestamp){
  if(k==KEY_Tab&&!nova::picker){if(down&&!repeat)nova::openMenu();return true;}
  if(nova::menu&&k==KEY_Escape){if(down&&!repeat)nova::menu->onClose(nullptr);return true;}
  return CCKeyboardDispatcher::dispatchKeyboardMSG(k,down,repeat,timestamp);
 }
};
class $modify(NovaMenu,MenuLayer){
 bool init(){
  if(!MenuLayer::init())return false;nova::load();nova::applyFPS();
#ifdef GEODE_IS_ANDROID
  auto m=CCMenu::create();m->setPosition({0,0});addChild(m,100);
  auto b=CCMenuItemSpriteExtra::create(nova::launcherSprite(),this,menu_selector(NovaMenu::onNova));
  b->setPosition({CCDirector::sharedDirector()->getWinSize().width-26,CCDirector::sharedDirector()->getWinSize().height-74});m->addChild(b);
#endif
  return true;
 }
 void onNova(CCObject*){nova::openMenu();}
};
class $modify(NovaPlay,PlayLayer){
 bool init(GJGameLevel* l,bool dontCreate){
  if(!PlayLayer::init(l,dontCreate))return false;nova::run={};nova::run.layer=this;nova::run.nativeTest=m_isTestMode;nova::mark();nova::sync(this);nova::lastPitch=-1;return true;
 }
 void onEnterTransitionDidFinish(){PlayLayer::onEnterTransitionDidFinish();nova::applyFPS();}
 void toggleMusicInPractice(){PlayLayer::toggleMusicInPractice();nova::sync(this);}
 void togglePracticeMode(bool p){if(p)nova::sync(this);PlayLayer::togglePracticeMode(p);}
 void destroyPlayer(PlayerObject* p,GameObject* o){
  if(nova::c.noclip&&p&&(o!=m_anticheatSpike||!m_anticheatSpike)&&!m_hasCompletedLevel){nova::mark();nova::run.hit=true;return;}
  PlayLayer::destroyPlayer(p,o);
 }
 void resetLevel(){
  if(nova::run.layer==this&&nova::run.dirty)m_isTestMode=true;
  PlayLayer::resetLevel();
  if(nova::run.layer==this){nova::run.total=0;nova::run.bad=0;nova::run.hit=false;
   if(!m_isPracticeMode)nova::run.reasons=0;nova::run.reasons|=nova::currentReasons();
   nova::run.dirty=core::dirtyAfterReset(nova::run.dirty,m_isPracticeMode,nova::cheats());m_isTestMode=nova::run.nativeTest||nova::run.dirty;nova::sync(this);}
 }
 bool shouldDrawDebugHitboxes(){return nova::c.hitboxes||PlayLayer::shouldDrawDebugHitboxes();}
 void levelComplete(){nova::mark();if(nova::run.dirty)m_isTestMode=true;PlayLayer::levelComplete();}
 void onExit(){nova::pitch(1.f);PlayLayer::onExit();if(nova::run.layer==this)nova::run={};}
};
class $modify(NovaSimulation,GJBaseGameLayer){
 struct Fields { double lastRate=240; };
 double getModifiedDelta(float dt){
  if(nova::tpsContext.layer!=this||!nova::tpsContext.enabled)return GJBaseGameLayer::getModifiedDelta(dt);
  if(m_resumeTimer>0){--m_resumeTimer;dt=0;}
  double remaining=m_extraDelta;
  auto batch=core::advance(dt,nova::c.tps,m_gameState.m_timeWarp,remaining);
  m_extraDelta=remaining;physics::expectedTicks=std::max(1u,batch.ticks);return batch.delta;
 }
 void update(float dt){
  auto p=PlayLayer::get();bool active=p&&static_cast<GJBaseGameLayer*>(p)==this&&!p->m_isPaused&&!p->m_hasCompletedLevel;
  if(!active){GJBaseGameLayer::update(dt);return;}
  if(nova::run.layer!=p){nova::run={};nova::run.layer=p;nova::run.nativeTest=p->m_isTestMode;}
  nova::mark();++nova::run.updates;nova::run.hit=false;
  bool custom=physics::ready&&core::customTPS(nova::c.tpsBypass,nova::c.tps)&&m_started&&!m_isEditor;
  double rate=custom?nova::c.tps:240.;if(m_fields->lastRate!=rate){m_extraDelta=0;m_fields->lastRate=rate;}
  const auto previous=nova::tpsContext;
  physics::expectedTicks=1; // Native code divides by this count, including zero-delta frames.
  physics::Scope patchScope(custom);nova::tpsContext={this,patchScope.enabled};
  bool track=nova::c.noclip&&dt>0&&!p->m_inResetDelay&&p->m_player1&&!p->m_player1->m_isDead;
  GJBaseGameLayer::update(dt);
  nova::tpsContext=previous;
  if(track&&nova::run.layer==p){++nova::run.total;if(nova::run.hit)++nova::run.bad;}
 }
};
class $modify(NovaPause,PauseLayer){
 void onPracticeMode(CCObject* s){nova::sync(PlayLayer::get());PauseLayer::onPracticeMode(s);}
};
class $modify(NovaManager,GameManager){
 bool isIconUnlocked(int id,IconType t){if(nova::c.icons&&id>0&&t!=IconType::Item)return true;return GameManager::isIconUnlocked(id,t);}
 bool isColorUnlocked(int id,UnlockType t){if(nova::c.icons&&id>=0&&(t==UnlockType::Col1||t==UnlockType::Col2))return true;return GameManager::isColorUnlocked(id,t);}
 void updateCustomFPS(){GameManager::updateCustomFPS();if(nova::c.bypass)nova::applyFPS();}
};
class $modify(NovaMainLevels,LevelPage){
 bool init(GJGameLevel* l){int coins=l?l->m_requiredCoins:0;if(l&&nova::c.levels)l->m_requiredCoins=0;bool ok=LevelPage::init(l);if(l)l->m_requiredCoins=coins;return ok;}
 void onPlay(CCObject* s){auto l=m_level;int coins=l?l->m_requiredCoins:0;if(l&&nova::c.levels)l->m_requiredCoins=0;LevelPage::onPlay(s);if(l)l->m_requiredCoins=coins;}
};
class $modify(NovaLevelData,GJGameLevel){
 void savePercentage(int percent,bool practice,int clicks,int attempts,bool valid){
  if(nova::run.layer&&nova::run.layer->m_level==this&&nova::run.dirty)return;GJGameLevel::savePercentage(percent,practice,clicks,attempts,valid);
 }
 std::string getAudioFileName(){auto p=nova::replacement(m_songID>0,m_songID>0?m_songID:m_audioTrack);return p.empty()?GJGameLevel::getAudioFileName():p;}
};
class $modify(NovaSongPaths,MusicDownloadManager){
 std::string pathForSong(int id){auto p=nova::replacement(true,id);return p.empty()?MusicDownloadManager::pathForSong(id):p;}
};
class $modify(NovaBuiltInSongs,LevelTools){
 static std::string getAudioFileName(int id){auto p=nova::replacement(false,id);return p.empty()?LevelTools::getAudioFileName(id):p;}
};
$on_mod(Loaded){
 physics::initialize();nova::load();listenForAllSettingChanges([](std::string_view,std::shared_ptr<SettingV3>){nova::load();nova::mark();nova::applyFPS();});
 log::info("Alpha hack 2.3.1 beta loaded. Tab opens the menu.");
}
