//
// Created by MSI-Gaming on 2022/7/26.
//


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "javaprofilerfile.h"

FILE *open_jitcache_file(pid_t pid){
    
    char filename[500];

    snprintf(filename, sizeof(filename), "/home/yuzhaoxi/tmp/jitcache-%d.map", pid);
    FILE *fd = fopen(filename, "w");
    if (!fd){
        fprintf(stderr, "Couldn't open %s: errno(%d)", filename, errno);
        exit(0);
    }
    return fd;
}

void write_jitcache_entry(FILE *jitcache_file, const void *code_addr, unsigned int code_size, 
    const char *entry, struct timespec ts, const char flg){
    if(jitcache_file){
        fprintf(jitcache_file, "[%c] %ld.%ld::%lx %x %s\n", flg, ts.tv_sec, ts.tv_nsec, (unsigned long) code_addr, code_size, entry);
    }
}

void write_agct_entry(FILE *agct_file, const void *entry){
    fprintf(agct_file, "%s\n", entry);    
}