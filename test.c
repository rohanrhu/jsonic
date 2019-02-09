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
	char* json_string = jsonic_from_file("test.json");
    
    struct timeval t0, t1;
	gettimeofday(&t0, NULL);
	
	jsonic_node_t* members = jsonic_get(json_string, NULL, "members", 0);
    jsonic_node_t* member = jsonic_get(json_string, members, NULL, 1);
    jsonic_node_t* powers = jsonic_get(json_string, member, "powers", 0);
    
    // inline usage: non-free'd nodes and non-safe pointers!..
    printf("Squad: %s\n", jsonic_get(json_string, NULL, "squadName", 0)->val);
    printf("Active: %s\n", jsonic_get(json_string, NULL, "active", 0)->val);
    printf("Formed: %s\n", jsonic_get(json_string, NULL, "formed", 0)->val);
    printf("Name: %s\n", jsonic_get(json_string, member, "name", 0)->val);
    printf("Age: %s\n", jsonic_get(json_string, member, "age", 0)->val);
    printf("Powers (%d total):\n", jsonic_array_length(json_string, powers));
    
    jsonic_node_t* power; 
    for (int i=0;; i++) {
    	power = jsonic_get(json_string, powers, NULL, i);
    	if (!power) break;
		
        if (power->type == JSONIC_NODE_TYPE_STRING) {
            printf("\t%s\n", power->val);
        }
		
		jsonic_free(power);
	}

    jsonic_free(members);
    jsonic_free(member);
    jsonic_free(powers);
    free(json_string);
    
    gettimeofday(&t1, NULL);
    printf("\n%ldus elapsed.\n", (t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec));
	
    return 0;
}