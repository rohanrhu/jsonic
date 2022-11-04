/*
 * jsonic is a json parser for C
 *
 * https://github.com/rohanrhu/jsonic
 * https://oguzhaneroglu.com/projects/jsonic/
 *
 * Licensed under MIT
 * Copyright (C) 2018, Oğuzhan Eroğlu (https://oguzhaneroglu.com/) <rohanrhu2@gmail.com>
 */

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include "jsonic.h"

int main() {
    char* json_string = jsonic_from_file("heroes.json");

    if (json_string == NULL) {
        printf("JSON file not found.\n");
        exit(0);
    }
    
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);
    
    jsonic_node_t* root = jsonic_get_root(json_string);

    jsonic_node_t* members = jsonic_object_get(json_string, root, "members");
    jsonic_node_t* member = jsonic_array_get(json_string, members, 1);
    jsonic_node_t* powers = jsonic_object_get(json_string, member, "powers");
    
    // inline usage: non-free'd nodes and non-safe pointers!..
    // using jsonic_object_get() is poor way because of reading it from start of json each time!
    // use jsonic_object_iter() or jsonic_object_iter_free() instead of jsonic_object_get().
    printf("Squad: %s\n", jsonic_object_get(json_string, root, "squadName")->val);
    printf("Active: %s\n", jsonic_object_get(json_string, root, "active")->val);
    printf("Formed: %s\n", jsonic_object_get(json_string, root, "formed")->val);
    printf("Name: %s\n", jsonic_object_get(json_string, member, "name")->val);
    printf("Age: %s\n", jsonic_object_get(json_string, member, "age")->val);
    printf("Powers (%d total):\n", jsonic_array_length(json_string, powers));
    
    jsonic_node_t* power = NULL;
    for (;;) {
        power = jsonic_array_iter_free(json_string, powers, power, 0);
        if (power->type == JSONIC_NONE) break;
        
        if (power->type == JSONIC_STRING) {
            printf(
                "\t%s (pos: %d, from len: %d)\n",
                power->val,
                power->pos,
                jsonic_array_length_from(json_string, powers, power)
            );
        }
    }

    jsonic_free(&members);
    jsonic_free(&member);
    jsonic_free(&powers);
    jsonic_free(&root);
    free(json_string);
    
    gettimeofday(&t1, NULL);
    long int usecs = (t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec);
    printf("%ldus elapsed.\n", usecs);
    printf("%.2fms elapsed.\n", usecs/1000000.0);
    
    return 0;
}