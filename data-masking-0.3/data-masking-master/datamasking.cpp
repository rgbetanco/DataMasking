#include <iostream>

#include "debug.h"
#include "datamasking.hpp"
#include "string_generator/tree_parser_yacc.tab.h"

namespace dm
{
// constructors
Result::Result(std::string orginial_data, dm::Options *options_pointer)
{
    output_data = orginial_data;
    opt_ptr = options_pointer;
}

// functions
dm::Options *API::CreateDataMaskRule(dm::Options *opt_ptr)
{
    if (opt_ptr->generator_re[0] != '\0')
    {
        opt_ptr->method_code = TP_CALL_STRING_GENERATOR;
    }
    else if (!opt_ptr->replace_string.empty() && !opt_ptr->group_name.empty())
    {
        opt_ptr->method_code = TP_REPLACE_BY_GROUP_NAME;
    }

    return opt_ptr;
}

Result API::Process(std::string *input_data, dm::Options *opt_ptr)
{
    dm::Result res((*input_data), opt_ptr);
    ErrorCode errCode = NoError;

    switch (opt_ptr->method_code)
    {
    case TP_REPLACE_BY_GROUP_NAME:
        errCode = API::tp_Replace_By_Group_Name(input_data, *opt_ptr);
        break;
    case TP_REPLACE:
        errCode = API::tp_Replace(input_data, *opt_ptr);
        break;
    case TP_CALL_STRING_GENERATOR:
        opt_ptr->generator_re = ConsumeDollarSigns(input_data, opt_ptr);
        errCode = API::tp_Call_String_Generator(input_data, *opt_ptr);
        break;
    }

    if (NoError != errCode)
    {
        res.errMsg = errCode;
        return res;
    }
    res.output_data = *input_data;
    return res;
}

bool API::tp_isMatch(std::string *input_data, const dm::Options *opt_ptr)
{
    RE2 re2{opt_ptr->validation_re};
    if (!re2.ok())
    {
        std::cout << "Error: Failed to compile regular expression.\n";
        return false;
    }

    return RE2::FullMatch((*input_data).c_str(), re2);
}

// using this function for lookup interface
ErrorCode API::tp_Replace(std::string *str, const dm::Options &opt)
{
    // validate input rules
    if (opt.validation_re[0] == '\0')
    {
        std::cout << "Error: please fill in validation RegExp.\n";
        return ErrorInternal;
    }
    else if (opt.replace_groups.empty())
    {
        std::cout << "Error: please fill in generator rule.\n";
        return ErrorInternal;
    }

    RE2 re2{opt.validation_re};
    if (!re2.ok())
    {
        std::cout << "Error: Failed to compile regular expression.\n";
        return ErrorInternal;
    }

    std::string replace_rule = "";

    // composed replace_rule
    for (std::size_t i = 1; i <= static_cast<std::size_t>(re2.NumberOfCapturingGroups()); i++)
    {
        std::size_t first = 0;
        std::string second = "";

        for (std::size_t j = 0; j < opt.replace_groups.size(); j++)
        {
            if (i == opt.replace_groups[j].first)
            {
                first = opt.replace_groups[j].first;
                second.append(opt.replace_groups[j].second);
            }
        }

        if (first != 0)
        {
            // found the matched group number -> mask with specific string
            replace_rule.append(second);
        }
        else
        {
            // this group number needs to be displayed
            replace_rule.append("\\").append(std::to_string(i));
        }
    }
    // std::cout << "replace_rule: " << replace_rule << '\n';
    if (!RE2::Replace(str, re2, replace_rule))
    {
        return ErrorTpReplace;
    }
    return NoError;
}

ErrorCode API::tp_Replace_By_Group_Name(std::string *input_data, const dm::Options &opt)
{

    RE2 re2{opt.validation_re};
    if (!re2.ok())
    {
        std::cout << "Failed to compile regular expression.\n";
        return ErrorInternal;
    }
    auto pair = re2.NamedCapturingGroups();
    int group_num = pair[opt.group_name];
    std::string extracted_str = "";
    std::string rewrite = "\\";
    rewrite.append(std::to_string(group_num));

    if (!RE2::Extract(*input_data, re2, rewrite, &extracted_str) ||
        !RE2::Replace(input_data, extracted_str, opt.replace_string))
    {
        return ErrorTpReplaceByGroupName;
    }
    // std::cout << *input_data << '\n';
    return NoError;
}

ErrorCode API::tp_Call_String_Generator(std::string *input_data, const dm::Options &opt)
{
    ErrorCode ret = NoError;
    const size_t len_dst = 1024;

    char *output = new char[len_dst];
    memset(output, 0, len_dst);

    tree_node *tree_root = opt.tree_root;
    treeB_execParser(opt.generator_re.c_str(), output, &tree_root);

    // Work arround solution: have to put "tp_isMatch" under "treeB_getResult".
    // If doing the opposite way, it will cause "Segmentation fault (core dumped)"
    if (tp_isMatch(input_data, &opt))
    {
        (*input_data) = output;
        debug_print(DEBUG_LVL_LIGHT, "Matched, (*input_data): %s\n", (*input_data).c_str());
    }
    else
    {
        printf("Error: Regexp Not Matched, (*input_data): %s\n", (*input_data).c_str());
        ret = ErrorTpCallStringGenerator;
    }

    delete[] output;
    output = nullptr;

    return ret;
}

bool check_db_options_existed(dm::Options *opt_ptr)
{
    return (opt_ptr->dict_options.size() > 0 &&
            opt_ptr->lookupMap.size() > 0);
}

std::string check_db_group_num_existed(dm::Options *opt_ptr, int num)
{
    auto &db_opts = opt_ptr->dict_options;
    for (auto it = db_opts.begin(); it != db_opts.end(); ++it)
    {
        auto &vec = (*it).group_nums;
        auto inner_it = find(vec.begin(), vec.end(), num);
        if (inner_it != vec.end())
            return (*it).map_key;
    }

    return NOT_FOUND_DB_GROUP;
}

std::string check_db_group_name_existed(dm::Options *opt_ptr, std::string name)
{
    auto &db_opts = opt_ptr->dict_options;
    for (auto it = db_opts.begin(); it != db_opts.end(); ++it)
    {
        auto &vec = (*it).group_names;
        auto inner_it = find(vec.begin(), vec.end(), name);
        if (inner_it != vec.end())
            return (*it).map_key;
    }

    return NOT_FOUND_DB_GROUP;
}

std::string API::ConsumeDollarSigns(std::string *input_data, dm::Options *opt_ptr)
{
    std::string altered_generator_re = opt_ptr->generator_re;

    // $$ is the escape symbol to present $ symbol.
    // To deal with $$, replace with a unique string to prevent detecting by ${} or $n
    RE2::GlobalReplace(&altered_generator_re, "\\$\\$", UNIQUE_STRING);
    opt_ptr->generator_re = altered_generator_re;

    // start to deal with ${} and $n, then return result
    altered_generator_re = AlterGeneratorRegexp(input_data, opt_ptr);

    // replace the unique strings back to the $
    RE2::GlobalReplace(&altered_generator_re, UNIQUE_STRING, "$");
    return altered_generator_re;
}

std::string API::AlterGeneratorRegexp(std::string *input_data, dm::Options *opt_ptr)
{
    std::string altered_generator_re = opt_ptr->generator_re;

    // replace $n
    re2::StringPiece out{altered_generator_re};
    while (RE2::PartialMatch(out, CHECK_DOLLAR_DIGITS))
    {
        altered_generator_re = ReplaceDollarSignByDigit(input_data,
                                                        opt_ptr->validation_re,
                                                        altered_generator_re.c_str(),
                                                        opt_ptr);
        out = re2::StringPiece(altered_generator_re);
    }

    // replace ${}
    while (RE2::PartialMatch(out, CHECK_DOLLAR_WORDS))
    {
        altered_generator_re = ReplaceDollarSignByName(input_data,
                                                       opt_ptr->validation_re,
                                                       out.ToString().c_str(),
                                                       opt_ptr);
        out = re2::StringPiece(altered_generator_re);
    }

    return altered_generator_re;
}

std::string API::ReplaceDollarSignByDigit(std::string *input_data,
                                          std::string validation_re,
                                          const char *generator_re,
                                          dm::Options *opt_ptr)
{
    int digit{0};
    std::string extracted_str = "";
    std::string rewrite = "\\";
    std::string altered_generator_re{generator_re};

    RE2::PartialMatch(re2::StringPiece(altered_generator_re), CHECK_DOLLAR_DIGITS, &digit);
    rewrite.append(std::to_string(digit));

    std::string map_key = check_db_group_num_existed(opt_ptr, digit);
    bool is_db_options_existed = check_db_options_existed(opt_ptr);
    bool is_group_num_existed = (NOT_FOUND_DB_GROUP != map_key);

    // Using database data to replace target
    if (is_db_options_existed && is_group_num_existed)
    {
        extracted_str = opt_ptr->lookupMap[map_key]->genData();
        RE2::Replace(&altered_generator_re, CHECK_DOLLAR_DIGITS, extracted_str);
    }
    else // Using tagging input to replace target
    {
        // Extract sepcific data from input data
        RE2::Extract(*input_data, validation_re, rewrite, &extracted_str);
        RE2::Replace(&altered_generator_re, CHECK_DOLLAR_DIGITS, extracted_str);
    }
    return altered_generator_re;
}

std::string API::ReplaceDollarSignByName(std::string *input_data,
                                         std::string validation_re,
                                         const char *generator_re,
                                         dm::Options *opt_ptr)
{
    RE2 re2{validation_re};
    auto pair = re2.NamedCapturingGroups();

    std::string group_name{""};
    std::string extracted_str{""};
    std::string rewrite{"\\"};
    std::string altered_generator_re{generator_re};

    RE2::PartialMatch(re2::StringPiece(altered_generator_re), CHECK_DOLLAR_WORDS, &group_name);
    int group_num = pair[group_name];
    rewrite.append(std::to_string(group_num));

    std::string map_key = check_db_group_name_existed(opt_ptr, group_name);
    bool is_db_options_existed = check_db_options_existed(opt_ptr);
    bool is_group_name_existed = (NOT_FOUND_DB_GROUP != map_key);

    // Using database data to replace target
    if (is_db_options_existed && is_group_name_existed)
    {
        extracted_str = opt_ptr->lookupMap[map_key]->genData();
        RE2::Replace(&altered_generator_re, CHECK_DOLLAR_WORDS, extracted_str);
    }
    else // Using tagging input to replace target
    {
        // Extract sepcific data from input data
        RE2::Extract(*input_data, re2, rewrite, &extracted_str);
        RE2::Replace(&altered_generator_re, CHECK_DOLLAR_WORDS, extracted_str);
    }

    return altered_generator_re;
}
} // namespace dm