#include "spxml.h"
#include <iostream>

int main(){
   

	sp::file_reader r(_t("test.xml"));

	std::vector<sp::token> vt;

	sp::token t = r.get_next_token();

	while (t.data[0] != _t('\0')) {
		std::wcout << t.data << std::endl;
		vt.push_back(t);
		t = r.get_next_token();
		
	}

    return 0;
}