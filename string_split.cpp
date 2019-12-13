// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Testing string::split
// This is about stuff manni63 mentioned in thread http://irrlicht.sourceforge.net/forum/viewtopic.php?f=7&t=51584&p=299634#p299634

#include <irrlicht.h>
#include <iostream>
 
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
 
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif
 
void printResult(const irr::core::array<irr::core::stringc>& arr)
{
    for ( u32 i=0; i<arr.size(); ++i )
    {
        std::cout << i << ")" << "\"" << arr[i].c_str() << "\"" << std::endl;
    }
}

void mergeResult(core::stringc& merged, const irr::core::array<irr::core::stringc>& arr)
{
	for ( u32 i=0; i<arr.size(); ++i )
		merged += arr[i];
}

void splitTest(const core::stringc& str, const char* const delimiter, u32 countDelimiters, bool ignoreEmptyTokens, bool keepSeparators)
{
    std::cout << "test with ";
	if ( ignoreEmptyTokens )
		std::cout <<  "ignEmpty ";
	else	
		std::cout << "keepEmpty ";
	if ( keepSeparators )
		std::cout << "keepSep :";
	else 
		std::cout << "dropSep :";
	std::cout << "\"" << str.c_str() << "\"";
	if ( delimiter )
		std::cout << " delimiter: " << "\"" << delimiter << "\"";
	std::cout << std::endl;
	
	array<stringc> result;
    str.split(result, delimiter, countDelimiters, ignoreEmptyTokens, keepSeparators);
    printResult(result);
	
	if ( keepSeparators )
	{
		core::stringc merged;
		mergeResult(merged, result);
		if ( merged == str )
			std::cout << "successfully merged back\n";
		else
			std::cout << "****failed merging back\n";
	}
	
    std::cout << std::endl;	
}
 
int main()
{
    stringc splitTest1(".see.you..cowboy...bebop..");
    stringc splitTest2(".,,.");
    stringc splitTest3("..");
	stringc splitTest4(".");
	stringc splitTest5("");
	stringc splitTest6("For once a normal string.");
	stringc manni63nums1("1;2;3;4;5");
	stringc manni63nums2("1;2;3;4;");

    splitTest(splitTest1, ".", 1, true, true);
    splitTest(splitTest1, ".", 1, true, false);
    splitTest(splitTest1, ".", 1, false, false);
    splitTest(splitTest1, ".", 1, false, true);
	
    splitTest(splitTest1, "", 0, true, true);
    splitTest(splitTest1, "", 0, true, false);
    splitTest(splitTest1, "", 0, false, false);
    splitTest(splitTest1, "", 0, false, true);	
   
    splitTest(splitTest2, ".,", 2, true, true);
    splitTest(splitTest2, ".,", 2, true, false);
    splitTest(splitTest2, ".,", 2, false, false);
    splitTest(splitTest2, ".,", 2, false, true);
  
    splitTest(splitTest3, ".", 1, true, true);
    splitTest(splitTest3, ".", 1, true, false);
    splitTest(splitTest3, ".", 1, false, false);
    splitTest(splitTest3, ".", 1, false, true);
	
    splitTest(splitTest4, ".", 1, true, true);
    splitTest(splitTest4, ".", 1, true, false);
    splitTest(splitTest4, ".", 1, false, false);
    splitTest(splitTest4, ".", 1, false, true);	
	
    splitTest(splitTest5, ".", 1, true, true);
    splitTest(splitTest5, ".", 1, true, false);
    splitTest(splitTest5, ".", 1, false, false);
    splitTest(splitTest5, ".", 1, false, true);		
	
    splitTest(splitTest6, ". ", 2, true, true);
    splitTest(splitTest6, ". ", 2, true, false);
    splitTest(splitTest6, ". ", 2, false, false);
    splitTest(splitTest6, ". ", 2, false, true);			

	splitTest(manni63nums1, ";", 1, false, false);
	splitTest(manni63nums2, ";", 1, false, false);

	std::cin.get();

    return 0;
}
