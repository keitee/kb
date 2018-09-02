#include <iostream>
#include <set>
#include <vector>

#include "gmock/gmock.h"

// g++ -g -std=c++0x t_override.cpp

using namespace std;
using namespace testing;


// ={=========================================================================

/* Copyright (C) 1999 Lucent Technologies
From 'Programming Pearls' by Jon Bentley

13 Searching. Appendix 5

Most common `sorted set` representation.
*/


// set set version

class IntSetStl
{
  private:
    set<int> S;
  public:
    IntSetStl(int maxelms, int maxval) { (void)maxelms; (void)maxval; }
    int size() { return S.size(); }
    void insert(int t) { S.insert(t); }
    void report(int *v)
    {
      int i{0}, n{};
      for (auto it = S.begin(); it != S.end(); ++it)
      {
        v[i++] = *it;
        ++n;
      }
      cout << setw(20) << "IntSetStl: n : " << n << endl;
    }
};


void run_set_stl(int m, int maxval)
{
  IntSetStl S(m, maxval);
  while (S.size() < m)
    S.insert(rand() % maxval);

  // int *v = new int[m];
  // S.report(v);
}


/* 
Linear structure: set array version

o Use one extra element for a sentinel that has maxval at the end of the sorted
elements. This replaces the test for running off the end of the list with a test
for finding a greater element. That will make our insertion code simpler and
will incidentally make it faseter.

o Need to handle a case where n == maxelms when calls insert()? No since check
size() before calling insert().

o Can have a memmber search function that use binary search since it's sorted
set.

o When the size of set is known in advance or space is enough or the max of
elements are not big.

*/

class IntSetArray
{
  private:
    int n, *x;

  public:
    IntSetArray(int maxelms, int maxval)
    {
      x = new int[1 + maxelms];
      n = 0;
      x[0] = maxval;
    }
    
    int size() { return n; }

    void insert(int t)
    {
      int i{}, j{};

      // search pos to insert
      for (i = 0; x[i] < t; ++i)
        ;

      // slide down including sentinel.
      for (j = n; j >= i; --j)
        x[j+1] = x[j];

      x[i] = t;
      ++n;
    }

    void report(int *v)
    {
      for (int i = 0; i < n; ++i)
      {
        v[i] = x[i];
      }
      cout << setw(20) << "IntSetArray: n : " << n << endl;
    }
};


class IntSetArrayNoSentinel
{
  private:
    int n, *x;

  public:
    IntSetArrayNoSentinel(int maxelms, int maxval)
    {
      (void)maxval;
      x = new int[maxelms];
      n = 0;
      // x[0] = maxval;
    }
    
    int size() { return n; }

    void insert(int t)
    {
      int i{}, j{};

      if (n == 0)
      {
        x[0] = t;
        ++n;
        return;
      }

      // search pos to insert. add check
      for (i = 0; i < n && x[i] < t; ++i)
        ;

      // slide down. remove "=" and change assign since there is no sentinel. 
      for (j = n; j > i; --j)
        x[j] = x[j-1];

      x[i] = t;
      ++n;

// #define SET_ARRARY_DEBUG
#ifdef SET_ARRARY_DEBUG
      cout << "i: " << i << ", debug: ";
      for (int d = 0; d < n; ++d)
        cout << x[d] << ", ";
      cout << endl;
#endif
    }

    void report(int *v)
    {
      for (int i = 0; i < n; ++i)
      {
        v[i] = x[i];
      }
      cout << setw(20) << "IntSetArrayNoSentinel: n : " << n << endl;
    }
};

void run_set_array(int m, int maxval)
{
  IntSetArray S(m, maxval);
  while (S.size() < m)
    S.insert(rand() % maxval);

  // int *v = new int[m];
  // S.report(v);
}


/* 
Linear structure: set list version

o Do not move elements to make a space to insert. Should be faster than the
array version? No according to the book.

o Use more space than the array version.
*/

class IntSetList
{
  private:
    int n;
    struct node
    {
      int val;
      node *next;
      node(int v, node *p) 
      { val = v; next = p; }
    };

    node *head, *sentinel;

    node *rinsert(node* p, int value)
    {
      // search a pos to insert
      if (p->val < value)
      {
        p->next = rinsert(p->next, value);
      }
      // ends recursion
      else if (p->val > value)
      {
        p = new node(value, p);
        ++n;
      }

      // handles when value == t as well.
      return p;
    }

