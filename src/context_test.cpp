#include <string>
#include <gmock/gmock.h>

#include <libplatform/libplatform.h>
#include <v8.h>

#include <cstdio>

#include "context.h"







std::string rootpath;

GTEST_API_ int main(int argc, char** argv) {
  rootpath = argv[0];
printf("%s\n", rootpath.c_str());
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}



std::string code = R"JSCODE(

  log();
  log(1, 2, 3);
  log(log);
  log(log());
  log('----');

  callback = function () {
    return 'XYZ';
  };

  onrequest = function () {
    throw Exception('DDD');
    return 'requested';
  };

  x = 1;
  log("xx");

  throw Exception('CCC');

)JSCODE";



class ContextTest : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    v8::V8::InitializeICUDefaultLocation(rootpath.c_str());
    v8::V8::InitializeExternalStartupData(rootpath.c_str());
    platform_ = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform_.get());
    v8::V8::Initialize();

    create_params_.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate_ = v8::Isolate::New(create_params_);
  }

  static void TearDownTestCase() {
    isolate_->Dispose();
    delete create_params_.array_buffer_allocator;

    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
  }

 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

 protected:
  static std::unique_ptr<v8::Platform> platform_;
  static v8::Isolate::CreateParams create_params_;
  static v8::Isolate * isolate_;
};

std::unique_ptr<v8::Platform> ContextTest::platform_;
v8::Isolate::CreateParams ContextTest::create_params_;
v8::Isolate * ContextTest::isolate_;



TEST_F(ContextTest, test) {

  for (int i = 0; i < 10; ++i) {

    {

  Context * ctx = new Context(isolate_);
  ctx->Execute(code, "test");
  ctx->Call(); 
  ctx->FireOnRequest();
  delete ctx;

    }

    printf("\n[%d] ================\n", i);
  }

}



TEST_F(ContextTest, inspector) {

  Context * ctx = new Context(isolate_);
  ctx->Execute(code, "test");
  ctx->Call(); 
  ctx->FireOnRequest();
  delete ctx;
}
