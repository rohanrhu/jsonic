/*
 * jsonic is a json parser for C
 *
 * https://github.com/rohanrhu/jsonic
 * https://oguzhaneroglu.com/projects/jsonic/
 *
 * Licensed under MIT
 * Copyright (C) 2018, Oğuzhan Eroğlu (https://oguzhaneroglu.com/) <rohanrhu2@gmail.com>
 */

#ifndef __JSONIC_H__
#define __JSONIC_H__

enum JSONIC_NODE_TYPES {
    NONE,
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOLEAN,
    NULLVAL
};
typedef enum JSONIC_NODE_TYPES jsonic_node_type_t;

enum JSONIC_PARSER_STATES {
    EXPECT_ANY,
    EXPECT_KEY_START,
    EXPECT_KEY_END,
    EXPECT_VAL_START,
    EXPECT_VAL_END,
    EXPECT_NUMVAL,
    EXPECT_BOOLVAL,
    EXPECT_NULLVAL,
    EXPECT_ARR_END,
    EXPECT_ARR_STR_END,
    EXPECT_ARR_NUMVAL,
    EXPECT_ARR_BOOLVAL,
    EXPECT_ARR_NULLVAL
};
typedef enum JSONIC_PARSER_STATES jsonic_parser_state_t;

typedef struct jsonic_node jsonic_node_t;
struct jsonic_node {
    char* key;
    char* val;
    jsonic_node_type_t type;
    jsonic_parser_state_t parser_state;
    unsigned int len;
    unsigned int plevel;
    unsigned int clevel;
    unsigned int ind;
    unsigned int kind;
    unsigned int arrind;
    unsigned int from;
    unsigned int pos;
    unsigned int ksync;
    unsigned int meta;
};

extern char* jsonic_from_file(char* fname);
extern void jsonic_free_addr(jsonic_node_t* node);
extern void jsonic_free(jsonic_node_t** node);
extern jsonic_node_t* jsonic_get(char* json_str, jsonic_node_t* current, char* key, int index, int from, jsonic_node_t* from_node, int is_get_root, int is_kv_iter);
extern jsonic_node_t* jsonic_get_root(char* json_str);
extern jsonic_node_t* jsonic_object_get(char* json_str, jsonic_node_t* current, char* key);
extern jsonic_node_t* jsonic_object_iter(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
extern jsonic_node_t* jsonic_object_iter_free(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
extern jsonic_node_t* jsonic_object_iter_kv(char* json_str, jsonic_node_t* current, jsonic_node_t* from);
extern jsonic_node_t* jsonic_object_iter_kv_free(char* json_str, jsonic_node_t* current, jsonic_node_t* from);
extern jsonic_node_t* jsonic_array_get(char* json_str, jsonic_node_t* current, int index);
extern jsonic_node_t* jsonic_array_iter(char* json_str, jsonic_node_t* current, jsonic_node_t* node, int index);
extern jsonic_node_t* jsonic_array_iter_free(char* json_str, jsonic_node_t* current, jsonic_node_t* node, int index);
extern int jsonic_array_length(char* json_str, jsonic_node_t* array);
extern int jsonic_array_length_from(char* json_str, jsonic_node_t* array, jsonic_node_t* from);
extern int jsonic_from_node(jsonic_node_t* node);
extern int jsonic_from_node_free(jsonic_node_t* node);

int is_bool_char(char c);
int is_null_char(char c);

#endif