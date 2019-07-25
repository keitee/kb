#include <iostream>
#include <cstdlib>

#include "gtest/gtest.h"

typedef int EntryType;

typedef struct node
{
  EntryType entry;
  node*     pnext;
} Node;

typedef struct {
  int    count;
  Node*  header;
} List;

Node* MakeNode( EntryType entry )
{
  Node* pnode = NULL;

  if( (pnode = (Node*) malloc( sizeof(Node))) == NULL )
  {
    std::cout << "no more memory" << std::endl;
    return NULL;
  }

  pnode->entry = entry;
  pnode->pnext = NULL;

  return pnode;
}

void CreatList( List* list )
{ 
  list->count = 0;
  list->header = NULL; 
}

bool ListEmpty( List* list )
{ return ( list->header == NULL ); }

// add only at the end. see there are approaches to find end; one is to use pnext and the other is
// to use count. 
bool AddList( List* list, EntryType entry )
{
  Node* pnode, *pend;

  if( (pnode = MakeNode(entry)) == NULL )
  {
    std::cout << "add: mem is full" << std::endl;
    return false;
  }

  if( ListEmpty( list ) )
  {
    list->header = pnode;
  }
  else
  {
#ifdef USE_PNEXT
    // search the end using pnext
    for( pend = list->header; pend->pnext; pend = pend->pnext )
      ;
#else
    // search the end using count
    pend = list->header;
    for( int current = 1; current < list->count; current++) // note that less than
      pend = pend->pnext;
#endif

    pend->pnext = pnode;
  }

  list->count++;

  return true;
}

typedef void(*TRAVERSEFUNC)(EntryType);

void TraverseList( List* list, TRAVERSEFUNC func)
{
  Node* pnode;

  if( ListEmpty(list) )
  {
    std::cout << "list is empty" << std::endl;
    return;
  }

  pnode = list->header;

  while(pnode)
  {
    func(pnode->entry);
    pnode = pnode->pnext;
  }
}

void EntryPrint(EntryType item)
{
  std::cout << "item is: " << item << std::endl;
}

TEST(ListTest, checkEmpty) {
  List list;
  CreatList(&list);

  EXPECT_EQ( 1, ListEmpty(&list) );

  // to make fail
  // EXPECT_EQ( 0, ListEmpty(&list) );     // note: line #112 
}

// int main(int argc, char **argv)
// {
//   int item = 0;
// 
//   testing::InitGoogleTest(&argc, argv);
// 
// //  List list;
// //  CreatList(&list);
// //
// //  std::cout << "type in 5 numbers." << std::endl;
// //
// //  for(int i=0; i < 5; i++)
// //  {
// //    std::cin >> item;
// //    AddList(&list, item );
// //  }
// //
// //  TraverseList(&list, EntryPrint);
// //
//   return RUN_ALL_TESTS();
// }
