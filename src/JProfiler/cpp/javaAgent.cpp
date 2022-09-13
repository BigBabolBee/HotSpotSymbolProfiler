#include "javaAgent.hpp"
#include "javaProfiler.hpp"
#include <string.h>
#include <sys/time.h>

jvmtiEnv *JavaAgent::jvmti = NULL;
JavaVM *JavaAgent::vm = NULL;

JavaAgent::JavaAgent(JavaVM *attached_vm)
{
    vm = attached_vm;
    vm->GetEnv((void **)&jvmti, JVMTI_VERSION_1);
    instance = this;
}

void JavaAgent::handleError(jint rc)
{
    JavaProfiler::errorLog();
}

jvmtiError JavaAgent::agentPrepare()
{
    if (enableCap() != JVMTI_ERROR_NONE)
    {
        JavaProfiler::errorLog();
    }

    if (setCallBack() != JVMTI_ERROR_NONE)
    {
        JavaProfiler::errorLog();
    }

    if (setNotif() != JVMTI_ERROR_NONE)
    {
        JavaProfiler::errorLog();
    }

    if (genEvents() != JVMTI_ERROR_NONE)
    {
        JavaProfiler::errorLog();
    }
    return JVMTI_ERROR_NONE;
}

void JNICALL JavaAgent::cbCompiledMethodLoad(jvmtiEnv *jvmti,
                                             jmethodID method,
                                             jint code_size,
                                             const void *code_addr,
                                             jint map_length,
                                             const jvmtiAddrLocationMap *map,
                                             const void *compile_info)
{
}

void JNICALL JavaAgent::cbDynamicCodeGenerated(jvmtiEnv *jvmti,
                                               const char *name,
                                               const void *address,
                                               jint length)
{
}

void JNICALL JavaAgent::cbClassLoad(jvmtiEnv *jvmti,
                                    JNIEnv *jni_env,
                                    jthread thread,
                                    jclass klass)
{
}

void JNICALL JavaAgent::cbClassPrepare(jvmtiEnv *jvmti,
                                       JNIEnv *jni_env,
                                       jthread thread,
                                       jclass klass)
{
    jint method_count;
    jmethodID *methods;
    jvmti->GetClassMethods(klass, &method_count, &methods);
    delete[] methods;
}

void JNICALL JavaAgent::cbVMDeath(jvmtiEnv *jvmti_env,
                                  JNIEnv *jni_env)
{
    signal(SIGPROF, SIG_IGN);
    long sec = 0;
    long usec = 0;
    struct itimerval tv = {{sec, usec}, {sec, usec}};
    setitimer(ITIMER_PROF, &tv, NULL);
    JavaAgent::printTraceData();
}

jvmtiError JavaAgent::enableCap()
{
    jvmtiCapabilities capabilities;

    memset(&capabilities, 0, sizeof(capabilities));

    capabilities.can_get_constant_pool = 1;
    capabilities.can_tag_objects = 1;
    capabilities.can_get_source_file_name = 1;
    capabilities.can_get_line_numbers = 1;
    capabilities.can_get_bytecodes = 1;
    capabilities.can_generate_compiled_method_load_events = 1;

    return jvmti->AddCapabilities(&capabilities);
}

jvmtiError JavaAgent::setCallBack()
{
    jvmtiEventCallbacks callbacks;

    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.CompiledMethodLoad = &JavaAgent::cbCompiledMethodLoad;
    callbacks.DynamicCodeGenerated = &JavaAgent::cbDynamicCodeGenerated;
    callbacks.ClassLoad = &JavaAgent::cbClassLoad;
    callbacks.ClassPrepare = &JavaAgent::cbClassPrepare;
    callbacks.VMDeath = &JavaAgent::cbVMDeath;
    return jvmti->SetEventCallbacks(&callbacks, (jint)sizeof(callbacks));
}

jvmtiError JavaAgent::setNotif()
{
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_COMPILED_METHOD_LOAD, (jthread)NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_DYNAMIC_CODE_GENERATED, (jthread)NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_LOAD, (jthread)NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE, (jthread)NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, (jthread)NULL);
    return JVMTI_ERROR_NONE;
}

jvmtiError JavaAgent::genEvents()
{
    jvmti->GenerateEvents(JVMTI_EVENT_DYNAMIC_CODE_GENERATED);
    jvmti->GenerateEvents(JVMTI_EVENT_COMPILED_METHOD_LOAD);
    jvmti->GenerateEvents(JVMTI_EVENT_CLASS_LOAD);
    jvmti->GenerateEvents(JVMTI_EVENT_CLASS_PREPARE);
    return JVMTI_ERROR_NONE;
}

void JavaAgent::printTraceData()
{

    JavaProfiler::writeFile(JavaProfiler::dumpfile, "hi!");
    // for (call_trace_t::iterator it = JavaProfiler::sample_data.begin(); it != JavaProfiler::sample_data.end(); it++)

    for (int i = 0; i < JavaProfiler::sample_num; i++)
    {
        char entry[MAX_STRING_BUF];
        // if (it->num_frames <= 0)
        // {
        //     strncpy(entry, "JIT or GC", sizeof(entry));
        // }
        // else
        // {
        char *method_name = NULL;
        char *msig = NULL;
        jclass klass = NULL;
        char *csig = NULL;
        // ASGCT_CallFrame *frames = it->frames;
        // jint num_frames = it->num_frames;

        for(int j = 0; j < 50; j++)
        // for (int i = 0; i < num_frames; i++)
        {
            
            // jmethodID mid = (frames + i)->method_id;
            // if (!jvmti->GetMethodName(mid, &method_name, &msig, NULL))

            if(!jvmti->GetMethodName(JavaProfiler::sample_data[i][j].method_id, &method_name, &msig, NULL))
            {
                if (!jvmti->GetMethodDeclaringClass(JavaProfiler::sample_data[i][j].method_id, &klass) &&
                    !jvmti->GetClassSignature(klass, &csig, NULL))
                {
                    char source_info[1000] = "";
                    char *source_file = NULL;
                    jint entrycount = 0;
                    jvmtiLineNumberEntry *ptable = NULL;
                    if (!jvmti->GetSourceFileName(klass, &source_file))
                    {
                        if (!jvmti->GetLineNumberTable(JavaProfiler::sample_data[i][j].method_id, &entrycount, &ptable))
                        {
                            int lineno = -1;
                            if (entrycount > 0)
                            {
                                lineno = ptable[0].line_number;
                            }
                            deallocate(ptable);

                            snprintf(source_info, sizeof(source_info), "(%s:%d)", source_file, lineno);
                        }
                        deallocate((unsigned char *)source_file);
                    }
                    snprintf(entry, MAX_STRING_BUF, "%s::%s::%s::%s", csig,
                             method_name, msig, source_info);

                    JavaProfiler::writeFile(JavaProfiler::dumpfile, entry);
                    deallocate((unsigned char *)csig);
                }
                deallocate((unsigned char *)method_name);
                deallocate((unsigned char *)msig);
            }
        }
        // }
    }
}

void JavaAgent::deallocate(void *string)
{
    if (string != NULL)
        jvmti->Deallocate((unsigned char *)string);
}