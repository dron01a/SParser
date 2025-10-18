#include "spxml.h"
#include <iostream>

int main(){

	sp::xml_parser pars; 
	sp::parse_result r_res = pars.load_from_file(_t("test.xml"));

	sp::write_result w_res = pars.load_to_file(_t("dest_test.xml"));

    return 0;
}