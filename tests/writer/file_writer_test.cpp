#include "spxml.h"
#include <iostream>

int main(){
	sp::tag root(_t("test")); 

	for (size_t i = 0; i < 100000; i++ ) {
		sp::tag ch(_t("test_child"));
		for (size_t j = 0; j < 10; j++) {
			sp::string_t n = _t("a") +  std::to_wstring(j);
			ch.attributes().add(n, (int)j);
		}
		ch.type() = sp::tag_type::autoclose_tag;
		root.add_tag(ch);
	}

	sp::file_writer fw(_t("dest_1.xml"));

	if (fw.ready()) {
		fw.write(root);
	}

    return 0;
}