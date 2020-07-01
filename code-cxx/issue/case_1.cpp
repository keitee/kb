/*

core dump and race.

See this when trying to mock 3rd party feature which is DTT feature and use
polling to check status and to play live. This includes polling thread which
uses this service.

It crashes when a polling thread tries to call to DTT. run GDB and see that
vtable ptr is null when a thread tries to call. See the same address meaning
same object. How?


(gdb) p m_param
$1 = (M2::MockParam *) 0x5555559d4400

(gdb) p *m_param
$2 = {<M2::Param> = {_vptr.Param = 0x5555559b66c8 <vtable for testing::NiceMock<M2::MockParam>+16>, m_param1 = "", m_param2 = ""},
  gmock00_getParam_1_269 = {<testing::internal::UntypedFunctionMockerBase> = {<No data fields>}, <No data fields>},
  gmock00_getParam_2_270 = {<testing::internal::UntypedFunctionMockerBase> = {<No data fields>}, <No data fields>}}

(gdb) print m_param->getParam_1
$3 = {testing::internal::Function<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >()>::Result (M2::MockParam * const)} 0x5555556e4450 <M2::MockParam::getParam_1[abi:cxx11]()>

(gdb) continue
Continuing.
[New Thread 0x7ffff6e85700 (LWP 14526)]
[Switching to Thread 0x7ffff6e85700 (LWP 14526)]

Thread 2 "test_gtest" hit Breakpoint 2, M2::ExecutorThreaded::execute_ (this=0x5555559d4980) at /home/keitee/git/kb/code-cxx/test_gtest/fixture.cpp:355
355         void execute_()
(gdb) next
358           auto param = m_env->getParam();
(gdb)
361           auto param_1 = param->getParam_1();

(gdb) p param
$4 = (M2::Param *) 0x5555559d4400
(gdb) p *param
$5 = {_vptr.Param = 0x0, m_param1 = "", m_param2 = ""}

(gdb) p *(M2::MockParam*)param
$6 = {<M2::Param> = {_vptr.Param = 0x0, m_param1 = "", m_param2 = ""}, gmock00_getParam_1_269 = warning: RTTI symbol not found for class 'testing::internal::UntypedFunctionMockerBase'
{<testing::internal::UntypedFunctionMockerBase> = {<No data fields>}, <No data fields>}, gmock00_getParam_2_270 = warning: RTTI symbol not found for class 'testing::internal::UntypedFunctionMockerBase'

{<testing::internal::UntypedFunctionMockerBase> = {<No data fields>}, <No data fields>}}


Is there something special about using virtuals in thread? Turns out that there
is no gurantee that object is around when a thread tries to use it. so it's race
and cause crash.

*/

#include <functional>
#include <iostream>
#include <string>
#include <thread>

#define TO_FIX_ISSUE

namespace
{
  class Param
  {
  private:
    std::string m_param1{};
    std::string m_param2{};

  public:
    virtual ~Param() {}

  public:
    virtual std::string getParam_1()
    {
      std::cout << "Param::getParam_1\n";
      return "Param::getParam_1";
    }
  };

  class MockParam : public Param
  {
  public:
    std::string getParam_1() override
    {
      std::cout << "MockParam::getParam_1\n";
      return "MockParam::getParam_1";
    }
  };

  class Env
  {
  public:
    virtual ~Env() {}

  public:
    virtual Param *getParam() = 0;
  };

  class MockEnv : public Env
  {
  private:
    Param *m_param;

  public:
    MockEnv(Param *param)
        : m_param(param)
    {}

    Param *getParam() override { return m_param; }
  };

  class ExecutorThreaded
  {
  private:
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
#ifdef TO_FIX_ISSUE
      // m_thread.join();
#endif
    }

#ifdef TO_FIX_ISSUE
    void wait() { m_thread.join(); }
#endif

    void execute_()
    {
      // firstly, get the param object
      auto param = m_env->getParam();

      // then call param_1
      auto param_1 = param->getParam_1();
      std::cout << "ExecutorThreaded: getParam_1: param_1 is " << param_1
                << "\n";

      // std::string param_1{};
      // for (int i = 0; i < 10; ++i)
      // {
      //   // firstly, get the param object
      //   Param *param = m_env->getParam();
      //   // then call param_1
      //   param_1 = param->getParam_1();
      //   std::cout << "Executor:doFunction1: param_1 is " << param_1 << "\n";
      // }

      return;
    }
  };
} // namespace

int main()
{
  MockParam *m_param{nullptr};

  std::shared_ptr<MockEnv> m_env{};

  std::unique_ptr<ExecutorThreaded> m_executor{};

  {
    std::cout << "creates objects()\n";

    // use mocks and pass m_param than using ON_CALL
    // ON_CALL(*m_env, getParam).WillByDefault(Return(m_param));

    m_param = new MockParam;
    m_env   = std::make_shared<MockEnv>(m_param);
  }

  std::cout << "m_param->getParam_1(): " << m_param->getParam_1() << "\n";

  // create executor, "OUT"
  m_executor = std::make_unique<ExecutorThreaded>(m_env);

#ifdef TO_FIX_ISSUE
  m_executor->wait();
#endif

  {
    delete m_param;
  }
}
