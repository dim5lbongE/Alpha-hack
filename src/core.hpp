#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace core {
inline double speed(double n){return std::isfinite(n)?std::clamp(n,.1,5.):1.;}
inline double fps(double n){return std::isfinite(n)?std::clamp(n,30.,2000.):240.;}
inline double tps(double n){return std::isfinite(n)?std::clamp(n,120.,1000.):240.;}
inline bool customTPS(bool on,double n){return on&&std::abs(tps(n)-240.)>.001;}
struct TickBatch { double delta;uint32_t ticks; };
inline TickBatch advance(double dt,double rate,double warp,double& remainder){
 if(!std::isfinite(dt)||dt<0)dt=0;
 if(!std::isfinite(remainder))remainder=0;
 const double step=std::clamp(std::isfinite(warp)?warp:1.,.01,1.)/tps(rate);
 const double pending=remainder+dt;
 auto count=static_cast<uint32_t>(std::clamp(std::floor(pending/step+.5),0.,2048.));
 double applied=count*step;remainder=pending-applied;return {applied,count};
}
inline uint32_t reasons(bool noclip,bool speedOn,double scale,bool tpsOn,double rate,bool hitboxes){
 return (noclip?1u:0u)|((speedOn&&std::abs(speed(scale)-1.)>.0001)?2u:0u)|(customTPS(tpsOn,rate)?4u:0u)|(hitboxes?8u:0u);
}
inline double accuracy(uint64_t n,uint64_t bad){return n?100.*(n-std::min(n,bad))/n:100.;}
inline bool dirtyAfterReset(bool previous,bool practice,bool enabled){return enabled||(practice&&previous);}
}
