#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <jni.h>
#include <jvmti.h>
#include <jvmticmlr.h>
#include <stddef.h>
#include "javaprofilerfile.h"
#include <signal.h>
#include <sys/time.h>
#include <sys/syscall.h>

#define STRING_BUFFER_SIZE 2000
#define MAXFRAMES 128

FILE *jitcache_file = NULL;

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
AsyncGetCallTrace pAGCT;

typedef struct sigaction sa_t;

jvmtiEnv *jvmti;
// JNIEnv *jni;

JavaVM *javavm;

int counter = 0;
void *getLibHandle(const char *name)
{
    void *handle = dlopen(name, RTLD_NOW);
    if (handle != NULL)
    {
        return handle;
    }
    return NULL;
}

void deallocate(jvmtiEnv *jvmti, void *string)
{
    if (string != NULL)
        jvmti->Deallocate((unsigned char *)string);
}

void signalHandler(int signum, siginfo_t *siginfo, void *ucontext)
{
    ASGCT_CallTrace trace;
    jint depth;
    jclass klass;
    jvmtiLineNumberEntry *lines = NULL;
    jint entrycount = 0;

    char *sourcefile = NULL;
    char *method_name = NULL;
    char *msig = NULL;
    char *csig = NULL;
    char entry[STRING_BUFFER_SIZE];

    //为AGCT的栈追踪，预留足够的空间
    ASGCT_CallFrame frames[MAXFRAMES];

    JNIEnv *jni;
    javavm->GetEnv((void **)&jni, JNI_VERSION_1_6);
    trace.env_id = jni;

    //这些空间需要置零
    char *base = reinterpret_cast<char *>(frames);
    for (char *p = base; p < base + sizeof(frames); p++)
    {
        *p = 0;
    }

    trace.frames = frames;
    pAGCT(&trace, MAXFRAMES, ucontext);

    if (trace.num_frames > 0)
    {
        //尝试找出当前线程的名称
        /*Thread class -> Static mid(smid) -> thread
          Thread class -> Instance mid(imid) (getiId) -> Call Java Method -> tid
          Thread class -> Instance mid(imid) (getName) -> Call Java Method -> tname
          首先找到 java/lang/Thread,然后找到它的静态方法currentThread,对象和方法都有了下一步就
          找当前线程
        */
        jclass klass_thread = jni->FindClass("java/lang/Thread");
        // javap -s -p java.lang.Thread | grep -A 1 currentThread
        jmethodID smid = jni->GetStaticMethodID(klass_thread, "currentThread",
                                                "()Ljava/lang/Thread;");
        jobject current_thread = jni->CallStaticObjectMethod(klass_thread, smid);
        if (current_thread == NULL)
        {
        }

        jmethodID imid_getid = jni->GetMethodID(klass_thread, "getId", "()J");
        if (imid_getid == NULL)
        {
        }

        jlong jlong_tid = jni->CallLongMethod(current_thread, imid_getid);

        jmethodID imid_getname = jni->GetMethodID(klass_thread, "getName", "()Ljava/lang/String;");
        if (imid_getname == NULL)
        {
        }

        jobject thread_name = jni->CallObjectMethod(current_thread, imid_getname);

        // const char *str_tname;
        // str_tname = jni->GetStringUTFChars((jstring)thread_name, NULL);
        // if (str_tname == NULL)
        // {
        // }

        snprintf(entry, STRING_BUFFER_SIZE, "%ld::%s", jlong_tid, "Noname");
        write_agct_entry(jitcache_file, entry);

        // jni->ReleaseStringUTFChars((jstring)thread_name, str_tname);
    }

    // strncpy(entry, "0", STRING_BUFFER_SIZE);

    // if (trace.num_frames > 0)
    // {
    //     for (int i = 0; i < trace.num_frames; i++)
    //     {

    //         jmethodID mid = trace.frames[i].method_id;

    //         if (!jvmti->GetMethodName(mid, &method_name, &msig, NULL))
    //         {
    //             if (!jvmti->GetMethodDeclaringClass(mid, &klass) && // get class of compiled method
    //                 !jvmti->GetClassSignature(klass, &csig, NULL))
    //             { // get class signature of compiled name
    //                 char source_info[1000] = "";

    //                 if (!jvmti->GetSourceFileName(klass, &sourcefile))
    //                 { // get source file ptr of compiled method
    //                     if (!jvmti->GetLineNumberTable(mid, &entrycount, &lines))
    //                     { // get line number table of compiled method
    //                         int lineno = -1;
    //                         if (entrycount > 0)
    //                             lineno = lines[0].line_number;                                         // only first line???
    //                         // snprintf(source_info, sizeof(source_info), "(%s:%d)", sourcefile, lineno); // now we have source info(file and linenumber)

    //                         deallocate(jvmti, lines);
    //                     }
    //                     deallocate(jvmti, (unsigned char *)sourcefile);
    //                 }
    //                 // snprintf(entry, STRING_BUFFER_SIZE, "tid:%d::%s::%s::%s::%s",tid, csig, method_name, msig, source_info);
    //                 deallocate(jvmti, (unsigned char *)csig);
    //             }
    //             // snprintf(entry, STRING_BUFFER_SIZE, "method name: %s", method_name);
    //             deallocate(jvmti, (unsigned char *)method_name);
    //             deallocate(jvmti, (unsigned char *)msig);
    //         }

    //     }
    //     // snprintf(entry, STRING_BUFFER_SIZE, "%s::%s", msg, meths);
    //     // write_agct_entry(jitcache_file, entry);
    // }
    // else
    // {
    //     //
    //     snprintf(entry, STRING_BUFFER_SIZE, "tid:%d::not known frame %d",tid, trace.num_frames);
    //     write_agct_entry(jitcache_file, entry);
    // }
}

