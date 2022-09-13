#include <jni.h>
#include <signal.h>
#include <stdint.h>
#include <vector>
#include <mutex>
#include <atomic>
#include "javaAgent.hpp"

typedef struct
{
    jint lineno;
    jmethodID method_id;
} ASGCT_CallFrame;

typedef struct
{
    JNIEnv *env_id;
    jint num_frames;
    ASGCT_CallFrame *frames;
} ASGCT_CallTrace;

typedef void (*AsyncGetCallTrace)(ASGCT_CallTrace *, jint, void *);
typedef std::vector<ASGCT_CallTrace> call_trace_t;

class JavaProfiler{
    public:   
        static void errorLog();
        static int getLibHandle(const char *libname);
        static int getpAGCT();
        static int openDumpFile(pid_t);
        static void setSampleTrigger();
        static void ready();
        static void start();
        static void getJAgent(JavaAgent *);        
        static void writeFile(FILE*, const char*);

        static FILE *dumpfile;
        // static call_trace_t sample_data;
        static ASGCT_CallFrame sample_data[20000][50];  //tmp
        static std::atomic<int> sample_num;
    private:
        
        static JavaAgent *jagent;
        static void *plibjvm;
        static JavaVM *jvm;
        static AsyncGetCallTrace pAGCT;
        
        static std::mutex mylock;

        static void sigHandler(int, siginfo_t *, void *);

        static void test();
};