#include "../src/core.hpp"
#include <cassert>
#include <limits>
#include <iostream>
void approx(double a,double b,double e=1e-7){assert(std::abs(a-b)<e);}
int main(){
 unsigned scenarios=0;
 for(double fps:{30.,60.,120.,144.,240.,360.})for(double tps:{120.,240.,360.,480.,960.,1000.})for(double speed:{.1,.5,1.,1.5,2.,5.})for(double warp:{.25,.5,1.,2.}){
  double rest=0,total=0,input=0;uint64_t ticks=0;
  double step=std::min(warp,1.)/tps;
  for(int f=0;f<int(fps*10);++f){double dt=speed/fps;auto b=core::advance(dt,tps,warp,rest);total+=b.delta;input+=dt;ticks+=b.ticks;assert(b.ticks<=2048);assert(std::abs(rest)<=step/2+1e-9);}
  approx(total+rest,input);approx(total,ticks*step);assert(std::abs(total-input)<=step/2+1e-7);++scenarios;
 }
 double rest=0;auto backlog=core::advance(10,1000,1,rest);assert(backlog.ticks==2048);approx(backlog.delta+rest,10);double total=backlog.delta;
 while(rest>.001){auto b=core::advance(0,1000,1,rest);total+=b.delta;}approx(total+rest,10);
 rest=0;assert(core::advance(-1,480,1,rest).ticks==0);assert(core::advance(std::numeric_limits<double>::quiet_NaN(),480,1,rest).ticks==0);
 assert(core::speed(-1)==.1&&core::speed(99)==5);assert(core::speed(std::numeric_limits<double>::quiet_NaN())==1);
 assert(core::tps(0)==120&&core::tps(5000)==1000);assert(core::fps(0)==30&&core::fps(9999)==2000);
 assert(core::reasons(false,true,1,true,240,false)==0);assert(core::reasons(true,true,2,true,480,true)==15);
 assert(core::dirtyAfterReset(true,true,false));assert(!core::dirtyAfterReset(true,false,false));assert(core::dirtyAfterReset(false,false,true));
 assert(core::accuracy(0,0)==100&&core::accuracy(10,3)==70&&core::accuracy(10,12)==0);
 std::cout<<scenarios<<" timing scenarios + budget, invalid inputs, indicator history and accuracy: PASS\n";
}
