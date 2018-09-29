#include "keyvalue.h"
#include <stdio.h>
#define _GNU_SOURCE
#include <search.h>
#include <string.h>
#include <stdlib.h>

struct hsearch_data *hashtable;

struct store {
  char *value;
};

char *get(char *key) {
  ENTRY item;
  ENTRY *found_item;
  char ret_msg[256], *answer;

  item.key = key;
  while (1) {
    hsearch_r(item, FIND, found_item, hashtable);
    if (found_item != NULL) {
      return ((char *)found_item->data);
    } else {
      return "NULL";
    }
    // if ((found_item = hsearch(item, FIND)) != NULL) {
    //   // printf("found: %s, value: %s\n", found_item->key, ((char *)found_item->data));
    //   return ((char *)found_item->data);
    // } else {
    //   // printf("not found\n");
    //   return "NULL";
    // }
  }
}

void put(char *key, char *value) {
  struct store store_space[64];
  struct store *store_ptr = store_space;
  ENTRY item, *found_item;
  int finished = 1;

  store_ptr->value = strdup(value);
  // printf("%s\n", store_ptr->value);
  while (finished) {
    item.key = strdup(key);
    item.data = store_ptr->value;
    store_ptr++;
    hsearch_r(item, ENTER, hashtable);
    // hsearch(item, ENTER);
    finished = 0;
    // printf("%s\n", item.data);
  }
}

void create(int size) {
  hcreate_r(size, hashtable);
}
