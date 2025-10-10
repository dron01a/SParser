#include "spxml.h"
#include <iostream>

int main(){
    
	sp::string_t str = _t("<name a  = \" &apos;12&apos;&quot; \" b = \"\"><sub_name>text in sub_name &amp;</sub_name></name>");

	sp::string_reader r(str);

	std::vector<sp::token> vt;

	sp::token t = r.get_next_token();

	while (r.get_last_token().type != sp::token_type::end_of_data) {
		vt.push_back(t);
		t = r.get_next_token();
	}

    return 0;
}