  public:
    IntSetList(int maxelms, int maxval)
    {
      (void)maxelms;
      head = sentinel = new node(maxval, nullptr);
      n = 0;
    }

    int size() { return n; }

    void insert(int t)
    {
      head = rinsert(head, t); 
    }

    // void report(int *v)
    // {
    //   node *p = head;
    //   for(int i = 0; i < n; ++i)
    //   {
    //     v[i] = p->val;
    //     p = p->next;
    //   }
    // }

    // use sentinel
    void report(int *v)
    {
      int i = 0;
      for(node *p = head; p != sentinel; p = p->next)
        v[i++] = p->val;

      cout << setw(20) << "IntSetList: n : " << n << endl;
    }
};

void run_set_list(int m, int maxval)
{
  IntSetList S(m, maxval);
  while (S.size() < m)
    S.insert(rand() % maxval);

  // int *v = new int[m];
  // S.report(v);
}


/* 
13.3 Binary Search Tree

o Do not use sentinel.

o Do not worry about balancing since uses random inputs
*/

class IntSetBST
{
  private:
    int n, *x, vn;

    struct node
    {
      int val;
      node *left, *right;
      node(int i) { val = i; left = right = nullptr; }
    };

    node *root;

    node* rinsert(node *p, int value)
    {
      // the search falls out of the tree
      if (p == nullptr)
      {
        p = new node(value);
        ++n;
      }
      else if (value < p->val)
      {
        p->left = rinsert(p->left, value);
      }
      else if (p->val < value)
      {
        p->right = rinsert(p->right, value);
      }

      // do nothing when the value is found and search ends.
      return p;
    }

    // in-order traverse
    void traverse(node *p)
    {
      if (p == nullptr)
        return;

      traverse(p->left);
      x[vn++] = p->val;
      traverse(p->right);
    }

  public:
    IntSetBST(int maxelms, int maxval)
    {
      (void)maxelms;
      (void)maxval;
      root = nullptr;
      n = 0;
    }

    int size() { return n; }

    void insert(int t)
    {
      root = rinsert(root, t);
    }

    void report(int *v)
    {
      x = v;
      vn = 0;
      traverse(root);
    }
};

void run_set_bst(int m, int maxval)
{
  IntSetStl S(m, maxval);
  while (S.size() < m)
    S.insert(rand() % maxval);

  // int *v = new int[m];
  // S.report(v);
}


/* 
13.4 Structure for integers
*/

class IntSetBitVec
{
  private:
    const int BITSPERWORD=32;
    const int SHIFT=5;
    const int MASK=0x1F;

    int max, n, *x;

    void set_bit(int i)
    {
      x[i>>SHIFT] |= (1 << (i & MASK));
    }

    void clear_bit(int i)
    {
      x[i>>SHIFT] &= ~(1 << (i & MASK));
    }

    // check if 0 or else after & op.
    int test_bit(int i)
    {
      return x[i>>SHIFT] & (1 << (i & MASK));
    }

  public:
    IntSetBitVec(int maxelms, int maxval)
    {
      (void)maxelms;
      max = maxval;
      n = 0;

      int size = maxval/BITSPERWORD+1;
      x = new int[size];

      for (int i = 0; i < size; ++i)
        x[i] = 0;
    }

    int size() { return n; }

    void insert(int t)
    {
      if (test_bit(t))
        return;

      set_bit(t);
      ++n;
    }

    void report(int *v)
    {
      // have to have j but not use i.
      int j = 0;

      for(int i = 0; i <= max; ++i)
        if (test_bit(i))
          v[j++] = i;
    }
};

void run_set_bitvec(int m, int maxval)
{
  IntSetBitVec S(m, maxval);
  while (S.size() < m)
    S.insert(rand() % maxval);

  // int *v = new int[m];
  // S.report(v);
}


/* 
13.4 Structure for integers
*/

class IntSetBins
{
  private:
    int n, bins, max;

    struct node
    {
      int val;
      node *next;
      node(int v, node *p)
      { val = v; next = p; }
    };

    node **bin, *sentinel;

    // copy from IntSetList
    node *rinsert(node* p, int value)
    {
      // search a pos to insert
      if (p->val < value)
      {
        p->next = rinsert(p->next, value);
      }
      // ends recursion
      else if (p->val > value)
      {
        p = new node(value, p);
        ++n;
      }

      // handles when value == t as well.
      return p;
    }

