#include <iostream>
#include <jsonutils/json_utils.h>

// TEST(JsonUtilsTest, loadFromUrl)
// {
//     auto doc = jsonutils::loadJson("https://books.com/novels");
//
//     ASSERT_TRUE(doc);
//     ASSERT_TRUE(doc->HasMember("source"));
//     auto source = (*doc)["source"].GetString();
//     ASSERT_STREQ("url", source);
// }

// TEST(JsonUtilsTest, loadFromFile)
int main(int argc, char **argv)
{
  auto doc = jsonutils::loadJson("/home/pablo/Documents/test.json");

  if (doc)
  {
    if(doc->HasMember("source"))
    {
      auto source = (*doc)["source"].GetString();
      // ASSERT_STREQ("file", source);
      std::cout << "source: " << source << std::endl;
    }
  }
  return EXIT_SUCCESS;
}
