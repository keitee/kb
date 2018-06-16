#include <iostream>
#include <vector>

#include "gmock/gmock.h"
#include "tinyxml.h"
#include "animal.hpp"

using namespace std;
using namespace testing;


// ={=========================================================================
// tinyxml-

TEST(TinyXml, X)
{
  try {
    vector<Aninal> animal_list;

    // psrse xml
    TiXmlDocument doc("animals.xml");
    if (!doc.LoadFile())
      throw runtime_error("bad parse");

    // verify that root is an animal-list
    TiXmlElement *root = doc.RootElement();
    if (strcmp(root->Value(), "animalList") != 0)
    {
      throw runtime_error(string("bad root: ") + root->Value());
    }
  }
  catch(const exception &e)
  {
    cout << e.what() << endl;
  }
}

// ={=========================================================================
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

