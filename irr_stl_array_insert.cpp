// Code is under the zlib license (same as Irrlicht)
// Written by Michael Zeilfelder
// 
// Figuring out how stl and Irrlicht handle copying elements in their vector/array classes.

#include <irrlicht.h>
#include <vector>
#include <iostream>

static int counter = 0;

struct Foo
{
	Foo()
	{
		x = counter;
		++counter;
		std::cout << x << " construct Foo\n";
	}
	
	Foo(const Foo& foo) : x(foo.x)
	{
		std::cout << x << "copy construct Foo\n";
	}
	
	~Foo()
	{
		std::cout << x << "destruct Foo\n";
	}	
	
	Foo& operator=(const Foo& foo)
	{
		x = foo.x;
		std::cout << x << "Foo::operator=\n";
		return *this;
	}
	
	int x;	
};

void StlInsert()
{
	counter = 0;
	std::cout << "\nStlInsert\n";
	Foo a;
	Foo b;
	
	std::vector<Foo> myFoos;
	myFoos.reserve(2);

	std::cout << "push a\n";
	myFoos.push_back( a );
	std::cout << "insert b\n";
	myFoos.insert( myFoos.begin(), b );
	std::cout << "finished std\n";	
}

void IrrInsert()
{
	counter = 0;
	std::cout << "\nIrrInsert\n";
	Foo a;
	Foo b;
	
	irr::core::array<Foo> myIrrFoos;
	myIrrFoos.reallocate(2);
	
	std::cout << "push a\n";
	myIrrFoos.push_back( a );
	std::cout << "insert b\n";
	myIrrFoos.insert( b, 0 );
	std::cout << "finished irr\n";	
}

int main()
{
	StlInsert();
	IrrInsert();
	
	return 0;
} 
