#include <iostream>
#include <Windows.h>

#include "sparser.h"

#include <cassert>

#include <io.h>
#include <fcntl.h>
#include <locale>
#include <codecvt>

/*

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convertToUtf16;
auto utf16Input = convertToUtf16.from_bytes(input.data());
auto utf16Letter = convertToUtf16.from_bytes(letter.data());



std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convertToUtf8;

*/

namespace conv {

	namespace multibyte {

		std::string convert(const std::wstring& s)
		{
			return convert(s.c_str());
		}
		std::wstring convert(const std::string& s)
		{
			return convert(s.c_str());
		}

		std::string convert(const wchar_t* s)
		{
			const size_t len = wcslen(s);
			std::string result(len, '\0');

#ifdef _MSC_VER
			size_t count = 0;
			const auto size_ = result.size();
			wcstombs_s(&count, &result[0], size_, s, size_);
#else
			std::mbstate_t state = std::mbstate_t();
			std::wcsrtombs(&result[0], &s, result.size(), &state);
#endif

			return result;
		}

		std::wstring convert(const char* s)
		{
			std::mbstate_t state = std::mbstate_t();

			size_t len;
			wchar_t wc[4] = {};
			std::wstring result;
            const char* end = s + strlen(s);
			while ((len = std::mbrtowc(wc, s, end - s, &state)) > 0)
			{
				result += wc; s += len;
				for (auto& ch : wc)  ch = L'\0';
			}
			return result;
		}

	}//namespace multibyte

	namespace utf8 {

		std::string  convert(const std::wstring& s)
		{
			typedef std::codecvt_utf8<wchar_t>
				convert_typeX;
			std::wstring_convert<convert_typeX, wchar_t>
				converterX;
			return converterX.to_bytes(s);
		}
		std::wstring convert(const std::string&  s)
		{
			typedef std::codecvt_utf8<wchar_t>
				convert_typeX;
			std::wstring_convert<convert_typeX, wchar_t>
				converterX;
			return converterX.from_bytes(s);
		}

	}//namespace utf8 

	namespace stdlocal {


		std::wstring convert1(const std::wstring& s, const size_t len, const std::locale& loc = std::locale())
		{
			if (len == 0)
				return std::wstring();
			const std::ctype<wchar_t>& facet =
				std::use_facet< std::ctype<wchar_t> >(loc);
			const wchar_t* last = s.c_str() + len;
			std::wstring result(len, L'\0');
			char * temp = new char[len];
			facet.narrow(s.c_str(), last, '?',  temp);
			const char * last1 = temp + len;
			
			facet.widen(temp, last1, &result[0]);
			return result;
		}

		std::wstring convert(const char* first, const size_t len, const std::locale& loc = std::locale())
		{
			if (len == 0)
				return std::wstring();
			const std::ctype<wchar_t>& facet =
				std::use_facet< std::ctype<wchar_t> >(loc);
			const char* last = first + len;
			std::wstring result(len, L'\0');
			facet.widen(first, last, &result[0]);
			return result;
		}

		std::string convert(
			const wchar_t* first,
			const size_t len,
			const std::locale& loc,
			const char default_char
		)
		{
			if (len == 0)
				return std::string();
			const std::ctype<wchar_t>& facet =
				std::use_facet<std::ctype<wchar_t> >(loc);
			const wchar_t* last = first + len;
			std::string result(len, default_char);
			facet.narrow(first, last, default_char, &result[0]);
			return result;
		}

		std::string convert(const wchar_t* s, const std::locale& loc = std::locale(), const char default_char = '?')
		{
			return convert(s, std::wcslen(s), loc, default_char);
		}
		std::string convert(const std::wstring& s, const std::locale& loc = std::locale(), const char default_char = '?')
		{
			return convert(s.c_str(), s.length(), loc, default_char);
		}
		std::wstring convert(const char* s, const std::locale& loc = std::locale())
		{
            return convert(s, strlen(s), loc);
		}
		std::wstring convert(const std::string& s, const std::locale& loc = std::locale())
		{
			return convert(s.c_str(), s.length(), loc);
		}

	}//namespace stdlocal
}//namespace conv

int main() {
	setlocale(LC_CTYPE, ".UTF-8");
	sp::xml_parser pars;
	sp::parse_result pars_res = pars.load_from_file(_t("test.xml"));

	sp::tag_array arr = pars.root()->select(_t("obj"));

	bool cmp_result = arr[0] == arr[1];


	arr[0].attribute(_t("b")).set(101);
	//std::wstring dest_s = tag->text().to_string();
	//std::string temp_s = conv::stdlocal::convert(dest_s);
	//std::wstring res_s = conv::stdlocal::convert(temp_s, std::locale("rus_rus.1251"));

	pars.load_to_stream(std::wcout);
    sp::write_result write_r= pars.load_to_file(_t("dest2.xml"));
	return 0;
}
