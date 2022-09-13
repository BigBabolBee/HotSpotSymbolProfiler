#include "javaProfiler.hpp"
#include <dlfcn.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "global.h"

//test
#include <chrono>
#include <thread>

JavaAgent *JavaProfiler::jagent = NULL;
void *JavaProfiler::plibjvm = NULL;
JavaVM *JavaProfiler::jvm = NULL;
AsyncGetCallTrace JavaProfiler::pAGCT = NULL;
FILE *JavaProfiler::dumpfile = NULL;
// call_trace_t JavaProfiler::sample_data;
ASGCT_CallFrame JavaProfiler::sample_data[20000][50]; // tmp
std::mutex JavaProfiler::mylock;
std::atomic<int> JavaProfiler::sample_num = {};


void JavaProfiler::errorLog()
{
    //还没想好怎么写
}

int JavaProfiler::getLibHandle(const char *libname)
{
    if ((plibjvm = dlopen("libjvm.so", RTLD_NOW)) == NULL)
    {
        errorLog();
        return 1;
    }
    return 0;
}

int JavaProfiler::getpAGCT()
{
    if ((pAGCT = (AsyncGetCallTrace)dlsym(plibjvm, "AsyncGetCallTrace")) == NULL)
    {
        errorLog();
        return 1;
    }
    return 0;
}

int JavaProfiler::openDumpFile(pid_t pid)
{
    char filename[MAX_FILENAME];

    snprintf(filename, sizeof(filename), "/home/yuzhaoxi/tmp/jdump_%d.map", pid);
    dumpfile = fopen(filename, "w");
    if (!dumpfile)
    {
        return 1;
    }
    return 0;
}

void JavaProfiler::sigHandler(int signum, siginfo_t *siginfo, void *ucontext)
{
    ASGCT_CallTrace trace;
    jint depth;
    jclass klass;
    jvmtiLineNumberEntry *lines = NULL;
    jint entrycount = 0;
    ASGCT_CallFrame frames[MAX_FRAMES];
    JNIEnv *jni;

    JavaAgent::vm->GetEnv((void **)&jni, JNI_VERSION_1_6);
    trace.env_id = jni;
    memset(frames, 0, MAX_FRAMES);
    trace.frames = frames;

    pAGCT(&trace, MAX_FRAMES, ucontext);
    sample_num.fetch_add(1);
    ASGCT_CallFrame *pframe;

    test();
    mylock.lock();
    if (trace.num_frames <= 0)
    {
        sample_data[sample_num][0].lineno = 0;
        sample_data[sample_num][0].method_id = 0;
    }
    else
    {
        for (int i = 0; i < trace.num_frames; i++)
        {
            sample_data[sample_num][i].lineno = (trace.frames+i)->lineno;
            sample_data[sample_num][i].method_id = (trace.frames+i)->method_id;
        }
    }
    mylock.unlock();
}

void JavaProfiler::setSampleTrigger()
{
    //先用setitimer凑付一下
    struct sigaction sa, old_sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = &sigHandler;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGPROF, &sa, &old_sa);

    long sec = 0;
    long usec = 100000;
    struct itimerval tv = {{sec, usec}, {sec, usec}};
    setitimer(ITIMER_PROF, &tv, NULL);
}

void JavaProfiler::ready()
{   
    
    setSampleTrigger();
}

void JavaProfiler::start()
{
    std::atomic_init(&sample_num, 0);

    long sec = 0;
    long usec = 100000;
    struct itimerval tv = {{sec, usec}, {sec, usec}};
    setitimer(ITIMER_PROF, &tv, NULL);
}

void JavaProfiler::getJAgent(JavaAgent *javaagent)
{
    jagent = javaagent;
}

void JavaProfiler::writeFile(FILE *file, const char *entry)
{
    if (file)
    {
        fprintf(file, "%s\n", entry);
    }
}

void JavaProfiler::test(){
    mylock.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    mylock.unlock();


}