#pragma once

#include "DataInfo.hpp"
#include "FunctionInfo.hpp"
#include "NamespaceInfo.hpp"
#include "TypeInfo.hpp"

namespace reflect
{
  class Reflection
  {
  private: // data

    NamespaceInfo globalNamespace;

    std::unordered_map<std::string, const TypeInfo*> types;

  public: // properties

    NamespaceInfo const& GlobalNamespace = globalNamespace;

    std::unordered_map<std::string, const TypeInfo*> const& Types = types;

  public: // methods

    Reflection() :
      globalNamespace("", "")
    {
      // Register void.
      types["void"] = &TypeOf<void>();

      // Register fundamental types.
      RegisterFundamentalType<bool>();
      RegisterFundamentalType<char>();
      RegisterFundamentalType<double>();
      RegisterFundamentalType<float>();
      RegisterFundamentalType<int>();
      RegisterFundamentalType<long>();
      RegisterFundamentalType<long long>();
      RegisterFundamentalType<short>();
      RegisterFundamentalType<signed char>();
      RegisterFundamentalType<unsigned char>();
      RegisterFundamentalType<unsigned short>();
      RegisterFundamentalType<unsigned int>();
      RegisterFundamentalType<unsigned long>();
      RegisterFundamentalType<unsigned long long>();
    }

    template <class... Args>
    void BindNamespace(std::string const& fullName, Args&&... args)
    {
      NamespaceInfo& info = GetNamespace(fullName);
      info.Bind(fullName, std::forward<Args>(args)...);
    }

    template <class T>
    void BindType()
    {
      static_assert(std::is_object<T>::value, "Only object types can be bound to Reflection.");

      // Bind the type.
      TypeInfo& type = detail::TypeOf<T>();
      Binding<T>{};
      types[type.Name] = &type;
    }

    NamespaceInfo& GetNamespace(std::string const& fullName)
    {
      auto splitName = SplitQualifiedName(fullName);
      NamespaceInfo* namespace_ = &globalNamespace;

      for (std::string const& namespaceName : splitName)
      {
        namespace_ = &namespace_->GetNamespace(namespaceName);
      }

      return *namespace_;
    }

    static Reflection& Instance()
    {
      static Reflection r;
      return r;
    }

    template <class OStream>
    friend std::ostream& operator<<(OStream& os, const Reflection& r)
    {
      os << "Reflection ";
      for (auto& type : r.types)
      {
        os << "\n" << "  " << type.second->Name;
        os << "\n" << "    " << "Fields ";
        for (auto& field : type.second->Fields)
          os << "\n" << "      " << field;
        os << "\n" << "    " << "Methods ";
        for (auto& method : type.second->Methods)
          os << "\n" << "      " << method;
      }

      return os;
    }

  private: // methods

    template <class T>
    TypeInfo& RegisterFundamentalType()
    {
      // Register the type and set its name.
      TypeInfo& type = detail::TypeOf<T>();
      type.name = typeid(T).name();
      types[type.Name] = &type;

      return type;
    }

    static std::vector<std::string> SplitQualifiedName(std::string name)
    {
      std::vector<std::string> vec;

      // For each scope qualification "::":
      for (auto colon = name.find("::"); colon != name.npos; colon = name.find("::"))
      {
        // Push back the substring before the scope.
        vec.push_back(name.substr(0, colon));

        // Update the substring to be the rest.
        name = name.substr(colon + 2);
      }

      // Push back the last part of the name.
      vec.push_back(name);

      return std::move(vec);
    }
  };

  // Base class for all binding structs.
  template <class Type>
  class BindingBase
  {
  public: // types

    typedef Type T;

    // Binds the type to reflection on construction.
    struct AutoBind
    {
      AutoBind()
      {
        Reflection::Instance().BindType<T>();
      }
    };

  public: // data

    // Global object to construct and 
    //  automatically bind the type.
    static AutoBind autoBind;

  public: // methods

    // Force a reference to the global object.
    ~BindingBase()
    {
      &autoBind;
    }

