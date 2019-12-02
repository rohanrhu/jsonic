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

#define JSONIC_NODE_TYPE_NONE 0
#define JSONIC_NODE_TYPE_OBJECT 1
#define JSONIC_NODE_TYPE_ARRAY 2
#define JSONIC_NODE_TYPE_STRING 3
#define JSONIC_NODE_TYPE_NUMBER 4
#define JSONIC_NODE_TYPE_BOOLEAN 5
#define JSONIC_NODE_TYPE_NULL 6

#define JSONIC_PARSER_STATE_EXPECT_ANY 1
#define JSONIC_PARSER_STATE_EXPECT_KEY_START 2
#define JSONIC_PARSER_STATE_EXPECT_KEY_END 3
#define JSONIC_PARSER_STATE_EXPECT_VAL_START 4
#define JSONIC_PARSER_STATE_EXPECT_VAL_END 5
#define JSONIC_PARSER_STATE_EXPECT_NUMVAL 6
#define JSONIC_PARSER_STATE_EXPECT_BOOLVAL 7
#define JSONIC_PARSER_STATE_EXPECT_NULLVAL 8
#define JSONIC_PARSER_STATE_EXPECT_ARR_END 9
#define JSONIC_PARSER_STATE_EXPECT_ARR_STR_END 10
#define JSONIC_PARSER_STATE_EXPECT_ARR_NUMVAL 11
#define JSONIC_PARSER_STATE_EXPECT_ARR_BOOLVAL 12
#define JSONIC_PARSER_STATE_EXPECT_ARR_NULLVAL 13

typedef struct jsonic_node jsonic_node_t;
struct jsonic_node {
    int type;
    char* val;
    unsigned int len;
    unsigned int plevel;
    unsigned int clevel;
    unsigned int ind;
    unsigned int kind;
    unsigned int arrind;
    int parser_state;
    int ksync;
    int meta;
    unsigned int from;
    unsigned int pos;
};

extern char* jsonic_from_file(char* fname);
extern void jsonic_free_addr(jsonic_node_t* node);
extern void jsonic_free(jsonic_node_t** node);
extern jsonic_node_t* jsonic_get(char* json_str, jsonic_node_t* current, char* key, int index, int from, jsonic_node_t* from_node, int is_get_root);
extern jsonic_node_t* jsonic_get_root(char* json_str);
extern jsonic_node_t* jsonic_object_get(char* json_str, jsonic_node_t* current, char* key);
extern jsonic_node_t* jsonic_object_iter(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
extern jsonic_node_t* jsonic_object_iter_free(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
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