/*

*/
#include <string>
#include <thread>

#include "gmock/gmock.h"

using namespace testing;

#if 0
={=========================================================================
when has "Param" and "Env" as members of OUT, Executor.
#endif

namespace M1
{
  class Param
  {
  private:
    std::string m_param1{};
    std::string m_param2{};

  public:
    virtual ~Param() {}

  public:
    virtual std::string getParam1() = 0;
    virtual std::string getParam2() = 0;
  };

  class MockParam : public Param
  {
  public:
    // MOCK_METHOD0(getParam1, std::string());
    // MOCK_METHOD0(getParam2, std::string());

    MOCK_METHOD(std::string, getParam1, ());
    MOCK_METHOD(std::string, getParam2, ());
  };

  class Env
  {
  private:
    std::string m_result{};

  public:
    virtual ~Env() {}

  public:
    virtual int writeResult(std::string &code, std::string &message) = 0;
  };

  class MockEnv : public Env
  {
  public:
    // MOCK_METHOD2(writeResult, int(std::string &code, std::string &message));
    MOCK_METHOD(int, writeResult, (std::string & code, std::string &message));
  };

  class Executor
  {
  private:
    std::shared_ptr<Param> m_param{};
    std::shared_ptr<Env> m_env{};

  public:
    Executor(std::shared_ptr<Param> param, std::shared_ptr<Env> env)
        : m_param(param)
        , m_env(env)
    {
      // // firstly, get the param1
      // std::cout << "Executor: param1 is " << m_param->getParam1() << "\n";
    }

    std::string doFunction1()
    {
      // firstly, get the param1
      auto param = m_param->getParam1();
      std::cout << "Executor:doFunction1: param1 is " << param << "\n";
      return param;
    }

    std::string doFunction2()
    {
      // firstly, get the param1
      auto param = m_param->getParam2();
      std::cout << "Executor:doFunction1: param1 is " << param << "\n";
      return param;
    }
  };
} // namespace M1

class ExecutorTest : public ::testing::Test
{
public:
  explicit ExecutorTest() { std::cout << "ExecutorTest()\n"; }

  ~ExecutorTest() { std::cout << "~ExecutorTest()\n"; }

protected:
  // NOTE:
  // https://stackoverflow.com/questions/44028922/gmock-pointer-to-base-class-has-no-member-gmock
  //
  // error: ‘using element_type = class {anonymous}::Param {aka class {anonymous}::Param}’
  // has no member named ‘gmock_getParam1’; did you mean ‘getParam1’?
  //
  //      ON_CALL(*m_param, getParam1).WillByDefault(Return("default from mock"));
  //      ^
  //
  // since "using base type of shared pointer" works in usual inheritance, it
  // seems it's specific to gmock
  //
  // std::shared_ptr<Param> m_param{};
  // std::shared_ptr<Env> m_env{};

  std::shared_ptr<M1::MockParam> m_param{};
  std::shared_ptr<M1::MockEnv> m_env{};

  std::unique_ptr<M1::Executor> m_executor{};

  virtual void SetUp() override
  {
    std::cout << "SetUp()\n";

    // create mocks
    m_param = std::make_shared<M1::MockParam>();
    m_env   = std::make_shared<M1::MockEnv>();

    // create executor, "OUT"
    m_executor = std::make_unique<M1::Executor>(m_param, m_env);

    // default action
    ON_CALL(*m_param, getParam1).WillByDefault(Return("default from mock"));
  }

  virtual void TearDown() override { std::cout << "TearDown()\n"; }
};

#if 0
[ RUN      ] ExecutorTest.check_default_return
ExecutorTest()
SetUp()

GMOCK WARNING:
Uninteresting mock function call - taking default action specified at:
/home/keitee/git/kb/code-cxx/test_gtest/fixture.cpp:108:
    Function call: getParam1()
          Returns: "default from mock"
NOTE: You can safely ignore the above warning unless this call should not
happen.  Do not suppress it by blindly adding an EXPECT_CALL() if you do not
mean to enforce the call. See
https://github.com/google/googletest/blob/master/googlemock/docs/cook_book.md#knowing-when-to-expect
for details.

Executor:doFunction1: param1 is default from mock
TearDown()
~ExecutorTest()
[       OK ] ExecutorTest.check_default_return (0 ms)
[ RUN      ] ExecutorTest.check_specific_return_from_oncall
ExecutorTest()
SetUp()

GMOCK WARNING:
Uninteresting mock function call - taking default action specified at:
/home/keitee/git/kb/code-cxx/test_gtest/fixture.cpp:123:
    Function call: getParam1()
          Returns: "defined return from mock"
NOTE: You can safely ignore the above warning unless this call should not
happen.  Do not suppress it by blindly adding an EXPECT_CALL() if you do not
mean to enforce the call. See
https://github.com/google/googletest/blob/master/googlemock/docs/cook_book.md#knowing-when-to-expect
for details.

Executor:doFunction1: param1 is defined return from mock
TearDown()
~ExecutorTest()
[       OK ] ExecutorTest.check_specific_return_from_oncall (0 ms)
[ RUN      ] ExecutorTest.check_specific_return_from_expect
ExecutorTest()
SetUp()
Executor:doFunction1: param1 is expection return from mock
TearDown()
~ExecutorTest()
[       OK ] ExecutorTest.check_specific_return_from_expect (0 ms)
#endif

// since there is no oncall for default action, gmock provide default action for
// std::string.
//
// NOTE that still see "uninteresting call" warning since there is no
// expectation placed even if ON_CALL is used.

TEST_F(ExecutorTest, check_default_return_when_no_oncall)
{
  EXPECT_THAT(m_executor->doFunction2(), "");
}

TEST_F(ExecutorTest, check_default_return_when_oncall)
{
  EXPECT_THAT(m_executor->doFunction1(), "default from mock");
}

// override default return with specific action
TEST_F(ExecutorTest, check_override_default_return_when_oncall)

{
  ON_CALL(*m_param, getParam1)
    .WillByDefault(Return("defined return from mock"));

  EXPECT_THAT(m_executor->doFunction1(), "defined return from mock");
}

// override default action using expect
TEST_F(ExecutorTest, check_override_default_return_when_expect)
{
  EXPECT_CALL(*m_param, getParam1)
    .Times(1)
    .WillOnce(Return("expection return from mock"));

  EXPECT_THAT(m_executor->doFunction1(), "expection return from mock");
}

// override default action using expect
TEST_F(ExecutorTest, check_lambda_action)
{
  EXPECT_CALL(*m_param, getParam1).Times(1).WillOnce([] {
    return std::string("expection return from mock");
  });

  EXPECT_THAT(m_executor->doFunction1(), "expection return from mock");
}

#if 0
={=========================================================================
Before Env and Param are dependancies which are the same level; Executor take
them and use them both.

Now Env provides getParam() to get and Executor has to use this call to get
Param. So Env mock returns Param mock
#endif

namespace M2
{
  class Param
  {
  private:
    std::string m_param1{};
    std::string m_param2{};

  public:
    virtual ~Param() {}

  public:
    // NOTE: okay to use non pure virtuals
    virtual std::string getParam_1()
    {
      std::cout << "Param::getParam_1\n";
      return "Param::getParam_1";
    }

    virtual std::string getParam_2() = 0;
  };

  class MockParam : public Param
  {
  public:
    MOCK_METHOD(std::string, getParam_1, (), (override));
    MOCK_METHOD(std::string, getParam_2, (), (override));
  };

  class Env
  {
  private:
    std::string m_result{};

  public:
    virtual ~Env() {}

  public:
    virtual int writeResult(std::string &code, std::string &message) = 0;
    virtual Param *getParam()                                        = 0;
  };

  class MockEnv : public Env
  {
  public:
    MOCK_METHOD(int,
                writeResult,
                (std::string & code, std::string &message),
                (override));
    MOCK_METHOD(Param *, getParam, (), (override));
  };

  class Executor
  {
  private:
    // std::shared_ptr<Param> m_param{};
    std::shared_ptr<Env> m_env{};

  public:
    Executor(std::shared_ptr<Env> env)
        : m_env(env)
    {}

    // Executor(std::shared_ptr<Param> param, std::shared_ptr<Env> env)
    //     : m_param(param)
    //     , m_env(env)
    // {
    //   // // firstly, get the param1
    //   // std::cout << "Executor: param1 is " << m_param->getParam1() << "\n";
    // }

    std::string doFunction1()
    {
      // firstly, get the param object
      auto param = m_env->getParam();

      // then call param_1
      auto param_1 = param->getParam_1();
      std::cout << "Executor:doFunction1: param_1 is " << param_1 << "\n";
      return param_1;
    }
  };

  class ExecutorThreaded
  {
  private:
    // std::shared_ptr<Param> m_param{};
    std::shared_ptr<Env> m_env{};
    std::thread m_thread{};

  public:
    ExecutorThreaded(std::shared_ptr<Env> env)
        : m_env(env)
    {
      m_thread = std::thread(std::bind(&ExecutorThreaded::execute_, this));
    }

    ~ExecutorThreaded()
    {
      // m_thread.join();
    }

    void wait() { m_thread.join(); }

    // std::string doFunction1()
    void execute_()
    {
      // firstly, get the param object
      auto param = m_env->getParam();

      // then call param_1
      auto param_1 = param->getParam_1();
      std::cout << "ExecutorThreaded:execute_: param_1 is: " << param_1 << "\n";

      return;
    }
  };
} // namespace M2

class ExecutorTest_2 : public ::testing::Test
{
public:
  explicit ExecutorTest_2() { std::cout << "ExecutorTest_2()\n"; }

  ~ExecutorTest_2() { std::cout << "~ExecutorTest_2()\n"; }

protected:
  // NOTE:
  // https://stackoverflow.com/questions/44028922/gmock-pointer-to-base-class-has-no-member-gmock
  //
  // error: ‘using element_type = class {anonymous}::Param {aka class {anonymous}::Param}’
  // has no member named ‘gmock_getParam1’; did you mean ‘getParam1’?
  //
  //      ON_CALL(*m_param, getParam1).WillByDefault(Return("default from mock"));
  //      ^
  //
  // since "using base type of shared pointer" works in usual inheritance, it
  // seems it's specific to gmock
  //
  // std::shared_ptr<Param> m_param{};
  // std::shared_ptr<Env> m_env{};

  M2::MockParam *m_param{nullptr};

  std::shared_ptr<M2::MockEnv> m_env{};

  std::unique_ptr<M2::Executor> m_executor{};

  virtual void SetUp() override
  {
    std::cout << "SetUp()\n";

    // create mocks
    // m_param = new M2::MockParam;
    // m_env   = std::make_shared<M2::MockEnv>();

    // use nicemock to avoid warnings
    m_param = new NiceMock<M2::MockParam>;
    m_env   = std::make_shared<NiceMock<M2::MockEnv>>();

    // create executor, "OUT"
    m_executor = std::make_unique<M2::Executor>(m_env);

    // default action
    ON_CALL(*m_env, getParam).WillByDefault(Return(m_param));
    ON_CALL(*m_param, getParam_1).WillByDefault(Return("default from mock"));
  }

  virtual void TearDown() override
  {
    std::cout << "TearDown()\n";
    delete m_param;
  }
};

// default action but no warning.
TEST_F(ExecutorTest_2, check_other_dependency)
{
  EXPECT_THAT(m_executor->doFunction1(), "default from mock");
}

#if 0
={=========================================================================
Before Env and Param are dependancies which are the same level; Executor take
them and use them both.

Now Env provides getParam() to get and Executor has to use this call to get
Param. So Env mock returns Param mock
#endif

TEST(ExecutorTest_3, check_executor_threaded)
{
  M2::MockParam *m_param{nullptr};
  std::shared_ptr<M2::MockEnv> m_env{};

  std::unique_ptr<M2::ExecutorThreaded> m_executor{};

  {
    // use nicemock to avoid warnings
    m_param = new NiceMock<M2::MockParam>;
    m_env   = std::make_shared<NiceMock<M2::MockEnv>>();

    // default action
    ON_CALL(*m_env, getParam).WillByDefault(Return(m_param));
    ON_CALL(*m_param, getParam_1).WillByDefault(Return("default from mock"));
  }

  EXPECT_THAT(m_param->getParam_1(), "default from mock");

  // create executor, "OUT"
  m_executor = std::make_unique<M2::ExecutorThreaded>(m_env);

  m_executor->wait();

  {
    delete m_param;
  }
}
