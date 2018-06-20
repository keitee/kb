#include <iostream>
#include <vector>

#include "gmock/gmock.h"
#include "tinyxml2.h"
#include "animal.hpp"

using namespace std;
using namespace testing;
using namespace tinyxml2;

// ={=========================================================================
// tinyxml2-
//
// https://github.com/leethomason/tinyxml2
//
// <?xml version="1.0" encoding="UTF-8"?>
// <!-- Feldman Family Circus Animals -->
// <animalList>
//   <animal>
//     <name>Herby</name>
//     <species>elephant</species>
//     <dateOfBirth>1992-04-23</dateOfBirth>
//     <veterinarian name="Dr. Hal Brown" phone="(801)595-9627"/>
//     <trainer name="Bob Fisk" phone="(801)881-2260"/>
//   </animal>
//   <animal>
//     <name>Sheldon</name>
//     <species>parrot</species>
//     <dateOfBirth>1998-09-30</dateOfBirth>
//     <veterinarian name="Dr. Kevin Wilson" phone="(801)466-6498"/>
//     <trainer name="Eli Wendel" phone="(801)929-2506"/>
//   </animal>
//   <animal>
//     <name>Dippy</name>
//     <species>penguin</species>
//     <dateOfBirth>2001-06-08</dateOfBirth>
//     <veterinarian name="Dr. Barbara Swayne" phone="(801)459-7746"/>
//     <trainer name="Ben Waxman" phone="(801)882-3549"/>
//   </animal>
// </animalList>

// // extract the content of an XML element that contains only text
// const char *textValue(XMLElement *e)
// {
//   XMLNode *first = e->FirstChild();
//
//   // the element e has a single child, of type TEXT.
//   if (first !=0 &&
//       first == e->LastChild() &&
//       first->Type() == XMLNode::TEXT)
//   {
//     return first->Value();
//   }
//   else
//     throw runtime_error(string("bad ") + e->Value() + " element");
// }

// construct a Contact from vet or trainer element
Contact nodeToContact(XMLElement *contact)
{
  const char *name;
  const char *phone;

  // the element is `childless` and has "name" and "phone" attribute. use these
  // values to construct a Contact

  if (contact->FirstChild() == 0 &&
      (name = contact->Attribute("name")) &&
      (phone = contact->Attribute("phone")))
  {
    return Contact(name, phone);
  }
  else
    throw runtime_error(string("bad ") + contact->Value() + " element");
}

Animal nodeToAnimal(XMLElement *animal)
{
  // verify that animal corresponds to an "animal" element
  if (strcmp(animal->Value(), "animal") != 0)
  {
    throw runtime_error(string("bad animal: ") + animal->Value());
  }

  Animal result;
  XMLElement *element = animal->FirstChildElement();

  // read name
  if (element && strcmp(element->Value(), "name") == 0)
  {
    result.SetName(element->GetText());
    // result.SetName(textValue(element));
  }
  else
    throw runtime_error(string("no name element/attribute"));

  // read species
  element = element->NextSiblingElement();
  if (element && strcmp(element->Value(), "species") == 0)
  {
    result.SetSpecies(element->GetText());
    // result.SetSpecies(textValue(element));
  }
  else
    throw runtime_error(string("no species element/attribute"));

  // read DOB
  element = element->NextSiblingElement();
  if (element && strcmp(element->Value(), "dateOfBirth") == 0)
  {
    result.SetDOB(element->GetText());
    // result.SetDOB(textValue(element));
  }
  else
    throw runtime_error(string("no DOB element/attribute"));

  // read vet
  element = element->NextSiblingElement();
  if (element && strcmp(element->Value(), "veterinarian") == 0)
  {
    result.SetVet(nodeToContact(element));
  }
  else
    throw runtime_error(string("no vet element/attribute"));

  // read trainer
  element = element->NextSiblingElement();
  if (element && strcmp(element->Value(), "trainer") == 0)
  {
    result.SetTrainer(nodeToContact(element));
  }
  else
    throw runtime_error(string("no vet element/attribute"));

  // check that there are no more children
  element = element->NextSiblingElement();
  if (element != 0)
  {
    throw runtime_error(string("unexpected element: ") + element->Value());
  }

  return result;
}

TEST(TinyXml, X)
{
  try {
    vector<Animal> animal_list;

    // TinyXml 1
    //
    // psrse xml
    //
    // TiXmlDocument doc("animals.xml");
    // if (!doc.LoadFile())
    //   throw runtime_error("bad parse");
    //
    // verify that root is an animal-list
    //
    // TiXmlElement *root = doc.RootElement();
    // if (strcmp(root->Value(), "animalList") != 0)
    // {
    //   throw runtime_error(string("bad root: ") + root->Value());
    // }

    // Load an XML file from disk. Returns XML_SUCCESS (0) on success, or an
    // errorID.

    XMLDocument doc;
    if (doc.LoadFile("animals.xml"))
    {
      doc.PrintError();
      throw runtime_error("loadfile: bad parse");
    }

    // verify that root is an animal-list
    XMLElement *root = doc.RootElement();
    if (strcmp(root->Value(), "animalList") != 0)
    {
      throw runtime_error(string("bad root: ") + root->Value());
    }

    // traverse children of root, populating the list of animals
    // Get the first child element, or optionally the first child element with
    // the specified name.

    for (XMLElement *animal = root->FirstChildElement(); 
        animal;
        animal = animal->NextSiblingElement())
    {
      animal_list.push_back(nodeToAnimal(animal));
    }

    // print the animals
    for (const auto &e : animal_list)
      cout << e << endl;
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

