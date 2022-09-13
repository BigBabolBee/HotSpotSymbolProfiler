#include<jni.h>
#include <jvmti.h>
#include <jvmticmlr.h>
#include "global.h"
#ifndef _JAVAAGENT_H_
#define _JAVAAGENT_H_

class JavaAgent{
    public:             
        JavaAgent(JavaVM *attached_vm);
        jvmtiError agentPrepare();
        void handleError(jint rc);
        static jvmtiEnv *jvmti;   
        static JavaVM *vm;
    private:        
        JavaAgent * instance;
        jvmtiError enableCap();
        jvmtiError setCallBack();
        jvmtiError setNotif();
        jvmtiError genEvents(); 
        static void printTraceData();
        static void deallocate(void *);
        static void JNICALL cbVMDeath(jvmtiEnv *jvmti_env,
                                    JNIEnv* jni_env);
        static void JNICALL cbCompiledMethodLoad(jvmtiEnv *jvmti,
                                                jmethodID method,
                                                jint code_size,
                                                const void *code_addr,
                                                jint map_length,
                                                const jvmtiAddrLocationMap *map,
                                                const void *compile_info);
        static void JNICALL cbDynamicCodeGenerated(jvmtiEnv *jvmti,
                                                const char *name,
                                                const void *address,
                                                jint length);
        static void JNICALL cbClassLoad(jvmtiEnv *jvmti,
                                        JNIEnv *jni_env,
                                        jthread thread,
                                        jclass klass);
        static void JNICALL cbClassPrepare(jvmtiEnv *jvmti,
                                        JNIEnv *jni_env,
                                        jthread thread,
                                        jclass klass);

        
};
#endif