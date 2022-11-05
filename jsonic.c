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
    fd = fopen(fname, "r");

    if (fd == NULL) {
        return NULL;
    }

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
        (node->type == JSONIC_BOOLEAN)
        ||
        (node->type == JSONIC_NULL)
        ||
        (node->type == JSONIC_STRING)
        ||
        (node->type == JSONIC_NUMBER)
    ) {
        free(node->val);
    }

    free(node);
}

extern void jsonic_free(jsonic_node_t** node) {
    jsonic_free_addr(*node);
    *node = NULL;
}

extern jsonic_length_t jsonic_array_length(char* json_str, jsonic_node_t* array) {
    jsonic_length_t result;
    result.length = 0;
    result.index = array->ind;

    int level = 0;
    int is_esc = 0;
    char curr = 0;
    char c;

    PRELOOP:

    c = json_str[result.index];

    if ((c == ' ') && (c == '\r') && (c == '\t') && (c == '\n')) {
        result.index++;
        goto PRELOOP;
    } else if (c == ']') {
        array->ending = result.index;
        return result;
    } else {
        result.length++;
        goto LOOP;
    }

    goto PRELOOP;

    LOOP:

    c = json_str[result.index];

    if (c == '\0') {
        array->ending = result.index;
        return result;
    }

    if (level == 0) {
        if (c == ']') {
            array->ending = result.index;
            return result;
        } else if (c == ',') {
            result.length++;
            result.index++;
            goto LOOP;
        }
    }

    if (curr == '"') {
        if (c == '"') {
            if (!is_esc) {
                level--;
                curr = 0;
            } else {
                is_esc = 0;
            }
            
            result.index++;
            goto LOOP;
        } else if (c == '\\') {
            if (is_esc) {
                is_esc = 0;
            } else {
                is_esc = 1;
            }
        } else if (is_esc) {
            is_esc = 0;
        }

        result.index++;
        goto LOOP;
    }

    switch (c) {
        case '{':
        case '[':
            level++;
            break;
        case '}':
        case ']':
            level--;
            break;
        case '"':
            level++;
            curr = '"';
            break;
    }
    
    result.index++;
    goto LOOP;
}

extern jsonic_length_t jsonic_array_length_from(
    char* json_str,
    jsonic_node_t* array,
    jsonic_node_t* from
) {
    jsonic_length_t result;
    result.length = 0;
    result.index = from->ind;

    int level = 0;
    int is_esc = 0;
    char curr = 0;
    char c;

    LOOP:

    c = json_str[result.index];

    if (c == '\0') {
        array->ending = result.index;
        return result;
    }

    if (level == 0) {
        if (c == ']') {
            array->ending = result.index;
            return result;
        } else if (c == ',') {
            result.length++;
            result.index++;
            goto LOOP;
        }
    }

    if (curr == '"') {
        if (c == '"') {
            if (!is_esc) {
                level--;
                curr = 0;
            } else {
                is_esc = 0;
            }
            
            result.index++;
            goto LOOP;
        } else if (c == '\\') {
            if (is_esc) {
                is_esc = 0;
            } else {
                is_esc = 1;
            }
        } else if (is_esc) {
            is_esc = 0;
        }

        result.index++;
        goto LOOP;
    }

    switch (c) {
        case '{':
        case '[':
            level++;
            break;
        case '}':
        case ']':
            level--;
            break;
        case '"':
            level++;
            curr = '"';
            break;
    }
    
    result.index++;
    goto LOOP;
}

extern unsigned int jsonic_from_node(jsonic_node_t* node) {
    if (!node) {
        return 0;
    }

    if (node->type == JSONIC_OBJECT) {
        return node->ind-1;
    } else if (node->type == JSONIC_ARRAY) {
        return node->ind-1;
    } else if (node->type == JSONIC_STRING) {
        return node->ind;
    } else if (node->type == JSONIC_NUMBER) {
        return node->ind;
    } else if (node->type == JSONIC_BOOLEAN) {
        return node->ind;
    } else if (node->type == JSONIC_NULL) {
        return node->ind;
    }

    return 0;
}

extern unsigned int jsonic_from_node_free(jsonic_node_t* node) {
    int from = jsonic_from_node(node);
    if (node) jsonic_free_addr(node);
    return from;
}

