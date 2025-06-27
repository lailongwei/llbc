// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "core/nlohmann/TestCase_NlohmannJson.h"

// 自定义类和json之间的序列化反序列化
namespace
{
  // official demo13 Arbitrary types conversions
  class Person
  {
  public:
    int age;
    std::string name;
  };

  // 使用该宏, 并定义需要的字段, 可以实现Person 与 json之间的序列化反序列化(该宏实现了to_json/from_json方法)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Person, age, name)
    
}

TestCase_NlohmannJson::TestCase_NlohmannJson()
{
    
}

TestCase_NlohmannJson::~TestCase_NlohmannJson()
{
    
}

int TestCase_NlohmannJson::Run(int argc, char *argv[])
{
    std::cout <<"nlohmann/json test: " << std::endl;

    LLBC_String raw = "{\"key1\":11, \"key2\":\"hello json\", \"key3\":[12, 13], \"key4\":{\"subkey1\":true}}";

    // 自动回收json
    {
        std::shared_ptr<LLBC_Json> autoJson(new LLBC_Json(LLBC_Json::object()));
        *autoJson = LLBC_Json::parse(raw);
        std::cout << "dump auto json:" << autoJson->dump() << std::endl;
    }

    // 反序列化
    LLBC_Json json = LLBC_Json::parse(raw);
    std::cout <<"json:" << json.dump() <<std::endl;

    // 序列化
    LLBC_String &&dumpData = json.dump();
    std::cout <<"dumpData:" << dumpData <<std::endl;
    // 缩进4个空格
    dumpData = json.dump(4);
    std::cout <<"pretty dump dumpData:" << dumpData <<std::endl;

    // 设置空
    auto setNullJson = LLBC_Json::parse(raw);
    std::cout <<"setNullJson type:" << setNullJson.type_name() <<std::endl;
    setNullJson = nullptr;
    std::cout <<"setNullJson type:" << setNullJson.type_name() <<std::endl;

    // 设置字符串
    auto setStringJson = LLBC_Json::parse(raw);
    std::cout <<"setStringJson type:" << setStringJson.type_name() <<std::endl;
    setStringJson = "hello json";
    std::cout <<"setStringJson type:" << setStringJson.type_name() <<std::endl;

    // 设置数值
    json["key1"] = 500;
    // 设置字符串
    json["key2"] = "hello TestCase_NlohmannJson";
    // 设置数组
    json["key3"] = {55, 16};
    // 设置对象
    LLBC_Json subJson;
    subJson["subKey1"] = "hello sub obj";
    json["key5"] = subJson;
    std::cout <<"json after setters:" << json.dump() <<std::endl;

    // 获取值
    auto &&key2Value = json["key2"].get<std::string>();
    std::cout <<"json key2 value:" << key2Value <<std::endl;
    auto key1Value = json["key1"].get<int>();
    std::cout <<"json key1 value:" << key1Value <<std::endl;

    // 遍历数组
    auto &&arr = json["key3"];
    assert(arr.is_array());
    auto count = static_cast<int>(arr.size());
    for(int idx = 0; idx < count; ++idx)
    {
        auto elem = arr[idx].get<int>();
        std::cout << "json key3 array elem index:" << idx << ", value:" << elem << std::endl;
    }

    // 获取对象
    auto &&key4Object = json["key4"];
    assert(key4Object.is_object());
    std::cout <<"json key4 value:" << key4Object.dump() <<std::endl;

    // 获取bool值
    auto subkey1Value = key4Object["subkey1"];
    assert(subkey1Value.is_boolean());
    std::cout <<"json subkey1 value:" << subkey1Value.dump() <<std::endl;

    // 删除元素
    json.erase("key1");
    std::cout <<"json has key1?:" << json.contains("key1") <<std::endl;

    // official demo
    {
      // Using (raw) string literals and json::parse
        auto ex1 = LLBC_Json::parse(R"(
            {
              "pi": 3.141,
              "happy": true
            }
          )");

        std::cout <<"json ex1:" << ex1.dump() <<std::endl;

        // Using user-defined (raw) string literals
        auto ex2 = R"(
            {
              "pi": 3.141,
              "happy": true
            }
          )"_json;

        std::cout <<"json ex2:" << ex2.dump() <<std::endl;
        
        // Using initializer lists
        LLBC_Json ex3 = {
          {"happy", true},
          {"pi", 3.141},
        };

        std::cout <<"json ex3:" << ex3.dump() <<std::endl;
    }

    // official demo2
    {
      // create an empty structure (null)
      LLBC_Json j;

      // add a number that is stored as double (note the implicit conversion of j to an object)
      j["pi"] = 3.141;

      // add a Boolean that is stored as bool
      j["happy"] = true;

      // add a string that is stored as std::string
      j["name"] = "Niels";

      // add another null object by passing nullptr
      j["nothing"] = nullptr;

      // add an object inside the object
      j["answer"]["everything"] = 42;

      // add an array that is stored as std::vector (using an initializer list)
      j["list"] = { 1, 0, 2 };

      // add another object (using an initializer list of pairs)
      j["object"] = { {"currency", "USD"}, {"value", 42.99} };

      std::cout <<"json j:" << j.dump() <<std::endl;

      // instead, you could also write (which looks very similar to the JSON above)
      LLBC_Json j2 = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {
      {"everything", 42}
        }},
        {"list", {1, 0, 2}},
        {"object", {
      {"currency", "USD"},
      {"value", 42.99}
        }}
      };

      std::cout <<"json j2:" << j2 <<std::endl;
    }

    // official demo3
    {
      // a way to express the empty array []
      LLBC_Json empty_array_explicit = LLBC_Json::array();
      
      // ways to express the empty object {}
      LLBC_Json empty_object_implicit = LLBC_Json({});
      LLBC_Json empty_object_explicit = LLBC_Json::object();
      
      // a way to express an _array_ of key/value pairs [["currency", "USD"], ["value", 42.99]]
      LLBC_Json array_not_object = LLBC_Json::array({ {"currency", "USD"}, {"value", 42.99} });
      std::cout <<"json array_not_object:" << array_not_object.dump() <<std::endl;
    }

    // official demo4
    {
      // create object from string literal
      LLBC_Json j = "{ \"happy\": true, \"pi\": 3.141 }"_json;

      // or even nicer with a raw string literal
      auto j2 = R"(
        {
          "happy": true,
          "pi": 3.141
        }
      )"_json;

      // explicit conversion to string
      LLBC_String s = j.dump();    // {"happy":true,"pi":3.141}

      // serialization with pretty printing
      // pass in the amount of spaces to indent
      std::cout << j.dump(4) << std::endl;
      // {
      //     "happy": true,
      //     "pi": 3.141
      // }
    }

    // official demo5
    {
      // store a string in a JSON value
      LLBC_Json j_string = "this is a string";

      // retrieve the string value
      auto cpp_string = j_string.template get<std::string>();
      // retrieve the string value (alternative when a variable already exists)
      std::string cpp_string2;
      j_string.get_to(cpp_string2);

      // retrieve the serialized value (explicit JSON serialization)
      std::string serialized_string = j_string.dump();

      // output of original string
      std::cout << cpp_string << " == " << cpp_string2 << " == " << j_string.template get<std::string>() << '\n';
      // output of serialized value
      std::cout << j_string << " == " << serialized_string << std::endl;
    }

    // official demo6
    {
      // deserialize from standard input
      // LLBC_Json j;
      // std::cin >> j;
      //
      // // serialize to standard output
      // std::cout << j;
      //
      // // the setw manipulator was overloaded to set the indentation for pretty printing
      // std::cout << std::setw(4) << j << std::endl;
    }

    // official demo7
    {
      // read a JSON file
      // std::ifstream i("file.json");
      // LLBC_Json j;
      // i >> j;
      //
      // // write prettified JSON to another file
      // std::ofstream o("pretty.json");
      // o << std::setw(4) << j << std::endl;
    }

    // official demo8 STL-like access
    {
      // create an array using push_back
      LLBC_Json j;
      j.push_back("foo");
      j.push_back(1);
      j.push_back(true);

      // also use emplace_back
      j.emplace_back(1.78);

      // iterate the array
      for (LLBC_Json::iterator it = j.begin(); it != j.end(); ++it) {
        std::cout << *it << '\n';
      }

      // range-based for
      for (auto& element : j) {
        std::cout << element << '\n';
      }

      // getter/setter
      const auto tmp = j[0].template get<std::string>();
      j[1] = 42;
      bool foo = j.at(2);
      std::cout << "foo:" << foo << std::endl;

      // comparison
      auto equal = j == R"(["foo", 1, true, 1.78])"_json;  // true
      std::cout << "j equal r value?:" << equal << std::endl;

      // other stuff
      j.size();     // 4 entries
      j.empty();    // false
      j.type();     // json::value_t::array
      j.clear();    // the array is empty again

      // convenience type checkers
      j.is_null();
      j.is_boolean();
      j.is_number();
      j.is_object();
      j.is_array();
      j.is_string();
      std::cout << "j:" << j << std::endl;

      // create an object
      LLBC_Json o;
      o["foo"] = 23;
      o["bar"] = false;
      o["baz"] = 3.141;

      // also use emplace
      o.emplace("weather", "sunny");

      // special iterator member functions for objects
      for (LLBC_Json::iterator it = o.begin(); it != o.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
      }

      // the same code as range for
      for (auto& el : o.items()) {
        std::cout << el.key() << " : " << el.value() << "\n";
      }

      // even easier with structured bindings (C++17)
      for (auto& [key, value] : o.items()) {
        std::cout << key << " : " << value << "\n";
      }

      // find an entry
      if (o.contains("foo")) {
        // there is an entry with key "foo"
      }

      // or via find and an iterator
      if (o.find("foo") != o.end()) {
        // there is an entry with key "foo"
      }

      // or simpler using count()
      auto foo_present = o.count("foo"); // 1
      auto fob_present = o.count("fob"); // 0
      std::cout << "foo_present:" << foo_present << std::endl;
      std::cout << "fob_present:" << fob_present << std::endl;

      // delete an entry
      o.erase("foo");
      std::cout << "o:" << o << std::endl;
    }

    // official demo9 Conversion from STL containers
    {
      std::vector<int> c_vector {1, 2, 3, 4};
      LLBC_Json j_vec(c_vector);
      // [1, 2, 3, 4]
      std::cout << "j_vec:" << j_vec.dump() << std::endl;

      std::deque<double> c_deque {1.2, 2.3, 3.4, 5.6};
      LLBC_Json j_deque(c_deque);
      // [1.2, 2.3, 3.4, 5.6]
      std::cout << "j_deque:" << j_deque.dump() << std::endl;

      std::list<bool> c_list {true, true, false, true};
      LLBC_Json j_list(c_list);
      // [true, true, false, true]
      std::cout << "j_list:" << j_list.dump() << std::endl;

      std::forward_list<int64_t> c_flist {12345678909876, 23456789098765, 34567890987654, 45678909876543};
      LLBC_Json j_flist(c_flist);
      // [12345678909876, 23456789098765, 34567890987654, 45678909876543]
      std::cout << "j_flist:" << j_flist.dump() << std::endl;
      
      std::array<unsigned long, 4> c_array {{1, 2, 3, 4}};
      LLBC_Json j_array(c_array);
      // [1, 2, 3, 4]
      std::cout << "j_array:" << j_array.dump() << std::endl;

      std::set<std::string> c_set {"one", "two", "three", "four", "one"};
      LLBC_Json j_set(c_set); // only one entry for "one" is used
      // ["four", "one", "three", "two"]
      std::cout << "j_set:" << j_set.dump() << std::endl;

      std::unordered_set<std::string> c_uset {"one", "two", "three", "four", "one"};
      LLBC_Json j_uset(c_uset); // only one entry for "one" is used
      // maybe ["two", "three", "four", "one"]
      std::cout << "j_uset:" << j_uset.dump() << std::endl;

      std::multiset<std::string> c_mset {"one", "two", "one", "four"};
      LLBC_Json j_mset(c_mset); // both entries for "one" are used
      // maybe ["one", "two", "one", "four"]
      std::cout << "j_mset:" << j_mset.dump() << std::endl;

      std::unordered_multiset<std::string> c_umset {"one", "two", "one", "four"};
      LLBC_Json j_umset(c_umset); // both entries for "one" are used
      // maybe ["one", "two", "one", "four"]
      std::cout << "j_umset:" << j_umset.dump() << std::endl;
    }

    // official demo10 JSON Pointer and JSON Patch
    {
      // a JSON value
      LLBC_Json j_original = R"({
        "baz": ["one", "two", "three"],
        "foo": "bar"
      })"_json;

      // access members with a JSON pointer (RFC 6901)
      std::cout << "/baz/1:" << j_original["/baz/1"_json_pointer] << std::endl;
      // "two"

      // a JSON patch (RFC 6902)
      LLBC_Json j_patch = R"([
          { "op": "replace", "path": "/baz", "value": "boo" },
          { "op": "add", "path": "/hello", "value": ["world"] },
          { "op": "remove", "path": "/foo"}
        ])"_json;

      // apply the patch
      LLBC_Json j_result = j_original.patch(j_patch);
      // {
      //    "baz": "boo",
      //    "hello": ["world"]
      // }
      std::cout << "j_result:" << j_result.dump() << std::endl;

      // calculate a JSON patch from two JSON values
      std::cout << "patch from two JSON values:" << LLBC_Json::diff(j_result, j_original) << std::endl;
      // [
      //   { "op":" replace", "path": "/baz", "value": ["one", "two", "three"] },
      //   { "op": "remove","path": "/hello" },
      //   { "op": "add", "path": "/foo", "value": "bar" }
      // ]
    }

    // official demo11 JSON Merge Patch
    {
      // a JSON value
      LLBC_Json j_document = R"({
        "a": "b",
        "c": {
          "d": "e",
          "f": "g"
        }
      })"_json;

      // a patch
      LLBC_Json j_patch = R"({
        "a":"z",
        "c": {
          "f": null
        }
      })"_json;

      // apply the patch
      j_document.merge_patch(j_patch);
      // {
      //  "a": "z",
      //  "c": {
      //    "d": "e"
      //  }
      // }

      std::cout << "j_document:" << j_document << std::endl;
    }

    // official demo12 Implicit conversions
    {
      // strings
      std::string s1 = "Hello, world!";
      LLBC_Json js = s1;
      auto s2 = js.template get<std::string>();
      // NOT RECOMMENDED
      std::string s3 = js;
      std::string s4;
      s4 = js;

      // Booleans
      bool b1 = true;
      LLBC_Json jb = b1;
      auto b2 = jb.template get<bool>();
      std::cout << "b2:" << b2 << std::endl;
      
      // NOT RECOMMENDED
      bool b3 = jb;
      bool b4;
      b4 = jb;
      std::cout << "b3:" << b3 << std::endl;
      std::cout << "b4:" << b4 << std::endl;

      // numbers
      int i = 42;
      LLBC_Json jn = i;
      auto f = jn.template get<double>();
      std::cout << "f:" << f << std::endl;
      // NOT RECOMMENDED
      // double f2 = jb;
      // double f3;
      // f3 = jb;

      // etc.
    }

    // official demo13 Arbitrary types conversions(自定义类型数据添加宏：LLBC_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE)
    {
      // 从json 反序列化数据
      LLBC_Json j = {
      {"name", "hello llbc json"},
        {"age", 11}
      };
      std::cout << "j:" << j << std::endl;
      
      auto person = j.template get<Person>();
      // 从自定义数据序列化成json
      LLBC_Json dump = person;
      std::cout << "person:" << dump << std::endl;

      // 更多高级的用法可以见官方稳定, 比如: adl_serializer
    }

    std::cout <<"press any key to continue ..." <<std::endl;
    getchar();

    return 0;
}
