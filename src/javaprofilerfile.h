//
// Created by MSI-Gaming on 2022/7/26.
//

#ifndef JAVAPROFILERAGENT_JAVAPROFILERFILE_H
#define JAVAPROFILERAGENT_JAVAPROFILERFILE_H
FILE *open_jitcache_file(pid_t pid);
void write_jitcache_entry(FILE *jitcache_file, 
    const void *code_addr, unsigned int code_size, const char *entry, struct timespec ts, const char flg );
void write_agct_entry(FILE *agct_file, const void *entry);
#endif //JAVAPROFILERAGENT_JAVAPROFILERFILE_H