  public:
    IntSetBins(int maxelms, int maxval, int maxbins)
    {
      (void)maxelms;
      // number of bins(buckets)
      bins = maxbins;
      max = maxval;

      bin = new node*[bins];
      sentinel = new node(maxval, 0);

      for (int i = 0; i < bins; ++i)
        bin[i] = sentinel;

      n = 0;
    }

    int size() { return n; }

    void insert(int t)
    {
      int bucket = t / (1 + max/bins);
      bin[bucket] = rinsert(bin[bucket], t);
    }

    void report(int *v)
    {
      int j = 0;
      for(int i = 0; i < bins; ++i)
      {
        for(node *p = bin[i]; p != sentinel; p = p->next)
          v[j++] = p->val;
      }
    }
};

// unlike others, maxelems should not be used as maxbin.
void run_set_bins(int m, int maxval, int maxbin)
{
  IntSetBins S(m, maxval, maxbin);
  while (S.size() < m)
    S.insert(rand() % maxval);

  // int *v = new int[m];
  // S.report(v);
}


// ={=========================================================================

#define MAXN  10000

TEST(SetStl, Run)
{
  run_set_stl(MAXN, MAXN*10);
}

TEST(SetArray, Run)
{
  run_set_array(MAXN, MAXN*10);
}

TEST(SetArray, SortSet)
{
  {
    vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};

    int *v = new int[coll.size()];
    IntSetArray s(coll.size(), 100);
    for (auto e : coll)
      s.insert(e);

    s.report(v);

    vector<int> collo;

    for (int i = 0; i < s.size(); ++i)
      collo.push_back(v[i]);

    EXPECT_THAT(collo, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }

  {
    vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};

    int *v = new int[coll.size()];
    IntSetArrayNoSentinel s(coll.size(), 100);
    for (auto e : coll)
      s.insert(e);

    s.report(v);

    vector<int> collo;

    for (int i = 0; i < s.size(); ++i)
      collo.push_back(v[i]);

    EXPECT_THAT(collo, 
        ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
  }
}

TEST(SetList, Run)
{
  run_set_list(MAXN, MAXN*10);
}

TEST(SetList, SortSet)
{
  vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};

  int *v = new int[coll.size()];
  IntSetList s(coll.size(), 100);
  for (auto e : coll)
    s.insert(e);

  s.report(v);

  vector<int> collo;

  for (int i = 0; i < s.size(); ++i)
    collo.push_back(v[i]);

  EXPECT_THAT(collo, 
      ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
}

TEST(SetBST, Run)
{
  run_set_bst(MAXN, MAXN*10);
}

TEST(SetBST, SortSet)
{
  vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};

  int *v = new int[coll.size()];
  IntSetBST s(coll.size(), 100);
  for (auto e : coll)
    s.insert(e);

  s.report(v);

  vector<int> collo;

  for (int i = 0; i < s.size(); ++i)
    collo.push_back(v[i]);

  EXPECT_THAT(collo, 
      ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
}

TEST(SetBitVec, Run)
{
  run_set_bitvec(MAXN, MAXN*10);
}

TEST(SetBitVec, SortSet)
{
  vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};

  int *v = new int[coll.size()];
  IntSetBitVec s(coll.size(), 100);
  for (auto e : coll)
    s.insert(e);

  s.report(v);

  vector<int> collo;

  for (int i = 0; i < s.size(); ++i)
    collo.push_back(v[i]);

  EXPECT_THAT(collo, 
      ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
}

TEST(SetBins, Run)
{
  run_set_bins(MAXN, MAXN*10, 20);
}

TEST(SetBins, SortSet)
{
  vector<int> coll{33, 2, 31, 5, 30, 6, 12, 10, 13, 15, 17, 29, 3};

  int *v = new int[coll.size()];
  IntSetBins s(coll.size(), 100, 5);
  for (auto e : coll)
    s.insert(e);

  s.report(v);

  vector<int> collo;

  for (int i = 0; i < s.size(); ++i)
    collo.push_back(v[i]);

  EXPECT_THAT(collo, 
      ElementsAreArray({2, 3, 5, 6, 10, 12, 13, 15, 17, 29, 30, 31, 33}));
}


// ={=========================================================================

int main(int argc, char** argv)
{
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
