# jsonic
JSON parser for C.

## Including and Compiling

### Including
```c
#include "jsonic.h"
```

### Compiling (GCC)
You can use `-I /path/to/jsonic` parameter for GCC.

## Reading JSON

### jsonic node
Every JSON result ([except not found result](#not-found-results)) is a `jsonic_node_t` object in jsonic.

```c
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
```

You will use structure members: `type`, `val`, `len` for reading JSON.

`val` is useable for only string and boolean types. `val` is a NULL Terminated String and you can get length of string via `len` for string values.

#### Boolean Values
Boolean values come as NULL Terminated String in `val`. `val` maybe `"1"` or `"0"` for boolean values.

#### Result Length
`len` is for only string type and does not give length of an array. You can use [jsonic_array_length()](#jsonic_array_length) for getting an array's length.

### jsonic node types
There are a few node types:

```c
#define JSONIC_NODE_TYPE_NONE 0
#define JSONIC_NODE_TYPE_OBJECT 1
#define JSONIC_NODE_TYPE_ARRAY 2
#define JSONIC_NODE_TYPE_STRING 3
#define JSONIC_NODE_TYPE_INTEGER 4
#define JSONIC_NODE_TYPE_BOOLEAN 5
```

#### Not Found Results:
You will never get a node with type `JSONIC_NODE_TYPE_NONE`, you will get `NULL` instead of a `jsonic_node_t` object pointer if result is not found.

### Functions
There are a few functions in jsonic.

#### jsonic_from_file
```c
char* jsonic_from_file(char* fname);
```

#### jsonic_free
```c
void jsonic_free(jsonic_node_t* node);
```

#### jsonic_get
```c
jsonic_node_t* jsonic_get(char* json_str, jsonic_node_t* current, char* key, int index);
```

##### Get a Object Key
Get a key from JSON root:
```c
jsonic_get(json_string, NULL, "someKey", 0);
```
or find an existing node:
```c
jsonic_get(json_string, some_object, "someKey", 0);
```
*(**index** parameter should be 0 for getting an object key.)*

##### Get an Element of Array
Get an array element from JSON Array root:
```c
jsonic_get(json_string, NULL, NULL, 5);
```
or find an existing node:
```c
jsonic_get(json_string, some_array, NULL, 5);
```
*(**key** parameter should be NULL for getting an element of array.)*

##### Inline Usage of jsonic_get
If you are using `jsonic_get()` as inline, application will have memory leaks and it will be non-memory-safe, so you may get `SIGSEGV`!

###### Example
Non-memory-safe usage:
```c
printf("Squad: %s\n", jsonic_get(json_string, NULL, "squadName", 0)->val);
```

This is memory-safe usage:
```c
jsonic_node_t* name = jsonic_get(json_string, NULL, "squadName", 0);

if (name != NULL) {
    if (name->type == JSONIC_NODE_TYPE_STRING) {
        printf("Squad: %s\n", name->val);
    }

    jsonic_free(name);
}
```

#### jsonic_array_length
```c
int jsonic_array_length(char* json_str, jsonic_node_t* array);
```

## Compile and Run test
You can compile and run test with these commands:
```bash
make
```
and run on Windows:
```bash
test.exe
```
on Linux:
```bash
./test.exe
```

## Example
An example for reading JSON data

```c
char* json_string = jsonic_from_file("test.json");

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
```

Test JSON (test.json):
```json
{
    "squadName": "Super hero squad",
    "homeTown": "Metro City",
    "formed": 2016,
    "secretBase": "Super tower",
    "active": true,
    "members": [
    {
        "name": "Molecule Man",
        "age": 29,
        "secretIdentity": "Dan Jukes",
        "powers": [
            "Radiation resistance",
            "Turning tiny",
            "Radiation blast"
        ]
    },
    {
        "name": "Madame Uppercut",
        "age": 39,
        "secretIdentity": "Jane Wilson",
        "powers": [
            "Million tonne punch",
            "Damage resistance",
            "Superhuman reflexes"
        ]
    },
    {
        "name": "Eternal Flame",
        "age": 1000000,
        "secretIdentity": "Unknown",
        "powers": [
            "Immortality",
            "Heat Immunity",
            "Inferno",
            "Teleportation",
            "Interdimensional travel"
        ]
    }
    ]
}
```

## Performance
I had did this lib for a need sometime. I have no idea for performance of this. If you do a benchmark, you can send me the results.

## TODO
* jsonic is only a reader for now. Editing feature is a TODO.
* A function for getting object keys as array

## License
MIT