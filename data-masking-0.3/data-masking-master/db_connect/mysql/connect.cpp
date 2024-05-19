#include <mysql.h>
#include <vector>

#include "connect.hpp"
#include "auth_info.h"

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

void MySqlConnect::getOneColumn(
    std::string const &db_name,
    std::string const &db_table,
    std::vector<std::string> &strings_vector)
{
    MYSQL *con = mysql_init(NULL);

    std::string query_use_db = "USE ";
    query_use_db.append(db_name);

    std::string query_select = "SELECT * FROM ";
    query_select.append(db_table);

    if (con == NULL)
    {
        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }

    if (mysql_real_connect(con, HOST, USER, PASSWORD, db_name.c_str(), 0, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }

    if (mysql_query(con, query_use_db.c_str()))
    {
        finish_with_error(con);
    }
    
    if (mysql_query(con, query_select.c_str()))
    {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
    {
        finish_with_error(con);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
    
    while ((row = mysql_fetch_row(result)))
    {
        for (int i = 0; i < num_fields; i++)
        {
            // printf("%s ", row[i] ? row[i] : "NULL");
            std::string item{row[i] ? row[i] : "NULL"};
            strings_vector.push_back(item);
        }
        // printf("\n");
    }
    mysql_free_result(result);
    mysql_close(con);
}