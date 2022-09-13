#include <dlfcn.h>
#include <iostream>

int main(){
    void *plib;
    plib = dlopen("/home/yuzhaoxi/Profiler/src/JProfiler/cpp/libjpa.so", RTLD_NOW);
    if(plib){
        std::cout<<"Get"<<std::endl;
    }
}