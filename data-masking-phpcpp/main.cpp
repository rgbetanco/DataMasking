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
    dm::Options* opt_ptr = new dm::Options();
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
    // treeB_getResult(buffer);
    std::cout << "test1: " << output << '\n';

    memset(output, 0, len_dst);
    treeB_execParser("\\t{2,3}(市)\\t{2,3}(里)[0-9]{2,3}(號)", output, &tree_root);
    // treeB_getResult(buffer);
    std::cout << "test2: " << output << '\n';
    delete_tree(&tree_root);
    free(output);
}

int main()
{
    test_gen_string_with_same_tree();

    test_address_capture_and_replace();
    
    // legacy_tests();

    return 0;
}