#ifndef DATA_MASKING_H
#define DATA_MASKING_H

#include <map>
#include <vector>
#include <re2/re2.h>

#include "lookup.hpp"
#include "string_generator/treebuilder.h"

const std::string CHECK_DOLLAR_DIGITS{"\\$(\\d+)"};
const std::string CHECK_DOLLAR_WORDS{"\\$\\{(\\w+)\\}"};
const std::string UNIQUE_STRING{"ItIsAnUNiqueStr"};
const std::string NOT_FOUND_DB_GROUP{"NOT_FOUND_DB_GROUP"};

enum ErrorCode
{
    NoError = 0,

    // Unexpected error
    ErrorInternal,
    ErrorTpReplace,
    ErrorTpReplaceByGroupName,
    ErrorTpCallStringGenerator
};

enum MethodCode
{
    TP_REPLACE = 0,
    TP_REPLACE_BY_GROUP_NAME,
    TP_CALL_STRING_GENERATOR
};

namespace dm
{

// the options which define the replace group name/number in dictionary/lookup interface
typedef struct dict_option
{
    std::string map_key{NOT_FOUND_DB_GROUP};
    std::vector<int> group_nums{};
    std::vector<std::string> group_names{};
} dict_option;

class Options
{
public:
    std::string validation_re{""};
    std::string generator_re{""};

    // Lookup interface related options
    std::map<std::string, Lookup *> lookupMap{}; // key: name of interface, value: Lookup interface
    std::vector<dict_option> dict_options{};
    
    // Misc. No need to setup these items
    MethodCode method_code{TP_REPLACE};
    tree_node *tree_root{nullptr};

    // debug only
    std::vector<std::pair<std::size_t, std::string>> replace_groups{};
    std::string replace_string{""};
    std::string group_name{""};
};

class Result
{
public:
    Result(std::string orginial_data, dm::Options *options_pointer);
    ErrorCode errMsg{NoError};
    std::string output_data{""};
    dm::Options *opt_ptr{nullptr};
};

class API
{
public:
    static dm::Options *CreateDataMaskRule(dm::Options *opt_ptr);
    static Result Process(std::string *input_data, dm::Options *opt_ptr);

private:
    static bool tp_isMatch(std::string *input_data, const dm::Options *opt_ptr);
    static ErrorCode tp_Replace(std::string *str, const dm::Options &opt);
    static ErrorCode tp_Replace_By_Group_Name(std::string *input_data, const dm::Options &opt);
    static ErrorCode tp_Call_String_Generator(std::string *input_data, const dm::Options &opt);

    static std::string ConsumeDollarSigns(std::string *input_data, dm::Options *opt_ptr);
    static std::string AlterGeneratorRegexp(std::string *input_data, dm::Options *opt_ptr);
    static std::string ReplaceDollarSignByDigit(std::string *input_data,
                                                std::string validation_re,
                                                const char *generator_re,
                                                dm::Options *opt_ptr);
    static std::string ReplaceDollarSignByName(std::string *input_data,
                                               std::string validation_re,
                                               const char *generator_re,
                                               dm::Options *opt_ptr);
};

} // namespace dm
#endif //DATA_MASKING_H