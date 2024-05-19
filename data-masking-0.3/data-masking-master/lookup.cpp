#include <fstream>
#include <sstream>
#include <sodium.h>

#include "lookup.hpp"
#include "db_connect/mysql/connect.hpp"

// constructors
dm::LookupTable::LookupTable(std::string const &path)
{
    // printf("%s, %s, %d\n", __FILE__, __func__, __LINE__);
    std::ifstream file(path);

    std::string line;
    while (getline(file, line))
    {
        std::stringstream linestream(line);
        std::string value;

        while (getline(linestream, value, '|'))
        {
            // std::cout << "Value(" << value << ")\n";
            strings_vector.push_back(value);
        }
        // std::cout << "Line Finished" << std::endl;
    }
}

dm::LookupDB::LookupDB(std::string const &db_type,
                       std::string const &db_name,
                       std::string const &db_table)
{
    if (db_type.compare("MySQL") == 0)
    {
        MySqlConnect::getOneColumn(db_name,
                                   db_table,
                                   strings_vector);
    }
    else
    {
        printf("Error: Unknown db_type: %s\n", db_type.c_str());
        exit(1);
    }
}

// functions
const char *dm::LookupTable::genData()
{
    // printf("%s, %s, line %d, size = %zu\n", __FILE__, __func__, __LINE__, strings_vector.size());
    return strings_vector[rand_from_zero_to_max(strings_vector.size())].c_str();
}

const char *dm::LookupDB::genData()
{
    // printf("%s, %s, line %d, size = %zu\n", __FILE__, __func__, __LINE__, strings_vector.size());
    return strings_vector[rand_from_zero_to_max(strings_vector.size())].c_str();
}

unsigned int dm::Lookup::rand_from_zero_to_max(unsigned int max)
{
    char myString[32];
    uint32_t myInt;

    if (sodium_init() < 0)
    {
        printf("panic! the Sodium library couldn't be initialized, it is not safe to use\n");
        return 0;
    }

    // myString will be an array of 32 random bytes, not null-terminated
    randombytes_buf(myString, 32);

    // myInt will be a random number between 0 and RAND_MAX
    myInt = randombytes_uniform(RAND_MAX);
    return myInt % max;
}