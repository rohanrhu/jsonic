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
    std::ifstream ifs("canada.json");
    
    std::string buffer;
    buffer.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    // Boost

    {
        boost::json::monotonic_resource mr;
        std::string path = "/features/0/geometry/coordinates";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        auto jv = boost::json::parse(buffer, &mr);

        auto& coordinates = jv.at_pointer(path).as_array();
        auto  size = coordinates.size();

        uint total = 0;
        for (auto& inner : coordinates) {
            total += inner.as_array().size();
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        std::cout << "Boost: " << std::endl;
        std::cout << "Total: " << total << std::endl;
        std::cout << "Time: " << duration.count() << " microseconds" << std::endl;
    }

    std::cout << "----------------------------" << std::endl;

    // Jsonic

    {
        char* json_string = (char*) buffer.c_str();

        auto start = std::chrono::high_resolution_clock::now();

        jsonic_node_t* root = jsonic_get_root(json_string);
        jsonic_node_t* features = jsonic_object_get(json_string, root, "features");
        jsonic_node_t* feature = jsonic_array_get(json_string, features, 0);
        jsonic_node_t* geometry = jsonic_object_get(json_string, feature, "geometry");
        jsonic_node_t* coordinates = jsonic_object_get(json_string, geometry, "coordinates");

        uint total = 0;
        jsonic_node_t* coord = NULL;
        for (;;) {
            coord = jsonic_array_iter(json_string, coordinates, coord, 0);
            if (coord->type == JSONIC_NONE)
                break;

            total += jsonic_array_length(json_string, coord).length;
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        std::cout << "Jsonic: " << std::endl;
        std::cout << "Total: " << total << std::endl;
        std::cout << "Time: " << duration.count() << " microseconds" << std::endl;
    }
    
    return 0;
}