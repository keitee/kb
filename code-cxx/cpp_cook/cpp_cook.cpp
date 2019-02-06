#include <iostream>
#include <vector>
#include <fstream>

#include "gmock/gmock.h"
#include "tinyxml2.h"
#include "animal.hpp"

#include "pugixml.hpp"
#include "json.hpp"

using namespace std;
using namespace testing;
using namespace tinyxml2;


/*
={=============================================================================

C++ Cookbook
14.1 Parsing a Simple XML Document

1. get tinyxml2 and build it. it will make .a file to link with. 

TINYXML_DIR = $(HOME)/works/tinyxml2

tinyxml2-

https://github.com/leethomason/tinyxml2

2.

<?xml version="1.0" encoding="UTF-8"?>
<!-- Feldman Family Circus Animals -->
<animalList>
  <animal>
    <name>Herby</name>
    <species>elephant</species>
    <dateOfBirth>1992-04-23</dateOfBirth>
    <veterinarian name="Dr. Hal Brown" phone="(801)595-9627"/>
    <trainer name="Bob Fisk" phone="(801)881-2260"/>
  </animal>
  <animal>
    <name>Sheldon</name>
    <species>parrot</species>
    <dateOfBirth>1998-09-30</dateOfBirth>
    <veterinarian name="Dr. Kevin Wilson" phone="(801)466-6498"/>
    <trainer name="Eli Wendel" phone="(801)929-2506"/>
  </animal>
  <animal>
    <name>Dippy</name>
    <species>penguin</species>
    <dateOfBirth>2001-06-08</dateOfBirth>
    <veterinarian name="Dr. Barbara Swayne" phone="(801)459-7746"/>
    <trainer name="Ben Waxman" phone="(801)882-3549"/>
  </animal>
</animalList>

// extract the content of an XML element that contains only text
const char *textValue(XMLElement *e)
{
  XMLNode *first = e->FirstChild();

  // the element e has a single child, of type TEXT.
  if (first !=0 &&
      first == e->LastChild() &&
      first->Type() == XMLNode::TEXT)
  {
    return first->Value();
  }
  else
    throw runtime_error(string("bad ") + e->Value() + " element");
}
 
*/


// construct a Contact from vet or trainer element which has childrens

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

// fill Animal structure from xml node

