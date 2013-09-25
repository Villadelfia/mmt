#include "ConfigReader.h"
#include <fstream>
#include <string>
#include <iostream>

ConfigReader::ConfigReader() {
    failState = "";
}

ConfigReader::ConfigReader(const std::string &fName) {
    failState = "";
    read(fName);
}


bool ConfigReader::read(const std::string &fName) {
    std::fstream cfg(fName.data(), std::ios::in);
    if(cfg.fail()) {
        // There was an error opening the file.
        failState = "Can't open file";
        return false;
    }

    for(std::string line; std::getline(cfg, line); ) {
        // Lines may be empty
        if(line == "") {
            continue;
        }

        // Lines may not have an empty key
        if(line[0] == '=') {
            failState = "Invalid syntax";
            return false;
        }

        std::size_t idx = line.find_first_of("=");
        // If there is no equals sign, we have a problem.
        if(idx == std::string::npos) {
            failState = "Invalid syntax";
            return false;
        } else {
            // key is everything before the =, value is everything after.
            config[line.substr(0, idx)] = line.substr(idx+1);
        }
    }

    return true;
}

bool ConfigReader::getKeyValue(const std::string &key, std::string &value) {
    if(config.find(key) == config.end()) {
        failState = "No such key";
        return false;
    }
    value = config[key];
    return true;
}

void ConfigReader::printAll() const {
    for(std::map<std::string, std::string>::const_iterator it = config.begin();
            it != config.end();
            ++it) {
        std::cout << it->first << "=" << it->second << std::endl;
    }
}

void ConfigReader::clear() {
    config.clear();
}

std::string ConfigReader::getErrorDescription() const {
    if(failState == "") {
        return "Success";
    } else {
        return failState;
    }
}
