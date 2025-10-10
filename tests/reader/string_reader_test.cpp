#include "spxml.h"
#include <iostream>

int main() {

	sp::string_t str = _t("<name a  = \" &apos;12&apos;&quot; \"><sub_name>text in sub_name &amp;</sub_name></name>");

	sp::string_reader r(str);

	std::vector<sp::token> vt;

	sp::token t = r.get_next_token();

	while (t.data[0] != _t('\0')) {
		std::wcout << t.data << std::endl;
		vt.push_back(t);
		t = r.get_next_token();

	}

	return 0;
}