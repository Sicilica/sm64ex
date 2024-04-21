#include "json/cJSON.h"
#include <stdio.h>

void jank_test() {
  cJSON *obj = cJSON_CreateObject();
  if (obj == NULL) {
    printf("failed to create json object\n");
    return;
  }

  if (cJSON_AddStringToObject(obj, "name", "Big Dude") == NULL) {
    printf("failed to add string to object\n");
  }


  // TODO can also use cJSON_PrintPreallocated to avoid allocating any memory at all
  char *str = cJSON_PrintUnformatted(obj);
  if (str == NULL) {
    printf("failed to generate json for some reason\n");
  } else {
    printf("json: %s\n", str);
  }
  cJSON_Delete(obj);
}
