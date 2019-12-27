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
    json_string = malloc(1*flen);
    fread(json_string, 1, flen, fd);
    fclose(fd);

    return json_string;
}

extern void jsonic_free_addr(jsonic_node_t* node) {
    if (node == NULL) return;

    if (
        (node->type == BOOLEAN)
        ||
        (node->type == NULLVAL)
        ||
        (node->type == STRING)
        ||
        (node->type == NUMBER)
    ) {
        free(node->val);
    }

    free(node);
}

extern void jsonic_free(jsonic_node_t** node) {
    jsonic_free_addr(*node);
    *node = NULL;
}

extern int jsonic_array_length(char* json_str, jsonic_node_t* array) {
    int length;
    jsonic_node_t* node = NULL;

    for (length=0;;) {
        node = jsonic_get(json_str, array, NULL, 0, jsonic_from_node_free(node), NULL, 0, 0);
        if ((node == NULL) || (node->type == NONE)) break;
        length++;
    }

    return length;
}

extern int jsonic_array_length_from(
    char* json_str,
    jsonic_node_t* array,
    jsonic_node_t* from
) {
    int length;
    jsonic_node_t* node = NULL;

    int wf = 0;
    int from_i;
    jsonic_node_t* from_n;
    for (length=0;;) {
        if ((node == NULL) || (node->type == NONE)) {
            from_i = jsonic_from_node(from);
            from_n = from;
        } else {
            from_i = jsonic_from_node(node);
            from_n = node;
            wf = 1;
        }

        node = jsonic_get(json_str, array, NULL, 0, from_i, from_n, 0, 0);
        if ((node == NULL) || (node->type == NONE)) break;
        if (wf) {
            jsonic_free_addr(from_n);
        }
        length++;
    }

    return length;
}

extern int jsonic_from_node(jsonic_node_t* node) {
    if (!node) {
        return 0;
    }

    if (node->type == OBJECT) {
        return node->ind-1;
    } else if (node->type == ARRAY) {
        return node->ind-1;
    } else if (node->type == STRING) {
        return node->ind;
    } else if (node->type == NUMBER) {
        return node->ind;
    } else if (node->type == BOOLEAN) {
        return node->ind;
    } else if (node->type == NULLVAL) {
        return node->ind;
    }

    return 0;
}

extern int jsonic_from_node_free(jsonic_node_t* node) {
    int from = jsonic_from_node(node);
    if (node) jsonic_free_addr(node);
    return from;
}

extern jsonic_node_t* jsonic_get_root(
    char* json_str
) {
    return jsonic_get(json_str, NULL, NULL, 0, 0, NULL, 1, 0);
}

extern jsonic_node_t* jsonic_object_get(
    char* json_str,
    jsonic_node_t* current,
    char* key
) {
    return jsonic_get(json_str, current, key, 0, 0, NULL, 0, 0);
}

extern jsonic_node_t* jsonic_array_get(
    char* json_str,
    jsonic_node_t* current,
    int index
) {
    return jsonic_get(json_str, current, NULL, index, 0, NULL, 0, 0);
}

extern jsonic_node_t* jsonic_array_iter(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from,
    int index
) {
    return jsonic_get(json_str, current, NULL, index, jsonic_from_node(from), from, 0, 0);
}

extern jsonic_node_t* jsonic_array_iter_free(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from,
    int index
) {
    jsonic_node_t* node = jsonic_get(json_str, current, NULL, index, jsonic_from_node(from), from, 0, 0);
    jsonic_free_addr(from);
    return node;
}

extern jsonic_node_t* jsonic_object_iter(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from,
    char* key
) {
    return jsonic_get(json_str, current, key, 0, jsonic_from_node(from), from, 0, 0);
}

extern jsonic_node_t* jsonic_object_iter_free(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from,
    char* key
) {
    jsonic_node_t* node = jsonic_get(json_str, current, key, 0, jsonic_from_node(from), from, 0, 0);
    jsonic_free_addr(from);
    return node;
}

extern jsonic_node_t* jsonic_object_iter_kv(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from
) {
    return jsonic_get(json_str, current, NULL, 0, jsonic_from_node(from), from, 0, 1);
}

extern jsonic_node_t* jsonic_object_iter_kv_free(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from
) {
    jsonic_node_t* node = jsonic_get(json_str, current, NULL, 0, jsonic_from_node(from), from, 0, 1);
    jsonic_free_addr(from);
    return node;
}

