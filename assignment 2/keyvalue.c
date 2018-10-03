#define _GNU_SOURCE

#include "keyvalue.h"
#include <stdio.h>
#include <search.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

struct hsearch_data *hashtable;

struct store {
  char *value;
};

char *get(char *key) {
  ENTRY item;
  ENTRY *found_item;
  char *answer;

  item.key = strdup(key);
  while (1) {
    hsearch_r(item, FIND, &found_item, hashtable);
    if (found_item != NULL) {
      return ((char *)found_item->data);
    } else {
      return "NULL";
    }
  }
}

void put(char *key, char *value) {
  struct store store_space[64];
  struct store *store_ptr = store_space;
  ENTRY item, *found_item;
  int finished = 1;

  store_ptr->value = strdup(value);
  printf("%s\n", store_ptr->value);
  while (finished) {
    item.key = strdup(key);
    item.data = store_ptr->value;
    store_ptr++;
    hsearch_r(item, ENTER, &found_item, hashtable);
    found_item->data = item.data;
    finished = 0;
  }
}

void create(int size) {
  hashtable = calloc(1, sizeof(struct hsearch_data));
  hcreate_r(size, hashtable);
}
