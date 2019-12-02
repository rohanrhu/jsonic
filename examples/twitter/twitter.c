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
    char* json_string = jsonic_from_file("twitter.json");
    
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    jsonic_node_t* root = jsonic_get_root(json_string);
    
    jsonic_node_t* statuses = jsonic_object_get(json_string, root, "statuses");
    int statuses_length = jsonic_array_length(json_string, statuses);

    jsonic_node_t* id;
    jsonic_node_t* user;
    jsonic_node_t* name;
    jsonic_node_t* text;
    
    printf("Statuses Total: %d\n", statuses_length);

    jsonic_node_t* status = NULL;
    for (;;) {
        status = jsonic_array_iter_free(json_string, statuses, status, 0);
        if (!status) break;

        id = jsonic_object_get(json_string, status, "id");

        // JSON object iteration with keys
        text = jsonic_object_iter(json_string, status, id, "text");
        user = jsonic_object_iter(json_string, status, text, "user");

        name = jsonic_object_get(json_string, user, "name");

        printf("ID: %s\n", id->val);
        printf("Name: %s\n", name->val);
        printf("Text: %s\n", text->val);
        printf("\n");

        jsonic_free(&id);
        jsonic_free(&user);
        jsonic_free(&name);
        jsonic_free(&text);
    }

    jsonic_free(&statuses);
    jsonic_free(&root);
    free(json_string);
    
    gettimeofday(&t1, NULL);
    long int usecs = (t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec);
    printf("\033[1;32m%ld\033[0mus elapsed.\n", usecs);
    printf("\033[1;32m%.2f\033[0ms elapsed.\n", usecs/1000000.0);
    
    return 0;
}