#include "gmock/gmock.h"
#include "cxx_sp.h"

using namespace std;
using namespace testing;

// =============
// ::HasPtr(): hi mon! used 1
// ::HasPtr(const HasPtr &): hi mon! used 2
// ::HasPtr(const HasPtr &): hi mon! used 3
// -------------
// ::~HasPtr(): hi mon! used 0

TEST(CxxSpTest, UseHasPtrCopyCtor) {

    // h1, h2, h3 shares the same underlying.
    
    cout << "=============" << endl;

    HasPtr h1("hi mon!");

    HasPtr h2 = h1;
    HasPtr h3{h2};

    cout << "-------------" << endl;
}

// =============
// ::HasPtr(): hi mom! used 1
// ::HasPtr(HasPtr &&): hi mom! used 1
// ::HasPtr(HasPtr &&): hi mom! used 1
// -------------
// ::~HasPtr(): hi mom! used 0

TEST(CxxSpTest, UseHasPtrMove) {

    // h, h2, and h3 use the same one underlying string
 
    cout << "=============" << endl;

    HasPtr h1("hi mom!");
    HasPtr h2 = std::move(h1);
    HasPtr h3 = std::move(h2);

    cout << "-------------" << endl;
}

// =============
// ::HasPtr(): hi mon! used 1
// ::HasPtr(): hi dad! used 1
// ::HasPtr &operator=(const HasPtr &): hi mon! used 2
// -------------
// ::~HasPtr(): hi mon! used 0

TEST(CxxSpTest, UseHasPtrCopyAssign) {

    cout << "=============" << endl;

    HasPtr h1("hi mon!");
    HasPtr h2("hi dad!");

    h2 = h1;

    cout << "-------------" << endl;
}

// =============
// ::HasPtr(): hi mon! used 1
// ::HasPtr(): hi dad! used 1
// ::HasPtr &operator=(HasPtr &&): hi mon! used 1
// -------------
// ::~HasPtr(): hi mon! used 0

TEST(CxxSpTest, UseHasPtrMoveAssign) {

    cout << "=============" << endl;

    HasPtr h1("hi mon!");
    HasPtr h2("hi dad!");

    h2 = std::move(h1);

    cout << "-------------" << endl;
}

// =============
// ::HasPtr(): hi mon! used 1
// ::HasPtr(const HasPtr &): hi mon! used 2
// ::HasPtr():  used 1
// use_ptr: use copy controls
// ::HasPtr &operator=(const HasPtr &): hi mon! used 3
// -------------
// ::~HasPtr(): hi mon! used 0

TEST(CxxSpTest, UseHasPtrUseFunction) {

    cout << "=============" << endl;

    HasPtr h1("hi mon!");
    HasPtr h2 = use_ptr(h1);

    cout << "-------------" << endl;
}

int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
