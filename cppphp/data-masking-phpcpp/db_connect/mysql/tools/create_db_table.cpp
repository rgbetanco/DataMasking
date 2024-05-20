#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#include "../auth_info.h"

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv)
{
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, HOST, USER, PASSWORD, NULL, 0, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "DROP DATABASE IF EXISTS lookup_db"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "CREATE DATABASE lookup_db"))
    {
        finish_with_error(con);
    }

    if (mysql_query(con, "USE lookup_db"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "DROP TABLE IF EXISTS hk_address"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "CREATE TABLE hk_address(Name TEXT)"))
    {
        finish_with_error(con);
    }

    if (mysql_query(con, "INSERT INTO hk_address VALUES('烏蛟騰')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('烏溪沙')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('西貢')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('西灣河')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('西營盤')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('喜靈洲')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('香港仔')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('西營盤')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('西貢')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('小瀝源')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('小西灣')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('欣澳')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('新蒲崗')"))
    {
        finish_with_error(con);
    }
    if (mysql_query(con, "INSERT INTO hk_address VALUES('新田')"))
    {
        finish_with_error(con);
    }

    mysql_close(con);
    exit(0);
}