/**
 * Randy Thiemann 2013
 **/

#include <string>
#include <map>

#ifndef CONFIGREADER_H_
#define CONFIGREADER_H_

class ConfigReader {
public:
    ConfigReader();
    ConfigReader(const std::string& fName);

    bool read(const std::string& fName);
    bool getKeyValue(const std::string& key, std::string& value);
    bool keyValueExists(const std::string& key);
    void printAll() const;
    void clear();

    std::string getErrorDescription() const;

private:
    std::string failState;
    std::map<std::string, std::string> config;
};

#endif /* CONFIGREADER_H_ */
