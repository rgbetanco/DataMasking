#include <iostream>

#include "datamasking.hpp"
#include "string_generator/tree_parser_yacc.tab.h"

void test_tagging_input()
{
    std::cout << "\n-----Testing replace tagging input by \"taipay\"-----\n";
    // Input string
    std::string s1 = "billy@example.com";
    std::cout << "before: " << s1 << '\n';
    // Options
    dm::Options* opt_ptr = new dm::Options();
    opt_ptr->validation_re =    "^(?P<firstName>[a-z]+)@(?P<domainName>[a-z]+).com$"; 
    opt_ptr->replace_string =   "taipay";
    opt_ptr->group_name =       "domainName";
    dm::API::CreateDataMaskRule(opt_ptr);

    // Start Processing
    dm::Result res = dm::API::Process(&s1, opt_ptr);
    std::cout << " after: " << res.output_data << ", res.opt_ptr: " << res.opt_ptr << "\n\n";

    res.opt_ptr->replace_string = "nsecured";
    res = dm::API::Process(&s1, opt_ptr);
    std::cout << " second: " << res.output_data << ", res.opt_ptr: " << res.opt_ptr << "\n\n";
    delete opt_ptr;
}

void test_fixed_table_lookup()
{
    std::cout << "\n-----Testing lookup interface of fixed table-----\n";
    std::string s1 = "台北市大業路";

    dm::Options *opt_ptr = new dm::Options();
    // group 1: city name, group 2: 市|縣, group 3: road name, group 4: 路
    opt_ptr->validation_re = "([\\x{4E00}-\\x{9FFF}]{2})(市|縣)([\\x{4E00}-\\x{9FFF}]{2})(路)";

    // load terms from fixed text file.
    dm::LookupTable *lookupTbl_ptr = new dm::LookupTable("look_up_tables/HK_names.txt");
    std::string key = "Fixed Table";
    opt_ptr->lookupMap[key] = lookupTbl_ptr;
    
    for (int i = 0; i < 10; i++)
    {
        std::string buffer = s1.substr(0, s1.size());

        std::cout << "test " << (i + 1) << '\n';
        std::cout << "before s = " << buffer << '\n';

        // group 3 and group 1 replace by data from fixed table.
        opt_ptr->replace_groups =
        {
            {3, opt_ptr->lookupMap[key]->genData()},
            {1, opt_ptr->lookupMap[key]->genData()}
        };

        // processing rule then return result
        dm::Result res = dm::API::Process(&buffer, opt_ptr);
        std::cout << " after s = " << res.output_data << "\n\n";
    }

    delete opt_ptr->lookupMap[key];
    delete opt_ptr;
}

void test_db_table_data_random_retrieve()
{
    std::cout << "\n-----Testing random retrieve data from a designated MySQL's table-----\n";
    dm::LookupDB *lookupDB_ptr = new dm::LookupDB("MySQL", "lookup_db", "hk_address");
    std::cout << lookupDB_ptr->genData() << '\n';
    delete lookupDB_ptr;
}

// For now, there are only two kinds of interface, fixed table and database table.
void test_lookup_in_random_interface()
{
    std::cout << "\n-----Testing lookup in random interface-----\n";
    std::string s1 = "桃園市中正路";

    dm::Options *opt_ptr = new dm::Options();
    // group 1: city name, group 2: 市|縣, group 3: road name, group 4: 路
    opt_ptr->validation_re = "([\\x{4E00}-\\x{9FFF}]{2})(市|縣)([\\x{4E00}-\\x{9FFF}]{2})(路)";

    // load terms from fixed text file.
    dm::LookupTable *lookupTbl_ptr = new dm::LookupTable("look_up_tables/HK_names.txt");
    // load terms from MySQL
    dm::LookupDB *lookupDB_ptr = new dm::LookupDB("MySQL", "lookup_db", "hk_address");
    // Add the lookup pointers into the map, it supposes to have several methods
    std::string names_of_lookup[] = {"Fixed Table", "MySQL1"};
    opt_ptr->lookupMap[names_of_lookup[0]] = lookupTbl_ptr;
    opt_ptr->lookupMap[names_of_lookup[1]] = lookupDB_ptr;

    for (int i = 0; i < 10; i++)
    {
        std::string buffer = s1.substr(0, s1.size());

        std::cout << "test " << (i + 1) << '\n';
        std::cout << "before s = " << buffer << '\n';

        // choose interface randomly
        unsigned int idx = dm::Lookup::rand_from_zero_to_max(opt_ptr->lookupMap.size());
        std::cout << "Load terms from interface \"" << names_of_lookup[idx] << "\"...\n";
        
        // group 3 and group 1 replace by data from fixed table.
        opt_ptr->replace_groups =
        {
            {3, opt_ptr->lookupMap[names_of_lookup[idx]]->genData()},
            {1, opt_ptr->lookupMap[names_of_lookup[idx]]->genData()}
        };

        // processing rule then return result
        dm::Result res = dm::API::Process(&buffer, opt_ptr);
        std::cout << " after s = " << res.output_data << "\n\n";
    }

    //delete pointers in lookupMap
    for (std::size_t i = 0; i < opt_ptr->lookupMap.size(); i++)
    {
        delete opt_ptr->lookupMap[names_of_lookup[i]];
    }
    delete opt_ptr;
}

void legacy_tests()
{
    test_tagging_input();

    test_fixed_table_lookup();

    test_db_table_data_random_retrieve();

    test_lookup_in_random_interface();
}