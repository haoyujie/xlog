#pragma once
#include <stdint.h>


#define XLOG_MAGIC_FLAG 0x5a
#define XLOG_MAX_VARS 20

typedef struct {
    int16_t magicflag;
    int16_t cpu_core;
    uint32_t threadid;
    uint32_t threadpad;
	uint32_t thread_def_type;
	uint32_t thread_def_id;	
	uint32_t logtype;
    int16_t radioframe;
    int16_t slot;
	uint64_t timestamp;
    int16_t datacount;

    uint32_t var[XLOG_MAX_VARS];
} LogCell;
typedef struct LogCellNode {
    LogCell *data_prt;
    struct LogCellNode* next;
} LogCellNode;

#define CELLS_COUNT_PER_BLOCK 65536
#define CPU_CORE_COUNT 8
#define BLOCKS_COUNT_PER_FILE CPU_CORE_COUNT

#ifdef __cplusplus
extern "C" {
#endif

LogCell* getNextLogCell(int cpu_core);

// Function prototypes for Xlog module
void xlog_init(void);
//int xlog_write(Xblock* block);
//int xlog_read(Xblock* block);
void xlog_close(void);

int xlogTest();

#ifdef __cplusplus
}
#endif
