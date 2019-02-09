/*
 * jsonic is a json parser for C
 *
 * https://github.com/rohanrhu/jsonic
 * https://oguzhaneroglu.com/projects/jsonic/
 *
 * Licensed under MIT
 * Copyright (C) 2018, Oğuzhan Eroğlu (https://oguzhaneroglu.com/) <rohanrhu2@gmail.com>
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "jsonic.h"

extern char* jsonic_from_file(char* fname) {
    char* json_string;

    FILE *fd;
    fd = fopen(fname,"r");
    fseek(fd, 0, SEEK_END);
    int flen = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    json_string = malloc(sizeof(char)*flen);
    fread(json_string, 1, flen, fd);
    fclose(fd);

    return json_string;
}

extern void jsonic_free(jsonic_node_t* node) {
    if (
        (node->type == JSONIC_NODE_TYPE_BOOLEAN)
        ||
        (node->type == JSONIC_NODE_TYPE_STRING)
        ||
        (node->type == JSONIC_NODE_TYPE_INTEGER)
    ) {
        free(node->val);
    }

    free(node);
}

extern int jsonic_array_length(char* json_str, jsonic_node_t* array) {
    int length;
    jsonic_node_t* node;

    for (length=0;; length++) {
        node = jsonic_get(json_str, array, NULL, length);
        if (!node) break;
        jsonic_free(node);
    }

    return length;
}

extern jsonic_node_t* jsonic_get(
    char* json_str,
    jsonic_node_t* current,
    char* key,
    int index
) {
    jsonic_node_t* node = malloc(sizeof(jsonic_node_t));
    node->type = JSONIC_NODE_TYPE_NONE;
    node->parser_state = (current == NULL) 
                         ? JSONIC_PARSER_STATE_EXPECT_ANY
                         : current->parser_state;
    node->plevel = (current == NULL) ? 0: current->plevel;
    node->clevel = node->plevel;
    node->ind = (current == NULL) ? 0: current->ind;
    node->ksync = 0;
    node->kind = 0;
    node->arrind = 0;
    
    int keylen = 0;
    
    if (key != NULL) {
        keylen = strlen(key);
    }

    char c;

    next:

    c = json_str[node->ind];

    if (c == '\0') {
        if (node->plevel == 0) {
            if (node->type == JSONIC_NODE_TYPE_INTEGER) {
                return node;
            }
        }
        
        goto end;
    }
    
    if (
        (node->plevel > 0)
        &&
        (node->plevel > node->clevel)
    ) {
        if ((c == '{') || (c == '[')) {
            node->plevel++;
        } else if ((c == '}') || (c == ']')) {
            node->plevel--;
        }
        
        node->ind++;
        goto next;
    }

    if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_ANY) {
        if (c == '{') {
            node->clevel = (node->plevel++ == 0) ? 1: node->clevel;
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
            node->ntype = JSONIC_NODE_TYPE_OBJECT;
            
            if (node->plevel > 1) {
                if (key == NULL) {
                    return node;
                }
            }
            
            if (key == NULL) {
                return node;
            }
        } else if (c == '}') {
            jsonic_free(node);
            return NULL;
        } else if (c == '[') {
            node->clevel = (node->plevel++ == 0) ? 1: node->clevel;
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_END;
            node->ntype = JSONIC_NODE_TYPE_ARRAY;
        } else {
            if (key != NULL) {
                jsonic_free(node);
                return NULL;
            }

            if ((current != NULL) && (node->plevel > node->clevel)) {
                node->ind++;
                goto next;
            }

            if (c == '"') {
                if (node->plevel != 0) {
                    jsonic_free(node);
                    return NULL;
                }

                node->parser_state = JSONIC_PARSER_STATE_EXPECT_VAL_END;
                node->type = JSONIC_NODE_TYPE_STRING;
                node->val = malloc(sizeof(char));
                *node->val = '\0';
                node->len = 0;
                node->ksync = 1;
            } else if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_INTVAL;
                node->type = JSONIC_NODE_TYPE_INTEGER;
                node->val = malloc(sizeof(char)*2);
                *node->val = c;
                node->val[1] = '\0';
                node->ksync = 1;
            }
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_INTVAL) {
        if (node->ksync) {
            if (!((c > 47) && (c < 58))) {
                return node;
            }

            int len = strlen(node->val);
            char* v = node->val;
            node->val = malloc(sizeof(char)*(len+2));
            strcpy(node->val, v);
            node->val[len] = c;
            node->val[len+1] = '\0';
        } else {
            if ((c > 47) && (c < 58)) {
            } else if (c =='}') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
                node->plevel--;
            } else {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
            }
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_VAL_END) {
        if (c == '"') {
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
            node->ind++;
            
            if (node->ksync) {
                if (node->clevel == node->plevel) {
                    return node;
                }
            }
        } else if (node->ksync) {
            char* v = node->val;
            node->val = malloc(sizeof(char)*(node->len+2));
            strcpy(node->val, v);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_KEY_START) {
        if (node->plevel == 0) {
            jsonic_free(node);
            return NULL;
        }

        if (c == '"') {
            node->ksync = 1;
            node->kind = 0;
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_END;
        } else if (c == '}') {
            jsonic_free(node);
            return NULL;
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_KEY_END) {
        if (c == '"') {
            node->kind = 0;
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_VAL_START;

            node->ind++;
            goto next;
        }

        if (node->ksync == 0) {
            node->ind++;
            node->kind++;
            goto next;
        }
        
        if (
            (keylen == 0)
            ||
            (c != key[node->kind])
        ) {
            node->ksync = 0;
            node->ind++;
            node->kind++;
            goto next;
        }

        node->kind++;
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_VAL_START) {
        if (node->ksync) {
            if (c == '{') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
                node->ksync = 0;
                node->kind = 0;
                node->type = JSONIC_NODE_TYPE_OBJECT;
                
                return node;
            } else if (c == '[') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_END;
                node->type = JSONIC_NODE_TYPE_ARRAY;
                node->ksync = 0;
                node->kind = 0;
                node->plevel++;
                node->ind++;
                
                return node;
            }
            
            if (
                (c == 't') || (c == 'T')
            ) {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_BOOLVAL;
                node->type = JSONIC_NODE_TYPE_BOOLEAN;
                
                node->val = malloc(sizeof(char)*2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_BOOLVAL;
                node->type = JSONIC_NODE_TYPE_BOOLEAN;
                
                node->val = malloc(sizeof(char)*2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_INTVAL;
                node->type = JSONIC_NODE_TYPE_INTEGER;
                node->val = malloc(sizeof(char)*2);
                *node->val = c;
                *(node->val+1) = '\0';
            } else if (c == '"') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_VAL_END;
                node->type = JSONIC_NODE_TYPE_STRING;
                node->val = malloc(sizeof(char));
                *node->val = '\0';
                node->len = 0;
            }
        } else {
            if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_INTVAL;
            } else if (c == '"') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_VAL_END;
            } else if (c =='{') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
                node->plevel++;
            } else if (c == '[') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_END;
                node->plevel++;
            } else if (
                (c == 't') || (c == 'T')
            ) {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_BOOLVAL;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_BOOLVAL;
            }
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_ARR_END) {
        if (
            (c == 't') || (c == 'T')
        ) {
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_BOOLVAL;
            node->type = JSONIC_NODE_TYPE_BOOLEAN;
            
            if (node->arrind == index) {
                node->val = malloc(sizeof(char)*2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            }
        } else if (
            (c == 'f') || (c == 'F')
        ) {
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_BOOLVAL;
            node->type = JSONIC_NODE_TYPE_BOOLEAN;
            
            if (node->arrind == index) {
                node->val = malloc(sizeof(char)*2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
        }
        
        if ((c == '-') || ((c > 47) && (c < 58))) {
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_INTVAL;
            
            if (node->arrind == index) {
                node->type = JSONIC_NODE_TYPE_INTEGER;
                node->val = malloc(sizeof(char)*2);
                *node->val = c;
                *(node->val+1) = '\0';
            }
        } else if (c == '"') {
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_STR_END;
            node->val = malloc(sizeof(char));
            *node->val = '\0';
            node->len = 0;
        } else if (c == ',') {
            node->arrind++;
        } else if (c == ']') {
            if (node->plevel == node->clevel) {
                jsonic_free(node);
                return NULL;
            }
        } else if ((node->plevel == node->clevel) && (node->arrind == index)) {
            if (c == '{') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
                node->ksync = 0;
                node->kind = 0;
                node->type = JSONIC_NODE_TYPE_OBJECT;
                
                return node;
            } else if (c == '[') {
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_END;
                node->type = JSONIC_NODE_TYPE_ARRAY;
                node->ksync = 0;
                node->kind = 0;
                node->ind++;
                
                return node;
            }
        } else {
            if (c == '{') {
                node->plevel++;
            } else if (c == '[') {
                node->plevel++;
            }
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_ARR_STR_END) {
        if (c == '"') {
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_END;
            
            if (node->arrind == index) {
                node->type = JSONIC_NODE_TYPE_STRING;
                return node;
            }
        } else if (node->arrind == index) {
            char* v = node->val;
            node->val = malloc(sizeof(char)*(node->len+2));
            strcpy(node->val, v);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_ARR_INTVAL) {
        if (node->arrind == index) {
            if (!((c > 47) && (c < 58))) {
                return node;
            }

            int len = strlen(node->val);
            char* v = node->val;
            node->val = malloc(sizeof(char)*(len+2));
            strcpy(node->val, v);
            node->val[len] = c;
            node->val[len+1] = '\0';
        } else if (node->plevel == node->clevel) {
            if (c ==']') {
                jsonic_free(node);
                return NULL;
            } else if (c == ',') {
                node->arrind++;
                node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_END;
            }
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_BOOLVAL) {
        if (is_bool_char(c)) {
        } else {
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_KEY_START;
        }
    } else if (node->parser_state == JSONIC_PARSER_STATE_EXPECT_ARR_BOOLVAL) {
        if (is_bool_char(c)) {
        } else if (c == ']') {
            jsonic_free(node);
            return NULL;
        } else {
            if (c == ',') {
                node->arrind++;
            }
            
            node->parser_state = JSONIC_PARSER_STATE_EXPECT_ARR_END;
        }
    }

    node->ind++;

    goto next;

    end:

    return (node->type == JSONIC_NODE_TYPE_NONE) ? NULL: node;
}

int is_bool_char(char c) {
    return (
        (c == 't') || (c == 'r') || (c == 'u') || (c == 'e') || (c == 'f') || (c == 'a') || (c == 'l') || (c == 's') || (c == 'e')
        ||
        (c == 'T') || (c == 'R') || (c == 'U') || (c == 'E') || (c == 'F') || (c == 'A') || (c == 'L') || (c == 'S') || (c == 'E')
    );
}