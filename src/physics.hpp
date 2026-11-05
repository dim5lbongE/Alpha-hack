#pragma once
#include <Geode/Geode.hpp>
#include <cstring>
#ifdef GEODE_IS_ANDROID
#include <dlfcn.h>
#endif
namespace physics {
inline geode::Patch* patch=nullptr;
inline uint32_t expectedTicks=0;
inline bool ready=false;
inline std::string reason="Not initialized";
inline void word(std::vector<uint8_t>& out,uint32_t v){for(int i=0;i<4;++i)out.push_back(static_cast<uint8_t>(v>>(8*i)));}
inline void initialize(){
 if(geode::Loader::get()->getLoadedMod("syzzi.click_between_frames")){reason="Disable Click Between Frames to use TPS bypass";return;}
 uint8_t* address=nullptr;std::vector<uint8_t> code;
#ifdef GEODE_IS_WINDOWS
 // GD 2.2091: replace only the native tick-count calculation, preserving
 // the preceding xmm10 initialization and all subsequent simulation work.
 auto base=reinterpret_cast<uint8_t*>(geode::base::get());
 address=base+0x3408f8;
 constexpr uint8_t original[]={0x41,0x0f,0x28,0xcf,0xf2,0x0f,0x5e,0xc8,0xf2,0x0f,0x59,0x0d,0xe8,0x49,0x4f,0x00,0x0f,0x57,0xc0,0xf2,0x0f,0x5a,0xc1,0xff,0x15,0x73,0xdb,0x38,0x00,0x41,0x0f,0x28,0xca,0xf3,0x0f,0x5f,0xc8,0xf3,0x0f,0x2c,0xf9};
 if(std::memcmp(address,original,sizeof(original))){reason="TPS unavailable: native code differs from GD 2.2091";return;}
 code={0x48,0xb8};auto pointer=reinterpret_cast<uint64_t>(&expectedTicks);
 for(int i=0;i<8;++i)code.push_back(static_cast<uint8_t>(pointer>>(8*i)));
 code.insert(code.end(),{0x8b,0x38,0xe9});word(code,24); // mov edi,[rax]; jump to original +41
#elif defined(GEODE_IS_ANDROID64)
 auto function=reinterpret_cast<uint8_t*>(dlsym(RTLD_DEFAULT,"_ZN15GJBaseGameLayer6updateEf"));
 if(!function){reason="TPS unavailable: physics symbol not found";return;}
 constexpr uint8_t signature[]={0xab,0x19,0x60,0x1e,0x0a,0x10,0x62,0x1e,0x6a,0x09,0x6a,0x1e};
 unsigned matches=0;
 for(size_t i=0;i+40<=0x500;i+=4)if(!std::memcmp(function+i,signature,sizeof(signature))){address=function+i;++matches;}
 if(matches!=1){reason="TPS unavailable: physics signature is not unique";return;}
 auto pointer=reinterpret_cast<uint64_t>(&expectedTicks);
 for(unsigned i=0;i<4;++i)word(code,(i?0xf2800009u:0xd2800009u)|(i<<21)|(((pointer>>(16*i))&0xffffu)<<5));
 word(code,0xb9400120);word(code,0x14000005); // ldr w0,[x9]; jump from +20 to +40
#else
 reason="TPS is unsupported on this platform";return;
#endif
 auto result=geode::Mod::get()->patch(address,code);
 if(result.isErr()){reason="TPS unavailable: patch creation failed";return;}
 patch=result.unwrap();auto disabled=patch->disable();
 if(disabled.isErr()){reason="TPS unavailable: patch could not be disabled";return;}
 ready=true;reason="Ready (experimental)";
}
struct Scope {
 bool enabled=false;
 explicit Scope(bool requested){if(requested&&ready&&patch){auto result=patch->enable();enabled=result.isOk();}}
 ~Scope(){if(enabled)(void)patch->disable();}
};
}
