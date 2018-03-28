#include "inspector.h"



InspectorClient::InspectorClient(v8::Isolate * isolate) {
}

InspectorClient::~InspectorClient() {
}

void InspectorClient::runMessageLoopOnPause(int contextGroupId) {
}

void InspectorClient::quitMessageLoopOnPause() {
}





InspectorChannel::InspectorChannel() {
}

InspectorChannel::~InspectorChannel() {
}

void InspectorChannel::sendProtocolResponse(
      int callId,
      const StringView& message) {
}

void InspectorChannel::sendProtocolNotification(const StringView & message) {
}

void InspectorChannel::flushProtocolNotifications() {
}

std::unique_ptr<V8InspectorSession> InspectorChannel::connect(
      int contextGroupId,
      Channel * channel,
      const StringView & state) {

  return nullptr;
}

std::unique_ptr<V8StackTrace> InspectorChannel::createStackTrace(
      v8::Local<v8::StackTrace> stack_trace) {

  return nullptr;
}

std::unique_ptr<V8StackTrace> InspectorChannel::captureStackTrace(bool fullStack) {

  return nullptr;
}





Inspector::Inspector(v8::Isolate * isolate)
  : client_(isolate)
  , inspector_(isolate, &client_) {
}

Inspector::~Inspector() {
}

Inspector::Start() {

  const std::unique_ptr<v8_inspector::StringBuffer> name_buffer(Utf8ToStringView(name));
  const v8_inspector::V8ContextInfo info(context, CONTEXT_GROUP_ID, name_buffer->string());
  inspector_->contextCreated(info);
}
