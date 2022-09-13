//
// Created by MSI-Gaming on 2022/7/26.
//

#ifndef JAVAPROFILERAGENT_JAVAPROFILERAGENT_H
#define JAVAPROFILERAGENT_JAVAPROFILERAGENT_H

#endif //JAVAPROFILERAGENT_JAVAPROFILERAGENT_H


FILE *open_jitcache_file(pid_t pid);
void write_jitcache_entry(FILE *jitcache_file, 
    const void *code_addr, unsigned int code_size, const char *entry, struct timespec ts, const char flg );