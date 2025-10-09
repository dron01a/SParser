#include "spxml.h"
#include <iostream>

int main(){
    
	sp::string_t str = _t( "<name><sub_name>text in sub_name</sub_name></name>" );

	sp::string_reader r(str);

	std::vector<sp::token> vt;

	sp::token t = r.get_next_token();

	while (t.data[0] != _t('\0')) {
		vt.push_back(t);
		t = r.get_next_token();
	}

    return 0;
}