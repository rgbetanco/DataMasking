#ifndef DATA_MASKING_H
#define DATA_MASKING_H

#include <map>

#include "lookup.hpp"
#include "string_generator/treebuilder.h"

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

class Options
{
public:
    const char *validation_re{""};
    const char *generator_re{""};

    // Tagging input related options
    std::string replace_string{""};
    std::string group_name{""};

    // Lookup interface related options
    std::map<std::string, Lookup *> lookupMap{}; // key: name of interface, value: Lookup interface
    std::vector<std::pair<std::size_t, std::string>> replace_groups{};

    MethodCode method_code{TP_REPLACE};
    tree_node *tree_root{nullptr};
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
};

} // namespace dm
#endif //DATA_MASKING_H