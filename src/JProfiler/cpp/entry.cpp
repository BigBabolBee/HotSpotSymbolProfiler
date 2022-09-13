#include "javaAgent.hpp"
#include "javaProfiler.hpp"
#include <stdlib.h>
 #include <unistd.h>

extern "C" JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM *vm, char *options, void *reserved)
{   
    JavaAgent jagent(vm);
    jagent.agentPrepare();
    JavaProfiler::getJAgent(&jagent);

    if(JavaProfiler::openDumpFile(getpid())){
        JavaProfiler::errorLog();
        return 1;
    }

    if (JavaProfiler::getLibHandle("libjvm.so")){
        JavaProfiler::errorLog();  //
        return 1;
    }
    else{
        if (JavaProfiler::getpAGCT()){
            JavaProfiler::errorLog();  //
            return 1;
        }
    }    
    
    JavaProfiler::ready();
    JavaProfiler::start();
    return 0;
}
