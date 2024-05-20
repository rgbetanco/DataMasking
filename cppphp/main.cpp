#include <phpcpp.h>
#include <iostream>
#include "data-masking/datamasking.hpp"
#include "data-masking/string_generator/treebuilder.h"

using namespace std;

Php::Value Options(Php::Parameters &params){

	/*cout << "\n----Test----\n";
	string s1 = "台北市開山里255號";
	cout << "before: " << s1 << endl;*/

	dm::Options* opt_ptr = new dm::Options();
	opt_ptr->validation_re = params[0];
	opt_ptr->generator_re = params[1];
	cout << opt_ptr->generator_re;
	dm::API::CreateDataMaskRule(opt_ptr);

	/*dm::Result res = dm::API::Process(&s1, opt_ptr);
	if (res.errMsg != NoError)
	{
		cout << "Error #" << res.errMsg << endl;
	}
	cout << "after: " << res.output_data << endl << endl;*/
	//delete_tree(&opt_ptr->tree_root);
	//delete opt_ptr;
}

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("data_masking", "1.0");
		extension.add("Options", Options);

        // return the extension
        return extension;
    }
}
