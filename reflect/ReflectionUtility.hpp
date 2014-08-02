#pragma once

#include <cassert>
#include <functional>
#include <iosfwd>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <vector>

namespace reflect // forward declarations
{
  template <class T> struct Binding;
  class DataInfo;
  class FunctionInfo;
  class TypeInfo;
  template <class... Args> struct TypeOfN;

  template <class T> const TypeInfo& TypeOf();
  template <class OStream> std::ostream& operator<<(OStream&, DataInfo const&);
  template <class OStream> std::ostream& operator<<(OStream&, FunctionInfo const&);
} // namespace reflect

namespace reflect // functions
{
  namespace detail
  {
    // Converts a std::function from one call-type to another.
    //  This allows us to do some shady stuff, like converting a
    //  function<void(T&, float)> to simply function<void()>.
    template <class CallT, class ResultCallT>
    std::function<ResultCallT> CastFunction(std::function<CallT> fn)
    {
      return reinterpret_cast<std::function<ResultCallT>&&>(fn);
    }
  } // namespace detail
} // namespace reflect

namespace reflect // types
{
  namespace detail
  {
    template <class... Args>
    using common = typename std::common_type<Args...>::type;

    template <class T>
    using decayed = typename std::decay<T>::type;

    template <unsigned... Indices>
    struct index_sequence
    {
      template <unsigned N>
      using append = index_sequence<Indices..., N>;
    };

    template <unsigned N>
    struct make_index_sequence
    {
      using type = typename make_index_sequence<N - 1>::type::template append<N - 1>;
    };

    template<>
    struct make_index_sequence <0>
    {
      using type = index_sequence<>;
    };

    template <class... Args>
    using index_sequence_for = typename make_index_sequence<sizeof...(Args)>::type;
  } // namespace detail

  // Wraps a const version of a map for public user access.
  template <class Key, class Value>
  struct ConstMapWrapper
  {
    typedef std::unordered_map<Key, Value> MapType;
    MapType* data;

    // Initializes from a reference to a map.
    ConstMapWrapper(MapType& map) :
      data(&map)
    {}

    // Begin const_iterator.
    auto begin() const -> decltype(data->cbegin())
    {
      return data->cbegin();
    }

    // End const_iterator.
    auto end() const -> decltype(data->cend())
    {
      return data->cend();
    }

    // Find an item by key.
    auto find(Key const& key) const -> decltype(data->cbegin())
    {
      return data->find(key);
    }

    // Access a value by key.
    Value const& operator[](Key const& key) const
    {
      auto it = find(key);
      assert(it != end() && "Key not found");
      return it->second;
    }
  };

  // Wraps a const version of a map for public user access. (for pointer values)
  template <class Key, class Value>
  struct ConstMapWrapper<Key, Value*>
  {
    typedef std::unordered_map<Key, Value*> MapType;
    MapType* data;

    // Initializes from a reference to a map.
    ConstMapWrapper(MapType& map) :
      data(&map)
    {}

    // Begin const_iterator.
    auto begin() const -> decltype(data->cbegin())
    {
      return data->cbegin();
    }

    // End const_iterator.
    auto end() const -> decltype(data->cend())
    {
      return data->cend();
    }

    // Find an item by key.
    auto find(Key const& key) const -> decltype(data->cbegin())
    {
      return data->find(key);
    }

    // Access a value by key.
    Value const& operator[](Key const& key) const
    {
      auto it = find(key);
      assert(it != end() && "Key not found");
      return *it->second;
    }
  };

  // Creates a vector of TypeInfo pointers given a sequence of types.
  //  (Recursive case)
  template <class T, class... Args>
  struct TypeOfN < T, Args... >
  {
    static std::vector<const TypeInfo*> Get(std::vector<const TypeInfo*> types = {})
    {
      types.push_back(&TypeOf<T>());
      return TypeOfN<Args...>::Get(move(types));
    }
  };

  // Creates a vector of TypeInfo pointers given a sequence of types.
  //  (Base case)
  template <>
  struct TypeOfN < >
  {
    static std::vector<const TypeInfo*> Get(std::vector<const TypeInfo*> types = {})
    {
      return move(types);
    }
  };

