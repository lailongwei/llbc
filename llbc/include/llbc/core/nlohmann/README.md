# 概述

* LICENSE: MIT
* Github: [nlohmann](https://github.com/nlohmann/json)
* 当前版本: [version 3.11.3](https://github.com/nlohmann/json/releases/tag/v3.11.3)

# LLBC简单实用

* 只需要使用LLBCJson对象即可

* 详情见测试用例：TestCase_NlohmannJson

* 代码示例：

  * ```
          // instead, you could also write (which looks very similar to the JSON above)
          LLBCJson j2 = {
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
              LLBC_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Person, age, name)
            }
            
            // official demo13 Arbitrary types conversions(自定义类型数据添加宏：LLBC_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE)
            {
              // 从json 反序列化数据
              LLBCJson j = {
              {"name", "hello llbc json"},
                {"age", 11}
              };
              auto person = j.template get<Person>();
    
              // 从自定义数据序列化成json
              LLBCJson dump = person;
              std::cout << "person:" << dump.dump() << std::endl;
    
              // 更多高级的用法可以见官方稳定, 比如: adl_serializer
            }
    
    ```

# 官方部分文档



## Examples

Here are some examples to give you an idea how to use the class.

Beside the examples below, you may want to:

→ Check the [documentation](https://json.nlohmann.me/)\
→ Browse the [standalone example files](https://github.com/nlohmann/json/tree/develop/docs/examples)

Every API function (documented in the [API Documentation](https://json.nlohmann.me/api/basic_json/)) has a corresponding standalone example file. For example, the [`emplace()`](https://json.nlohmann.me/api/basic_json/emplace/) function has a matching [emplace.cpp](https://github.com/nlohmann/json/blob/develop/docs/examples/emplace.cpp) example file.

### Read JSON from a file

The `json` class provides an API for manipulating a JSON value. To create a `json` object by reading a JSON file:

```cpp
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// ...

std::ifstream f("example.json");
json data = json::parse(f);
```

### Creating `json` objects from JSON literals

Assume you want to create hard-code this literal JSON value in a file, as a `json` object:

```json
{
  "pi": 3.141,
  "happy": true
}
```

There are various options:

```cpp
// Using (raw) string literals and json::parse
json ex1 = json::parse(R"(
  {
    "pi": 3.141,
    "happy": true
  }
)");

// Using user-defined (raw) string literals
using namespace nlohmann::literals;
json ex2 = R"(
  {
    "pi": 3.141,
    "happy": true
  }
)"_json;

// Using initializer lists
json ex3 = {
  {"happy", true},
  {"pi", 3.141},
};
```

### JSON as first-class data type

Here are some examples to give you an idea how to use the class.

Assume you want to create the JSON object

```json
{
  "pi": 3.141,
  "happy": true,
  "name": "Niels",
  "nothing": null,
  "answer": {
    "everything": 42
  },
  "list": [1, 0, 2],
  "object": {
    "currency": "USD",
    "value": 42.99
  }
}
```

With this library, you could write:

```cpp
// create an empty structure (null)
json j;

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

// instead, you could also write (which looks very similar to the JSON above)
json j2 = {
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
```

Note that in all these cases, you never need to "tell" the compiler which JSON value type you want to use. If you want to be explicit or express some edge cases, the functions [`json::array()`](https://json.nlohmann.me/api/basic_json/array/) and [`json::object()`](https://json.nlohmann.me/api/basic_json/object/) will help:

```cpp
// a way to express the empty array []
json empty_array_explicit = json::array();

// ways to express the empty object {}
json empty_object_implicit = json({});
json empty_object_explicit = json::object();

// a way to express an _array_ of key/value pairs [["currency", "USD"], ["value", 42.99]]
json array_not_object = json::array({ {"currency", "USD"}, {"value", 42.99} });
```

### Serialization / Deserialization

#### To/from strings

You can create a JSON value (deserialization) by appending `_json` to a string literal:

```cpp
// create object from string literal
json j = "{ \"happy\": true, \"pi\": 3.141 }"_json;

// or even nicer with a raw string literal
auto j2 = R"(
  {
    "happy": true,
    "pi": 3.141
  }
)"_json;
```

Note that without appending the `_json` suffix, the passed string literal is not parsed, but just used as JSON string
value. That is, `json j = "{ \"happy\": true, \"pi\": 3.141 }"` would just store the string
`"{ "happy": true, "pi": 3.141 }"` rather than parsing the actual object.

The string literal should be brought into scope with `using namespace nlohmann::literals;`
(see [`json::parse()`](https://json.nlohmann.me/api/operator_literal_json/)).

The above example can also be expressed explicitly using [`json::parse()`](https://json.nlohmann.me/api/basic_json/parse/):

```cpp
// parse explicitly
auto j3 = json::parse(R"({"happy": true, "pi": 3.141})");
```

You can also get a string representation of a JSON value (serialize):

```cpp
// explicit conversion to string
std::string s = j.dump();    // {"happy":true,"pi":3.141}

// serialization with pretty printing
// pass in the amount of spaces to indent
std::cout << j.dump(4) << std::endl;
// {
//     "happy": true,
//     "pi": 3.141
// }
```

Note the difference between serialization and assignment:

```cpp
// store a string in a JSON value
json j_string = "this is a string";

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
```

[`.dump()`](https://json.nlohmann.me/api/basic_json/dump/) returns the originally stored string value.

Note the library only supports UTF-8. When you store strings with different encodings in the library, calling [`dump()`](https://json.nlohmann.me/api/basic_json/dump/) may throw an exception unless `json::error_handler_t::replace` or `json::error_handler_t::ignore` are used as error handlers.

#### To/from streams (e.g. files, string streams)

You can also use streams to serialize and deserialize:

```cpp
// deserialize from standard input
json j;
std::cin >> j;

// serialize to standard output
std::cout << j;

// the setw manipulator was overloaded to set the indentation for pretty printing
std::cout << std::setw(4) << j << std::endl;
```

These operators work for any subclasses of `std::istream` or `std::ostream`. Here is the same example with files:

```cpp
// read a JSON file
std::ifstream i("file.json");
json j;
i >> j;

// write prettified JSON to another file
std::ofstream o("pretty.json");
o << std::setw(4) << j << std::endl;
```

Please note that setting the exception bit for `failbit` is inappropriate for this use case. It will result in program termination due to the `noexcept` specifier in use.

#### Read from iterator range

You can also parse JSON from an iterator range; that is, from any container accessible by iterators whose `value_type` is an integral type of 1, 2 or 4 bytes, which will be interpreted as UTF-8, UTF-16 and UTF-32 respectively. For instance, a `std::vector<std::uint8_t>`, or a `std::list<std::uint16_t>`:

```cpp
std::vector<std::uint8_t> v = {'t', 'r', 'u', 'e'};
json j = json::parse(v.begin(), v.end());
```

You may leave the iterators for the range [begin, end):

```cpp
std::vector<std::uint8_t> v = {'t', 'r', 'u', 'e'};
json j = json::parse(v);
```

#### Custom data source

Since the parse function accepts arbitrary iterator ranges, you can provide your own data sources by implementing the `LegacyInputIterator` concept.

```cpp
struct MyContainer {
  void advance();
  const char& get_current();
};

struct MyIterator {
    using difference_type = std::ptrdiff_t;
    using value_type = char;
    using pointer = const char*;
    using reference = const char&;
    using iterator_category = std::input_iterator_tag;

    MyIterator& operator++() {
        MyContainer.advance();
        return *this;
    }

    bool operator!=(const MyIterator& rhs) const {
        return rhs.target != target;
    }

    reference operator*() const {
        return target.get_current();
    }

    MyContainer* target = nullptr;
};

MyIterator begin(MyContainer& tgt) {
    return MyIterator{&tgt};
}

MyIterator end(const MyContainer&) {
    return {};
}

void foo() {
    MyContainer c;
    json j = json::parse(c);
}
```

#### SAX interface

The library uses a SAX-like interface with the following functions:

```cpp
// called when null is parsed
bool null();

// called when a boolean is parsed; value is passed
bool boolean(bool val);

// called when a signed or unsigned integer number is parsed; value is passed
bool number_integer(number_integer_t val);
bool number_unsigned(number_unsigned_t val);

// called when a floating-point number is parsed; value and original string is passed
bool number_float(number_float_t val, const string_t& s);

// called when a string is parsed; value is passed and can be safely moved away
bool string(string_t& val);
// called when a binary value is parsed; value is passed and can be safely moved away
bool binary(binary_t& val);

// called when an object or array begins or ends, resp. The number of elements is passed (or -1 if not known)
bool start_object(std::size_t elements);
bool end_object();
bool start_array(std::size_t elements);
bool end_array();
// called when an object key is parsed; value is passed and can be safely moved away
bool key(string_t& val);

// called when a parse error occurs; byte position, the last token, and an exception is passed
bool parse_error(std::size_t position, const std::string& last_token, const detail::exception& ex);
```

The return value of each function determines whether parsing should proceed.

To implement your own SAX handler, proceed as follows:

1. Implement the SAX interface in a class. You can use class `nlohmann::json_sax<json>` as base class, but you can also use any class where the functions described above are implemented and public.
2. Create an object of your SAX interface class, e.g. `my_sax`.
3. Call `bool json::sax_parse(input, &my_sax)`; where the first parameter can be any input like a string or an input stream and the second parameter is a pointer to your SAX interface.

Note the `sax_parse` function only returns a `bool` indicating the result of the last executed SAX event. It does not return a  `json` value - it is up to you to decide what to do with the SAX events. Furthermore, no exceptions are thrown in case of a parse error - it is up to you what to do with the exception object passed to your `parse_error` implementation. Internally, the SAX interface is used for the DOM parser (class `json_sax_dom_parser`) as well as the acceptor (`json_sax_acceptor`), see file [`json_sax.hpp`](https://github.com/nlohmann/json/blob/develop/include/nlohmann/detail/input/json_sax.hpp).

### STL-like access

We designed the JSON class to behave just like an STL container. In fact, it satisfies the [**ReversibleContainer**](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer) requirement.

```cpp
// create an array using push_back
json j;
j.push_back("foo");
j.push_back(1);
j.push_back(true);

// also use emplace_back
j.emplace_back(1.78);

// iterate the array
for (json::iterator it = j.begin(); it != j.end(); ++it) {
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

// comparison
j == R"(["foo", 1, true, 1.78])"_json;  // true

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

// create an object
json o;
o["foo"] = 23;
o["bar"] = false;
o["baz"] = 3.141;

// also use emplace
o.emplace("weather", "sunny");

// special iterator member functions for objects
for (json::iterator it = o.begin(); it != o.end(); ++it) {
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
int foo_present = o.count("foo"); // 1
int fob_present = o.count("fob"); // 0

// delete an entry
o.erase("foo");
```


### Conversion from STL containers

Any sequence container (`std::array`, `std::vector`, `std::deque`, `std::forward_list`, `std::list`) whose values can be used to construct JSON values (e.g., integers, floating point numbers, Booleans, string types, or again STL containers described in this section) can be used to create a JSON array. The same holds for similar associative containers (`std::set`, `std::multiset`, `std::unordered_set`, `std::unordered_multiset`), but in these cases the order of the elements of the array depends on how the elements are ordered in the respective STL container.

```cpp
std::vector<int> c_vector {1, 2, 3, 4};
json j_vec(c_vector);
// [1, 2, 3, 4]

std::deque<double> c_deque {1.2, 2.3, 3.4, 5.6};
json j_deque(c_deque);
// [1.2, 2.3, 3.4, 5.6]

std::list<bool> c_list {true, true, false, true};
json j_list(c_list);
// [true, true, false, true]

std::forward_list<int64_t> c_flist {12345678909876, 23456789098765, 34567890987654, 45678909876543};
json j_flist(c_flist);
// [12345678909876, 23456789098765, 34567890987654, 45678909876543]

std::array<unsigned long, 4> c_array {{1, 2, 3, 4}};
json j_array(c_array);
// [1, 2, 3, 4]

std::set<std::string> c_set {"one", "two", "three", "four", "one"};
json j_set(c_set); // only one entry for "one" is used
// ["four", "one", "three", "two"]

std::unordered_set<std::string> c_uset {"one", "two", "three", "four", "one"};
json j_uset(c_uset); // only one entry for "one" is used
// maybe ["two", "three", "four", "one"]

std::multiset<std::string> c_mset {"one", "two", "one", "four"};
json j_mset(c_mset); // both entries for "one" are used
// maybe ["one", "two", "one", "four"]

std::unordered_multiset<std::string> c_umset {"one", "two", "one", "four"};
json j_umset(c_umset); // both entries for "one" are used
// maybe ["one", "two", "one", "four"]
```

Likewise, any associative key-value containers (`std::map`, `std::multimap`, `std::unordered_map`, `std::unordered_multimap`) whose keys can construct an `std::string` and whose values can be used to construct JSON values (see examples above) can be used to create a JSON object. Note that in case of multimaps only one key is used in the JSON object and the value depends on the internal order of the STL container.

```cpp
std::map<std::string, int> c_map { {"one", 1}, {"two", 2}, {"three", 3} };
json j_map(c_map);
// {"one": 1, "three": 3, "two": 2 }

std::unordered_map<const char*, double> c_umap { {"one", 1.2}, {"two", 2.3}, {"three", 3.4} };
json j_umap(c_umap);
// {"one": 1.2, "two": 2.3, "three": 3.4}

std::multimap<std::string, bool> c_mmap { {"one", true}, {"two", true}, {"three", false}, {"three", true} };
json j_mmap(c_mmap); // only one entry for key "three" is used
// maybe {"one": true, "two": true, "three": true}

std::unordered_multimap<std::string, bool> c_ummap { {"one", true}, {"two", true}, {"three", false}, {"three", true} };
json j_ummap(c_ummap); // only one entry for key "three" is used
// maybe {"one": true, "two": true, "three": true}
```

### JSON Pointer and JSON Patch

The library supports **JSON Pointer** ([RFC 6901](https://tools.ietf.org/html/rfc6901)) as alternative means to address structured values. On top of this, **JSON Patch** ([RFC 6902](https://tools.ietf.org/html/rfc6902)) allows describing differences between two JSON values - effectively allowing patch and diff operations known from Unix.

```cpp
// a JSON value
json j_original = R"({
  "baz": ["one", "two", "three"],
  "foo": "bar"
})"_json;

// access members with a JSON pointer (RFC 6901)
j_original["/baz/1"_json_pointer];
// "two"

// a JSON patch (RFC 6902)
json j_patch = R"([
  { "op": "replace", "path": "/baz", "value": "boo" },
  { "op": "add", "path": "/hello", "value": ["world"] },
  { "op": "remove", "path": "/foo"}
])"_json;

// apply the patch
json j_result = j_original.patch(j_patch);
// {
//    "baz": "boo",
//    "hello": ["world"]
// }

// calculate a JSON patch from two JSON values
json::diff(j_result, j_original);
// [
//   { "op":" replace", "path": "/baz", "value": ["one", "two", "three"] },
//   { "op": "remove","path": "/hello" },
//   { "op": "add", "path": "/foo", "value": "bar" }
// ]
```

### JSON Merge Patch

The library supports **JSON Merge Patch** ([RFC 7386](https://tools.ietf.org/html/rfc7386)) as a patch format. Instead of using JSON Pointer (see above) to specify values to be manipulated, it describes the changes using a syntax that closely mimics the document being modified.

```cpp
// a JSON value
json j_document = R"({
  "a": "b",
  "c": {
    "d": "e",
    "f": "g"
  }
})"_json;

// a patch
json j_patch = R"({
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
```

### Implicit conversions

Supported types can be implicitly converted to JSON values.

It is recommended to **NOT USE** implicit conversions **FROM** a JSON value.
You can find more details about this recommendation [here](https://www.github.com/nlohmann/json/issues/958).
You can switch off implicit conversions by defining `JSON_USE_IMPLICIT_CONVERSIONS` to `0` before including the `json.hpp` header. When using CMake, you can also achieve this by setting the option `JSON_ImplicitConversions` to `OFF`.

```cpp
// strings
std::string s1 = "Hello, world!";
json js = s1;
auto s2 = js.template get<std::string>();
// NOT RECOMMENDED
std::string s3 = js;
std::string s4;
s4 = js;

// Booleans
bool b1 = true;
json jb = b1;
auto b2 = jb.template get<bool>();
// NOT RECOMMENDED
bool b3 = jb;
bool b4;
b4 = jb;

// numbers
int i = 42;
json jn = i;
auto f = jn.template get<double>();
// NOT RECOMMENDED
double f2 = jb;
double f3;
f3 = jb;

// etc.
```

Note that `char` types are not automatically converted to JSON strings, but to integer numbers. A conversion to a string must be specified explicitly:

```cpp
char ch = 'A';                       // ASCII value 65
json j_default = ch;                 // stores integer number 65
json j_string = std::string(1, ch);  // stores string "A"
```

### Arbitrary types conversions

Every type can be serialized in JSON, not just STL containers and scalar types. Usually, you would do something along those lines:

```cpp
namespace ns {
    // a simple struct to model a person
    struct person {
        std::string name;
        std::string address;
        int age;
    };
}

ns::person p = {"Ned Flanders", "744 Evergreen Terrace", 60};

// convert to JSON: copy each value into the JSON object
json j;
j["name"] = p.name;
j["address"] = p.address;
j["age"] = p.age;

// ...

// convert from JSON: copy each value from the JSON object
ns::person p {
    j["name"].template get<std::string>(),
    j["address"].template get<std::string>(),
    j["age"].template get<int>()
};
```

It works, but that's quite a lot of boilerplate... Fortunately, there's a better way:

```cpp
// create a person
ns::person p {"Ned Flanders", "744 Evergreen Terrace", 60};

// conversion: person -> json
json j = p;

std::cout << j << std::endl;
// {"address":"744 Evergreen Terrace","age":60,"name":"Ned Flanders"}

// conversion: json -> person
auto p2 = j.template get<ns::person>();

// that's it
assert(p == p2);
```

#### Basic usage

To make this work with one of your types, you only need to provide two functions:

```cpp
using json = nlohmann::json;

namespace ns {
    void to_json(json& j, const person& p) {
        j = json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
    }

    void from_json(const json& j, person& p) {
        j.at("name").get_to(p.name);
        j.at("address").get_to(p.address);
        j.at("age").get_to(p.age);
    }
} // namespace ns
```

That's all! When calling the `json` constructor with your type, your custom `to_json` method will be automatically called.
Likewise, when calling `template get<your_type>()` or `get_to(your_type&)`, the `from_json` method will be called.

Some important things:

* Those methods **MUST** be in your type's namespace (which can be the global namespace), or the library will not be able to locate them (in this example, they are in namespace `ns`, where `person` is defined).
* Those methods **MUST** be available (e.g., proper headers must be included) everywhere you use these conversions. Look at [issue 1108](https://github.com/nlohmann/json/issues/1108) for errors that may occur otherwise.
* When using `template get<your_type>()`, `your_type` **MUST** be [DefaultConstructible](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible). (There is a way to bypass this requirement described later.)
* In function `from_json`, use function [`at()`](https://json.nlohmann.me/api/basic_json/at/) to access the object values rather than `operator[]`. In case a key does not exist, `at` throws an exception that you can handle, whereas `operator[]` exhibits undefined behavior.
* You do not need to add serializers or deserializers for STL types like `std::vector`: the library already implements these.

#### Simplify your life with macros

If you just want to serialize/deserialize some structs, the `to_json`/`from_json` functions can be a lot of boilerplate.

There are two macros to make your life easier as long as you (1) want to use a JSON object as serialization and (2) want to use the member variable names as object keys in that object:

- `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(name, member1, member2, ...)` is to be defined inside the namespace of the class/struct to create code for.
- `NLOHMANN_DEFINE_TYPE_INTRUSIVE(name, member1, member2, ...)` is to be defined inside the class/struct to create code for. This macro can also access private members.

In both macros, the first parameter is the name of the class/struct, and all remaining parameters name the members.

##### Examples

The `to_json`/`from_json` functions for the `person` struct above can be created with:

```cpp
namespace ns {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(person, name, address, age)
}
```

Here is an example with private members, where `NLOHMANN_DEFINE_TYPE_INTRUSIVE` is needed:

```cpp
namespace ns {
    class address {
      private:
        std::string street;
        int housenumber;
        int postcode;

      public:
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(address, street, housenumber, postcode)
    };
}
```

#### How do I convert third-party types?

This requires a bit more advanced technique. But first, let's see how this conversion mechanism works:

The library uses **JSON Serializers** to convert types to json.
The default serializer for `nlohmann::json` is `nlohmann::adl_serializer` (ADL means [Argument-Dependent Lookup](https://en.cppreference.com/w/cpp/language/adl)).

It is implemented like this (simplified):

```cpp
template <typename T>
struct adl_serializer {
    static void to_json(json& j, const T& value) {
        // calls the "to_json" method in T's namespace
    }

    static void from_json(const json& j, T& value) {
        // same thing, but with the "from_json" method
    }
};
```

This serializer works fine when you have control over the type's namespace. However, what about `boost::optional` or `std::filesystem::path` (C++17)? Hijacking the `boost` namespace is pretty bad, and it's illegal to add something other than template specializations to `std`...

To solve this, you need to add a specialization of `adl_serializer` to the `nlohmann` namespace, here's an example:

```cpp
// partial specialization (full specialization works too)
namespace nlohmann {
    template <typename T>
    struct adl_serializer<boost::optional<T>> {
        static void to_json(json& j, const boost::optional<T>& opt) {
            if (opt == boost::none) {
                j = nullptr;
            } else {
              j = *opt; // this will call adl_serializer<T>::to_json which will
                        // find the free function to_json in T's namespace!
            }
        }

        static void from_json(const json& j, boost::optional<T>& opt) {
            if (j.is_null()) {
                opt = boost::none;
            } else {
                opt = j.template get<T>(); // same as above, but with
                                           // adl_serializer<T>::from_json
            }
        }
    };
}
```

#### How can I use `get()` for non-default constructible/non-copyable types?

There is a way, if your type is [MoveConstructible](https://en.cppreference.com/w/cpp/named_req/MoveConstructible). You will need to specialize the `adl_serializer` as well, but with a special `from_json` overload:

```cpp
struct move_only_type {
    move_only_type() = delete;
    move_only_type(int ii): i(ii) {}
    move_only_type(const move_only_type&) = delete;
    move_only_type(move_only_type&&) = default;

    int i;
};

namespace nlohmann {
    template <>
    struct adl_serializer<move_only_type> {
        // note: the return type is no longer 'void', and the method only takes
        // one argument
        static move_only_type from_json(const json& j) {
            return {j.template get<int>()};
        }

        // Here's the catch! You must provide a to_json method! Otherwise, you
        // will not be able to convert move_only_type to json, since you fully
        // specialized adl_serializer on that type
        static void to_json(json& j, move_only_type t) {
            j = t.i;
        }
    };
}
```

#### Can I write my own serializer? (Advanced use)

Yes. You might want to take a look at [`unit-udt.cpp`](https://github.com/nlohmann/json/blob/develop/tests/src/unit-udt.cpp) in the test suite, to see a few examples.

If you write your own serializer, you'll need to do a few things:

- use a different `basic_json` alias than `nlohmann::json` (the last template parameter of `basic_json` is the `JSONSerializer`)
- use your `basic_json` alias (or a template parameter) in all your `to_json`/`from_json` methods
- use `nlohmann::to_json` and `nlohmann::from_json` when you need ADL

Here is an example, without simplifications, that only accepts types with a size <= 32, and uses ADL.

```cpp
// You should use void as a second template argument
// if you don't need compile-time checks on T
template<typename T, typename SFINAE = typename std::enable_if<sizeof(T) <= 32>::type>
struct less_than_32_serializer {
    template <typename BasicJsonType>
    static void to_json(BasicJsonType& j, T value) {
        // we want to use ADL, and call the correct to_json overload
        using nlohmann::to_json; // this method is called by adl_serializer,
                                 // this is where the magic happens
        to_json(j, value);
    }

    template <typename BasicJsonType>
    static void from_json(const BasicJsonType& j, T& value) {
        // same thing here
        using nlohmann::from_json;
        from_json(j, value);
    }
};
```

Be **very** careful when reimplementing your serializer, you can stack overflow if you don't pay attention:

```cpp
template <typename T, void>
struct bad_serializer
{
    template <typename BasicJsonType>
    static void to_json(BasicJsonType& j, const T& value) {
      // this calls BasicJsonType::json_serializer<T>::to_json(j, value);
      // if BasicJsonType::json_serializer == bad_serializer ... oops!
      j = value;
    }

    template <typename BasicJsonType>
    static void to_json(const BasicJsonType& j, T& value) {
      // this calls BasicJsonType::json_serializer<T>::from_json(j, value);
      // if BasicJsonType::json_serializer == bad_serializer ... oops!
      value = j.template get<T>(); // oops!
    }
};
```

### Specializing enum conversion

By default, enum values are serialized to JSON as integers. In some cases this could result in undesired behavior. If an enum is modified or re-ordered after data has been serialized to JSON, the later de-serialized JSON data may be undefined or a different enum value than was originally intended.

It is possible to more precisely specify how a given enum is mapped to and from JSON as shown below:

```cpp
// example enum type declaration
enum TaskState {
    TS_STOPPED,
    TS_RUNNING,
    TS_COMPLETED,
    TS_INVALID=-1,
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM( TaskState, {
    {TS_INVALID, nullptr},
    {TS_STOPPED, "stopped"},
    {TS_RUNNING, "running"},
    {TS_COMPLETED, "completed"},
})
```

The `NLOHMANN_JSON_SERIALIZE_ENUM()` macro declares a set of `to_json()` / `from_json()` functions for type `TaskState` while avoiding repetition and boilerplate serialization code.

**Usage:**

```cpp
// enum to JSON as string
json j = TS_STOPPED;
assert(j == "stopped");

// json string to enum
json j3 = "running";
assert(j3.template get<TaskState>() == TS_RUNNING);

// undefined json value to enum (where the first map entry above is the default)
json jPi = 3.14;
assert(jPi.template get<TaskState>() == TS_INVALID );
```

Just as in [Arbitrary Type Conversions](#arbitrary-types-conversions) above,

- `NLOHMANN_JSON_SERIALIZE_ENUM()` MUST be declared in your enum type's namespace (which can be the global namespace), or the library will not be able to locate it, and it will default to integer serialization.
- It MUST be available (e.g., proper headers must be included) everywhere you use the conversions.

Other Important points:

- When using `template get<ENUM_TYPE>()`, undefined JSON values will default to the first pair specified in your map. Select this default pair carefully.
- If an enum or JSON value is specified more than once in your map, the first matching occurrence from the top of the map will be returned when converting to or from JSON.

### Binary formats (BSON, CBOR, MessagePack, UBJSON, and BJData)

Though JSON is a ubiquitous data format, it is not a very compact format suitable for data exchange, for instance over a network. Hence, the library supports [BSON](https://bsonspec.org) (Binary JSON), [CBOR](https://cbor.io) (Concise Binary Object Representation), [MessagePack](https://msgpack.org), [UBJSON](https://ubjson.org) (Universal Binary JSON Specification) and [BJData](https://neurojson.org/bjdata) (Binary JData) to efficiently encode JSON values to byte vectors and to decode such vectors.

```cpp
// create a JSON value
json j = R"({"compact": true, "schema": 0})"_json;

// serialize to BSON
std::vector<std::uint8_t> v_bson = json::to_bson(j);

// 0x1B, 0x00, 0x00, 0x00, 0x08, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0x00, 0x01, 0x10, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

// roundtrip
json j_from_bson = json::from_bson(v_bson);

// serialize to CBOR
std::vector<std::uint8_t> v_cbor = json::to_cbor(j);

// 0xA2, 0x67, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0xF5, 0x66, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00

// roundtrip
json j_from_cbor = json::from_cbor(v_cbor);

// serialize to MessagePack
std::vector<std::uint8_t> v_msgpack = json::to_msgpack(j);

// 0x82, 0xA7, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0xC3, 0xA6, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00

// roundtrip
json j_from_msgpack = json::from_msgpack(v_msgpack);

// serialize to UBJSON
std::vector<std::uint8_t> v_ubjson = json::to_ubjson(j);

// 0x7B, 0x69, 0x07, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0x54, 0x69, 0x06, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x69, 0x00, 0x7D

// roundtrip
json j_from_ubjson = json::from_ubjson(v_ubjson);
```

The library also supports binary types from BSON, CBOR (byte strings), and MessagePack (bin, ext, fixext). They are stored by default as `std::vector<std::uint8_t>` to be processed outside the library.

```cpp
// CBOR byte string with payload 0xCAFE
std::vector<std::uint8_t> v = {0x42, 0xCA, 0xFE};

// read value
json j = json::from_cbor(v);

// the JSON value has type binary
j.is_binary(); // true

// get reference to stored binary value
auto& binary = j.get_binary();

// the binary value has no subtype (CBOR has no binary subtypes)
binary.has_subtype(); // false

// access std::vector<std::uint8_t> member functions
binary.size(); // 2
binary[0]; // 0xCA
binary[1]; // 0xFE

// set subtype to 0x10
binary.set_subtype(0x10);

// serialize to MessagePack
auto cbor = json::to_msgpack(j); // 0xD5 (fixext2), 0x10, 0xCA, 0xFE
```


## Supported compilers

Though it's 2023 already, the support for C++11 is still a bit sparse. Currently, the following compilers are known to work:

- GCC 4.8 - 12.0 (and possibly later)
- Clang 3.4 - 15.0 (and possibly later)
- Apple Clang 9.1 - 13.1 (and possibly later)
- Intel C++ Compiler 17.0.2 (and possibly later)
- Nvidia CUDA Compiler 11.0.221 (and possibly later)
- Microsoft Visual C++ 2015 / Build Tools 14.0.25123.0 (and possibly later)
- Microsoft Visual C++ 2017 / Build Tools 15.5.180.51428 (and possibly later)
- Microsoft Visual C++ 2019 / Build Tools 16.3.1+1def00d3d (and possibly later)
- Microsoft Visual C++ 2022 / Build Tools 19.30.30709.0 (and possibly later)

I would be happy to learn about other compilers/versions.

Please note:

- GCC 4.8 has a bug [57824](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=57824)): multiline raw strings cannot be the arguments to macros. Don't use multiline raw strings directly in macros with this compiler.

- Android defaults to using very old compilers and C++ libraries. To fix this, add the following to your `Application.mk`. This will switch to the LLVM C++ library, the Clang compiler, and enable C++11 and other features disabled by default.

  ```
  APP_STL := c++_shared
  NDK_TOOLCHAIN_VERSION := clang3.6
  APP_CPPFLAGS += -frtti -fexceptions
  ```

  The code compiles successfully with [Android NDK](https://developer.android.com/ndk/index.html?hl=ml), Revision 9 - 11 (and possibly later) and [CrystaX's Android NDK](https://www.crystax.net/en/android/ndk) version 10.

- For GCC running on MinGW or Android SDK, the error `'to_string' is not a member of 'std'` (or similarly, for `strtod` or `strtof`) may occur. Note this is not an issue with the code,  but rather with the compiler itself. On Android, see above to build with a newer environment.  For MinGW, please refer to [this site](https://tehsausage.com/mingw-to-string) and [this discussion](https://github.com/nlohmann/json/issues/136) for information on how to fix this bug. For Android NDK using `APP_STL := gnustl_static`, please refer to [this discussion](https://github.com/nlohmann/json/issues/219).

- Unsupported versions of GCC and Clang are rejected by `#error` directives. This can be switched off by defining `JSON_SKIP_UNSUPPORTED_COMPILER_CHECK`. Note that you can expect no support in this case.

The following compilers are currently used in continuous integration at [AppVeyor](https://ci.appveyor.com/project/nlohmann/json), [Cirrus CI](https://cirrus-ci.com/github/nlohmann/json), and [GitHub Actions](https://github.com/nlohmann/json/actions):

| Compiler                                                     | Operating System   | CI Provider    |
| ------------------------------------------------------------ | ------------------ | -------------- |
| Apple Clang 11.0.3 (clang-1103.0.32.62);  Xcode 11.7         | macOS 11.7.1       | GitHub Actions |
| Apple Clang 12.0.0 (clang-1200.0.32.29);  Xcode 12.4         | macOS 11.7.1       | GitHub Actions |
| Apple Clang 12.0.5 (clang-1205.0.22.11);  Xcode 12.5.1       | macOS 11.7.1       | GitHub Actions |
| Apple Clang 13.0.0 (clang-1300.0.29.3);   Xcode 13.0         | macOS 11.7.1       | GitHub Actions |
| Apple Clang 13.0.0 (clang-1300.0.29.3);   Xcode 13.1         | macOS 12.6.1       | GitHub Actions |
| Apple Clang 13.0.0 (clang-1300.0.29.30);  Xcode 13.2.1       | macOS 12.6.1       | GitHub Actions |
| Apple Clang 13.1.6 (clang-1316.0.21.2.3); Xcode 13.3.1       | macOS 12.6.1       | GitHub Actions |
| Apple Clang 13.1.6 (clang-1316.0.21.2.5); Xcode 13.4.1       | macOS 12.6.1       | GitHub Actions |
| Apple Clang 14.0.0 (clang-1400.0.29.102); Xcode 14.0         | macOS 12.6.1       | GitHub Actions |
| Apple Clang 14.0.0 (clang-1400.0.29.102); Xcode 14.0.1       | macOS 12.6.1       | GitHub Actions |
| Apple Clang 14.0.0 (clang-1400.0.29.202); Xcode 14.1         | macOS 12.6.1       | GitHub Actions |
| Clang 3.5.2                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 3.6.2                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 3.7.1                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 3.8.1                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 3.9.1                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 4.0.1                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 5.0.2                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 6.0.1                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 7.0.1                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 8.0.0                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 9.0.0                                                  | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 10.0.0                                                 | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 10.0.0 with GNU-like command-line                      | Windows-10.0.17763 | GitHub Actions |
| Clang 11.0.0 with GNU-like command-line                      | Windows-10.0.17763 | GitHub Actions |
| Clang 11.0.0 with MSVC-like command-line                     | Windows-10.0.17763 | GitHub Actions |
| Clang 11.0.0                                                 | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 12.0.0                                                 | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 12.0.0 with GNU-like command-line                      | Windows-10.0.17763 | GitHub Actions |
| Clang 13.0.0                                                 | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 13.0.0 with GNU-like command-line                      | Windows-10.0.17763 | GitHub Actions |
| Clang 14.0.0                                                 | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 14.0.0 with GNU-like command-line                      | Windows-10.0.17763 | GitHub Actions |
| Clang 15.0.0 with GNU-like command-line                      | Windows-10.0.17763 | GitHub Actions |
| Clang 15.0.4                                                 | Ubuntu 20.04.3 LTS | GitHub Actions |
| Clang 16.0.0 (16.0.0-++20221031071727+500876226c60-1~exp1~20221031071831.439) | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 4.8.5 (Ubuntu 4.8.5-4ubuntu2)                            | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 4.9.4                                                    | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 5.5.0                                                    | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 6.5.0                                                    | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 7.5.0                                                    | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 8.1.0 (i686-posix-dwarf-rev0, Built by MinGW-W64 project) | Windows-10.0.17763 | GitHub Actions |
| GCC 8.1.0 (x86_64-posix-seh-rev0, Built by MinGW-W64 project) | Windows-10.0.17763 | GitHub Actions |
| GCC 8.5.0                                                    | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 9.5.0                                                    | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 10.4.0                                                   | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 11.1.0                                                   | Ubuntu (aarch64)   | Cirrus CI      |
| GCC 11.3.0                                                   | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 12.2.0                                                   | Ubuntu 20.04.3 LTS | GitHub Actions |
| GCC 13.0.0 20220605 (experimental)                           | Ubuntu 20.04.3 LTS | GitHub Actions |
| Intel C++ Compiler 2021.5.0.20211109                         | Ubuntu 20.04.3 LTS | GitHub Actions |
| NVCC 11.0.221                                                | Ubuntu 20.04.3 LTS | GitHub Actions |
| Visual Studio 14 2015 MSVC 19.0.24241.7 (Build Engine version 14.0.25420.1) | Windows-6.3.9600   | AppVeyor       |
| Visual Studio 15 2017 MSVC 19.16.27035.0 (Build Engine version 15.9.21+g9802d43bc3 for .NET Framework) | Windows-10.0.14393 | AppVeyor       |
| Visual Studio 16 2019 MSVC 19.28.29912.0 (Build Engine version 16.9.0+57a23d249 for .NET Framework) | Windows-10.0.17763 | GitHub Actions |
| Visual Studio 16 2019 MSVC 19.28.29912.0 (Build Engine version 16.9.0+57a23d249 for .NET Framework) | Windows-10.0.17763 | AppVeyor       |
| Visual Studio 17 2022 MSVC 19.30.30709.0 (Build Engine version 17.0.31804.368 for .NET Framework) | Windows-10.0.20348 | GitHub Actions |


## Integration

[`json.hpp`](https://github.com/nlohmann/json/blob/develop/single_include/nlohmann/json.hpp) is the single required file in `single_include/nlohmann` or [released here](https://github.com/nlohmann/json/releases). You need to add

```cpp
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;
```

to the files you want to process JSON and set the necessary switches to enable C++11 (e.g., `-std=c++11` for GCC and Clang).

You can further use file [`include/nlohmann/json_fwd.hpp`](https://github.com/nlohmann/json/blob/develop/include/nlohmann/json_fwd.hpp) for forward-declarations. The installation of json_fwd.hpp (as part of cmake's install step), can be achieved by setting `-DJSON_MultipleHeaders=ON`.

### CMake

You can also use the `nlohmann_json::nlohmann_json` interface target in CMake.  This target populates the appropriate usage requirements for `INTERFACE_INCLUDE_DIRECTORIES` to point to the appropriate include directories and `INTERFACE_COMPILE_FEATURES` for the necessary C++11 flags.

#### External

To use this library from a CMake project, you can locate it directly with `find_package()` and use the namespaced imported target from the generated package configuration:

```cmake
# CMakeLists.txt
find_package(nlohmann_json 3.2.0 REQUIRED)
...
add_library(foo ...)
...
target_link_libraries(foo PRIVATE nlohmann_json::nlohmann_json)
```

The package configuration file, `nlohmann_jsonConfig.cmake`, can be used either from an install tree or directly out of the build tree.

#### Embedded

To embed the library directly into an existing CMake project, place the entire source tree in a subdirectory and call `add_subdirectory()` in your `CMakeLists.txt` file:

```cmake
# Typically you don't care so much for a third party library's tests to be
# run from your own project's code.
set(JSON_BuildTests OFF CACHE INTERNAL "")

# If you only include this third party in PRIVATE source files, you do not
# need to install it when your main project gets installed.
# set(JSON_Install OFF CACHE INTERNAL "")

# Don't use include(nlohmann_json/CMakeLists.txt) since that carries with it
# unintended consequences that will break the build.  It's generally
# discouraged (although not necessarily well documented as such) to use
# include(...) for pulling in other CMake projects anyways.
add_subdirectory(nlohmann_json)
...
add_library(foo ...)
...
target_link_libraries(foo PRIVATE nlohmann_json::nlohmann_json)
```

##### Embedded (FetchContent)

Since CMake v3.11,
[FetchContent](https://cmake.org/cmake/help/v3.11/module/FetchContent.html) can
be used to automatically download a release as a dependency at configure time.

Example:

```cmake
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(foo PRIVATE nlohmann_json::nlohmann_json)
```

**Note**: It is recommended to use the URL approach described above which is supported as of version 3.10.0. See
<https://json.nlohmann.me/integration/cmake/#fetchcontent> for more information.

#### Supporting Both

To allow your project to support either an externally supplied or an embedded JSON library, you can use a pattern akin to the following:

``` cmake
# Top level CMakeLists.txt
project(FOO)
...
option(FOO_USE_EXTERNAL_JSON "Use an external JSON library" OFF)
...
add_subdirectory(thirdparty)
...
add_library(foo ...)
...
# Note that the namespaced target will always be available regardless of the
# import method
target_link_libraries(foo PRIVATE nlohmann_json::nlohmann_json)
```

```cmake
# thirdparty/CMakeLists.txt
...
if(FOO_USE_EXTERNAL_JSON)
  find_package(nlohmann_json 3.2.0 REQUIRED)
else()
  set(JSON_BuildTests OFF CACHE INTERNAL "")
  add_subdirectory(nlohmann_json)
endif()
...
```

`thirdparty/nlohmann_json` is then a complete copy of this source tree.