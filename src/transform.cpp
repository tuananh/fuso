#include "jsoncons/json.hpp"
#include "jsoncons_ext/jsonpath/json_query.hpp"
#include <string>

using namespace jsoncons;
using namespace jsoncons::jsonpath;
using string = std::string;
using v8::Local;
using v8::String;
using v8::Object;
using v8::Array;
using v8::Isolate;

// forward declaration
void walk(json &root, string &key, json &node, Isolate *isolate,
          Local<Object> &output);

Local<Array> seek_array(json &root, string &key, json &path, Isolate *isolate) {
  Local<Array> arr = Array::New(isolate);
  string base_path = path[0].as<string>();
  json inner = path[1];
  json result = json_query(root, base_path);
  if (result.size() > 0) {
    json rs = result[0];
    // std::cout << key << " [seek_array] => " << path[0].as<string>() << std::endl;
    // std::cout << key << " [seek_array] => " << path[1].as<string>() << std::endl;
    // std::cout << key << " [seek_array] => " << rs.as<string>() << std::endl;
    for (size_t i = 0; i < rs.size(); ++i) {
      if (inner.is_object()) {
        Local<Object> obj = Object::New(isolate);

        for (const auto& kv : inner.object_range()) {
          string inner_key = kv.key();
          json inner_node = kv.value();
          walk(rs[i], inner_key, inner_node, isolate, obj);
        }
        const char *pkey = std::to_string(i).c_str();
        arr->Set(String::NewFromUtf8(isolate, pkey), obj);
      }

      // case string
    }
  }

  return arr;
}

Local<Object> seek_object(json &root, string &key, json &path,
                          Isolate *isolate) {
  // std::cout << key << " [type object] => " << path.as<std::string>() << std::endl;
  Local<Object> output = Object::New(isolate);
  for (auto it = path.object_range().begin(); it != path.object_range().end();
       ++it) {
    string inner_key = it->key();
    json inner_path = it->value();
    walk(root, inner_key, inner_path, isolate, output);
  }

  return output;
}

Local<String> seek_string(json &root, string &key, string &path,
                          Isolate *isolate) {
  json result = json_query(root, path);
  if (result.size() > 0) {
    return String::NewFromUtf8(isolate, result[0].as<string>().c_str());
  } else {
    string empty = "";
    return String::NewFromUtf8(isolate, empty.c_str());
  }
}

void walk(json &root, string &key, json &path, Isolate *isolate,
          Local<Object> &output) {
  if (path.is_array()) {
    output->Set(String::NewFromUtf8(isolate, key.c_str()),
                seek_array(root, key, path, isolate));
  } else if (path.is_object()) {
    output->Set(String::NewFromUtf8(isolate, key.c_str()),
                seek_object(root, key, path, isolate));
  } else if (path.is_string()) {
    string p = path.as<string>();
    output->Set(String::NewFromUtf8(isolate, key.c_str()),
                seek_string(root, key, p, isolate));
  }
}

void transform_json(string data, string fmt,
                    const Nan::FunctionCallbackInfo<v8::Value> &args,
                    Local<Object> &output) {
  Isolate *isolate = args.GetIsolate();
  json root = json::parse(data);
  json t = json::parse(fmt);

  for (auto it = t.object_range().begin(); it != t.object_range().end(); ++it) {
    string key = it->key();
    json path = it->value();
    walk(root, key, path, isolate, output);
  }
}