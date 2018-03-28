#ifndef V8EXAMPLE_SRC_CONTEXT_H_
#define V8EXAMPLE_SRC_CONTEXT_H_

#include <string>
#include <v8.h>


class Context {
 public:
  Context(v8::Isolate * isolate);
  ~Context();

  bool Execute(
      const std::string & script,
      const std::string & filename);
  void Call();
  void FireOnRequest();


 public:
  static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AccessorGetterCallback(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value> & info);
  static void AccessorSetterCallback(
      v8::Local<v8::String> property,
      v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<void> &info);

 private:
  bool Execute(
      v8::Local<v8::String> & code,
      v8::Local<v8::String> & from,
      bool print_result,
      bool report_exceptions);
  void ReportException(v8::TryCatch * try_catch);


 private:
  v8::Isolate * isolate_;
  v8::Isolate::Scope isolate_scope_;
  v8::HandleScope handle_scope_;
  v8::Local<v8::Context> context_;
  v8::Context::Scope context_scope_;

 public:
  v8::Persistent<v8::Function> onrequest_;
};


#endif  // V8EXAMPLE_SRC_CONTEXT_H_
