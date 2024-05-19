#include <iostream>

#include "datamasking.hpp"

void legacy_tests();

void test_address_capture_and_replace()
{
    std::cout << "\n-----Testing address capture and replace-----\n";
    // Input string
    std::string s1 = "台北市開山里255號";
    std::cout << "before: " << s1 << '\n';
    // Options
    dm::Options *opt_ptr = new dm::Options();
    opt_ptr->validation_re = "([\\x{4E00}-\\x{9FFF}]{2})市([\\x{4E00}-\\x{9FFF}]{2})里([0-9]{2,3})號";
    opt_ptr->generator_re = "\\t{2,3}(市)\\t{2,3}(里)[0-9]{2,3}(號)";
    dm::API::CreateDataMaskRule(opt_ptr);

    // Start Processing
    dm::Result res = dm::API::Process(&s1, opt_ptr);
    if (res.errMsg != NoError)
    {
        std::cout << "Error #" << res.errMsg << '\n';
    }
    std::cout << " after: " << res.output_data << "\n\n";
    delete_tree(&opt_ptr->tree_root);
    delete opt_ptr;
}

#include "string_generator/tree_parser_yacc.tab.h"
#include <cstring>
void test_gen_string_with_same_tree()
{
    std::cout << "\n-----Testing generate string with the same tree-----\n";
    const size_t len_dst = 1024;
    char *output = (char *)malloc(len_dst);
    memset(output, 0, len_dst);

    tree_node *tree_root{nullptr};
    treeB_execParser("\\t{2,3}(市)\\t{2,3}(里)[0-9]{2,3}(號)", output, &tree_root);
    std::cout << "test1: " << output << '\n';

    memset(output, 0, len_dst);
    treeB_execParser("\\t{2,3}(市)\\t{2,3}(里)[0-9]{2,3}(號)", output, &tree_root);
    std::cout << "test2: " << output << '\n';

    delete_tree(&tree_root);
    free(output);
}

void test_consume_dollar_signs()
{
    std::cout << "\n--------Testing Consume $ signs with tagging input data------------\n";
    std::string buffer{"billy@example.com"};
    std::string input = buffer.substr(0, buffer.size());
    dm::Options *opt_ptr = new dm::Options();
    opt_ptr->validation_re = "^(?P<firstName>[a-z]+)@(?P<domainName>[a-z]+).com$";
    opt_ptr->generator_re = "$1com${domainName}$$1234 testing ${firstName}";

    std::cout << "input: " << input << '\n';
    std::cout << "validation_re: " << opt_ptr->validation_re << '\n';
    std::cout << "generator_re: " << opt_ptr->generator_re << '\n';
    dm::API::CreateDataMaskRule(opt_ptr);

    // Start Processing
    dm::Result res = dm::API::Process(&input, opt_ptr);
    std::cout << " after process: " << res.output_data << ", res.opt_ptr: " << res.opt_ptr << "\n\n";

    std::cout << "\n--------Testing Consume $ signs with database dictionary------------\n";
    // recover to original conditions
    input = buffer.substr(0, buffer.size());
    opt_ptr->generator_re = "$1com${domainName}$$1234 testing ${firstName}";

    // set up the replace group and dictionary/database mapping
    dm::dict_option dict_opt1;
    dict_opt1.map_key = "hk_address";
    dict_opt1.group_names = {"firstName"};
    dict_opt1.group_nums = {1, 2};
    opt_ptr->dict_options.push_back(dict_opt1);
    dm::LookupDB *lookupDB_ptr = new dm::LookupDB("MySQL", "lookup_db", "hk_address");
    opt_ptr->lookupMap[dict_opt1.map_key] = lookupDB_ptr;
    // if there are another dictionary/datatbase, set up another dict_opt2, 3, 4 ...etc.

    std::cout << "input: " << input << '\n';
    std::cout << "validation_re: " << opt_ptr->validation_re << '\n';
    std::cout << "generator_re: " << opt_ptr->generator_re << '\n';

    // Start Processing
    res = dm::API::Process(&input, opt_ptr);
    std::cout << " after process: " << res.output_data << ", res.opt_ptr: " << res.opt_ptr << "\n\n";

    // release resources
    delete_tree(&opt_ptr->tree_root);
    delete opt_ptr;
}

int main()
{
    // test_gen_string_with_same_tree();

    // test_address_capture_and_replace();

    // legacy_tests();

    test_consume_dollar_signs();

    return 0;
}