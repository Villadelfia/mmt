#include <string>
#include <map>

class ConfigReader {
public:
    ConfigReader();
    ConfigReader(const std::string &fName);

    bool read(const std::string &fName);
    bool getKeyValue(const std::string &key, std::string &value);
    void printAll() const;
    void clear();

    std::string getErrorDescription() const;

private:
    std::string failState;
    std::map<std::string, std::string> config;
};
