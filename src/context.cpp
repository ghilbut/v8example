#include <cassert>
#include "context.h"



v8::Local<v8::ObjectTemplate> GetGlobal(v8::Isolate * isolate) {

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(
        v8::String::NewFromUtf8(isolate, "log"),
        v8::FunctionTemplate::New(isolate, &Context::LogCallback));

    global->SetAccessor(
        v8::String::NewFromUtf8(isolate, "onrequest"),
        &Context::AccessorGetterCallback,
        &Context::AccessorSetterCallback);
    global->SetAccessor(
        v8::String::NewFromUtf8(isolate, "x"),
        &Context::AccessorGetterCallback,
        &Context::AccessorSetterCallback);

  return global;
}



Context::Context(v8::Isolate * isolate)
  : isolate_(isolate)
  , isolate_scope_(isolate)
  , handle_scope_(isolate)
  , context_(v8::Context::New(isolate, nullptr, GetGlobal(isolate)))
  , context_scope_(context_) {
  isolate_->SetData(0, this);
}

Context::~Context() {
  // nothing
}

bool Context::Execute(
    const std::string & script,
    const std::string & filename) {

  v8::Local<v8::String> source =
    v8::String::NewFromUtf8(isolate_, script.c_str(), v8::NewStringType::kNormal)
      .ToLocalChecked();
  v8::Local<v8::String> from =
    v8::String::NewFromUtf8(isolate_, filename.c_str(), v8::NewStringType::kNormal)
      .ToLocalChecked();
  return Execute(source, from, true, true);
}

void Context::Call() {
  v8::Local<v8::Object> global = context_->Global();
  v8::Local<v8::String> name =
    v8::String::NewFromUtf8(isolate_, "callback", v8::NewStringType::kNormal)
      .ToLocalChecked();
  v8::Local<v8::Value> callback = global->Get(name);
  if (callback->IsFunction()) {
    v8::Handle<v8::Value> argv[] = {};
    v8::Handle<v8::Function> f = v8::Handle<v8::Function>::Cast(callback);
    v8::Handle<v8::Value> retval = f->Call(global, 0, argv);
    v8::String::Utf8Value utf8f(isolate_, retval);
    printf("%s\n", *utf8f);
  }
  else {
    fprintf(stderr, "callback is not function\n");
  }
}

void Context::FireOnRequest() {
  v8::Local<v8::Object> global = context_->Global();
  v8::Handle<v8::Value> argv[] = {};
  v8::Local<v8::Function> f = v8::Local<v8::Function>::New(isolate_, onrequest_);
  v8::Handle<v8::Value> retval = f->Call(global, 0, argv);
  v8::String::Utf8Value utf8f(isolate_, retval);
  printf("%s\n", *utf8f);
}



// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

void Context::LogCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate * isolate = info.GetIsolate();
  v8::HandleScope handle_scope(isolate);

  if (info.IsConstructCall()) {
    v8::Local<v8::String> message = v8::String::NewFromUtf8(isolate, "log is not not a constructor");
    isolate->ThrowException(v8::Exception::TypeError(message));
  }

  std::string log;
  for (int i = 0, len = info.Length(); i < len; ++i) {
    if (i > 0) {
      log.append(" ");
    }
    v8::Local<v8::Value> value = info[i];
    v8::String::Utf8Value utf8(isolate, value);
    log.append(*utf8);
  }
  printf("%s\n", log.c_str());
}

void Context::AccessorGetterCallback(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value> & info) {
  printf("getter onrequest\n");
}

void Context::AccessorSetterCallback(
    v8::Local<v8::String> property,
    v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<void> &info) {

  v8::Isolate * iso = info.GetIsolate();
  v8::HandleScope scope(iso);

  if (value->IsFunction()) {
    Context * ctx = static_cast<Context*>(iso->GetData(0));
    v8::Persistent<v8::Function> & onrequest = ctx->onrequest_;
    v8::Handle<v8::Function> f = v8::Handle<v8::Function>::Cast(value);
    onrequest.Reset(iso, f);
  } else {
    v8::String::Utf8Value utf8(iso, property);
    char text[256];
    v8::Local<v8::String> message = v8::String::NewFromUtf8(iso, text, v8::NewStringType::kNormal).ToLocalChecked();
    iso->ThrowException(v8::Exception::TypeError(message));
  }
}






bool Context::Execute(
    v8::Local<v8::String> & code,
    v8::Local<v8::String> & from,
    bool print_result,
    bool report_exceptions) {

  v8::HandleScope handle_scope(isolate_);
  v8::TryCatch try_catch(isolate_);
  v8::ScriptOrigin origin(from);
  v8::Local<v8::Context> context(isolate_->GetCurrentContext());
  v8::Local<v8::Script> script;
  if (!v8::Script::Compile(context, code, &origin).ToLocal(&script)) {
    if (report_exceptions) {
      ReportException(&try_catch);
    }
    return false;
  }

  v8::Local<v8::Value> result;
  if (!script->Run(context).ToLocal(&result)) {
    assert(try_catch.HasCaught());
    if (report_exceptions) {
      ReportException(&try_catch);
    }
    return false;
  }

  assert(!try_catch.HasCaught());
  if (print_result && !result->IsUndefined()) {
    v8::String::Utf8Value utf8(isolate_, result);
    const char * cstr = ToCString(utf8);
    printf("%s\n", cstr);
  }
  return true;
}

void Context::ReportException(v8::TryCatch * try_catch) {

  fprintf(stderr, "Exception Report >>>>\n");

  v8::HandleScope handle_scope(isolate_);
  v8::String::Utf8Value exception(try_catch->Exception());
  const char * exception_string = ToCString(exception);
  v8::Local<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    fprintf(stderr, "%s\n", exception_string);
  }
  else {

    // Print (filename):(line number): (message). 
    v8::String::Utf8Value filename(message->GetScriptOrigin().ResourceName());
    v8::Local<v8::Context> context(isolate_->GetCurrentContext());
    const char * filename_string = ToCString(filename);
    const int linenum = message->GetLineNumber(context).FromJust();
    fprintf(stderr, "[Exception] %s:%i: %s\n", filename_string, linenum, exception_string);

    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine(context).ToLocalChecked());
    const char * sourceline_string = ToCString(sourceline);
    fprintf(stderr, "%s\n", sourceline_string);

    // Print wavy underline (GetUnderline is deprecated).
    const int start = message->GetStartColumn(context).FromJust();
    for (int i = 0; i < start; ++i) {
      fprintf(stderr, " "); 
    }
    const int end = message->GetEndColumn(context).FromJust();
    for (int i = 0; i < end; ++i) {
      fprintf(stderr, "^");
    }
    fprintf(stderr, "\n");

    // 
    v8::Local<v8::Value> stack_trace_string;
    if (try_catch->StackTrace(context).ToLocal(&stack_trace_string)
        && stack_trace_string->IsString()
        && v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
      v8::String::Utf8Value stack_trace(stack_trace_string);
      const char* stack_trace_string = ToCString(stack_trace);
      fprintf(stderr, "[STACK] %s\n", stack_trace_string);
    }
  }

  fprintf(stderr, "<<<< Exception Report\n"); 
}

