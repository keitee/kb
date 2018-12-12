#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <chrono>
#include <limits>
#include <thread>
#include <list>
#include <forward_list>
#include <regex>
#include <boost/lexical_cast.hpp>

#include "gmock/gmock.h"

using namespace std;
using namespace std::placeholders;
using namespace testing;


// cxx_ex.cpp: In instantiation of ‘class Vector<short int, 2>’:
// cxx_ex.cpp:39:19:   required from here
// cxx_ex.cpp:31:2: error: static assertion failed: Vector size is too small!
//   static_assert(Size > 3, "Vector size is too small!");
//   ^
//
// CPP code to demonstrate 
// static assertion using static_assert 
#include <iostream> 
using namespace std; 

template <class T, int Size> 
class Vector { 
	// Compile time assertion to check if 
	// the size of the vector is greater than 
	// 3 or not. If any vector is declared whose 
	// size is less than 4, the assertion will fail 
	static_assert(Size > 3, "Vector size is too small!"); 

	T m_values[Size]; 
}; 

TEST(Cpp, Assert)
{ 
	Vector<int, 4> four; // This will work 
  (void) four;

	// Vector<short, 2> two; // This will fail 
} 

// ={=========================================================================

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

