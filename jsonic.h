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
#define JSONIC_NODE_TYPE_INTEGER 4
#define JSONIC_NODE_TYPE_BOOLEAN 5

#define JSONIC_PARSER_STATE_EXPECT_ANY 1
#define JSONIC_PARSER_STATE_EXPECT_KEY_START 2
#define JSONIC_PARSER_STATE_EXPECT_KEY_END 3
#define JSONIC_PARSER_STATE_EXPECT_VAL_START 4
#define JSONIC_PARSER_STATE_EXPECT_VAL_END 5
#define JSONIC_PARSER_STATE_EXPECT_INTVAL 6
#define JSONIC_PARSER_STATE_EXPECT_BOOLVAL 7
#define JSONIC_PARSER_STATE_EXPECT_ARR_END 8
#define JSONIC_PARSER_STATE_EXPECT_ARR_STR_END 9
#define JSONIC_PARSER_STATE_EXPECT_ARR_INTVAL 10
#define JSONIC_PARSER_STATE_EXPECT_ARR_BOOLVAL 11

typedef struct jsonic_node jsonic_node_t;
struct jsonic_node {
    int type;
    char* val;
    int len;
    int plevel;
    int clevel;
    int ind;
    int kind;
    int arrind;
    int ntype;
    int parser_state;
    int ksync;
};

extern char* jsonic_from_file(char* fname);
extern void jsonic_free(jsonic_node_t* node);
extern jsonic_node_t* jsonic_get(char* json_str, jsonic_node_t* current, char* key, int index);
extern int jsonic_array_length(char* json_str, jsonic_node_t* array);

int is_bool_char(char c);

#endif