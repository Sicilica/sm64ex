#include "config.h"

#define BINGO_CONFIG_FILE "bingo.json"
#define HOST_KEY "host"
#define PORT_KEY "port"
#define USERNAME_KEY "username"
#define COLOR_KEY "color"

char* host = "127.0.0.1";
int port = 6000;
char* username = "Player";
int preferred_color = 0;

void load_bingo_config() {
  FILE *file = fopen(BINGO_CONFIG_FILE, "r");
  if (file == NULL) {
    NOUVEAU_LOG("bingo.json not found, creating one...");
    save_bingo_config();
    return;
  }

  char buf[1024];
  fread(buf, sizeof(char), 1024, file);
  fclose(file);

  cJSON *obj = cJSON_ParseWithLength(buf, 1024);
  cJSON *entry;

  entry = cJSON_GetObjectItemCaseSensitive(obj, HOST_KEY);
  if (cJSON_IsString(entry)) {
    host = malloc(strlen(entry->valuestring));
    strcpy(host, entry->valuestring);
  }

  entry = cJSON_GetObjectItemCaseSensitive(obj, PORT_KEY);
  if (cJSON_IsNumber(entry)) {
    port = entry->valueint;
  }

  entry = cJSON_GetObjectItemCaseSensitive(obj, USERNAME_KEY);
  if (cJSON_IsString(entry)) {
    username = malloc(strlen(entry->valuestring));
    strcpy(username, entry->valuestring);
  }

  entry = cJSON_GetObjectItemCaseSensitive(obj, COLOR_KEY);
  if (cJSON_IsNumber(entry)) {
    preferred_color = entry->valueint;
  }

  cJSON_Delete(obj);

  NOUVEAU_LOG("config loaded (user=%s, host=%s, port=%d)", username, host, port);
}

void save_bingo_config() {
  cJSON *obj = cJSON_CreateObject();
  cJSON_AddStringToObject(obj, HOST_KEY, host);
  cJSON_AddNumberToObject(obj, PORT_KEY, port);
  cJSON_AddStringToObject(obj, USERNAME_KEY, username);
  cJSON_AddNumberToObject(obj, COLOR_KEY, preferred_color);
  char* contents = cJSON_Print(obj);
  cJSON_Delete(obj);

  FILE *file = fopen(BINGO_CONFIG_FILE, "w");
  if (file == NULL) {
    NOUVEAU_ERR("fopen bingo.json for writing");
    free(contents);
    return;
  }

  fprintf(file, "%s\n", contents);

  fclose(file);
  free(contents);
}
