/*
 * jsonic is a json parser for C
 *
 * https://github.com/rohanrhu/jsonic
 * https://oguzhaneroglu.com/projects/jsonic/
 *
 * Licensed under MIT
 * Copyright (C) 2018, Oğuzhan Eroğlu (https://oguzhaneroglu.com/) <rohanrhu2@gmail.com>
 */

/*
 * Important:
 * * These examples are not benchmarks so you must always use iterations functions rather than *_get() variants!
 * * You must never use inline usages of Jsonic functions!
 */

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include "jsonic.h"

int main() {
    char* json_string = jsonic_from_file("kviteration.json");
    
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);
    
    jsonic_node_t* root = jsonic_get_root(json_string);
    jsonic_node_t* keys = jsonic_object_get(json_string, root, "keys");

    jsonic_node_t* not_exists_key = jsonic_object_get(json_string, root, "not_exists_key");
    printf("Key Count: %d (keys itered at total)\n", not_exists_key->pos);

    jsonic_node_t* arr = jsonic_object_iter(json_string, root, keys, "arr");
    jsonic_node_t* not_exists_item = jsonic_array_iter(json_string, arr, NULL, 100);
    printf("Item Count: %d (items itered at total)\n\n", not_exists_item->pos);
    
    jsonic_node_t* item;

    item = jsonic_array_get(json_string, arr, 5);
    printf("Item 5 Pos: %d\n", item->pos);
    
    item = jsonic_array_iter(json_string, arr, item, 0);
    printf("Item 6 Pos: %d\n", item->pos);

    printf("Key Value Pairs\n");

    jsonic_node_t* key = NULL;
    for (;;) {
        key = jsonic_object_iter_kv_free(json_string, keys, key);
        if (key->type == JSONIC_NONE) break;
        
        if ((key->type == JSONIC_STRING) || (key->type == JSONIC_NUMBER)) {
            printf("  %s => %s\n", key->key, key->val);
        }
    }

    printf("\n");

    jsonic_free(&key);
    jsonic_free(&root);
    free(json_string);
    
    gettimeofday(&t1, NULL);
    long int usecs = (t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec);
    printf("\033[1;32m%ld\033[0mus elapsed.\n", usecs);
    printf("\033[1;32m%.2f\033[0ms elapsed.\n", usecs/1000000.0);
    
    return 0;
}