extern jsonic_node_t* jsonic_get(
    char* json_str,
    jsonic_node_t* current,
    char* key,
    int index,
    int from,
    jsonic_node_t* from_object,
    int is_get_root,
    int is_kv_iter
) {
    jsonic_node_t* node = malloc(sizeof(jsonic_node_t));
    node->type = NONE;
    node->parser_state = (current == NULL) 
                         ? EXPECT_ANY
                         : current->parser_state;
    node->plevel = (current == NULL) ? 0: current->plevel;
    node->clevel = node->plevel;
    node->ind = from ? from: ((current == NULL) ? 0: current->ind);
    node->ksync = 0;
    node->kind = 0;
    node->arrind = 0;
    node->meta = 0;
    node->pos = ((key == NULL) && !is_kv_iter) 
                ? ((from_object == NULL) ? 0: from_object->pos)
                : ((from_object == NULL) ? 0: from_object->pos);

    int is_key_itered = 0;

    if (key == NULL) {
        if (from > 0) {
            index++;
        }
    } else {
        if (from > 0) {
            if (from_object->type == OBJECT) {
                node->ind++;
                node->plevel++;
            } else if (from_object->type == ARRAY) {
                node->ind++;
                node->plevel++;
            } else {
                node->parser_state = EXPECT_KEY_START;
            }
        }
    }

    int keylen = 0;
    
    if (key != NULL) {
        keylen = strlen(key);
    }

    char c;
    int instr = 0;

    next:

    c = json_str[node->ind];

    if (c == '\0') {
        if (node->plevel == 0) {
            if (node->type == NUMBER) {
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
        if (!node->meta && (c == '"')) {
            if (!instr) instr = 1;
            else        instr = 0;
            node->meta = 0;
        }

        if (!instr) {
            if ((c == '{') || (c == '[')) {
                node->plevel++;
            } else if ((c == '}') || (c == ']')) {
                node->plevel--;
            }
        }
        
        node->ind++;
        goto next;
    }

    if (
        (node->meta == 0)
        &&
        (c == '\\')
        &&
        (
            (node->parser_state == EXPECT_VAL_END)
            ||
            (node->parser_state == EXPECT_ARR_STR_END)
        )
    ) {
        node->meta = 1;
        node->ind++;
        goto next;
    }

    if (node->parser_state == EXPECT_ANY) {
        if (c == '{') {
            node->clevel = (node->plevel++ == 0) ? 1: node->clevel;
            node->parser_state = EXPECT_KEY_START;
            
            if ((key == NULL) || is_get_root) {
                node->ksync = 0;
                node->kind = 0;
                node->type = OBJECT;
                node->plevel++;
                node->ind++;
                return node;
            }
        } else if (c == '[') {
            node->clevel = (node->plevel++ == 0) ? 1: node->clevel;
            node->parser_state = EXPECT_ARR_END;

            if (is_get_root) {
                node->type = ARRAY;
                node->ksync = 0;
                node->kind = 0;
                node->arrind = 0;
                node->plevel++;
                node->ind++;
                return node;
            }
        } else {
            if (key != NULL) {
                node->type = NONE;
                return node;
            }

            if (c == '"') {
                node->parser_state = EXPECT_VAL_END;
                node->type = STRING;
                node->val = malloc(1);
                *node->val = '\0';
                node->len = 0;
                node->ksync = 1;
            } else if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = EXPECT_NUMVAL;
                node->type = NUMBER;
                node->val = malloc(2);
                *node->val = c;
                node->val[1] = '\0';
                node->ksync = 1;
            } else if (
                (c == 't') || (c == 'T')
            ) {
                node->parser_state = EXPECT_BOOLVAL;
                node->type = BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = EXPECT_BOOLVAL;
                node->type = BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            } else if (
                (c == 'n') || (c == 'N')
            ) {
                node->parser_state = EXPECT_NULLVAL;
                node->type = NULLVAL;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
        }
    } else if (node->parser_state == EXPECT_NUMVAL) {
        if (node->ksync || is_kv_iter) {
            if (!((c > 47) && (c < 58)) && (c != '.')) {
                node->parser_state = EXPECT_KEY_START;
                return node;
            }

            int len = strlen(node->val);
            char* v = node->val;
            node->val = malloc(1*(len+2));
            strcpy(node->val, v);
            free(v);
            node->val[len] = c;
            node->val[len+1] = '\0';
        } else {
            if (((c > 47) && (c < 58)) || (c == '.')) {
            } else if (c =='}') {
                node->type = NONE;
                return node;
            } else {
                node->parser_state = EXPECT_KEY_START;
            }
        }
    } else if (node->parser_state == EXPECT_VAL_END) {
        if ((c == '"') && (node->meta == 0)) {
            node->parser_state = EXPECT_KEY_START;
            
            if (node->ksync || is_kv_iter) {
                if (node->clevel == node->plevel) {
                    node->ind++;
                    return node;
                }
            }
        } else if (node->ksync || is_kv_iter) {
            char* v = node->val;
            node->val = malloc(1*(node->len+2));
            strcpy(node->val, v);
            free(v);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        }
    } else if (node->parser_state == EXPECT_KEY_START) {
        if (node->plevel == 0) {
            node->type = NONE;
            return node;
        }

        if (c == '"') {
            if (!is_key_itered) {
                is_key_itered = 1;
            } else {
                node->pos++;
            }

            node->ksync = 1;
            node->kind = 0;
            node->parser_state = EXPECT_KEY_END;

            if (is_kv_iter) {
                node->key = malloc(1);
                *node->key = '\0';
            }
        } else if (c == '}') {
            node->type = NONE;
            return node;
        }
    } else if (node->parser_state == EXPECT_KEY_END) {
        if (c == '"') {
            if (keylen != node->kind) {
                node->ksync = 0;
            }

            node->kind = 0;
            node->parser_state = EXPECT_VAL_START;

            node->ind++;
            goto next;
        }

        if (!node->ksync && !is_kv_iter) {
            node->ind++;
            node->kind++;
            goto next;
        }
        
        if (
            !is_kv_iter
            &&
            (
                (keylen == 0)
                ||
                (keylen <= node->kind)
                ||
                (c != key[node->kind])
            )
        ) {
            node->ksync = 0;
            node->ind++;
            node->kind++;
            goto next;
        }

        if (is_kv_iter) {
            char* tmp = node->key;
            node->key = malloc(node->kind+2);
            strcpy(node->key, tmp);
            free(tmp);
            node->key[node->kind] = c;
            node->key[node->kind+1] = '\0';
        }

        node->kind++;
    } else if (node->parser_state == EXPECT_VAL_START) {
        if (node->ksync || is_kv_iter) {
            if (c == '{') {
                node->parser_state = EXPECT_KEY_START;
                node->ksync = 0;
                node->kind = 0;
                node->type = OBJECT;
                node->plevel++;
                node->ind++;
                
                return node;
            } else if (c == '[') {
                node->parser_state = EXPECT_ARR_END;
                node->type = ARRAY;
                node->ksync = 0;
                node->kind = 0;
                node->arrind = 0;
                node->plevel++;
                node->ind++;

                return node;
            }
            
            if (
                (c == 't') || (c == 'T')
            ) {
                node->parser_state = EXPECT_KEY_START;
                node->type = BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = EXPECT_KEY_START;
                node->type = BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }

            if (
                (c == 'n') || (c == 'N')
            ) {
                node->parser_state = EXPECT_KEY_START;
                node->type = NULLVAL;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = EXPECT_NUMVAL;
                node->type = NUMBER;
                node->val = malloc(2);
                *node->val = c;
                *(node->val+1) = '\0';
            } else if (c == '"') {
                node->parser_state = EXPECT_VAL_END;
                node->type = STRING;
                node->val = malloc(1);
                *node->val = '\0';
                node->len = 0;
            }
        } else {
            if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = EXPECT_NUMVAL;
            } else if (c == '"') {
                node->parser_state = EXPECT_VAL_END;
            } else if (c =='{') {
                node->parser_state = EXPECT_KEY_START;
                node->plevel++;
            } else if (c == '[') {
                node->parser_state = EXPECT_KEY_START;
                node->plevel++;
            } else if (
                (c == 't') || (c == 'T')
            ) {
                node->parser_state = EXPECT_BOOLVAL;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = EXPECT_BOOLVAL;
            } else if (
                (c == 'n') || (c == 'N')
            ) {
                node->parser_state = EXPECT_NULLVAL;
            }
        }
    } else if (node->parser_state == EXPECT_ARR_END) {
        if (
            (c == 't') || (c == 'T')
        ) {
            if (node->arrind == index) {
                node->type = BOOLEAN;
                node->len = 1;
                node->val = malloc(2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            node->parser_state = EXPECT_ARR_BOOLVAL;
        } else if (
            (c == 'f') || (c == 'F')
        ) {
            if (node->arrind == index) {
                node->type = BOOLEAN;
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            node->parser_state = EXPECT_ARR_BOOLVAL;
        }

        if (
            (c == 'n') || (c == 'N')
        ) {
            if (node->arrind == index) {
                node->type = NULLVAL;
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            node->parser_state = EXPECT_ARR_NULLVAL;
        }
        
        if ((c == '-') || ((c > 47) && (c < 58))) {
            node->parser_state = EXPECT_ARR_NUMVAL;
            
            if (node->arrind == index) {
                node->type = NUMBER;
                node->val = malloc(2);
                *node->val = c;
                *(node->val+1) = '\0';
            }
        } else if (c == '"') {
            node->parser_state = EXPECT_ARR_STR_END;
            node->val = malloc(1);
            *node->val = '\0';
            node->len = 0;
        } else if (c == ',') {
            node->pos++;
            node->arrind++;
        } else if (c == ']') {
            node->type = NONE;
            return node;
        } else if (node->arrind == index) {
            if (c == '{') {
                node->parser_state = EXPECT_KEY_START;
                node->ksync = 0;
                node->kind = 0;
                node->type = OBJECT;
                node->plevel++;
                node->ind++;
                
                return node;
            } else if (c == '[') {
                node->parser_state = EXPECT_ARR_END;
                node->type = ARRAY;
                node->ksync = 0;
                node->kind = 0;
                node->arrind = 0;
                node->plevel++;
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
    } else if (node->parser_state == EXPECT_ARR_STR_END) {
        if ((c == '"') && (node->meta == 0)) {
            node->parser_state = EXPECT_ARR_END;
            
            if (node->arrind == index) {
                node->parser_state = EXPECT_ARR_END;
                node->type = STRING;
                node->ind++;
                return node;
            }
        } else if (node->arrind == index) {
            char* v = node->val;
            node->val = malloc(1*(node->len+2));
            strcpy(node->val, v);
            free(v);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        }
    } else if (node->parser_state == EXPECT_ARR_NUMVAL) {
        if (node->arrind == index) {
            if (!((c > 47) && (c < 58)) && (c != '.')) {
                node->parser_state = EXPECT_ARR_END;
                return node;
            }

            int len = strlen(node->val);
            char* v = node->val;
            node->val = malloc(1*(len+2));
            strcpy(node->val, v);
            free(v);
            node->val[len] = c;
            node->val[len+1] = '\0';
        } if (c ==']') {
            node->type = NONE;
            return node;
        } else if (c == ',') {
            node->pos++;
            node->arrind++;
            node->parser_state = EXPECT_ARR_END;
        }
    } else if (node->parser_state == EXPECT_BOOLVAL) {
        if (is_bool_char(c)) {
        } else {
            node->parser_state = EXPECT_KEY_START;
        }
    } else if (node->parser_state == EXPECT_ARR_BOOLVAL) {
        if (is_bool_char(c)) {
        } else if (c == ']') {
            node->type = NONE;
            return node;
        } else {
            if (c == ',') {
                node->pos++;
                node->arrind++;
            }
            
            node->parser_state = EXPECT_ARR_END;
        }
    } else if (node->parser_state == EXPECT_NULLVAL) {
        if (is_null_char(c)) {
        } else {
            node->parser_state = EXPECT_KEY_START;
        }
    } else if (node->parser_state == EXPECT_ARR_NULLVAL) {
        if (is_null_char(c)) {
        } else if (c == ']') {
            node->type = NONE;
            return node;
        } else {
            if (c == ',') {
                node->pos++;
                node->arrind++;
            }
            
            node->parser_state = EXPECT_ARR_END;
        }
    }

    node->meta = 0;
    node->ind++;

    goto next;

    end:

    node->type = NONE;

    return node;
}

int is_bool_char(char c) {
    return (
        (c == 't') || (c == 'r') || (c == 'u') || (c == 'e') || (c == 'f') || (c == 'a') || (c == 'l') || (c == 's') || (c == 'e')
        ||
        (c == 'T') || (c == 'R') || (c == 'U') || (c == 'E') || (c == 'F') || (c == 'A') || (c == 'L') || (c == 'S') || (c == 'E')
    );
}

int is_null_char(char c) {
    return (
        (c == 'n') || (c == 'u') || (c == 'l')
        ||
        (c == 'N') || (c == 'U') || (c == 'L')
    );
}