void setSignalHandler()
{
    sa_t sa;
    sa_t old_handler;

    sa.sa_sigaction = &signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGPROF, &sa, &old_handler);
}

static void JNICALL
cbCompiledMethodLoad(jvmtiEnv *jvmti,
                     jmethodID method,
                     jint code_size,
                     const void *code_addr,
                     jint map_length,
                     const jvmtiAddrLocationMap *map,
                     const void *compile_info)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    char entry[STRING_BUFFER_SIZE];
    char *sourcefile = NULL;
    char *method_name = NULL;
    char *msig = NULL;
    char *csig = NULL;
    char flg = 'J';
    jvmtiLineNumberEntry *lines = NULL;

    jclass klass;
    jint entrycount = 0;

    strncpy(entry, "<error writing signature>", STRING_BUFFER_SIZE);

    if (!jvmti->GetMethodName(method, &method_name, &msig, NULL)) // get compiled method name
    {
        if (!jvmti->GetMethodDeclaringClass(method, &klass) && // get class of compiled method
            !jvmti->GetClassSignature(klass, &csig, NULL))
        { // get class signature of compiled name
            char source_info[1000] = "";

            if (!jvmti->GetSourceFileName(klass, &sourcefile))
            { // get source file ptr of compiled method
                if (!jvmti->GetLineNumberTable(method, &entrycount, &lines))
                { // get line number table of compiled method
                    int lineno = -1;
                    if (entrycount > 0)
                        lineno = lines[0].line_number;                                         // only first line???
                    snprintf(source_info, sizeof(source_info), "(%s:%d)", sourcefile, lineno); // now we have source info(file and linenumber)

                    deallocate(jvmti, lines);
                }
                deallocate(jvmti, (unsigned char *)sourcefile);
            }

            snprintf(entry, STRING_BUFFER_SIZE, "%s::%s::%s::%s", csig,
                     method_name, msig, source_info);
            deallocate(jvmti, (unsigned char *)csig);
        }
        deallocate(jvmti, (unsigned char *)method_name);
        deallocate(jvmti, (unsigned char *)msig);
    }

    write_jitcache_entry(jitcache_file, code_addr, (unsigned int)code_size, entry, ts, flg);
}