extern unsigned int jsonic_from_length(jsonic_length_t length) {
    return length.index;
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
    unsigned int offset;
    if (from && from->ending) {
        offset = from->ending+1;
    } else {
        offset = jsonic_from_node(from);
    }
    
    return jsonic_get(json_str, current, NULL, index, offset, from, 0, 0);
}

extern jsonic_node_t* jsonic_array_iter_free(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from,
    int index
) {
    unsigned int offset;
    if (from && from->ending) {
        offset = from->ending+1;
    } else {
        offset = jsonic_from_node(from);
    }

    jsonic_node_t* node = jsonic_get(json_str, current, NULL, index, offset, from, 0, 0);
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

extern jsonic_node_t* jsonic_object_iter_index(
    char* json_str,
    jsonic_node_t* current,
    jsonic_node_t* from_node_or_null,
    unsigned int from_index,
    char* key
) {
    return jsonic_get(json_str, current, key, 0, from_index, from_node_or_null, 0, 0);
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
    unsigned int index,
    unsigned int from,
    jsonic_node_t* from_object,
    int is_get_root,
    int is_kv_iter
) {
    jsonic_node_t* node = malloc(sizeof(jsonic_node_t));
    node->type = JSONIC_NONE;
    node->parser_state = (current == NULL) 
                         ? JSONIC_EXPECT_ANY
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
    node->ending = 0;

    int is_key_itered = 0;

    if (key == NULL) {
        if (from > 0) {
            index++;
        }
    } else {
        if (from) {
            if (from_object->ending) {
                node->ind = from_object->ending+1;
            } else {
                if (from_object->type == JSONIC_OBJECT) {
                    node->ind++;
                    node->plevel++;
                } else if (from_object->type == JSONIC_ARRAY) {
                    node->ind++;
                    node->plevel++;
                } else {
                    node->parser_state = JSONIC_EXPECT_KEY_START;
                }
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
            if (node->type == JSONIC_NUMBER) {
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
            (node->parser_state == JSONIC_EXPECT_VAL_END)
            ||
            (node->parser_state == JSONIC_EXPECT_ARR_STR_END)
        )
    ) {
        node->meta = 1;
        node->ind++;
        goto next;
    }

    if (node->parser_state == JSONIC_EXPECT_ANY) {
        if (c == '{') {
            node->clevel = (node->plevel++ == 0) ? 1: node->clevel;
            node->parser_state = JSONIC_EXPECT_KEY_START;
            
            if ((key == NULL) || is_get_root) {
                node->ksync = 0;
                node->kind = 0;
                node->type = JSONIC_OBJECT;
                node->plevel++;
                node->ind++;
                return node;
            }
        } else if (c == '[') {
            node->clevel = (node->plevel++ == 0) ? 1: node->clevel;
            node->parser_state = JSONIC_EXPECT_ARR_END;

            if (is_get_root) {
                node->type = JSONIC_ARRAY;
                node->ksync = 0;
                node->kind = 0;
                node->arrind = 0;
                node->plevel++;
                node->ind++;
                return node;
            }
        } else {
            if (key != NULL) {
                node->type = JSONIC_NONE;
                return node;
            }

            if (c == '"') {
                node->parser_state = JSONIC_EXPECT_VAL_END;
                node->type = JSONIC_STRING;
                node->val = malloc(1);
                *node->val = '\0';
                node->len = 0;
                node->ksync = 1;
            } else if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = JSONIC_EXPECT_NUMVAL;
                node->type = JSONIC_NUMBER;
                node->val = malloc(2);
                *node->val = c;
                node->val[1] = '\0';
                node->ksync = 1;
                node->len = 1;
            } else if (
                (c == 't') || (c == 'T')
            ) {
                node->parser_state = JSONIC_EXPECT_BOOLVAL;
                node->type = JSONIC_BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = JSONIC_EXPECT_BOOLVAL;
                node->type = JSONIC_BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            } else if (
                (c == 'n') || (c == 'N')
            ) {
                node->parser_state = JSONIC_EXPECT_NULLVAL;
                node->type = JSONIC_NULL;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
        }
    } else if (node->parser_state == JSONIC_EXPECT_NUMVAL) {
        if (node->ksync || is_kv_iter) {
            if (!((c > 47) && (c < 58)) && (c != '.')) {
                node->parser_state = JSONIC_EXPECT_KEY_START;
                return node;
            }

            node->val = realloc(node->val, node->len+2);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        } else {
            if (((c > 47) && (c < 58)) || (c == '.')) {
            } else if (c =='}') {
                node->type = JSONIC_NONE;
                return node;
            } else {
                node->parser_state = JSONIC_EXPECT_KEY_START;
            }
        }
    } else if (node->parser_state == JSONIC_EXPECT_VAL_END) {
        if ((c == '"') && (node->meta == 0)) {
            node->parser_state = JSONIC_EXPECT_KEY_START;
            
            if (node->ksync || is_kv_iter) {
                if (node->clevel == node->plevel) {
                    node->ind++;
                    return node;
                }
            }
        } else if (node->ksync || is_kv_iter) {
            node->val = realloc(node->val, node->len+2);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        }
    } else if (node->parser_state == JSONIC_EXPECT_KEY_START) {
        if (node->plevel == 0) {
            node->type = JSONIC_NONE;
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
            node->parser_state = JSONIC_EXPECT_KEY_END;

            if (is_kv_iter) {
                node->key = malloc(1);
                *node->key = '\0';
            }
        } else if (c == '}') {
            node->type = JSONIC_NONE;
            return node;
        }
    } else if (node->parser_state == JSONIC_EXPECT_KEY_END) {
        if (c == '"') {
            if (keylen != node->kind) {
                node->ksync = 0;
            }

            node->kind = 0;
            node->parser_state = JSONIC_EXPECT_VAL_START;

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
            node->key = realloc(node->key, node->kind+2);
            node->key[node->kind] = c;
            node->key[node->kind+1] = '\0';
        }

        node->kind++;
    } else if (node->parser_state == JSONIC_EXPECT_VAL_START) {
        if (node->ksync || is_kv_iter) {
            if (c == '{') {
                node->parser_state = JSONIC_EXPECT_KEY_START;
                node->ksync = 0;
                node->kind = 0;
                node->type = JSONIC_OBJECT;
                node->plevel++;
                node->ind++;
                
                return node;
            } else if (c == '[') {
                node->parser_state = JSONIC_EXPECT_ARR_END;
                node->type = JSONIC_ARRAY;
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
                node->parser_state = JSONIC_EXPECT_KEY_START;
                node->type = JSONIC_BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = JSONIC_EXPECT_KEY_START;
                node->type = JSONIC_BOOLEAN;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }

            if (
                (c == 'n') || (c == 'N')
            ) {
                node->parser_state = JSONIC_EXPECT_KEY_START;
                node->type = JSONIC_NULL;
                
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = JSONIC_EXPECT_NUMVAL;
                node->type = JSONIC_NUMBER;
                node->val = malloc(2);
                *node->val = c;
                *(node->val+1) = '\0';
                node->len = 1;
            } else if (c == '"') {
                node->parser_state = JSONIC_EXPECT_VAL_END;
                node->type = JSONIC_STRING;
                node->val = malloc(1);
                *node->val = '\0';
                node->len = 0;
            }
        } else {
            if ((c == '-') || ((c > 47) && (c < 58))) {
                node->parser_state = JSONIC_EXPECT_NUMVAL;
            } else if (c == '"') {
                node->parser_state = JSONIC_EXPECT_VAL_END;
            } else if (c =='{') {
                node->parser_state = JSONIC_EXPECT_KEY_START;
                node->plevel++;
            } else if (c == '[') {
                node->parser_state = JSONIC_EXPECT_KEY_START;
                node->plevel++;
            } else if (
                (c == 't') || (c == 'T')
            ) {
                node->parser_state = JSONIC_EXPECT_BOOLVAL;
            } else if (
                (c == 'f') || (c == 'F')
            ) {
                node->parser_state = JSONIC_EXPECT_BOOLVAL;
            } else if (
                (c == 'n') || (c == 'N')
            ) {
                node->parser_state = JSONIC_EXPECT_NULLVAL;
            }
        }
    } else if (node->parser_state == JSONIC_EXPECT_ARR_END) {
        if (
            (c == 't') || (c == 'T')
        ) {
            if (node->arrind == index) {
                node->type = JSONIC_BOOLEAN;
                node->len = 1;
                node->val = malloc(2);
                *node->val = '1';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            node->parser_state = JSONIC_EXPECT_ARR_BOOLVAL;
        } else if (
            (c == 'f') || (c == 'F')
        ) {
            if (node->arrind == index) {
                node->type = JSONIC_BOOLEAN;
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            node->parser_state = JSONIC_EXPECT_ARR_BOOLVAL;
        }

        if (
            (c == 'n') || (c == 'N')
        ) {
            if (node->arrind == index) {
                node->type = JSONIC_NULL;
                node->len = 1;
                node->val = malloc(2);
                *node->val = '0';
                *(node->val+1) = '\0';
                
                return node;
            }
            
            node->parser_state = JSONIC_EXPECT_ARR_NULLVAL;
        }
        
        if ((c == '-') || ((c > 47) && (c < 58))) {
            node->parser_state = JSONIC_EXPECT_ARR_NUMVAL;
            
            if (node->arrind == index) {
                node->type = JSONIC_NUMBER;
                node->val = malloc(2);
                *node->val = c;
                *(node->val+1) = '\0';
                node->len = 1;
            }
        } else if (c == '"') {
            node->parser_state = JSONIC_EXPECT_ARR_STR_END;
            node->val = malloc(1);
            *node->val = '\0';
            node->len = 0;
        } else if (c == ',') {
            node->pos++;
            node->arrind++;
        } else if (c == ']') {
            node->type = JSONIC_NONE;
            return node;
        } else if (node->arrind == index) {
            if (c == '{') {
                node->parser_state = JSONIC_EXPECT_KEY_START;
                node->ksync = 0;
                node->kind = 0;
                node->type = JSONIC_OBJECT;
                node->plevel++;
                node->ind++;
                
                return node;
            } else if (c == '[') {
                node->parser_state = JSONIC_EXPECT_ARR_END;
                node->type = JSONIC_ARRAY;
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
    } else if (node->parser_state == JSONIC_EXPECT_ARR_STR_END) {
        if ((c == '"') && (node->meta == 0)) {
            node->parser_state = JSONIC_EXPECT_ARR_END;
            
            if (node->arrind == index) {
                node->parser_state = JSONIC_EXPECT_ARR_END;
                node->type = JSONIC_STRING;
                node->ind++;
                return node;
            }
        } else if (node->arrind == index) {
            node->val = realloc(node->val, node->len+2);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        }
    } else if (node->parser_state == JSONIC_EXPECT_ARR_NUMVAL) {
        if (node->arrind == index) {
            if (!((c > 47) && (c < 58)) && (c != '.')) {
                node->parser_state = JSONIC_EXPECT_ARR_END;
                return node;
            }

            node->val = realloc(node->val, node->len+2);
            node->val[node->len] = c;
            node->val[node->len+1] = '\0';
            node->len++;
        } if (c ==']') {
            node->type = JSONIC_NONE;
            return node;
        } else if (c == ',') {
            node->pos++;
            node->arrind++;
            node->parser_state = JSONIC_EXPECT_ARR_END;
        }
    } else if (node->parser_state == JSONIC_EXPECT_BOOLVAL) {
        if (is_bool_char(c)) {
        } else {
            node->parser_state = JSONIC_EXPECT_KEY_START;
        }
    } else if (node->parser_state == JSONIC_EXPECT_ARR_BOOLVAL) {
        if (is_bool_char(c)) {
        } else if (c == ']') {
            node->type = JSONIC_NONE;
            return node;
        } else {
            if (c == ',') {
                node->pos++;
                node->arrind++;
            }
            
            node->parser_state = JSONIC_EXPECT_ARR_END;
        }
    } else if (node->parser_state == JSONIC_EXPECT_NULLVAL) {
        if (is_null_char(c)) {
        } else {
            node->parser_state = JSONIC_EXPECT_KEY_START;
        }
    } else if (node->parser_state == JSONIC_EXPECT_ARR_NULLVAL) {
        if (is_null_char(c)) {
        } else if (c == ']') {
            node->type = JSONIC_NONE;
            return node;
        } else {
            if (c == ',') {
                node->pos++;
                node->arrind++;
            }
            
            node->parser_state = JSONIC_EXPECT_ARR_END;
        }
    }

    node->meta = 0;
    node->ind++;

    goto next;

    end:

    node->type = JSONIC_NONE;

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