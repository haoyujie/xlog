#pragma once
#include <stdint.h>

#define XLOG_MAGIC_FLAG 0x5a
#define XLOG_MAX_VARS 20

typedef struct {
    int16_t magicflag;
    int16_t cpu_core;
    int16_t radioframe;
    int16_t slot;
    uint64_t curtimetick;
    int16_t datacount;
    int16_t pad[3];
    uint32_t var[XLOG_MAX_VARS];
} LogCell;
typedef struct LogCellNode {
    LogCell data;
    struct LogCellNode* next;
} LogCellNode;


#define BLOCKS_COUNT_PER_FILE CPU_CORE_COUNT

#ifdef __cplusplus
extern "C" {
#endif



// Function prototypes for Xlog module
void xlog_init(void);
int xlog_write(Xblock* block);
int xlog_read(Xblock* block);
void xlog_close(void);

int xlogTest();

#ifdef __cplusplus
}
#endif
