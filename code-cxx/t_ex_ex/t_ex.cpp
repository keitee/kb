#include "gmock/gmock.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <string>
#include <bitset>
#include <chrono>
#include <thread>
#include <boost/algorithm/string.hpp>


// g++ -g -std=c++0x t_override.cpp

using namespace std;
using testing::ElementsAre;
using testing::Eq;
using testing::StrEq;
using testing::FloatEq;

// ={=========================================================================

typedef long long lint;

int g_points[1000];
int g_weights[1000];

// the order of arg do not matter since it's abs. for example, abs(s-e) or abs(e-s)
#define ABS_DISTANCE(s,e) (abs(g_points[e] - g_points[s]))

lint sum_range(int start, int finish, lint start_distance = 0);
lint sum_range_with_start(int start, int begin, int end, lint start_distance =0);

// get weighted sum in (begin, end] from start offset. note that do not
// include start in its sum.
//
// * loop condition and distance math
// turns out have to have loop exit in the middle of loop as the orignial has.
// Other attemtps like these cause probblems:
//
// for (int i = begin + direction; i != end ; i += direction)
// cause to miss out the last in add so wrong result.
//
// for (int i = begin + direction; i != (end+1); i += direction)
// cause to seg fault when do the left direction, direction is -1. 
// 
// the proper is:
//
// for (int i = begin + direction; ; i += direction)
// {
//   start_distance += ABS_DISTANCE(i-direction, i);
//
//   if (i == end)
//    return weight_sum;
// }
//
// Why? In order to support both direction and not inclding the start, use
// clever way to do match. In particuler, the way `direction` is used.
//
// For example:
//
// sum_range(0,5)
//  not include start, 0.
//  direction = 1, i = 1, ABS(0,1), wi[1] 
//  direction = 1, i = 2, ABS(1,2), wi[2] 
//  direction = 1, i = 3, ABS(3,2), wi[3] 
//  direction = 1, i = 4, ABS(4,3), wi[4] 
//  direction = 1, i = 5, ABS(5,4), wi[5] 
//  exit
//
// sum_range(5,0)
//  not include start, 5.
//  direction = -1, i = 4, ABS(5,4), wi[4] 
//  direction = -1, i = 3, ABS(4,3), wi[3] 
//  direction = -1, i = 2, ABS(3,2), wi[2] 
//  direction = -1, i = 1, ABS(2,1), wi[1] 
//  direction = -1, i = 0, ABS(1,0), wi[0] 
//  exit
//
//
// running:
//
//  i   0    1    2     3     4     5
// wi   1    2    10    3     5     1
// -----+----+-----+-----+-----+-----+---
// xi   1    6    12    13    14    24
//      s--------------------------->   244
//      -----s
//      ---------------------------->   329
//      ----------s
//      ---------------------------->   246
//      ----------------s
//      ---------------------------->   196
//      ----------------------s
//      ---------------------------->   88
//      ...
//
// 86 is made of:
//
//                s-----*-----*
//      *----*----
//                            ------*
//
// where * denotes points used in sum.
//
//
// # DURATION     TID     FUNCTION
//             [   529] | search(0, 5) {
// 
//                          /* starts from 0 to 5 */
//                          /* sum(start, begin, end, dis); */
//    0.665 us [   529] |   sum(0, 5, 0, 5) = 244;
// 
//                          /* starts from 1 */
//                          /* sum(begin, end, dis); */
//             [   529] |   sum(1, 0, 5, 0) {
//    0.443 us [   529] |     sum(1, 0, 0, 0) = 5;
//    1.035 us [   529] |     sum(1, 5, 10, 5) = 329;
//    1.219 ms [   529] |   } = 334; /* sum */
// 
//                          /* starts from 2 */
//             [   529] |   sum(2, 0, 5, 0) {
//    0.514 us [   529] |     sum(2, 0, 0, 0) = 23;
//    0.499 us [   529] |     sum(2, 5, 22, 5) = 223;
//    1.225 ms [   529] |   } = 246; /* sum */
// 
//                          /* starts from 3 */
//             [   529] |   sum(3, 0, 5, 0) {
//    0.554 us [   529] |     sum(3, 0, 0, 0) = 36;
//    0.566 us [   529] |     sum(3, 5, 24, 5) = 160;
//    1.078 ms [   529] |   } = 196; /* sum */
// 
//                          /* update minimum sum and save start point */
// 
//                          /* starts from 4 */
//             [   529] |   sum(4, 0, 5, 0) {
//    0.493 us [   529] |     sum(4, 0, 0, 0) = 52;
//    0.431 us [   529] |     sum(4, 5, 26, 5) = 36;
//  190.644 us [   529] |   } = 88; /* sum */
// 
//                          /* update minimum sum and save start point */
// 
//                          /* search [0,5] ends and there are updates so search again [4,0] */
// 
//             [   529] |   search(4, 0) {
// 
//                          /* starts from 4 to 0 */
//    0.242 us [   529] |     sum(4, 0, 0, 0) = 52;
// 
//             [   529] |     sum(3, 4, 0, 0) {
//    0.202 us [   529] |       sum(3, 4, 0, 4) = 5;
//    0.394 us [   529] |       sum(3, 0, 2, 0) = 62;
//  280.063 us [   529] |     } = 67; /* sum */
// 
//             [   529] |     sum(2, 4, 0, 0) {
//    0.301 us [   529] |       sum(2, 4, 0, 4) = 13;
//    0.220 us [   529] |       sum(2, 0, 4, 0) = 35;
//   40.841 us [   529] |     } = 48; /* sum */
// 
//                          /* update minimum sum and save start point */
// 
//             [   529] |     sum(1, 4, 0, 0) {
//    0.217 us [   529] |       sum(1, 4, 0, 4) = 121;
//    0.211 us [   529] |       sum(1, 0, 16, 0) = 21;
//   37.859 us [   529] |     } = 142; /* sum */
// 
//                          /* search [4,0] ends and there are updates so search again [2,4] */
// 
//             [   529] |     search(2, 4) {
//    0.224 us [   529] |       sum(2, 4, 0, 4) = 13;
// 
//             [   529] |       sum(3, 2, 4, 0) {
//    3.156 us [   529] |         sum(3, 2, 0, 2) = 10;
//    0.221 us [   529] |         sum(3, 4, 2, 4) = 15;
//   44.632 us [   529] |       } = 25; /* sum */
// 
//    0.208 us [   529] |       sum(2, 4, 0, 4) = 13;
// 
//                              /* search [2,4] ends
//                                 add time[2, end 4], time +=2, time 2
//                                 <13, 2> = sum(ind:2, e:4, time:0)
//                              */
//
//                              /* means that start = 2 for minimul sum ! */ 
//
//   71.569 us [   529] |     } = 13; /* search */
// 
//                            /* still in search [4,0]
//                               ind was 2, add time[2, start 4], time +=2, time 4 
//                             */
// 
//    0.428 us [   529] |     sum(2, 0, 4, 0) = 35;
// 
//                            /* search [4,0] ends
//                               add time[2, start 0], time +=11, time 15
//                               <48, 15> = sum(ind:2, e:0, time:4)
//                            */
// 
//  955.074 us [   529] |   } = 48; /* search */
// 
//                            /* still in search [0,5]
//                               ind was 4, add time[4, start 0], time +=13, time 28 
//                             */
// 
//    0.235 us [   529] |   sum(4, 5, 28, 5) = 38;
// 
//                            /* search [0,5] ends
//                               add time[4, end 5], time +=10, time 38
//                               <86, 38> sum(ind:4, e:5, time:28)
//                            */
// 
//    6.085 ms [   529] | } = 86; /* search */
// 
//             [   529] | search(5, 0) {
//    0.239 us [   529] |   sum(5, 0, 0, 0) = 262;
//             [   529] |   sum(4, 5, 0, 0) {
//    0.206 us [   529] |     sum(4, 5, 0, 5) = 10;
//    0.223 us [   529] |     sum(4, 0, 20, 0) = 372;
//   49.363 us [   529] |   } = 382; /* sum */
//             [   529] |   sum(3, 5, 0, 0) {
//    0.300 us [   529] |     sum(3, 5, 0, 5) = 16;
//    0.295 us [   529] |     sum(3, 0, 22, 0) = 322;
//   50.948 us [   529] |   } = 338; /* sum */
//             [   529] |   sum(2, 5, 0, 0) {
//    0.287 us [   529] |     sum(2, 5, 0, 5) = 25;
//    0.305 us [   529] |     sum(2, 0, 24, 0) = 95;
//   52.524 us [   529] |   } = 120; /* sum */
//             [   529] |   sum(1, 5, 0, 0) {
//    0.254 us [   529] |     sum(1, 5, 0, 5) = 139;
//    0.249 us [   529] |     sum(1, 0, 36, 0) = 41;
//   42.398 us [   529] |   } = 180; /* sum */
//             [   529] |   search(2, 5) {
//    0.223 us [   529] |     sum(2, 5, 0, 5) = 25;
//             [   529] |     sum(3, 2, 5, 0) {
//    0.202 us [   529] |       sum(3, 2, 0, 2) = 10;
//    0.211 us [   529] |       sum(3, 5, 2, 5) = 28;
//   37.587 us [   529] |     } = 38; /* sum */
//             [   529] |     sum(4, 2, 5, 0) {
//    0.279 us [   529] |       sum(4, 2, 0, 2) = 23;
//    0.270 us [   529] |       sum(4, 5, 4, 5) = 14;
//   56.792 us [   529] |     } = 37; /* sum */
//    0.363 us [   529] |     sum(2, 5, 0, 5) = 25;
//  657.999 us [   529] |   } = 25; /* search */
//    0.286 us [   529] |   sum(2, 0, 24, 0) = 95;
//  954.739 us [   529] | } = 120; /* search */
//
//  86 = min(86, 120)
//
// Things to note:
//
// 1. Always calculate distance from the start point.
// 2. Do not count point which are already counted.
// 3. The start point do not included in the sum.