  // Stores nifty information on a function type.
  template <class CallT>
  struct FunctionTraits
  {
    static const bool IsFunction = false;
  };

  // Stores nifty information on a function type:
  //  for the call-type, e.g. int(float, void*).
  template <class R, class... Args>
  struct FunctionTraits < R(Args...) >
  {
    static const size_t         ArgumentCount = std::tuple_size<std::tuple<Args...>>::value;
    typedef std::tuple<Args...> ArgumentTuple;
    typedef                  R(*CFunctionType)(Args...);
    typedef void                ClassType;
    static const bool           IsConstMemberFunction = false;
    static const bool           IsFunction = true;
    static const bool           IsMemberFunction = false;
    typedef                  R(*Pointer)(Args...);
    typedef R                   ReturnType;

    // Returns the typeless version of the passed in function.
    static std::function<void()> GenerateTypelessFunction(R(*fn)(Args...))
    {
      // Reinterpret cast the typed function to a typeless function.
      return detail::CastFunction<R(Args...), void()>(fn);
    }

    // Returns an array of reflected argument types.
    static std::vector<const TypeInfo*> RuntimeArguments()
    {
      return TypeOfN<Args...>::Get();
    }

    // Returns the reflected return type.
    static const TypeInfo& RuntimeReturnType()
    {
      return TypeOf<R>();
    }
  };

  // Stores nifty information on a function type:
  //  for the C function type, e.g. int(*)(float, void*).
  template <class R, class... Args>
  struct FunctionTraits < R(*)(Args...) > :
    FunctionTraits < R(Args...) >
  {};

  // Stores nifty information on a function type:
  //  for std::function.
  template <class R, class... Args>
  struct FunctionTraits < std::function<R(Args...)> > :
    FunctionTraits < R(Args...) >
  {};

  // Stores nifty information on a function type:
  //  for the non-const member function, e.g. int(Foo::*)(float).
  template <class R, class T, class... Args>
  struct FunctionTraits < R(T::*)(Args...) > :
    FunctionTraits < R(T&, Args...) >
  {
    typedef T         ClassType;
    static const bool IsMemberFunction = true;
    typedef     R(T::*Pointer)(Args...);

    // Returns the typeless version of the passed in function.
    static std::function<void()> GenerateTypelessFunction(R(T::*fn)(Args...))
    {
      // Capture the member function into a lambda.
      auto lambda = [=](T& self, Args... args) -> R
      {
        return (self.*fn)(std::forward<Args>(args)...);
      };

      // Reinterpret cast the typed function to a typeless function.
      return detail::CastFunction<R(T&, Args...), void()>(lambda);
    }
  };

  // Stores nifty information on a function type:
  //  for the const member function, e.g. int(Foo::*)(float).
  template <class R, class T, class... Args>
  struct FunctionTraits <R(T::*)(Args...) const> :
    FunctionTraits < R(const T&, Args...) >
  {
    typedef T         ClassType;
    static const bool IsConstMemberFunction = true;
    static const bool IsMemberFunction = true;
    typedef     R(T::*Pointer)(Args...) const;

    // Returns the typeless version of the passed in function.
    static std::function<void()> GenerateTypelessFunction(R(T::*fn)(Args...) const)
    {
      // Capture the member function into a lambda.
      auto lambda = [=](const T& self, Args... args) -> R
      {
        return (self.*fn)(std::forward<Args>(args)...);
      };

      // Reinterpret cast the typed function to a typeless function.
      return detail::CastFunction<R(const T&, Args...), void()>(lambda);
    }
  };

  // Whether the given type is a pointer to a data 
  //  member or a global object. (member case)
  template <class T>
  struct IsField :
    std::is_member_object_pointer<T>
  {};

  // Whether the given type is a pointer to a data 
  //  member or a global object. (global case)
  template <class FieldT>
  struct IsField<FieldT*> :
    std::is_object<FieldT>
  {};

} // namespace reflect
