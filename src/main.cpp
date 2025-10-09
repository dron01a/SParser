#include "spxml.h"
#include <iostream>

int main(){
    
	sp::string_t str = _t( "</name > " );

	sp::string_reader r(str);

	sp::token t = r.get_next_token();

    return 0;
}