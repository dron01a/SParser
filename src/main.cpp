#include "spxml.h"
#include <iostream>

int main(){
    
	//sp::string_t str = _t("<name a  = \" &apos;12&apos;&quot; \" b = \"\"><sub_name>text in sub_name &amp;</sub_name></name>");

	//sp::file_reader r(_t("test.xml"));

	//std::vector<sp::token> vt;

	//sp::token t = r.get_next_token();

	//while (r.get_last_token().type != sp::token_type::end_of_data) {
	//	vt.push_back(t);
	//	t = r.get_next_token();
	//}

	//// attr test

	//sp::attribute a1(L"a", 1);
	//sp::attribute a2(L"a", 1);

	//bool res = a1 == a2;

	sp::xml_parser par;
	sp::parse_result p_res = par.load_from_file(_t("test.xml"));



    return 0;
}