Animal nodeToAnimal(XMLElement *animal)
{
  // verify that animal corresponds to an "animal" element
  if (strcmp(animal->Value(), "animal") != 0)
  {
    throw runtime_error(string("bad animal: ") + animal->Value());
  }

  // user class that comres from animal.hpp

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


// [ RUN      ] CCook.XML_TinyXml
// Animal {
//  name    = Herby;
//  species = elephant;
//  dob     = 1992-04-23;
//  vet     = Dr. Hal Brown (801)595-9627;
//  trainer = Bob Fisk (801)881-2260;
// }
// Animal {
//  name    = Sheldon;
//  species = parrot;
//  dob     = 1998-09-30;
//  vet     = Dr. Kevin Wilson (801)466-6498;
//  trainer = Eli Wendel (801)929-2506;
// }
// Animal {
//  name    = Dippy;
//  species = penguin;
//  dob     = 2001-06-08;
//  vet     = Dr. Barbara Swayne (801)459-7746;
//  trainer = Ben Waxman (801)882-3549;
// }
// [       OK ] CCook.XML_TinyXml (8 ms)

TEST(CCook, XML_TinyXml)
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


/*
={=============================================================================
The Morden C++ Challenge, Data Serialization

73. Serializing and deserializing data to/from XML

Write a program that can serialize a list of movies to an XML file, and
deserialize an XML file with a list of movies. Each movie has a numerical
identifier, title, release year, length in minutes, a list of directors, a list
of writers, and a list of casting roles with actor name and character name. Such
an XML may look like the following:

<?xml version="1.0"?>
<movies>
  <movie id="9871" title="Forrest Gump" year="1994" length="202">
    <cast>
      <role star="Tom Hanks" name="Forrest Gump" />
      <role star="Sally Field" name="Mrs. Gump" />
      <role star="Robin Wright" name="Jenny Curran" />
      <role star="Mykelti Williamson" name="Bubba Blue" />
    </cast>
    <directors>
      <director name="Robert Zemeckis" />
    </directors>
    <writers>
      <writer name="Winston Groom" />
      <writer name="Eric Roth" />
    </writers>
  </movie>
  <!-- more movie elements -->
</movies>


The C++ standard library does not have any support for XML, but there are
multiple open source, cross-platform libraries that you can use. Some libraries
are lightweight, supporting a set of basic XML features, while others are more
complex and rich in functionality. It is up to you to decide which is most
suitable for a particular project.

The list of libraries you may want to consider should include Xerces-C++,
libxml++, tinyxml or tinyxml2, pugixml, gSOAP, and RapidXml. 

For solving this particular task I will choose pugixml. This is a
cross-platform, lightweight library, with a fast, although non-validating, XML
parser. It has a DOM-like interface with rich traversal/modification
capabilities, with support for Unicode and XPath 1.0. On the limitations of the
library, it should be mentioned that it lacks support for schema validation. The
pugixml library is available at https://pugixml.org/.

1. install

kyoupark@kit-debian64:~/works$ git clone https://github.com/zeux/pugixml

build it. add search for a header in makefile and add pugixml.cpp.o in the link
line since this is sinele file library.


<?xml version="1.0"?>
<mesh name="mesh_root">
        <!-- here is a mesh node -->
        some text
        <![CDATA[someothertext]]>
        some more text
        <node attr1="value1" attr2="value2" />
        <node attr1="value2">
                <innernode/>
        </node>
</mesh>
<?include somedata?>

[ RUN      ] CCook.XML_PugiXml
Load result: No error, mesh name: mesh_root
[       OK ] CCook.XML_PugiXml (0 ms)

*/

TEST(CCook, XML_PugiXml_Test)
{
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_file("tree.xml");

  std::cout << "Load result: " << result.description() 
    << ", mesh name: " << doc.child("mesh").attribute("name").value() 
    << std::endl;
}

TEST(CCook, XML_PugiXml_Animal)
{
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_file("animals.xml");
  if (result)
  {
    auto root = doc.child("animalList");
    for (auto animal = root.child("animal");
        animal;
        animal = animal.next_sibling("animal"))
    {
      std::cout << "animal name: " << animal.child("name").child_value()
        << std::endl;
    } // for end
  }
}


namespace U73_Text
{
  // To represent the movies, as described in the problem, we shall use the
  // following structures:

  struct casting_role
  {
    std::string actor;
    std::string role;
  };

  struct movie
  {
    unsigned int              id;
    std::string               title;
    unsigned int              year;
    unsigned int              length;
    std::vector<casting_role> cast;
    std::vector<std::string>  directors;
    std::vector<std::string>  writers;
  };

  using movie_list = std::vector<movie>;

  // To create an XML document you must use the pugi::xml_document class. After
  // constructing the DOM tree you can save it to a file by calling save_file().
  // Nodes can be added by calling append_child(), and attributes with
  // append_attribute(). The following method serializes a list of movies in the
  // requested format:

  void serialize(movie_list const& movies, std::string file_path)
  {
    pugi::xml_document doc;
    auto root = doc.append_child("movies");

    for (auto const& m : movies)
    {
      auto movie_node = root.append_child("movie");

      movie_node.append_attribute("id").set_value(m.id);
      movie_node.append_attribute("title").set_value(m.title.c_str());
      movie_node.append_attribute("year").set_value(m.year);
      movie_node.append_attribute("length").set_value(m.length);

      // cast
      auto cast_node = movie_node.append_child("cast");
      for (auto const& c : m.cast)
      {
        auto node = cast_node.append_child("role");
        node.append_attribute("star").set_value(c.actor.c_str());
        node.append_attribute("name").set_value(c.role.c_str());
      }

      // directors
      auto directors_node = movie_node.append_child("directors");
      for (auto const& d : m.directors)
      {
        directors_node.append_child("director")
          .append_attribute("name")
          .set_value(d.c_str());
      }

      // writers
      auto writers_node = movie_node.append_child("writers");
      for (auto const& w : m.writers)
      {
        writers_node.append_child("writer")
          .append_attribute("name")
          .set_value(w.c_str());
      }
    } // for end

    // cout << "saving result: " << doc.save_file(file_path.c_str())
    //   << endl;

    doc.save_file(file_path.c_str());
  }


  // For the opposite operation, you can load the content of the XML file into a
  // pugi::xml_document by calling its load_file() method. You can access nodes
  // by calling methods such as child() and next_sibling(), and attributes by
  // calling attribute(). The deserialize() method, shown as follows, reads the
  // DOM tree and constructs a list of movies:

  movie_list deserialize(std::string filepath)
  {
    pugi::xml_document doc;
    movie_list movies;

    auto result = doc.load_file(filepath.c_str());
    if (result)
    {
      auto root = doc.child("movies");
      for (auto movie_node = root.child("movie");
          movie_node;
          movie_node = movie_node.next_sibling("movie"))
      {
        movie m;
        m.id = movie_node.attribute("id").as_uint();
        m.title = movie_node.attribute("title").as_string();
        m.year = movie_node.attribute("year").as_uint();
        m.length = movie_node.attribute("length").as_uint();

        for (auto role_node :       
            movie_node.child("cast").children("role"))
        {
          m.cast.push_back(casting_role{
              role_node.attribute("star").as_string(),
              role_node.attribute("name").as_string() });
        }

        for (auto director_node : 
            movie_node.child("directors").children("director"))
        {
          m.directors.push_back(
              director_node.attribute("name").as_string());
        }

        for (auto writer_node : 
            movie_node.child("writers").children("writer"))
        {
          m.writers.push_back(
              writer_node.attribute("name").as_string());
        }

        movies.push_back(m);
      }
    }

    return movies;
  }
} // namespace


/* 
 
movies.xml

<?xml version="1.0"?>
<movies>
        <movie id="11001" title="The Matrix" year="1999" length="196">
                <cast>
                        <role star="Keanu Reeves" name="Neo" />
                        <role star="Laurence Fishburne" name="Morpheus" />
                        <role star="Carrie-Anne Moss" name="Trinity" />
                        <role star="Hugo Weaving" name="Agent Smith" />
                </cast>
                <directors>
                        <director name="Lana Wachowski" />
                        <director name="Lilly Wachowski" />
                </directors>
                <writers>
                        <writer name="Lana Wachowski" />
                        <writer name="Lilly Wachowski" />
                </writers>
        </movie>
        <movie id="9871" title="Forrest Gump" year="1994" length="202">
                <cast>
                        <role star="Tom Hanks" name="Forrest Gump" />
                        <role star="Sally Field" name="Mrs. Gump" />
                        <role star="Robin Wright" name="Jenny Curran" />
                        <role star="Mykelti Williamson" name="Bubba Blue" />
                </cast>
                <directors>
                        <director name="Robert Zemeckis" />
                </directors>
                <writers>
                        <writer name="Winston Groom" />
                        <writer name="Eric Roth" />
                </writers>
        </movie>
</movies>

*/

TEST(CCook, U73_Text_Serialize)
{
  using namespace U73_Text;

  movie_list movies
  {
    {
      11001, "The Matrix",1999, 196,
        { {"Keanu Reeves", "Neo"},
          {"Laurence Fishburne", "Morpheus"},
          {"Carrie-Anne Moss", "Trinity"}, 
          {"Hugo Weaving", "Agent Smith"} },
        {"Lana Wachowski", "Lilly Wachowski"},
        {"Lana Wachowski", "Lilly Wachowski"},
    },
      {
        9871, "Forrest Gump", 1994, 202,
        { {"Tom Hanks", "Forrest Gump"},
          {"Sally Field", "Mrs. Gump"},
          {"Robin Wright","Jenny Curran"},
          {"Mykelti Williamson","Bubba Blue"} },
        {"Robert Zemeckis"},
        {"Winston Groom", "Eric Roth"},
      }
  };

  serialize(movies, "movies.xml");
}

TEST(CCook, U73_Text_DeSerialize)
{
  using namespace U73_Text;

  auto result = deserialize("movies.xml");

  EXPECT_THAT(result.size(), 2);
  EXPECT_THAT(result[0].title, "The Matrix");
  EXPECT_THAT(result[1].title, "Forrest Gump");
}


/*
={=============================================================================
The Morden C++ Challenge, Data Serialization

74. Selecting data from XML using XPath

Consider an XML file with a list of movies as described for the previous
problem. Write a program that can select and print the following:

The title of all the movies released after a given year

The name of the last actor in the casting list for each movie in the file


Navigation through the elements and attributes of an XML file can be done with
XPath. XPath uses XPath expressions for that purpose and there is a long list of
built-in functions for that. pugixml supports XPath expressions and you can use
the select_nodes() method from the xml_document class for that purpose. Note
that, if an error occurs during the XPath selection, an xpath_exception is
thrown. The following XPath expressions can be used for selecting the nodes
according to the problem requirements:

For all movies released after a given year (in this example that year is 1995):
/movies/movie[@year>1995] 

For the last casting role of each movie:
/movies/movie/cast/role[last()] 

The following program loads an XML document from a string buffer and then
performs node selection using the XPath expressions listed earlier. 


[ RUN      ] U74.XPath
The Matrix
Hugo Weaving
Mykelti Williamson
[       OK ] U74.XPath (0 ms)

*/

TEST(CCook, U74_Text_XPath)
{
  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_file("movies.xml");
  if (result)
  {
    try
    {
      auto titles = doc.select_nodes("/movies/movie[@year>1995]");
      for (auto t : titles)
      {
        std::cout << t.node().attribute("title").as_string() << std::endl;
      }
    }
    catch (pugi::xpath_exception const& e)
    {
      std::cout << e.result().description() << std::endl;
    }

    try
    {
      auto cast = doc.select_nodes("/movies/movie/cast/role[last()]");
      for (auto c : cast)
      {
        std::cout << c.node().attribute("star").as_string() << std::endl;
      }
    }
    catch (pugi::xpath_exception const& e)
    {
      std::cout << e.result().description() << std::endl;
    }
  }
}


/*
={=============================================================================
The Morden C++ Challenge, Data Serialization

75. Serializing data to JSON

Write a program that can serialize a list of movies, as defined for the previous
problems, to a JSON file. Each movie has a numerical identifier, title, release
year, length in minutes, a list of directors, a list of writers, and a list of
casting roles with actor name and character name. The following is an example of
the expected JSON format:

{
  "movies": [{
    "id": 9871,
    "title": "Forrest Gump",
    "year": 1994,
    "length": 202,
    "cast": [{
        "star": "Tom Hanks",
        "name": "Forrest Gump"
      },
      {
        "star": "Sally Field",
        "name": "Mrs. Gump"
      },
      {
        "star": "Robin Wright",
        "name": "Jenny Curran"
      },
      {
        "star": "Mykelti Williamson",
        "name": "Bubba Blue"
      }
    ],
    "directors": ["Robert Zemeckis"],
    "writers": ["Winston Groom", "Eric Roth"]
  }]
}

As with XML, no standard support for JSON exists. However, there are a large
number of cross-platform libraries for this purpose. At the time of writing, the
nativejson-benchmark project, available at
https://github.com/miloyip/nativejson-benchmark, lists more than 40 libraries.
This project is a benchmark that evaluates the conformance and performance
(speed, memory, and code size) of open source C/C++ libraries with JSON
parsing/generation capabilities. This makes it perhaps a bit hard to choose the
right library, although top contenders may include RapidJSON, NLohmann,
taocpp/json, Configuru, json_spirit, jsoncpp. 

For solving this task we will use here the nlohmann/json library. It is a
cross-platform, header only library for C++11, with an intuitive syntax and good
documentation. This library is available at https://github.com/nlohmann/json.

We will use the same data structures to represent movies that we used for the
problem Serializing and deserializing data to/from XML. The nlohmann library
uses nlohmann::json as its main data type for representing JSON objects.
Although you can create JSON values with a more explicit syntax, there are also
implicit conversions to and from scalar types and standard containers. In
addition, you can also enable this implicit conversion to and from your custom
types `by providing a to_json() and from_json() method in the namespace of the
type to be converted.` There are some requirements for these functions that you
can read about in the documentation.

In the following code, this is the chosen approach. Since the movie and
casting_role types were defined in the global namespace, the to_json() overloads
that serialize these types are also defined in the global namespace. On the
other hand, the type movie_list is actually a type alias for std::vector<movie>
and can be serialized and deserialized directly because, as mentioned earlier,
the library supports implicit conversion to and from standard containers:

*/

namespace U75_Text
{
  // To represent the movies, as described in the problem, we shall use the
  // following structures:

  struct casting_role
  {
    std::string actor;
    std::string role;
  };

  struct movie
  {
    unsigned int              id;
    std::string               title;
    unsigned int              year;
    unsigned int              length;
    std::vector<casting_role> cast;
    std::vector<std::string>  directors;
    std::vector<std::string>  writers;
  };

  using movie_list = std::vector<movie>;

  movie_list movies
  {
    {
      11001, "The Matrix",1999, 196,
        { {"Keanu Reeves", "Neo"},
          {"Laurence Fishburne", "Morpheus"},
          {"Carrie-Anne Moss", "Trinity"}, 
          {"Hugo Weaving", "Agent Smith"} },
        {"Lana Wachowski", "Lilly Wachowski"},
        {"Lana Wachowski", "Lilly Wachowski"},
    },
      {
        9871, "Forrest Gump", 1994, 202,
        { {"Tom Hanks", "Forrest Gump"},
          {"Sally Field", "Mrs. Gump"},
          {"Robin Wright","Jenny Curran"},
          {"Mykelti Williamson","Bubba Blue"} },
        {"Robert Zemeckis"},
        {"Winston Groom", "Eric Roth"},
      }
  };

  using json = nlohmann::json;

  void to_json(json& j, casting_role const& c)
  {
    j = json{
        {"star", c.actor},
        {"name", c.role}
    };

  }

  void to_json(json& j, movie const& m)
  {
    j = json::object({
        {"id", m.id},
        {"title", m.title},
        {"year", m.year},
        {"length", m.length},
        {"cast", m.cast},
        {"directors", m.directors},
        {"writers", m.writers}
        });
  }

  void serialize(movie_list const& movies, std::string file_path)
  {
    json jdata{{"movies", movies}};

    std::ofstream of(file_path);
    if (of.is_open())
    {
      of << std::setw(2) << jdata << std::endl;
    }
  }
} // namespace

/*

movies.json

{
  "movies": [
    {
      "cast": [
        {
          "name": "Neo",
          "star": "Keanu Reeves"
        },
        {
          "name": "Morpheus",
          "star": "Laurence Fishburne"
        },
        {
          "name": "Trinity",
          "star": "Carrie-Anne Moss"
        },
        {
          "name": "Agent Smith",
          "star": "Hugo Weaving"
        }
      ],
      "directors": [
        "Lana Wachowski",
        "Lilly Wachowski"
      ],
      "id": 11001,
      "length": 196,
      "title": "The Matrix",
      "writers": [
        "Lana Wachowski",
        "Lilly Wachowski"
      ],
      "year": 1999
    },
    {
      "cast": [
        {
          "name": "Forrest Gump",
          "star": "Tom Hanks"
        },
        {
          "name": "Mrs. Gump",
          "star": "Sally Field"
        },
        {
          "name": "Jenny Curran",
          "star": "Robin Wright"
        },
        {
          "name": "Bubba Blue",
          "star": "Mykelti Williamson"
        }
      ],
      "directors": [
        "Robert Zemeckis"
      ],
      "id": 9871,
      "length": 202,
      "title": "Forrest Gump",
      "writers": [
        "Winston Groom",
        "Eric Roth"
      ],
      "year": 1994
    }
  ]
}

*/

TEST(CCook, U75_Text_Serialize)
{
  using namespace U75_Text;

  serialize(movies, "movies.json");
}


/*
={=============================================================================
The Morden C++ Challenge, Data Serialization

76. Deserializing data from JSON

Consider a JSON file with a list of movies as shown in the previous problem.
Write a program that can deserialize its content.

*/


/*
={=============================================================================
The Morden C++ Challenge, Data Serialization

77. Printing a list of movies to a PDF

Write a program that can print to a PDF file a list of movies in a tabular form,
with the following requirements:

There must be a heading to the list with the content List of movies. This must
appear only on the first page of the document.

For each movie, it should display the title, the release year, and the length.
The title, followed by the release year in parentheses, must be left-aligned.
The length, in hours and minutes (for example, 2:12), must be right-aligned.
There must be a line above and below the movie listing on each page.

*/

/*
={=============================================================================
The Morden C++ Challenge, Data Serialization

78. Creating a PDF from a collection of images

Write a program that can create a PDF document that contains images from a
user-specified directory. The images must be displayed one after another. If an
image does not fit on the remainder of a page, it must be placed on the next
page.

*/


/*
={=============================================================================
The Morden C++ Challenge, Archives, Images, and Databases

79. Finding files in a ZIP archive

Write a program that can search for and print all the files in a ZIP archive
whose name matches a user-provided regular expression (for instance, use
^.*\.jpg$ to find all files with the extension .jpg).


80. Compressing and decompressing files to/from a ZIP archive

Write a program that can do the following:

Compress either a file or the contents of a user-specified directory,
recursively, to a ZIP archive Decompress the contents of a ZIP archive to a
user-specified destination directory


81. Compressing and decompressing files to/from a ZIP archive with a password

Write a program that can do the following:

Compress either a file or the contents of a user-specified directory,
recursively, to a password-protected ZIP archive 

Decompress the content of a password-protected ZIP archive to a user-specified
destination directory


82. Creating a PNG that represents a national flag

Write a program that generates a PNG file that represents the national flag of
Romania, shown here. The size of the image in pixels, as well as the path to the
destination file, should be provided by the user:


83. Creating verification text PNG images

Write a program that can create Captcha-like PNG images for verifying human
users to a system. Such an image should have:

A gradient-colored background

A series of random letters displayed at different angles both to the right and
left

Several random lines of different colors across the image (on top of the text)


84. EAN-13 barcode generator

Write a program that can generate a PNG image with an EAN-13 barcode for any
international article number in version 13 of the standard. For simplicity, the
image should only contain the barcode and can skip the EAN-13 number printed
under the barcode. Here is an example of the expected output for the number
5901234123457:


85. Reading movies from an SQLite database

Write a program that reads movies from an SQLite database and displays them on
the console. Each movie must have a numerical identifier, a title, release year,
length in minutes, list of directors, list of writers, and a cast that includes
both the actor and the character names. The following is a diagram of the
database that should be used for this purpose:

*/

// ={=========================================================================
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

