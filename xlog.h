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

// Function to get a new xlogblock and save taskinfo to it
LogCell* xlog_push_taskgen(int cpu_core, uint32_t threadid, int16_t radioframe, int16_t slot, uint32_t tasktype, uint32_t taskid, uint32_t taskqueueid);
// task started and push inf to xlog
LogCell* xlog_push_taskexec(int cpu_core, uint32_t threadid, int16_t radioframe, int16_t slot, uint32_t tasktype, uint32_t taskid, uint32_t taskqueueid);
// task finished and push inf to xlog
LogCell* xlog_push_taskfinish(int cpu_core, uint32_t threadid, int16_t radioframe, int16_t slot, uint32_t tasktype, uint32_t taskid, uint32_t taskqueueid);


#ifdef __cplusplus
}
#endif