lint sum_range(int begin, int end, lint start_distance)
{
  if (begin == end)
    return 0;

  // forward or backward
  int direction = ((end > begin) ? 1: -1);

  lint weight_sum = 0;

  for (int i = begin + direction; ; i += direction)
  {
    start_distance += ABS_DISTANCE(i-direction, i);
    weight_sum += start_distance*(lint)g_weights[i];

    if (i == end)
      return weight_sum;
  }

  return weight_sum;
}


lint sum_range_with_start(int start, int begin, int end, lint start_distance)
{
	lint d1 = sum_range(start, begin, start_distance);

	// why 2? since used twice to make a turn
	lint d3 = sum_range(start, end, start_distance + 2*ABS_DISTANCE(begin, start));

	return d1 + d3;
}

// search(start index, end index)
pair<lint, lint> search(int begin, int end)
{
  lint current_sum = sum_range(begin, end, 0);
  int current_start = begin;

  if(begin == end)
    return make_pair(0, 0);

  int direction = ((end > begin)? 1:-1);

  lint calculated_sum = 0;
  lint calculated_distance = 0;

  for (int start = begin + direction; start != end; start += direction)
  {
    lint range_sum  = sum_range_with_start(start, begin, end);

    // found sum which is less than sum of [begin, end] and save start point.
    // so `ind` is saved start which shows the minimum sum so far.
    // 
    // redundant check
    // if (range_sum < current_sum && abs(start - begin) > abs(current_start - begin))

    if (range_sum < current_sum)
    {

      current_sum = range_sum;
      current_start = start;
    }
  }

  // when found start point which has lesser sum since current_start do not
  // change when there is no lesser sum found in the range.

  if (current_start != begin)
  {
    pair<lint, lint> result = search(current_start, begin);	
    calculated_sum += result.first;
    calculated_distance += result.second + ABS_DISTANCE(current_start, begin);		
  }

  calculated_sum += sum_range(current_start, end, calculated_distance);
  calculated_distance += ABS_DISTANCE(current_start, end);

  return make_pair(calculated_sum, calculated_distance);
}


