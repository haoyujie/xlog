#include "xlog.h"
#include "pthread.h"
#include "stdio.h"
#include "assert.h"

/*
	To test the library, include "xlog.h" from an application project
	and call xlogTest().
	
	Do not forget to add the library to Project Dependencies in Visual Studio.
*/
#define xprintf printf
static int s_Test = 0;

int xlogTest()
{
	return ++s_Test;
}
// Array of pointers to LogCell, one for each CPU core
static LogCell* s_logCells[CPU_CORE_COUNT] = {NULL};

// Mutex for thread-safe access to s_logCells
static pthread_mutex_t s_logCellsMutex = PTHREAD_MUTEX_INITIALIZER;

// Structure to represent a block of log cells

typedef struct {
    LogCellNode* cells[CELLS_COUNT_PER_BLOCK];  // Array of pointers to LogCellNodes
    int cellCount;  // Number of cells in the block
	LogCellNode* ptrCurrentCellNode;
} Xblock;

// Array of pointers to Xblocks, one for each CPU core
static Xblock* s_blocks[CPU_CORE_COUNT] = {NULL};


// Initialize the xlog system
void xlog_init(void) {
	// Allocate memory for each Xblock
	for (int icore = 0; icore < CPU_CORE_COUNT; icore++) {
        // Allocate memory for each Xblock
        s_blocks[icore] = (Xblock*)malloc(sizeof(Xblock)*CELLS_COUNT_PER_BLOCK);
		if(NULL == s_blocks[icore]){
			xprintf("memory aloc failed for s_blocks[%d].\n",icore);
			return;
		}
		// Initialize the Xblock
		memset(s_blocks[icore], 0, sizeof(Xblock)*CELLS_COUNT_PER_BLOCK);
    }

	// Initialize the Xblock for each core
	for (int icore = 0; icore < CPU_CORE_COUNT; icore++) {
		Xblock *pBlock = s_blocks[icore];
		pBlock->cellCount = CELLS_COUNT_PER_BLOCK;
		pBlock->ptrCurrentCellNode = NULL;
		//set initial value for each cell's data_prt
		for (int jcell = 0; jcell < CELLS_COUNT_PER_BLOCK; jcell++) {
			pBlock->cells[jcell]->data_prt = NULL;
		}
		//link the cells in a circular way
		for (int jcell = 0; jcell < CELLS_COUNT_PER_BLOCK - 1; jcell++) {
			pBlock->cells[jcell]->next = (pBlock->cells[(jcell + 1)%CELLS_COUNT_PER_BLOCK]);
		}
		//check whether the last cell link to the first cell
		assert(pBlock->cells[CELLS_COUNT_PER_BLOCK - 1]->next == pBlock->cells[0]);
		//set the first cell as the current cell
		pBlock->ptrCurrentCellNode = pBlock->cells[0];
    }
}

// Clean up and free all allocated memory in the xlog system
void xlog_close(void) {
    for (int i = 0; i < CPU_CORE_COUNT; i++) {
        if (s_blocks[i]) {
            // Free the Xblock itself
            free(s_blocks[i]);
            s_blocks[i] = NULL;
        }
    }
}

// Function to get or allocate a LogCell for a specific CPU core
static LogCell* get_or_allocate_log_cell(int cpu_core) {
	if (cpu_core < 0 || cpu_core >= CPU_CORE_COUNT) {
        return NULL;  // Invalid core number
    }

    if (NULL == s_logCells[cpu_core] ) {
        // Allocate memory for the LogCell if it doesn't exist
        s_logCells[cpu_core] = (LogCell*)malloc(sizeof(LogCell)*CELLS_COUNT_PER_BLOCK);
		assert(s_logCells[cpu_core] != NULL);
		if(s_logCells[cpu_core] == NULL){
			xprintf("memory aloc failed for s_logCells[%d].\n",cpu_core);
			return NULL;
		}
        {
            // Initialize the newly allocated LogCell
            memset(s_logCells[cpu_core], 0, sizeof(LogCell)*CELLS_COUNT_PER_BLOCK);
			//set magicflag and cpu_core for each cell
			for(int i=0;i<CELLS_COUNT_PER_BLOCK;i++){
				s_logCells[cpu_core][i].magicflag = XLOG_MAGIC_FLAG;
				s_logCells[cpu_core][i].cpu_core = cpu_core;
			}
			assert(s_logCells[cpu_core] != NULL);
			assert(s_blocks[cpu_core] != NULL);
            //check whether the callor have call xlog_init() before call this function.
            if (s_blocks[cpu_core] == NULL) {
				xprintf("s_blocks[cpu_core] == NULL,please init xlog first.\n");
                return NULL;
			}
 			// Bind the LogCell to the corresponding Xblock
			for(int i=0;i<CELLS_COUNT_PER_BLOCK;i++){
				s_blocks[cpu_core]->cells[i]->data_prt = &s_logCells[cpu_core][i];				
            }
        }
    }
    LogCell* result = s_logCells[cpu_core];
    return result;
}
// Function to get or allocate a LogCell for a specific CPU core and bind it to the corresponding Xblock
static LogCell* get_log_cell_sync_mutex(int cpu_core) {
    if (cpu_core < 0 || cpu_core >= CPU_CORE_COUNT) {
        return NULL;  // Invalid core number
    }

    pthread_mutex_lock(&s_logCellsMutex);
	LogCell* result = get_or_allocate_log_cell(cpu_core);
    pthread_mutex_unlock(&s_logCellsMutex);

    return result;
}


// Function to get or allocate a LogCell for a specific CPU core
LogCell* getNextLogCell(int cpu_core) 
{
	if (NULL == s_logCells[cpu_core])
	{
		get_log_cell_sync_mutex(cpu_core);
	}

	assert(NULL != s_blocks[cpu_core]->ptrCurrentCellNode);
	LogCellNode* currentNode = s_blocks[cpu_core]->ptrCurrentCellNode;
	LogCellNode* nextNode = currentNode->next;
    
	// Use CAS to atomically update ptrCurrentCellNode
	while (!__sync_bool_compare_and_swap(&(s_blocks[cpu_core]->ptrCurrentCellNode), 
		currentNode, 
		nextNode)) {
		// If CAS fails, update currentNode and nextNode, and try again
		currentNode = s_blocks[cpu_core]->ptrCurrentCellNode;
		nextNode = currentNode->next;
	}
	//get the current cell data_prt memory
	LogCell* result = s_blocks[cpu_core]->ptrCurrentCellNode->data_prt;
	return result;
}
