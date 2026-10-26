// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// Testing short string optimization

#include "irrlicht.h"
#include <iostream>

template <typename T>
struct nonTrivialChar
{
	T c;
	nonTrivialChar() {}
	nonTrivialChar(T t) { c = t;}

	operator T() const { return c; }
};

int main()
{
    std::cout << "size_t" << sizeof(size_t) << "\n";

	std::cout << "size cirr" << sizeof(irr::core::stringc) << "\n";	//	24
	std::cout << "size wirr" << sizeof(irr::core::stringw) << "\n";	//	24
	std::cout << "cap cirr" << irr::core::stringc().capacity() << "\n";		//	15
	std::cout << "cap wirr" << irr::core::stringw().capacity() << "\n";		//	 3

	std::cout << "size cstl" << sizeof(std::string) << "\n";			//	32
	std::cout << "size wstl" << sizeof(std::wstring) << "\n";			//	32
	std::cout << "cap cstl" << std::string().capacity() << "\n";		//	15
	std::cout << "cap wstl" << std::wstring().capacity() << "\n";		//	 3

#ifdef IRRLICHT_SDK_VERSION
	typedef nonTrivialChar<unsigned int> char32;
	irr::core::stringc irrString;
	irr::core::stringw irrStringw;
	irr::core::string<char32> irrString32;

	irr::core::stringc a,b;
	irr::core::stringc c;
	irr::core::stringc d = "";
	irr::core::stringc e = "foo";
	irr::core::stringc f = "this is gonna be a bit longer";
	irr::core::stringc g = "123456789012345";
	irr::core::stringc h = "1234567890123456";

	irr::core::stringw wa,wb;
	irr::core::stringw wc;
	irr::core::stringw wd = L"";
	irr::core::stringw we = L"foo";
	irr::core::stringw wf = L"this is gonna be a bit longer";
	irr::core::stringw wg = L"123456789012345";
	irr::core::stringw wh = L"1234567890123456";
	irr::core::stringw wh2 = h;

	a = "";
	a = "foo";
	a = "this is gonna be a bit longer";
	a = "short";
	a.clear();
	a = "this is gonna be a bit longer";

	c = "foo";
	b = "this is gonna be a bit longer";
	c = b;

#endif


	getchar();

	return 0;
}
