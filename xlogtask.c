#include "pthread.h"
#include "stdio.h"
#include "assert.h"
#include "xlog.h"
#include "xlogdef.h"
#include "xlogtimetick.h"
#include "xlogtask.h"
#include "xlogtimetick.h"


// Function to get a new xlogblock and save taskinfo to it
LogCell* xlog_push_taskgen(int cpu_core, uint32_t threadid, int16_t radioframe, int16_t slot, uint32_t tasktype, uint32_t taskid,  uint32_t taskqueueid) {
    // Get the current CPU core
    //int cpu_core = sched_getcpu();
    
    // Get the next available LogCell for this CPU core
    LogCell* cell = getNextLogCell(cpu_core);
    
    if (cell == NULL) {
        // Handle error: unable to get a new LogCell
        fprintf(stderr, "Error: Unable to get a new LogCell\n");
        return NULL;
    }
    cell->cpu_core = cpu_core;
    cell->threadid = threadid;
    cell->radioframe = radioframe;
    cell->slot = slot;
    // Initialize the LogCell
    cell->logtype = XLOG_TASK_GEN;  // Assuming XLOG_TASK_GEN is for task generation
	cell->timestamp = xlogTick(); // Assuming xlog_get_timestamp() exists
    
    // Create and populate xlogtaskinf
    xlogtaskinf *taskInfo = (xlogtaskinf *)cell->var;
    taskInfo->tasktype = tasktype;
    taskInfo->taskid = taskid;
    taskInfo->taskqueueid = taskqueueid;
    return cell;
}


// task started and push inf to xlog
LogCell* xlog_push_taskexec(int cpu_core, uint32_t threadid, int16_t radioframe, int16_t slot, uint32_t tasktype, uint32_t taskid,  uint32_t taskqueueid) {
    // Get the current CPU core
    //int cpu_core = sched_getcpu();
    
    // Get the next available LogCell for this CPU core
    LogCell* cell = getNextLogCell(cpu_core);
    
    if (cell == NULL) {
        // Handle error: unable to get a new LogCell
        fprintf(stderr, "Error: Unable to get a new LogCell\n");
        return NULL;
    }
    cell->cpu_core = cpu_core;
    cell->radioframe = radioframe;
    cell->slot = slot;
    // Initialize the LogCell
    cell->logtype = XLOG_TASK_EXEC;  // Assuming XLOG_TASK_GEN is for task generation
	cell->timestamp = xlogTick(); // Assuming xlog_get_timestamp() exists
    
    // Create and populate xlogtaskinf
    xlogtaskinf *taskInfo = (xlogtaskinf *)cell->var;
    taskInfo->tasktype = tasktype;
    taskInfo->taskid = taskid;
    taskInfo->taskqueueid = taskqueueid;
    return cell;
}


// task finished and push inf to xlog
LogCell* xlog_push_taskfinish(int cpu_core, uint32_t threadid, int16_t radioframe, int16_t slot, uint32_t tasktype, uint32_t taskid, uint32_t taskqueueid) {
	// Get the current CPU core
	//int cpu_core = sched_getcpu();
    
	// Get the next available LogCell for this CPU core
	LogCell* cell = getNextLogCell(cpu_core);
    
	if (cell == NULL) {
		// Handle error: unable to get a new LogCell
		fprintf(stderr, "Error: Unable to get a new LogCell\n");
		return NULL;
	}
	cell->cpu_core = cpu_core;
	cell->radioframe = radioframe;
	cell->slot = slot;
	// Initialize the LogCell
	cell->logtype = XLOG_TASK_FINISH; // Assuming XLOG_TASK_GEN is for task generation
	cell->timestamp = xlogTick(); // Assuming xlog_get_timestamp() exists
    
    // Create and populate xlogtaskinf
	xlogtaskinf *taskInfo = (xlogtaskinf *)cell->var;
	taskInfo->tasktype = tasktype;
	taskInfo->taskid = taskid;
	taskInfo->taskqueueid = taskqueueid;
	return cell;
}
