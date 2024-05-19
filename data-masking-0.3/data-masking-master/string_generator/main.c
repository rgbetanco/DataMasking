#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree_parser_yacc.tab.h"

#define NUMBER_OF_STRING 5
#define MAX_STRING_SIZE 40

extern const size_t len_dst;

void print_string_by_regexp(const char *input)
{
    tree_node *tree_root = NULL;
    char *output = (char *)malloc(len_dst);
    memset(output, 0, len_dst);

    treeB_execParser(input, output, &tree_root);

    printf("Result: %s\n", output);

    free(output);
    delete_tree(&tree_root);
}

void test_parse_utf8()
{
    printf("----Testing Parse UTF-8------\n");
    // Chinese
    printf("Chinese character ");
    print_string_by_regexp("中|文|字|博|大|精|深");
    // Russian
    printf("Russian character ");
    print_string_by_regexp("б|в|г|д|ж|з|к");
    // Japanese
    printf("Japanese character ");
    print_string_by_regexp("ｱ|ｲ|ｳ|ｴ|ｵ|ｶ|ｷ");
}

const char input_arr[NUMBER_OF_STRING][MAX_STRING_SIZE] =
{
    "([ABC]{2}|[123]){3}",
    "([ABC]{2}|[123]{6}|[gef]{7}){3}",
    "([ABC]{2}|[123]*){3}",
    "([ABC]{2}|[123]?|Y+){3}",
    "[ABC](123)*g{4,7}"
};

const char input_arr1[2][MAX_STRING_SIZE] =
{
    ".{5,9}",
    "[^0123456789]{100}"
};

const char input_arr2[2][MAX_STRING_SIZE] =
{
    "\\d+",
    "\\D{1,6}"
};

void test_tree_builder(const char arr[][MAX_STRING_SIZE], const size_t number_of_string)
{
    char *input = "";

    printf("\n----Testing Tree Builder------\n");
    for (size_t i = 0; i < number_of_string; i++)
    {
        input = (char* )arr[i];
        printf("\n-------------------Test %zu start, input = %s\n", i + 1, input);

        print_string_by_regexp(input);

        printf("\n-------------------Test %zu end-----------------------------\n", i + 1);
    }
}

void test_gen_chinese_by_pattern_t()
{
    printf("\n----Testing Generate Traditional Chinese ------\n");
    print_string_by_regexp("\\t");
    print_string_by_regexp("\\t*");
    print_string_by_regexp("\\t+");
    print_string_by_regexp("\\t?");
    print_string_by_regexp("\\t{4,8}");

    printf("\n----Testing consecutive Traditional Chinese ------\n");
    print_string_by_regexp("\\t ( 測試) ( Abc)");
    print_string_by_regexp("\\t* ( 測試) ( Abc)");
    print_string_by_regexp("\\t+ ( 測試) ( Abc)");
    print_string_by_regexp("\\t? ( 測試) ( Abc)");
    print_string_by_regexp("\\t{2,3} ( 測試) ( Abc)");
}

// not recommend to test
void test_gen_address()
{
    printf("------Testing generate chinese address--------\n");

    for (int i = 0; i < 10; i++)
    {
        print_string_by_regexp("\\t{2,3}( 市, )\\t{2,3}( 里, )[0-9]{2,3}(號)");
    }
}

void test_utf8_commands()
{
    printf("\n----Testing UTF8 commands ------\n");
    print_string_by_regexp("\\＊*");
    print_string_by_regexp("\\＊{2,8}");
    print_string_by_regexp("\\○+");
    print_string_by_regexp("\\○{4,6}");
    print_string_by_regexp("\\**");
    print_string_by_regexp("\\*{1,7}");
}

void test_consective_text_and_regexp()
{
    const char* input = "test1[a-c]*test2[w-z]?";
    printf("\ninput = %s\n", input);
    print_string_by_regexp(input);

    input = "test3[a-c]+";
    printf("\ninput = %s\n", input);
    print_string_by_regexp(input);

    input = "[a-c]+test4";
    printf("\ninput = %s\n", input);
    print_string_by_regexp(input);

    input = "中港台[a-i]+ Taipay";
    printf("\ninput = %s\n", input);
    print_string_by_regexp(input);

    input = "NSecured [A-H]+星馬澳";
    printf("\ninput = %s\n", input);
    print_string_by_regexp(input);
}

int main(void)
{
    test_parse_utf8();

    test_tree_builder(input_arr, NUMBER_OF_STRING);

    test_tree_builder(input_arr1, 2);       // test ^ and .
    
    test_tree_builder(input_arr2, 2);       // test \\d and \\D

    print_string_by_regexp("中港台|日韓菲|星馬澳|美歐");

    test_gen_chinese_by_pattern_t();
    
    test_utf8_commands();                   // test \\* , \\＊ and \\○

    test_consective_text_and_regexp();
}
