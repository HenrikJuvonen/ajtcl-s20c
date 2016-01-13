#include "aj_target.h"
#include "aj_debug.h"
#include "aj_util.h"
#include <socket.h>
#include <rubus.h>
#include <effs.h>
#include <system.h>

struct _arg {
	void (*task)(int, char**);
	int argc;
	char **argv;
} arg;

static void _task(void* p) {
	struct _arg* pArg = p;
	pArg->task(pArg->argc, pArg->argv);
}

AJ_Status AJ_CreateTask(void (*task)(int, char**), char* name, int argc, char** argv) {	
	arg.task = task;
	arg.argc = argc;
	arg.argv = argv;
	pthread_t th;
	pthread_attr_t pthread_attr;
	pthread_attr_init(&pthread_attr);
    pthread_attr.thread_name = name;
	int status = pthread_create(&th, (const pthread_attr_t*) &pthread_attr, (void*(*)(void*)) &_task, (void*) &arg);
	return status == 0 ? AJ_OK : AJ_ERR_NULL;
}

#define _log_buffersize 1024*8

void _errorHandler(int error, int id) {
	asm("bkpt");
}

AJ_Status AJ_InitSystem() {
	sysInitAttr_t sysattr;
	sysattr.dma[0] = 0;
	sysattr.dma[1] = 0x400;
	sysattr.dma[2] = 0;
	sysattr.dma[3] = 0x300;
	sysattr.dma[4] = 0;
	sysattr.dma[5] = 0;
	sysattr.dma[6] = 0;
	sysattr.dma[7] = 0;
	int status = sysInit(&sysattr);
	printf("Initialized System.\n");
	return status == 0 ? AJ_OK : AJ_ERR_NULL;
}

AJ_Status AJ_InitOS() {
	osinit_attr_t os_attr = {0};					 
    os_attr.log_buffersize = _log_buffersize;
    os_attr.log_buffer = (unsigned int*)malloc(_log_buffersize);
	os_attr.errorhandler = &_errorHandler;
	int status1 = os_init(&os_attr, _R_INIT);
	int status2 = os_logstart(_R_OVERWRITE);
	printf("Initialized OS.\n");
	return status1 == 0 && status2 == 0 ? AJ_OK : AJ_ERR_NULL;
}
				
AJ_Status AJ_StartOS() {
	printf("Starting OS...\n");	
	os_start();
	return AJ_ERR_NULL;	
}

