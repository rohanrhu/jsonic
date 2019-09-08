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
    // 180MB~ JSON file
    char* json_string = jsonic_from_file("cities.json");
    
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    jsonic_node_t* type = jsonic_object_get(json_string, NULL, "type");
    jsonic_node_t* features = jsonic_object_get(json_string, NULL, "features");
    int features_length = jsonic_array_length(json_string, features);

    printf("Type: %s\n", type->val);
    printf("Total Features: %d\n", features_length);

    jsonic_node_t* feature = NULL;
    jsonic_node_t* properties;
    jsonic_node_t* MAPBLKLOT;
    jsonic_node_t* geometry;
    jsonic_node_t* coordinates;
    jsonic_node_t* coordinate;
    jsonic_node_t* coordinate0;
    jsonic_node_t* x = NULL;
    jsonic_node_t* y = NULL;

    for (;;) {
        feature = jsonic_array_iter_free(json_string, features, feature, 0);
        if (!feature) break;

        properties = jsonic_object_get(json_string, feature, "properties");
        geometry = jsonic_object_iter(json_string, feature, properties, "geometry");
        MAPBLKLOT = jsonic_object_get(json_string, properties, "MAPBLKLOT");
        coordinates = jsonic_object_get(json_string, geometry, "coordinates");
        coordinate = jsonic_array_get(json_string, coordinates, 0);
        coordinate0 = jsonic_array_get(json_string, coordinate, 0);

        // if ((x = jsonic_array_get(json_string, coordinate0, 0))->type == JSONIC_NODE_TYPE_NUMBER) {
        //     y = jsonic_array_iter(json_string, coordinate0, x, 0);
        //     printf("MAPBLKLOT: %s, X: %s, Y: %s\n", MAPBLKLOT->val, x->val, y ? y->val: "(NONE)");
        // } else {
        //     y = NULL;
        //     printf("MAPBLKLOT: %s\n", MAPBLKLOT->val);
        // }

        jsonic_free(&properties);
        jsonic_free(&MAPBLKLOT);
        jsonic_free(&geometry);
        jsonic_free(&coordinates);
        jsonic_free(&coordinate);
        jsonic_free(&coordinate0);
        jsonic_free(&x);
        if (y) jsonic_free(&y);
    }

    jsonic_free(&type);
    jsonic_free(&features);
    free(json_string);
    
    gettimeofday(&t1, NULL);
    long int usecs = (t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec);
    printf("\033[1;32m%ld\033[0mus elapsed.\n", usecs);
    printf("\033[1;32m%.2f\033[0ms elapsed.\n", usecs/1000000.0);
    
    return 0;
}