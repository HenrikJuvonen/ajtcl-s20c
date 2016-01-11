#include "aj_target.h"
#include "aj_debug.h"
#include <effs.h>
#include <ish.h>
#include <rubus.h>
#include <socket.h>
#include <system.h>

char *pFlashBuffer;

void init() {
  effs_init();
  mount_flash(0, &pFlashBuffer);
  if (sysDeveloperBooted()) {
    int status = mount_developer_ramdisk(1);
    if (status) {
      printf("Failed to mount RAM disk: %d\n", status);
    }
  }
  setenv("ER_DEBUG_ALL", "1", 1);
}

static void main_task(int argc, char *argv[]) {
  init();
  AllJoyn_Start();
  while (1)
    ;
}

int main(int argc, char *argv[]) {
  AJ_InitSystem();
  AJ_InitOS();
  AJ_CreateTask(main_task, "AlljoynTask", argc, argv);
  AJ_StartOS();
  return 0;
}
