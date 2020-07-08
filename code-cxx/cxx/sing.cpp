
// original code with singleton
Response sendData(const Data &data)
{
  Request req;

  // transform Data into Request

  return CommSingleton::instance()->send(req);
}

1. makes source compatible by using default parameters 2. express
  dependancy on CommSingleton

    // x.h
    Response
    sendData(const Data &data, CommSingleton *comm = CommSingleton::instance());

// x.cpp
// transformation of original function with backward compatible non-singleton
// version
Response sendData(const Data &data, CommSingleton *comm)
{
  Request req;

  // transform Data into Request

  return comm.send(req);
}

1. use wrapper to replease singleton

  class CommWrapper
{
  enum
  {
    SERVICE_ID = 249409
  };

public:
  CommWrapper(int service_id = SERVICE_ID)
      : raw_client(service_id){...}

      Response send(const Request &req);

private:
  TcpClient raw_client;
};

struct Service
{
  static CommWrapper comm_;
}

1. back to original code

  // x.h
  Response
  sendData(const Data &data, CommWrapper &comm = Service::comm);

// x.cpp
// transformation of original function with backward compatible non-singleton
// version
Response sendData(const Data &data, CommWrapper &comm)
{
  Request req;

  // transform Data into Request

  return comm.send(req);
}

1. made it virtual

  class CommWrapper
{
  enum
  {
    SERVICE_ID = 249409
  };

public:
  CommWrapper(int service_id = SERVICE_ID)
      : raw_client(service_id)
  {
    ...
  }

  virtual Response send(const Request &req);

private:
  TcpClient raw_client;
};

1. drive off from CommWrapper 2. use test

  class CommTester : public CommWrapper
{
public:
  CommTester(Request &req)
      : req_(req)
  {}

  // override
  Response send(const Request &reg)
  {
    req_ = req;
    return Response();
  }

  Request &req_;
};

int TestSendData()
{
  Data rec;
  rec.id = 999;

  // fill in more rec values

  Request req;

  // sendData() will fill in `req` and CommTester use reference to take it back
  // so now we can see what sendData() do on Request `req`
  CommTester client(req);
  sendData(rec, client);

  if (req.senderId_ != rec.id)
    std::cout << "Error ..." << std::endl;

  // further validation of rec values...
}

1. use gmock

  class MockClient : public CommWrapper
{
public:
  MOCK_METHOD1(send, Response(const Request &));
};

TEST(X, sendData)
{
  MockClient client;
  Request req;

  EXPECT_CALL(client, send(_)).WillOnce(DoAll(SaveArg<0>(&req), return (resp)));

  Data rec;
  rec.id = 999;

  // fill in more rec values

  sendData(rec, client);

  ASSERT_EQ(req.senderId_, rec.id);

  // further validation of rec values...
}

1. use modern c++

  class CommWrapper
{
  enum
  {
    SERVICE_ID = 249409
  };

public:
  CommWrapper(int service_id = SERVICE_ID)
      : raw_client(service_id){...}

      Response
      operator()(Request req);

private:
  TcpClient raw_client;
};

struct Service
{
  static CommWrapper comm_;
}

using comms_func = std::function<Response(Request)>;

// x.h
Response sendData(const Data &data, comms_func comms = Service::comm_);

// x.cpp
Response sendData(const Data &data, comms_func comms)
{
  Request req;

  // transform Data into Request

  return comms(std::move(req));
}

1. to remove copy

  // x.h
  Response
  sendData(const Data &data, comms_func comms = std::ref(Service::comm_));

or

  using comms_func = std::function_ref<Response(Request)>;

class MockClient : public CommWrapper
{
public:
  MOCK_METHOD1(send, Response(const Request &));
  Response operator()(Request req) { return send(std::move(req)); }
};

TEST(X, sendData)
{
  MockClient client;

  Data rec;
  rec.id = 999;

  // fill in more rec values

  Request req;
  Response resp;

  EXPECT_CALL(client, send(_)).WillOnce(DoAll(SaveArg<0>(&req), return (resp)));

  sendData(rec, std::ref(client));

  ASSERT_EQ(req.senderId_, rec.id);

  // further validation of rec values...
}
