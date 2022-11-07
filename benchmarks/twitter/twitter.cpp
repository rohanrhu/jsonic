/*
 * jsonic is a json parser for C
 *
 * https://github.com/rohanrhu/jsonic
 * https://oguzhaneroglu.com/projects/jsonic/
 *
 * Licensed under MIT
 * json_stringright (C) 2018, Oğuzhan Eroğlu (https://oguzhaneroglu.com/) <rohanrhu2@gmail.com>
 */


#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "jsonic.h"
#include <boost/json.hpp>

int main() {
    std::ifstream ifs("../../examples/twitter/twitter.json");
    
    std::string buffer;
    buffer.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    std::chrono::microseconds boost_duration;
    std::chrono::microseconds jsonic_duration;

    // Jsonic

    {
        char* json_string = (char*) buffer.c_str();

        auto start = std::chrono::high_resolution_clock::now();

        jsonic_node_t* root = jsonic_get_root(json_string);
    
        jsonic_node_t* statuses = jsonic_object_get(json_string, root, "statuses");

        jsonic_node_t* id;
        jsonic_node_t* user;
        jsonic_node_t* name;
        jsonic_node_t* text;

        jsonic_node_t* status = NULL;
        for (;;) {
            status = jsonic_array_iter(json_string, statuses, status, 0);
            if (status->type == JSONIC_NONE) break;

            id = jsonic_object_get(json_string, status, "id");
            text = jsonic_object_iter(json_string, status, id, "text");
            user = jsonic_object_iter(json_string, status, text, "user");
            name = jsonic_object_get(json_string, user, "name");

            std::cout << "ID: " << id->val << std::endl;
            std::cout << "Name: " << name->val << std::endl;
            std::cout << "Text: " << text->val << std::endl;
        }

        auto stop = std::chrono::high_resolution_clock::now();
        jsonic_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    }

    std::cout << "----------------------------" << std::endl;

    // Boost

    {
        auto start = std::chrono::high_resolution_clock::now();
        
        boost::json::monotonic_resource mr;
        
        auto jv = boost::json::parse(buffer, &mr);

        auto& statuses = jv.as_object()["statuses"].as_array();

        for (auto& inner : statuses) {
            auto id = inner.as_object()["id"];
            auto name = inner.as_object()["user"].as_object()["name"];
            auto text = inner.as_object()["text"];

            std::cout << "ID: " << id << std::endl;
            std::cout << "Name: " << name << std::endl;
            std::cout << "Text: " << text << std::endl;
        }

        auto stop = std::chrono::high_resolution_clock::now();
        boost_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    }

    std::cout << std::endl;

    std::cout << "Jsonic: " << std::endl;
    std::cout << "Time: " << jsonic_duration.count() << " microseconds" << std::endl;
    std::cout << "----------------------------" << std::endl;
    std::cout << "Boost: " << std::endl;
    std::cout << "Time: " << boost_duration.count() << " microseconds" << std::endl;
    
    return 0;
}