TEST(XXX, XXX)
{
  {
    vector<int> dcall{1, 6, 12, 13, 14, 24};
    vector<int> wcall{1, 2, 10, 3, 5, 1};

    for (size_t i = 0; i < dcall.size(); ++i)
    {
      g_points[i] = dcall[i];
      g_weights[i] = wcall[i];
    }

    int N = dcall.size();

    pair<lint, lint> res1 = search(0, N - 1);
    pair<lint, lint> res2 = search(N - 1, 0);

    EXPECT_THAT(min(res1.first, res2.first), 86);
  }

  {
    vector<int> dcall{5, 34, 45, 49, 51, 52, 53, 56, 63, 81, 84, 88, 93, 99, 106};
    vector<int> wcall{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    for (size_t i = 0; i < dcall.size(); ++i)
    {
      g_points[i] = dcall[i];
      g_weights[i] = wcall[i];
    }

    int N = dcall.size();

    pair<lint, lint> res1 = search(0, N - 1);
    pair<lint, lint> res2 = search(N - 1, 0);

    EXPECT_THAT(min(res1.first, res2.first), 630);
  }
}


// ={=========================================================================

template <typename T>
void PRINT_ELEMENTS( T& coll, const string optstr="" )
{
    size_t count{};
    cout << optstr;

    for( const auto &elem : coll )
    {
        cout << "(" << elem.first << ", " << elem.second << ") ";
        ++count;
    }

    cout << "(" << count << ")" << endl;
}

using Point = std::pair<int, int>;

// 1. input and point(row, col) which is the same as point(y, x) but 
// not point(x, y). This has to do with array access.
//
// 2. If it's valid point which are not checked against to traveled points, call
// find_path for every possible move from a point and then this move is cheked
// against to the traveled points. So supporting diagonal move needs more
// recursive calls. Therefore, it use the traveled path to exit the call for
// points that it already traveled; to move forward, to move back when see the
// dead end, and naturally to prevent circular path.
//
// See again that calls find_path() for every possible move.
//
// 3. To support range from (1,1) to (N,N), used padding to the input data:
//
// {2,2,2,2,2,2},
// {2,0,0,0,0,0}, 
// {2,1,1,1,1,0},
// {2,0,0,0,0,0},
// {2,0,1,1,1,1},
// {2,2,0,0,0,0}
//
// the result of run:
//
// (1, 2) (1, 3) (1, 4) (1, 5) (2, 5) (3, 5) (3, 4) (3, 3) (3, 2) (3, 1) (4, 1) (5, 1) (5, 2) (5, 3) (5, 4) (5, 5) (16)
//
//             [ 10578] | find_path(1, 1) {
//             [ 10578] |   find_path(1, 2) {
//             [ 10578] |     find_path(1, 1) {
//   12.428 us [ 10578] |     } /* find_path */
//             [ 10578] |     find_path(1, 3) {
//             [ 10578] |       find_path(1, 2) {
//   10.787 us [ 10578] |       } /* find_path */
//             [ 10578] |       find_path(1, 4) {
//             [ 10578] |         find_path(1, 3) {
//   13.404 us [ 10578] |         } /* find_path */
//             [ 10578] |         find_path(1, 5) {
//             [ 10578] |           find_path(1, 4) {
//   12.767 us [ 10578] |           } /* find_path */
//             [ 10578] |           find_path(2, 5) {
//             [ 10578] |             find_path(1, 5) {
//   85.506 us [ 10578] |             } /* find_path */
//             [ 10578] |             find_path(3, 5) {
//             [ 10578] |               find_path(3, 4) {
//             [ 10578] |                 find_path(3, 3) {
//             [ 10578] |                   find_path(3, 2) {
//             [ 10578] |                     find_path(3, 1) {
//             [ 10578] |                       find_path(3, 2) {
//   28.249 us [ 10578] |                       } /* find_path */
//             [ 10578] |                       find_path(4, 1) {
//             [ 10578] |                         find_path(3, 1) {
//  182.678 us [ 10578] |                         } /* find_path */
//             [ 10578] |                         find_path(5, 1) {
//             [ 10578] |                           find_path(5, 2) {
//             [ 10578] |                             find_path(5, 1) {
//   82.954 us [ 10578] |                             } /* find_path */
//             [ 10578] |                             find_path(5, 3) {
//             [ 10578] |                               find_path(5, 2) {
//   17.163 us [ 10578] |                               } /* find_path */
//             [ 10578] |                               find_path(5, 4) {
//             [ 10578] |                                 find_path(5, 3) {
//   19.277 us [ 10578] |                                 } /* find_path */
//             [ 10578] |                                 find_path(5, 5) {
//   19.241 us [ 10578] |                                 } /* find_path */
//  116.223 us [ 10578] |                               } /* find_path */
//  213.627 us [ 10578] |                             } /* find_path */
//  391.920 us [ 10578] |                           } /* find_path */
//  967.572 us [ 10578] |                         } /* find_path */
//    2.857 ms [ 10578] |                       } /* find_path */
//    3.996 ms [ 10578] |                     } /* find_path */
//    4.060 ms [ 10578] |                   } /* find_path */
//    4.126 ms [ 10578] |                 } /* find_path */
//    4.220 ms [ 10578] |               } /* find_path */
//    4.287 ms [ 10578] |             } /* find_path */
//    5.416 ms [ 10578] |           } /* find_path */
//    6.411 ms [ 10578] |         } /* find_path */
//    6.485 ms [ 10578] |       } /* find_path */
//    6.559 ms [ 10578] |     } /* find_path */
//    6.708 ms [ 10578] |   } /* find_path */
//    7.233 ms [ 10578] | } /* find_path */


struct Maze
{
  std::vector<std::vector<int>> input;

  // std::array<std::array<int, 11>, 11> input;
  // std::array<std::array<int, 6>, 6> input;

  // set of visited points
  std::set<Point> visited_points{};

  // array of path points
  std::vector<Point> path_points{};

  Maze(int row, int col)
    : row_(row), col_(col) {}

  bool AlreadyTried(Point position)
  {
    return visited_points.find(position) == visited_points.end() ? false : true;
  }

  bool FoundTheExit(Point position)
  {
    return position == Point(row_-1, col_-1) ? true : false;
  }

  void RememberPosition(Point position)
  {
    auto result = visited_points.insert(position);
    if (!result.second)
    {
      cout << "RememberPosition: founds duplicates" << endl;
      cout << "RememberPosition: (" << position.first << ", " << position.second << ")" << endl;
      PRINT_ELEMENTS(visited_points);
    }
  }

  // if cannot move, return the input position
  Point GetPositionToMoveLeft(Point position)
  {
    Point point_to_move{position.first, position.second-1};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  Point GetPositionToMoveRight(Point position)
  {
    Point point_to_move{position.first, position.second+1};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  Point GetPositionToMoveUp(Point position)
  {
    Point point_to_move{position.first-1, position.second};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  Point GetPositionToMoveDown(Point position)
  {
    Point point_to_move{position.first+1, position.second};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  Point GetPositionToDiagRightUp(Point position)
  {
    Point point_to_move{position.first-1, position.second+1};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  Point GetPositionToDiagRightDown(Point position)
  {
    Point point_to_move{position.first+1, position.second+1};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  Point GetPositionToDiagLeftUp(Point position)
  {
    Point point_to_move{position.first-1, position.second-1};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  Point GetPositionToDiagLeftDown(Point position)
  {
    Point point_to_move{position.first+1, position.second-1};

    if (ValidPoint(point_to_move))
    {
      return point_to_move;
    }
    else 
    {
      return position;
    }
  }

  private:

  bool ValidPoint(Point position)
  {
    if ((0 <= position.first && position.first < row_) &&
        (0 <= position.second && position.second < col_) &&
        input[position.first][position.second] != 1
       )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  int row_{};
  int col_{};
};


// To use uftrace log, changed it from 
// bool find_path(Maze &maze, Point position)

bool find_path(Maze &maze, int row, int col)
{
  Point position{row, col};

  // to prevent circular path and going backwards and this make it search
  // forward. 
  if (maze.AlreadyTried(position))
  {
    return false;
  }

  // found a path which is stop condition
  if (maze.FoundTheExit(position))
  {
    return true;
  }

  // remember a position tried
  maze.RememberPosition(position);

  Point new_position{};

  // left
  if ((new_position = maze.GetPositionToMoveLeft(position)) != position)
  {
    cout << "l:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "left:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // right
  if ((new_position = maze.GetPositionToMoveRight(position)) != position)
  {
    cout << "r:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      cout << "right:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // up
  if ((new_position = maze.GetPositionToMoveUp(position)) != position)
  {
    cout << "u:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "up:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // down
  if ((new_position = maze.GetPositionToMoveDown(position)) != position)
  {
    cout << "d:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "down:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag right up 
  if ((new_position = maze.GetPositionToDiagRightUp(position)) != position)
  {
    cout << "dru:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:rup:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag right down 
  if ((new_position = maze.GetPositionToDiagRightDown(position)) != position)
  {
    cout << "drd:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:rdown:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag left up 
  if ((new_position = maze.GetPositionToDiagLeftUp(position)) != position)
  {
    cout << "dlu:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:lup:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  // diag left down 
  if ((new_position = maze.GetPositionToDiagLeftDown(position)) != position)
  {
    cout << "dld:(" << new_position.first << ", " << new_position.second << ")" << endl;

    if (find_path(maze, new_position.first, new_position.second))
    {
      // cout << "diag:ldown:insert(" << new_position.first << ", " << new_position.second << ")" << endl;
      maze.path_points.insert(maze.path_points.begin(), new_position);
      return true;
    }
  }

  return false;
}

TEST(Maze, ExerciseInterfaces)
{
  Maze maze(5, 5);
  maze.input = {
    {0,0,0,0,0},
    {1,1,1,1,0},
    {0,0,0,0,0},
    {0,1,1,1,1},
    {2,0,0,0,0}
  };

  maze.RememberPosition(Point(1, 1));
  maze.RememberPosition(Point(1, 2));

  EXPECT_THAT(maze.AlreadyTried(Point(1, 1)), true);
  EXPECT_THAT(maze.AlreadyTried(Point(1, 3)), false);

  EXPECT_THAT(maze.FoundTheExit(Point(4, 5)), false);
  EXPECT_THAT(maze.FoundTheExit(Point(5, 5)), false);

  // not able to move
  EXPECT_THAT(maze.GetPositionToMoveLeft(Point(0, 0)), Eq(Point(0, 0)));

  // able to move
  EXPECT_THAT(maze.GetPositionToMoveLeft(Point(0, 1)), Eq(Point(0, 0)));
}

TEST(DISABLED_Maze, Array5x5)
{
  Maze maze(5, 5);
  maze.input = {
    {0,0,0,0,0},
    {1,1,1,1,0},
    {0,0,0,0,0},
    {0,1,1,1,1},
    {2,0,0,0,0}
  };

  // use start point (0, 0) rather then (1, 1).
  find_path(maze, 0, 0);
  PRINT_ELEMENTS(maze.path_points);
}

// TEST(DISABLED_Maze, Array10x10)
TEST(Maze, Array10x10)
{
  // when not support diagonal move
  // Maze maze(10, 10);
  // maze.input = {
  //   {0,1,0,1,0,1,0,0,0,1},
  //   {0,1,0,1,0,1,1,1,0,1},
  //   {0,0,0,0,0,1,0,0,0,1},
  //   {0,1,0,1,1,1,0,1,1,1},
  //   {0,1,2,1,0,0,0,0,0,1},
  //   {1,1,0,1,0,1,1,1,1,1},
  //   {0,1,0,0,0,0,0,1,0,1},
  //   {0,1,1,1,0,1,1,1,0,1},
  //   {0,0,0,0,0,0,0,0,0,1},
  //   {1,1,1,1,1,1,1,1,0,0}
  // };

  Maze maze(10, 10);
  maze.input = {
    {0,1,0,1,0,1,0,0,0,1},
    {0,1,0,1,0,1,1,1,0,1},
    {0,0,0,0,0,1,0,0,0,1},
    {0,1,0,1,1,1,0,1,1,1},
    {0,1,2,1,0,0,0,0,0,1},
    {1,1,0,1,0,1,1,1,1,1},
    {0,1,0,0,0,0,0,1,0,1},
    {0,1,1,1,0,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,0}
  };
 
  find_path(maze, 0, 0);
  PRINT_ELEMENTS(maze.path_points);
}

TEST(DISABLED_Maze, Array15x15)
{
  Maze maze(15, 15);
  maze.input = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},  
    {0,1,0,1,0,0,0,1,0,1,0,1,0,1,0},
    {0,1,0,0,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,0,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
 
  find_path(maze, 0, 0);
  PRINT_ELEMENTS(maze.path_points);
}

// This input select the path which do have have "2" in. How to support this
// requirement?
//
// * change the traveled path to have the value and to keep only traveled points
// so far. So need to remove point when find_path() returns false so that
// traveled path only have points up to where it runs into
//
// * changed the end condition to see if the traveled path has the "2". if not
// return false so that can try other paths.


TEST(DISABLED_Maze, Array20x20)
{
  Maze maze(20, 20);
  maze.input = {

    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, 
    {0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0}
  };
 
  find_path(maze, 0, 0);
  PRINT_ELEMENTS(maze.path_points);
}

template <typename T>
void PRINT_ELEMENTS_MAP(T col, const string mesg, const string sep = ", ")
{
    cout << "=" << mesg << endl;

    for ( auto &e : col)
        cout << "{" << e.first << ", " << e.second << "}" << sep;
}

int count_occurance_from_sequence(const vector<int> &input, int key) 
{
  map<char,int> count_map;

  for (auto value : input)
  {
    string str = to_string(value);
    for (auto e : str)
    {
      ++count_map[e];
    }
  }

  string stringkey = to_string(key);
  auto ret = count_map.find(stringkey[0]);
  
  // if values are [0,9] and are ASCII then, can use:
  // auto ret = count_map.find(key+48);

  return ret->second;
}

TEST(X, XX) 
{
  vector<int> input_value{11,12,13,14,15};
  EXPECT_THAT(count_occurance_from_sequence(input_value, 1), 6);
}


// ={=========================================================================

int main(int argc, char **argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
