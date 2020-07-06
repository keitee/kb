/*
*/

#include <functional>
#include <iostream>
#include <string>
#include <thread>

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
      // m_thread.join();
    }

    void wait() { m_thread.join(); }

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

  m_executor->wait();

  {
    delete m_param;
  }
}