static void cbDynamicCodeGenerated(jvmtiEnv *jvmti,
                                   const char *name,
                                   const void *address,
                                   jint length)
{
    struct timespec ts;
    char flg = 'D';
    clock_gettime(CLOCK_MONOTONIC, &ts);
    write_jitcache_entry(jitcache_file, address, (unsigned int)length, name, ts, flg);
}

static void JNICALL
cbClassLoad(jvmtiEnv *jvmti,
            JNIEnv *jni_env,
            jthread thread,
            jclass klass) {}

static void JNICALL
cbClassPrepare(jvmtiEnv *jvmti,
               JNIEnv *jni_env,
               jthread thread,
               jclass klass)
{
    jint method_count;
    jmethodID *methods;
    jvmti->GetClassMethods(klass, &method_count, &methods);
    delete[] methods;
}

jvmtiError enable_capabilities(jvmtiEnv *jvmti)
{
    jvmtiCapabilities capabilities;

    memset(&capabilities, 0, sizeof(capabilities));

    // capabilities.can_generate_all_class_hook_events = 1;
    capabilities.can_get_constant_pool = 1;
    capabilities.can_tag_objects = 1;
    capabilities.can_get_source_file_name = 1;
    capabilities.can_get_line_numbers = 1;
    capabilities.can_get_bytecodes = 1;
    capabilities.can_generate_compiled_method_load_events = 1;
    // Request these capabilities for this JVM TI environment.
    return jvmti->AddCapabilities(&capabilities);
}

void set_notification_mode(jvmtiEnv *jvmti, jvmtiEventMode mode)
{
    jvmti->SetEventNotificationMode(mode, JVMTI_EVENT_COMPILED_METHOD_LOAD, (jthread)NULL);
    jvmti->SetEventNotificationMode(mode, JVMTI_EVENT_DYNAMIC_CODE_GENERATED, (jthread)NULL);
    jvmti->SetEventNotificationMode(mode, JVMTI_EVENT_CLASS_LOAD, (jthread)NULL);
    jvmti->SetEventNotificationMode(mode, JVMTI_EVENT_CLASS_PREPARE, (jthread)NULL);
}

jvmtiError set_callbacks(jvmtiEnv *jvmti)
{
    jvmtiEventCallbacks callbacks;

    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.CompiledMethodLoad = &cbCompiledMethodLoad;
    callbacks.DynamicCodeGenerated = &cbDynamicCodeGenerated;
    callbacks.ClassLoad = &cbClassLoad;
    callbacks.ClassPrepare = &cbClassPrepare;
    return jvmti->SetEventCallbacks(&callbacks, (jint)sizeof(callbacks));
}

extern "C" JNIEXPORT jint JNICALL
Agent_OnAttach(JavaVM *vm, char *options, void *reserved)
{
    if (!jitcache_file)
        jitcache_file = open_jitcache_file(getpid());

    javavm = vm;
    javavm->GetEnv((void **)&jvmti, JVMTI_VERSION_1);

    void *plibjvm;
    plibjvm = getLibHandle("libjvm.so");

    pAGCT = (AsyncGetCallTrace)dlsym(plibjvm, "AsyncGetCallTrace");

    setSignalHandler();

    enable_capabilities(jvmti);
    set_callbacks(jvmti);
    set_notification_mode(jvmti, JVMTI_ENABLE);
    jvmti->GenerateEvents(JVMTI_EVENT_DYNAMIC_CODE_GENERATED);
    jvmti->GenerateEvents(JVMTI_EVENT_COMPILED_METHOD_LOAD);
    jvmti->GenerateEvents(JVMTI_EVENT_CLASS_LOAD);
    jvmti->GenerateEvents(JVMTI_EVENT_CLASS_PREPARE);

    long sec = 0;
    long usec = 100000;
    struct itimerval tv = {{sec, usec}, {sec, usec}};
    setitimer(ITIMER_PROF, &tv, NULL);
    return 0;
}
