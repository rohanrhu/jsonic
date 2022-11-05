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
    JSONIC_NONE,
    JSONIC_OBJECT,
    JSONIC_ARRAY,
    JSONIC_STRING,
    JSONIC_NUMBER,
    JSONIC_BOOLEAN,
    JSONIC_NULL
};
typedef enum JSONIC_NODE_TYPES jsonic_node_type_t;

enum JSONIC_PARSER_STATES {
    JSONIC_EXPECT_ANY,
    JSONIC_EXPECT_KEY_START,
    JSONIC_EXPECT_KEY_END,
    JSONIC_EXPECT_VAL_START,
    JSONIC_EXPECT_VAL_END,
    JSONIC_EXPECT_NUMVAL,
    JSONIC_EXPECT_BOOLVAL,
    JSONIC_EXPECT_NULLVAL,
    JSONIC_EXPECT_ARR_END,
    JSONIC_EXPECT_ARR_STR_END,
    JSONIC_EXPECT_ARR_NUMVAL,
    JSONIC_EXPECT_ARR_BOOLVAL,
    JSONIC_EXPECT_ARR_NULLVAL
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
    int ksync;
    int meta;
    int ending;
};

typedef struct jsonic_length jsonic_length_t;
struct jsonic_length {
    unsigned int length;
    unsigned int index;
};

extern char* jsonic_from_file(char* fname);
extern void jsonic_free_addr(jsonic_node_t* node);
extern void jsonic_free(jsonic_node_t** node);
extern jsonic_node_t* jsonic_get(char* json_str, jsonic_node_t* current, char* key, unsigned int index, unsigned int from, jsonic_node_t* from_node, int is_get_root, int is_kv_iter);
extern jsonic_node_t* jsonic_get_root(char* json_str);
extern jsonic_node_t* jsonic_object_get(char* json_str, jsonic_node_t* current, char* key);
extern jsonic_node_t* jsonic_object_iter(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
extern jsonic_node_t* jsonic_object_iter_index(char* json_str, jsonic_node_t* current, jsonic_node_t* from_node_or_null, unsigned int from_index, char* key);
extern jsonic_node_t* jsonic_object_iter_free(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
extern jsonic_node_t* jsonic_object_iter_kv(char* json_str, jsonic_node_t* current, jsonic_node_t* from);
extern jsonic_node_t* jsonic_object_iter_kv_free(char* json_str, jsonic_node_t* current, jsonic_node_t* from);
extern jsonic_node_t* jsonic_array_get(char* json_str, jsonic_node_t* current, int index);
extern jsonic_node_t* jsonic_array_iter(char* json_str, jsonic_node_t* current, jsonic_node_t* from, int index);
extern jsonic_node_t* jsonic_array_iter_free(char* json_str, jsonic_node_t* current, jsonic_node_t* node, int index);
extern jsonic_length_t jsonic_array_length(char* json_str, jsonic_node_t* array);
extern jsonic_length_t jsonic_array_length_from(char* json_str, jsonic_node_t* array, jsonic_node_t* from);
extern unsigned int jsonic_from_node(jsonic_node_t* node);
extern unsigned int jsonic_from_node_free(jsonic_node_t* node);
extern unsigned int jsonic_from_length(jsonic_length_t length);

int is_bool_char(char c);
int is_null_char(char c);

#endif