#ifndef LOOKUP_H
#define LOOKUP_H

#include <vector>

namespace dm
{

// abstract class
class Lookup
{
public:
    // pure virtual function
    virtual const char *genData() = 0;
    // virtual destructor
    virtual ~Lookup(){};

    const char *name{""};

    static unsigned int rand_from_zero_to_max(unsigned int max);
};

// derived classes: generate data from a fixed table.
class LookupTable : public Lookup
{
public:
    // constructors
    LookupTable(std::string const &path);

    // functions
    const char *genData();

    // destructor
    ~LookupTable(){};

private:
    std::vector<std::string> strings_vector{};
};

// derived classes: generate data from a database.
class LookupDB : public Lookup
{
public:
    // constructors
    LookupDB(std::string const &db_type,
             std::string const &db_name,
             std::string const &db_table);

    // functions
    const char *genData();

    // destructor
    ~LookupDB(){};

private:
    std::vector<std::string> strings_vector{};
};

} // namespace dm
#endif // LOOKUP_H