# jsonic
JSON parser for C.

## Including and Compiling

### Including
```c
#include "jsonic.h"
```

### Compiling

###### GNU Compiler Collection (GCC)

```bash
cd /path/to/jsonic
make
cd /path/to/your/project
gcc -o example.exe example.c /path/to/jsonic/jsonic.o -I /path/to/jsonic
```

## Reading JSON

### jsonic node
Every JSON result ([except not found result](#not-found-results)) is a `jsonic_node_t` object in jsonic.

```c
typedef struct jsonic_node jsonic_node_t;
struct jsonic_node {
    int type;
    char* val;
    int len;
    ...
};
```

You will use structure members: `type`, `val`, `len` for reading JSON.

`val` is useable for **string**, **number**, **boolean** and **null** types. `val` is a NULL Terminated String and you can get length of string via `len` for string values.

#### String Values
String values come as NULL Terminated String in `val` and you can check them with `node->type == JSONIC_NODE_TYPE_STRING`.

#### Number Values
Number values come as NULL Terminated String in `val` and you can check them with `node->type == JSONIC_NODE_TYPE_NUMBER`.

#### Boolean Values
Boolean values come as NULL Terminated String in `val`. `val` maybe `"1"` or `"0"` for boolean values and you can check them with `node->type == JSONIC_NODE_TYPE_BOOLEAN`.

#### Null Values
Null values come as NULL Terminated String in `val` as `"0"` and you can check them with `node->type == JSONIC_NODE_TYPE_NULL`.

#### Result Length
`len` is useable for **string**, **number**, **boolean** and **null** types and does not give length of an **array**. You can use [jsonic_array_length()](#jsonic_array_length) for getting an array's length.

### jsonic node types
There are a few node types:

```c
#define JSONIC_NODE_TYPE_NONE 0
#define JSONIC_NODE_TYPE_OBJECT 1
#define JSONIC_NODE_TYPE_ARRAY 2
#define JSONIC_NODE_TYPE_STRING 3
#define JSONIC_NODE_TYPE_NUMBER 4
#define JSONIC_NODE_TYPE_BOOLEAN 5
#define JSONIC_NODE_TYPE_NULL 6
```

#### Not Found Results
You will never get a node with type `JSONIC_NODE_TYPE_NONE`, you will get `NULL` instead of a `jsonic_node_t` object pointer if result is not found.

### Functions
There are a few functions in jsonic.

#### jsonic_from_file()
```c
char* jsonic_from_file(char* fname);
```

#### jsonic_free()
```c
void jsonic_free(jsonic_node_t** node);
```
Example:

```c
jsonic_node_t* members = jsonic_object_get(json_string, NULL, "members");
...
jsonic_free(&members); // free'd and members is set to NULL
```

#### jsonic_free_addr()
```c
void jsonic_free_addr(jsonic_node_t* node);
```

#### jsonic_get_root()
```c
jsonic_node_t* jsonic_get_root(char* json_str);
```

##### Checking Root

```c
jsonic_node_t* root = jsonic_get_root(json_string);

if (root->type == JSONIC_NODE_TYPE_OBJECT) {
    printf("JSON root is an object.\n");
} else if (root->type == JSONIC_NODE_TYPE_ARRAY) {
    printf("JSON root is an array.\n");
} else if (root->type == JSONIC_NODE_TYPE_STRING) {
    printf("JSON root is a string.\n");
}
```

#### jsonic_object_get()
```c
jsonic_node_t* jsonic_object_get(char* json_str, jsonic_node_t* current, char* key);
```

**Notice:** `current` parameter must be `NULL` for getting value of a key from root.

##### Get an Object Key
Get a key from JSON root:
```c
jsonic_object_get(json_string, NULL, "someKey");
```
or find an existing node:
```c
jsonic_object_get(json_string, some_object, "someKey");
```

#### jsonic_array_get()
```c
jsonic_node_t* jsonic_array_get(char* json_str, jsonic_node_t* current, int index);
```

**Notice:** `current` parameter must be `NULL` for getting an item from root.

##### Get an Element of Array
Get an array element from JSON Array root:
```c
jsonic_array_get(json_string, NULL, 5);
```
or find an existing node:
```c
jsonic_array_get(json_string, some_array, 5);
```

##### Inline Usage of `jsonic_object_get()` or `jsonic_array_get()`
If you are using `jsonic_object_get()` or `jsonic_array_get()` as inline, application will have memory leaks and it will be non-memory-safe, so you may get `SIGSEGV`!

###### Example
Non-memory-safe usage:
```c
printf("Squad: %s\n", jsonic_object_get(json_string, NULL, "squadName")->val);
```

This is memory-safe usage:
```c
jsonic_node_t* name = jsonic_object_get(json_string, NULL, "squadName");

if (name != NULL) {
    if (name->type == JSONIC_NODE_TYPE_STRING) {
        printf("Squad: %s\n", name->val);
    }

    jsonic_free(&name);
}
```

#### jsonic_array_length()
```c
int jsonic_array_length(char* json_str, jsonic_node_t* array);
```

#### jsonic_array_iter()
```c
jsonic_node_t* jsonic_array_iter(char* json_str, jsonic_node_t* current, jsonic_node_t* node, int index);
```

Useable for iterating arrays. Starts reading from given `node` in `current`, so `index` parameter must start from `0`, it will start reading from given `node`.

#### jsonic_array_iter_free()
```c
jsonic_node_t* jsonic_array_iter_free(char* json_str, jsonic_node_t* current, jsonic_node_t* node, int index);
```

Same as `jsonic_array_iter()` but frees given jsonic object (`node`).

##### Iterating Arrays
You can use `jsonic_array_iter()` or `jsonic_array_iter_free()`.
```c
jsonic_node_t* power = NULL;
for (;;) {
    power = jsonic_array_iter_free(json_string, powers, power, 0);
    if (!power) break;
}
```

**Notice:** Using `jsonic_array_iter()` instead of `jsonic_array_iter_free()` would be non-memory safe for most of times.

#### jsonic_object_iter()
```c
jsonic_node_t* jsonic_object_iter(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
```

Useable for getting a key's value with more performance. It starts reading from given `node` in `current`.

**Notice:** You must be sure your key is located after given `node` in JSON.

#### jsonic_object_iter_free()
```c
jsonic_node_t* jsonic_object_iter_free(char* json_str, jsonic_node_t* current, jsonic_node_t* from, char* key);
```

Same as `jsonic_object_iter()` but frees given jsonic object (`from`).

##### Iterating Objects
You can use `jsonic_object_iter()` or `jsonic_object_iter_free()`.
```c
jsonic_node_t* node = jsonic_object_iter(json_string, something, previousNode, "someKey");
```

## Compile and Run Examples
You can compile and run examples in `examples/` directory:
```bash
cd examples/heroes
make
```
and run on Windows:
```bash
heroes.exe
```
on Linux:
```bash
./heroes.exe
```

## Example
An example for reading JSON data

```c
char* json_string = jsonic_from_file("heroes.json");

jsonic_node_t* members = jsonic_object_get(json_string, NULL, "members");
jsonic_node_t* member = jsonic_array_get(json_string, members, 1);
jsonic_node_t* powers = jsonic_object_get(json_string, member, "powers");

// inline usage: non-free'd nodes and non-safe pointers!..
printf("Squad: %s\n", jsonic_object_get(json_string, NULL, "squadName")->val);
printf("Active: %s\n", jsonic_object_get(json_string, NULL, "active")->val);
printf("Formed: %s\n", jsonic_object_get(json_string, NULL, "formed")->val);
printf("Name: %s\n", jsonic_object_get(json_string, member, "name")->val);
printf("Age: %s\n", jsonic_object_get(json_string, member, "age")->val);
printf("Powers (%d total):\n", jsonic_array_length(json_string, powers));

jsonic_node_t* power = NULL;
for (;;) {
    power = jsonic_array_iter_free(json_string, powers, power, 0);
    if (!power) break;
    
    if (power->type == JSONIC_NODE_TYPE_STRING) {
        printf("\t%s\n", power->val);
    }
}

jsonic_free(&members);
jsonic_free(&member);
jsonic_free(&powers);
free(json_string);
```

Example JSON (heroes.json):
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

## Syntax Checking
This library does not check JSON syntax, so you may get `SIGSEGV` or maybe infinite loops for **corrupt JSONs**. Likewise in some cases of corrupt JSONs, it would work as properly.

## Performance
There are some example JSONs and reading examples in `examples/` folder for profiling the performance.

## TODO
* Function for iterating `key`=>`value` pairs.

## License
MIT