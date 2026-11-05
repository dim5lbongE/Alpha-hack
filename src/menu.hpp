#pragma once
#include "runtime.hpp"
namespace nova {
class Menu:public Popup {
 CCNode* body=nullptr;CCMenu* controls=nullptr;TextInput* number=nullptr;TextInput* songID=nullptr;
 CCLabelBMFont* message=nullptr;Slider* speedSlider=nullptr;CCLabelBMFont* speedLabel=nullptr;
 bool songCustom=true,resumeOnClose=false;PlayLayer* pausedLayer=nullptr;int page=0;inline static int lastPage=0;
 struct Toggle{const char* key;const char* title;};std::vector<Toggle> toggles;
 const ccColor3B ink={30,53,78},muted={91,113,137},accent={36,145,134};
 CCScale9Sprite* panel(CCNode* parent,float x,float y,float w,float h,ccColor3B color){auto bg=CCScale9Sprite::create("square02_small.png");bg->setContentSize({w,h});bg->setPosition({x,y});bg->setColor(color);parent->addChild(bg);return bg;}
 CCLabelBMFont* text(CCNode* parent,std::string const& value,float x,float y,float scale,ccColor3B color,float width=0,bool left=false){
  auto l=CCLabelBMFont::create(value.c_str(),"chatFont.fnt");if(width>0)scale=std::min(scale,width/std::max(1.f,l->getContentSize().width));l->setScale(scale);l->setColor(color);l->setPosition({x,y});if(left)l->setAnchorPoint({0,.5f});parent->addChild(l);return l;
 }
 void label(std::string const& value,float x,float y,float scale=.55f){text(body,value,x,y,scale,muted,340);}
 void button(std::string const& value,float x,float y,int tag,SEL_MenuHandler fn,float width=160,float height=36,bool primary=false){
  auto bg=CCScale9Sprite::create("square02_small.png");bg->setContentSize({width,height});bg->setColor(primary?accent:ccColor3B{232,240,248});text(bg,value,width/2,height/2,.65f,primary?ccColor3B{255,255,255}:ink,width-16);
  auto b=CCMenuItemSpriteExtra::create(bg,this,fn);b->setTag(tag);b->setPosition({x,y});controls->addChild(b);
 }
 void toggle(char const* key,char const* title,float y){
  int tag=1000+static_cast<int>(toggles.size());toggles.push_back({key,title});bool on=Mod::get()->getSettingValue<bool>(key);
  auto bg=CCScale9Sprite::create("square02_small.png");bg->setContentSize({348,46});bg->setColor({248,251,255});text(bg,title,14,23,.64f,ink,242,true);
  auto badge=panel(bg,308,23,56,26,on?accent:ccColor3B{214,225,237});badge->setTag(10);
  auto state=text(bg,on?"ON":"OFF",308,23,.54f,on?ccColor3B{255,255,255}:muted);state->setTag(11);
  auto b=CCMenuItemSpriteExtra::create(bg,this,menu_selector(Menu::onToggle));b->setPosition({306,y});b->setTag(tag);controls->addChild(b);
 }
 void refreshToggle(char const* key){for(size_t i=0;i<toggles.size();++i)if(std::string_view(toggles[i].key)==key){auto item=typeinfo_cast<CCMenuItemSpriteExtra*>(controls->getChildByTag(1000+static_cast<int>(i)));if(!item)return;auto bg=item->getNormalImage();bool on=Mod::get()->getSettingValue<bool>(key);if(auto badge=typeinfo_cast<CCScale9Sprite*>(bg->getChildByTag(10)))badge->setColor(on?accent:ccColor3B{214,225,237});if(auto l=typeinfo_cast<CCLabelBMFont*>(bg->getChildByTag(11))){l->setString(on?"ON":"OFF");l->setColor(on?ccColor3B{255,255,255}:muted);}}}
 void status(char const* value){if(message){message->setString(value);message->setScale(std::min(.46f,344.f/std::max(1.f,message->getContentSize().width)));}}
 std::optional<int> target(){try{auto s=songID->getString();size_t used=0;int n=std::stoi(s,&used);if(used!=s.size()||n<0||(songCustom&&n==0))throw std::invalid_argument("id");return n;}catch(...){status("Enter a valid song ID first.");return std::nullopt;}}
 void input(int value,float y){number=TextInput::create(120,"Value");number->setCommonFilter(CommonFilter::Uint);number->setString(std::to_string(value));number->setScale(.85f);number->setPosition({270,y});body->addChild(number);}
public:
 static Menu* create(bool resume,PlayLayer* p){
  auto m=new Menu();m->resumeOnClose=resume;m->pausedLayer=p;m->page=lastPage;if(!m->init(500,320,"square02_small.png")){delete m;return nullptr;}m->autorelease();m->m_bgSprite->setColor({216,230,244});m->m_closeBtn->setVisible(false);
  auto size=CCDirector::sharedDirector()->getWinSize();m->m_mainLayer->setScale(std::min({1.f,(size.width-16)/500.f,(size.height-16)/320.f}));m->body=CCNode::create();m->m_mainLayer->addChild(m->body,3);m->controls=CCMenu::create();m->controls->setPosition({0,0});m->m_mainLayer->addChild(m->controls,101);
  if(auto layer=PlayLayer::get();layer&&layer->m_level)m->songCustom=layer->m_level->m_songID>0;m->render();return m;
 }
 void render(){
  body->removeAllChildren();controls->removeAllChildren();toggles.clear();number=nullptr;songID=nullptr;message=nullptr;speedSlider=nullptr;speedLabel=nullptr;
  text(body,"Alpha hack",20,297,.9f,ink,370,true);text(body,"2.3.0 BETA  /  QUICK CONTROLS",21,277,.40f,muted,370,true);button("X",474,294,0,menu_selector(Menu::onDismiss),30,28);
  panel(body,64,148,106,235,{203,219,235});char const* tabs[]={"Play","Speed","Video","Physics","HUD","Unlocks","Songs"};for(int i=0;i<7;++i)button(tabs[i],64,247-i*33,100+i,menu_selector(Menu::onPage),94,29,page==i);
  message=text(body,"Changes are saved automatically.",306,15,.46f,muted,344);
  if(page==0){toggle("enabled","Practice music sync",236);toggle("noclip","Noclip",182);toggle("hitboxes","Show hitboxes",128);label("Assisted runs are marked and use test mode.",306,70,.50f);}
  else if(page==1){
   toggle("speedhack","Speedhack",236);panel(body,306,155,348,102,{248,251,255});speedLabel=text(body,fmt::format("{:.2f}x",c.speed),306,184,1.05f,accent,170);
   speedSlider=Slider::create(this,menu_selector(Menu::onSpeedSlide),1.f);speedSlider->setPosition({306,145});speedSlider->setValue(static_cast<float>((c.speed-.1)/4.9));body->addChild(speedSlider);label("0.10x",185,120,.44f);label("5.00x",427,120,.44f);
   const int values[]={50,100,150,200};const char* names[]={"0.5x","1x","1.5x","2x"};for(int i=0;i<4;++i)button(names[i],172+i*89,83,values[i],menu_selector(Menu::onSpeedPreset),80,32,i==1);
   button(c.audio?"Audio speed: ON":"Audio speed: OFF",306,44,0,menu_selector(Menu::onAudio),348,30);status("Moving the slider turns Speedhack on.");
  }else if(page==2){
   toggle("fps-bypass","FPS bypass",236);
#ifdef GEODE_IS_WINDOWS
   toggle("vertical-sync","Vertical Sync",182);
#else
   panel(body,306,182,348,46,{226,235,244});text(body,"Vertical Sync",146,189,.60f,ink,235,true);text(body,"Managed by Android",146,173,.43f,muted,280,true);
#endif
   toggle("smooth-fix","Smooth Fix",128);text(body,"Target FPS",145,81,.55f,ink,95,true);input(static_cast<int>(c.fps),81);button("Apply",410,81,0,menu_selector(Menu::onValue),140,32,true);
   int values[]={60,120,240,360};for(int i=0;i<4;++i)button(std::to_string(values[i]),172+i*89,44,values[i],menu_selector(Menu::onValuePreset),80,30);status("FPS 30-2000. VSync may limit your target.");
  }else if(page==3){
   toggle("tps-bypass","TPS bypass (experimental)",236);text(body,"Target TPS",145,184,.55f,ink,95,true);input(static_cast<int>(c.tps),184);button("Apply",410,184,0,menu_selector(Menu::onValue),140,32,true);
   int values[]={120,240,360,480};for(int i=0;i<4;++i)button(std::to_string(values[i]),172+i*89,140,values[i],menu_selector(Menu::onValuePreset),80,32);
   label("240 = native physics. Range: 120-1000.",306,99,.50f);label("Custom TPS changes gameplay and marks the run.",306,76,.47f);label(physics::reason,306,48,.46f);status("Unavailable signatures fail closed; no forced patch.");
  }else if(page==4){toggle("show-fps","Show FPS",236);toggle("show-clock","Show clock",182);toggle("cheat-indicator","Cheat indicator + reasons",128);toggle("show-accuracy","Noclip accuracy",74);}
  else if(page==5){toggle("unlock-icons","Unlock icons + colors",236);toggle("unlock-levels","Unlock main levels",182);label("Reopen garage / level selection after changes.",306,117,.50f);label("No coins or completion records are granted.",306,91,.48f);}
  else{ text(body,"Replace a song",144,244,.75f,ink,330,true);button(songCustom?"Custom song / NG ID":"Built-in song / Audio ID",306,208,0,menu_selector(Menu::onSongType),348,38);text(body,"Song ID",145,163,.60f,ink,105,true);
   songID=TextInput::create(205,"Enter ID");songID->setCommonFilter(CommonFilter::Uint);songID->setScale(.85f);songID->setPosition({391,162});body->addChild(songID);if(auto p=PlayLayer::get();p&&p->m_level)songID->setString(std::to_string(songCustom?p->m_level->m_songID:p->m_level->m_audioTrack));
   button("Choose audio file",306,117,0,menu_selector(Menu::onChoose),348,42,true);button("Restore original",306,68,0,menu_selector(Menu::onRestore),348,34);status("MP3 / OGG / WAV / FLAC. Reopen level to apply.");
  }
 }
 void onDismiss(CCObject* s){onClose(s);}
 void onPage(CCObject* s){if(picker)return;page=s->getTag()-100;lastPage=page;render();}
 void onToggle(CCObject* s){if(picker)return;auto t=toggles.at(s->getTag()-1000);if(std::string_view(t.key)=="tps-bypass"&&!physics::ready){status(physics::reason.c_str());return;}auto m=Mod::get();m->setSettingValue<bool>(t.key,!m->getSettingValue<bool>(t.key));load();mark();sync(PlayLayer::get());applyFPS();refreshToggle(t.key);}
 void setSpeed(double n){Mod::get()->setSettingValue<double>("speed",n);Mod::get()->setSettingValue<bool>("speedhack",true);load();mark();refreshToggle("speedhack");if(speedLabel)speedLabel->setString(fmt::format("{:.2f}x",c.speed).c_str());}
 void onSpeedSlide(CCObject*){if(speedSlider)setSpeed(std::round(core::speed(.1+4.9*speedSlider->getValue())*100.)/100.);}
 void onSpeedPreset(CCObject* s){setSpeed(s->getTag()/100.);if(speedSlider)speedSlider->setValue(static_cast<float>((c.speed-.1)/4.9));}
 void onAudio(CCObject*){Mod::get()->setSettingValue<bool>("audio-speed",!c.audio);load();render();}
 void onValue(CCObject*){try{auto s=number->getString();size_t used=0;auto n=std::stoll(s,&used);bool tpsPage=page==3;if(used!=s.size()||n<(tpsPage?120:30)||n>(tpsPage?1000:2000))throw std::invalid_argument("range");Mod::get()->setSettingValue<int64_t>(tpsPage?"tps-target":"fps-target",n);load();mark();applyFPS();status(tpsPage?"TPS saved. Enable TPS bypass to apply.":"FPS saved. Enable FPS bypass to apply.");}catch(...){status(page==3?"Enter 120-1000 TPS.":"Enter 30-2000 FPS.");}}
 void onValuePreset(CCObject* s){number->setString(std::to_string(s->getTag()));onValue(nullptr);}
 void onSongType(CCObject*){if(!picker){songCustom=!songCustom;render();}}
 void onChoose(CCObject*){
  auto id=target();if(!id||picker)return;
  picker=true;retain();const bool custom=songCustom;const int song=*id;
  async::spawn(file::pick(file::PickMode::OpenFile,file::FilePickOptions{
   .filters={{"Audio files",{"*.mp3","*.ogg","*.wav","*.flac"}}}
  }),[this,custom,song](file::PickResult result){
   picker=false;
   if(result.isOk()&&result.unwrap().has_value())importSong(fs::path(result.unwrap().value().native()),custom,song);
   else if(result.isErr())status("Could not open file picker.");
   release();
  });
 }
 void importSong(fs::path const& src,bool custom,int id){
  try{
   auto ext=src.extension().string();std::transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
   if(ext!=".mp3"&&ext!=".ogg"&&ext!=".wav"&&ext!=".flac")throw std::runtime_error("type");
   if(!fs::file_size(src)||fs::file_size(src)>512ULL*1024*1024)throw std::runtime_error("size");
   auto dir=fs::path(Mod::get()->getSaveDir().native())/"songs";fs::create_directories(dir);
   auto dst=dir/fs::path(fmt::format("{}-{}",songKey(custom,id),std::chrono::system_clock::now().time_since_epoch().count()));dst+=ext;fs::copy_file(src,dst);
   FMOD::Sound* test=nullptr;auto result=FMODAudioEngine::sharedEngine()->m_system->createStream(utf8(dst).c_str(),FMOD_DEFAULT,nullptr,&test);
   if(result!=FMOD_OK){fs::remove(dst);throw std::runtime_error("decode");}test->release();
   auto key=songKey(custom,id);auto value=utf8(dst);Mod::get()->setSavedValue(key,value);songs[key]=value;status("Song linked. Leave and reopen the level.");
  }catch(...){status("Import failed: check audio, file size or disk space.");}
 }
 void onRestore(CCObject*){auto id=target();if(!id)return;auto key=songKey(songCustom,*id);Mod::get()->setSavedValue(key,std::string());songs[key]="";status("Original restored. Leave and reopen the level.");}
 void onClose(CCObject* s) override {
  if(picker)return;
  bool resume=resumeOnClose;auto p=pausedLayer;if(menu==this)menu=nullptr;Popup::onClose(s);
  if(resume&&p&&PlayLayer::get()==p){auto scene=CCDirector::sharedDirector()->getRunningScene();if(scene){auto pause=findFirstChildRecursive<PauseLayer>(scene,[](auto){return true;});if(pause)pause->onResume(nullptr);}}
 }
 ~Menu() override {if(menu==this)menu=nullptr;}
};
void openMenu(){
 if(picker)return;if(menu){menu->onClose(nullptr);return;}load();auto p=PlayLayer::get();bool resume=false;
 if(p&&!p->m_isPaused&&!p->m_hasCompletedLevel){p->pauseGame(false);resume=p->m_isPaused;}
 menu=Menu::create(resume,p);if(menu)menu->show();
}
}
