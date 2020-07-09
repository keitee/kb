#include <algorithm>
#include <bitset>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;

namespace algopad
{
  vector<string> grade_1(const vector<int> scores)
  {
    const vector<string> grade_table{"F", "D", "C", "B", "A"};
    vector<string> result;
    string grade{};

    for (auto e : scores)
    {
      // handled here if e < 60 so `band` below never be a minus value
      if (e < 60)
        result.push_back(grade_table[0]);
      else
      {
        grade.clear();

        auto band = (e - 50) / 10;

        grade = grade_table[band];

        if (band > 0)
        {
          int plus_or_minus = e % 10;
          if (plus_or_minus < 3)
            grade += "-";
          else if (plus_or_minus > 7)
            grade += "+";
        }

        result.push_back(grade);
      }
    }

    return result;
  }

  // 2020.07. For F, no need to have +/-
  std::vector<std::string> grade_2(const std::vector<int> scores)
  {
    const std::string table{"FFFFFFDCBA"};
    std::vector<std::string> result{};

    for (const auto e : scores)
    {
      std::string grade{};

      auto band = e / 10;

      grade.append(1, table[band]);

      if (band > 5)
      {
        auto remain = e % 10;

        if (remain > 7)
          grade.append("+");
        else if (remain < 3)
          grade.append("-");
      }

      result.push_back(grade);
    }

    return result;
  }

  // CodeComplete 18.4 Stair-Step Access Tables.
  // use loop rather than using index of table as aboves
  //    60    70    80    90    100
  //    |     |     |     |*****|
  //                 ***** 
  //  F    D     C     B     A

  std::string grade_3(int score)
  {
    const std::vector<int> band{60, 70, 80, 90, 100};
    const std::vector<std::string> grade{"F", "D", "C", "B", "A"};

    auto max_grade = grade.size() - 1;
    size_t grade_index{};
    std::string student_grade{"A"};

    // grade "A" means that not found yet and no need to check grade[max] since
    // if index gets that value, means "A" anyway.

    while ((student_grade == "A") && (grade_index < max_grade))
    {
      if (score < band[grade_index])
        student_grade = grade[grade_index];

      ++grade_index;
    }

    return student_grade;
  }
} // namespace algopad

TEST(Pad, xx)
{
  using namespace algopad;

  {
    // pair<input, expected result>
    const std::vector<std::pair<std::vector<int>, std::vector<std::string>>>
      inputs{{{42, 54, 60, 62, 66, 68, 71, 73, 78, 89, 98},
              {"F", "F", "D-", "D-", "D", "D+", "C-", "C", "C+", "B+", "A+"}}};

    const std::vector<
      std::function<std::vector<std::string>(const std::vector<int>)>>
      imps{grade_1, grade_2};

    for (const auto &f : imps)
    {
      for (const auto &i : inputs)
        EXPECT_THAT(f(i.first), i.second);
    }
  }

  // since grade_3 do not support +/-
  {
    const std::vector<int> inputs{42, 54, 60, 62, 66, 68, 71, 73, 78, 89, 98};
    const std::vector<std::string>
      expected{"F", "F", "D", "D", "D", "D", "C", "C", "C", "B", "A"};
    std::vector<std::string> results{};

    for (const auto &score : inputs)
    {
      results.push_back(grade_3(score));
    }

    EXPECT_THAT(results, expected);
  }
}

// ={=========================================================================
int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
