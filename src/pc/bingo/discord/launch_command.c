#include "launch_command.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <winuser.h>
#else
#include <unistd.h>
#define MAX_PATH 1024
#endif
#define MAX_LAUNCH_CMD (MAX_PATH + 12)

void register_launch_command(void) {
  char cmd[MAX_LAUNCH_CMD];
  int rc;
#if defined(_WIN32) || defined(_WIN64)
  HMODULE hModule = GetModuleHandle(NULL);
  if (hModule == NULL) {
    // LOG_ERROR("unable to retrieve absolute path!");
    return;
  }
  GetModuleFileName(hModule, cmd, sizeof(cmd));
#else
  rc = readlink("/proc/self/exe", cmd, sizeof(MAX_LAUNCH_CMD) - 1);
  if (rc) {
    // LOG_ERROR("unable to retrieve absolute path! rc = %d", rc);
    return;
  }
#endif
  // strncat(cmd, " --discord 1", MAX_LAUNCH_CMD - 1);
  DISCORD_CHECK(gDiscord.activity->register_command(gDiscord.activity, cmd), "register_command");
  DISCORD_LOG("registered launch command: %s", cmd);
}
