# Fixpp

Fixpp is a modern C++ FIX parser distributed as a **header-only** library. Fixpp is entirely written in pure C++11 and uses last features of C++11 and C++14 like *variadic templates* or *using aliases*. Primary goal of fixpp is to provide an entire **type-safe** FIX frame parser (visitor) as well as a fully featured serializer.

Fixpp does not aim to be a fully-featured FIX Engine able to connect and receive frames for FIX providers through a TCP or UDP connection. It does not handle any transport at all.

# API Usage

```cpp
using namespace Fix;
```

Fixpp provides a **type-safe** API, meaning that everything in the FIX world is represented as a C++ type in the API.
For example, a `Logon` message is represented as an instance of the `Fix::v42::Message::Logon` class. Also note that FIX versions are represented as sub-namespaces of the root `Fix` namespace.

## Getting/setting fields

Fixpp provides getters and setters to get and set fields inside a specific Message.

Use `Fix::set<T>` to **set** fields:

```cpp
Fix::v42::Message::Logon logon;
Fix::set<Fix::Tag::EncryptMethod>(logon, 0);
Fix::set<Fix::Tag::HeartBtInt>(logon, 60);
```

Use `Fix::get<T>` to **get** fields:

```cpp
auto heartbeat = Fix::get<Fix::Tag::HeartBtInt>(logon);
```

## Type-safety

As stated above, everything in fixpp is represented as a C++ type. This allows to

* Check that a tag is valid when attempting to set/get it from a message
* Check the type validity when attempting to set the value of a tag

For example, attempting to set an invalid value for a tag will trigger a **compile-time** error through a static assertion

```cpp
Fix::v42::Message::Logon logon;
Fix::set<Fix::Tag::HeartBtInt>(logon, "60s");
```

```sh
fixpp/include/fixpp/dsl/message.h:116:9: error: static assertion failed: Invalid data type for given Tag
```

Attempting to retrieve an invalid tag from a Message will also trigger a compile-time error.

## Frame parsing

Fixpp uses the [Visitor pattern](https://en.wikipedia.org/wiki/Visitor_pattern). If you are familiar with `boost::variant` or `std::variant`, fixpp features the exact same pattern. To visit a FIX frame, first define a `Callable` object that accepts every possible message that you want to handle:

```cpp
struct MyVisitor
{
     void operator()(const Fix::v42::Header& header, const Fix::v42::Message::Heartbeat& heartbeat)
     {
         // ...
     }
     
     void operator()(const Fix::v42::Header& header, const Fix::v42::Message::MarketDataSnapshot& snapshot)
     {
         // ...
     }
     
     template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
     {
     }
};
```

A generic, templated `operator()` must also be provided. Every non-handled Message will fall through this special overload. This is the equivalent of the `default` case of a `switch case` except that it is needed for the visitor to compile correctly.