    // Binds all members to the type.
    template <class... Args>
    static void Bind(std::string fullName, Args&&... args)
    {
      detail::TypeOf<T>().template Bind<T>(
        std::move(fullName),
        Constructor<>(), TagConstructor,
        Constructor<T const&>(), TagConstructor,
        Destructor(), TagDestructor,
        RightShift<std::istream, T&>(), TagRightShift,
        std::forward<Args>(args)...);
    }

    // Binds members of a namespace.
    template <class... Args>
    static void BindNamespace(std::string const& fullName, Args&&... args)
    {
      Reflection::Instance().BindNamespace(fullName, std::forward<Args>(args)...);
    }

  protected: // methods

    // Helper struct to retrieve a function pointer to a constructor.
    template <class... Args>
    struct ConstructorHelper
    {
      // Returns a C-style function pointer to the constructor.
      template <class U, class = decltype( U(std::declval<Args>()...) )>
      static auto Get() -> void(*)(void*, Args...)
      {
        return [](void* this_, Args... args) 
        { 
          new (this_) T(std::forward<Args>(args)...);
        };
      }

      // Returns nullptr, because the requested constructor does not exist
      //  or is not accessible from the current context.
      template <class U, class... Ignored>
      static auto Get(Ignored..., ...) -> void(*)(void*, Args...)
      {
        return nullptr;
      }
    };

    template <class... Args>
    static auto Constructor() -> void(*)(void*, Args...)
    {
      return ConstructorHelper<Args...>::template Get<T>();
    }

    // Helper struct to retrive a function pointer to a destructor.
    struct DestructorHelper
    {
      // Returns a C-style function pointer to the destructor.
      template <class U, class = typename std::enable_if<std::is_destructible<U>::value>::type>
      static auto Get() -> void(*)(void*)
      {
        return [](void* this_) 
        {
          reinterpret_cast<T*>(this_)->~T();
        };
      }

      // Returns nullptr, because the destructor is inaccessible.
      template <class U, class... Ignored>
      static auto Get(Ignored..., ...) -> void(*)(void*)
      {
        return nullptr;
      }
    };

    static auto Destructor() -> void(*)(void*)
    {
      return DestructorHelper::template Get<T>();
    }

    template <class IStream>
    struct RightShiftHelper
    {
      template <class Object, class = decltype(std::declval<IStream&>() >> std::declval<Object>())>
      static auto Get() -> IStream&(*)(IStream&, Object)
      {
        return [](IStream& s, Object o) -> IStream& { return s >> o; };
      }

      template <class Object, class... Ignored>
      static auto Get(Ignored..., ...) -> IStream&(*)(IStream&, Object)
      {
        return nullptr;
      }
    };

    template <class IStream, class Object>
    static auto RightShift() -> IStream&(*)(IStream&, Object)
    {
      return RightShiftHelper<IStream>::template Get<Object>();
    }
  };
  template <class Type> typename BindingBase<Type>::AutoBind BindingBase<Type>::autoBind;

  // All bindings to reflection specialize this struct to add in describe all members
  //  of the object.
  template <class Type> struct Binding : BindingBase<Type>
  {};
} // namespace reflect

namespace reflect
{
  template <class OStream>
  inline std::ostream& operator<<(OStream& os, const DataInfo& f)
  {
    return os << f.Type->Name << " " << f.Name;
  }

  template <class OStream>
  inline std::ostream& operator<<(OStream& os, const FunctionInfo& mi)
  {
    os << mi.ReturnType->Name << " " << mi.Name << "(";
    if (mi.ArgumentTypes.size())
    {
      os << mi.ArgumentTypes[0]->Name;
    }
    for (size_t i = 1; i < mi.ArgumentTypes.size(); ++i)
    {
      os << ", " << mi.ArgumentTypes[i]->Name;
    }
    return os << ")";
  }
} // namespace reflect

// Macro for type binding to save redundant typing.
#define refltype(T, ...) namespace reflect { template<> struct Binding<T> : BindingBase<T> { Binding() { Bind(#T, ##__VA_ARGS__); } };  }
