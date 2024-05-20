#ifndef MYSQL_CONNECT_HPP
#define MYSQL_CONNECT_HPP

#include <vector>
#include <string>

class MySqlConnect
{
public:
    static void getOneColumn(
        std::string const &db_name,
        std::string const &db_table,
        std::vector<std::string> &strings_vector);
};
#endif // MYSQL_CONNECT_HPP