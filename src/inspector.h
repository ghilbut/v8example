#ifndef V8EXAMPLE_SRC_INSPECTOR_H_
#define V8EXAMPLE_SRC_INSPECTOR_H_

#include <memory>
#include <v8.h>


class InspectorClient : public v8_inspector::V8InspectorClient {
 public:
  InspectorClient(v8::Isolate * isolate);
  virtual ~InspectorClient();

  virtual void runMessageLoopOnPause(int contextGroupId);
  virtual void quitMessageLoopOnPause();



 private:
  v8::Isolate * isolate_;
};



class InspectorChannel : public v8_inspector::V8Inspector::Channel {
 public:
  InspectorChannel();
  virtual ~InspectorChannel();

  virtual void sendProtocolResponse(
      int callId,
      const StringView& message);
  virtual void sendProtocolNotification(const StringView & message);
  virtual void flushProtocolNotifications();
  virtual std::unique_ptr<V8InspectorSession> connect(
      int contextGroupId,
      Channel * channel,
      const StringView & state);

  // API methods.
  virtual std::unique_ptr<V8StackTrace> createStackTrace(
      v8::Local<v8::StackTrace> stack_trace);
  virtual std::unique_ptr<V8StackTrace> captureStackTrace(bool fullStack);


 private:
};



class Inspector {
 public:
  Inspector(v8::Isolate * isolate);
  ~Inspector();


  void Start();


 private:
  InspectorClient client_;
  std::unique_ptr<v8_inspector::V8Inspector> inspector_;
};

#endif  // V8EXAMPLE_SRC_INSPECTOR_H_
