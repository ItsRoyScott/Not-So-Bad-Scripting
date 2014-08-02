#pragma once

#include "DataInfo.hpp"
#include "FunctionInfo.hpp"

namespace reflect
{
  // Meta information regarding a type in C++.
  class TypeInfo
  {
  private: // data

    const std::type_info*       cppType = nullptr;
    std::vector<DataInfo>      fields;
    bool                        isReference = false;
    std::vector<FunctionInfo>     methods;
    std::string                 name = "NotBoundToReflection";
    std::string                 namespaceName = "NotBoundToReflection";
    size_t                      size = 0;
    const TypeInfo*             valueType = nullptr;

    friend class Reflection;

  public: // properties

    // Array of this type's fields.
    std::vector<DataInfo> const& Fields = fields;

    // Whether this type represents a reference to a value-type.
    bool const& IsReference = isReference;

    // Array of this type's methods.
    std::vector<FunctionInfo> const& Methods = methods;

    // Name of the type.
    std::string const& Name = name;

    // Namespace string for the type.
    std::string const& NamespaceName = namespaceName;

    // Size of the type in bytes.
    size_t const& Size = size;

    // The value-type if this type is a reference.
    TypeInfo const* const& ValueType = valueType;

  public: // const methods

    // Finds a field using a predicate function. The signature
    //  of the predicate is bool(const DataInfo&).
    template <class Predicate>
    const DataInfo* FindFieldBy(Predicate pred) const
    {
      for (const DataInfo& field : fields)
      {
        if (pred(field)) return &field;
      }
      return nullptr;
    }

    // Finds a method using a predicate function. The signature
    //  of the predicate is bool(const FunctionInfo&).
    template <class Predicate>
    const FunctionInfo* FindMethodBy(Predicate pred) const
    {
      for (const FunctionInfo& method : methods)
      {
        if (pred(method)) return &method;
      }
      return nullptr;
    }

    // Finds a field by name. (May return null)
    const DataInfo* GetField(const std::string& name_) const
    {
      return FindFieldBy([&](const DataInfo& f) { return f.Name == name_; });
    }

    // Finds a method by name. (May return null)
    const FunctionInfo* GetMethod(const std::string& name_) const
    {
      return FindMethodBy([&](const FunctionInfo& m) { return m.Name == name_; });
    }

  public: // non-const methods

    TypeInfo() = default;
    TypeInfo(const TypeInfo&) = delete;
    TypeInfo& operator=(const TypeInfo&) = delete;
    ~TypeInfo() = default;

    // Add base-case; does nothing.
    template <class T>
    void Add()
    {}

    // Adds a field given its name and pointer.
    template <class T, class ValueT, class... Args>
    typename std::enable_if<IsField<ValueT*>::value>::type
      Add(std::string name, ValueT* ptr, Args&&... args)
    {
      detail::NotifyTypeBuilders<T>().NewStaticData(name, ptr);
      fields.emplace_back(move(name), ptr);
      Add<T>(std::forward<Args>(args)...);
    }

    template <class T, class ValueT, class... Args>
    void Add(std::string name, ValueT T::*ptr, Args&&... args)
    {
      detail::NotifyTypeBuilders<T>().NewMemberData(name, ptr);
      fields.emplace_back(move(name), ptr);
      Add<T>(std::forward<Args>(args)...);
    }

    // Adds a static copy constructor given its name pointer.
    template <class T, class... Args>
    void Add(void(*ptr)(void*, T const&), struct Constructor_*, Args&&... args)
    {
      if (ptr)
      {
        detail::NotifyTypeBuilders<T>().NewCopyConstructor(name, ptr);
        methods.emplace_back(Name, ptr);
      }
      Add<T>(std::forward<Args>(args)...);
    }

    // Adds a static default constructor given its pointer.
    template <class T, class... Args>
    void Add(void(*ptr)(void*), struct Constructor_*, Args&&... args)
    {
      if (ptr)
      {
        detail::NotifyTypeBuilders<T>().NewDefaultConstructor(name, ptr);
        methods.emplace_back(Name, ptr);
      }
      Add<T>(std::forward<Args>(args)...);
    }

    // Adds a static constructor given its pointer.
    template <class T, class CtorArg0, class... CtorArgs, class... Args>
    typename std::enable_if<!std::is_same<CtorArg0, T const&>::value>::type
      Add(void(*ptr)(void*, CtorArg0, CtorArgs...), struct Constructor_*, Args&&... args)
    {
      if (ptr)
      {
        detail::NotifyTypeBuilders<T>().NewConstructor(name, ptr);
        methods.emplace_back(Name, ptr);
      }
      Add<T>(std::forward<Args>(args)...);
    }

    // Adds a static destructor given its pointer.
    template <class T, class... Args>
    void Add(void(*ptr)(void*), struct Destructor_*, Args&&... args)
    {
      if (ptr)
      {
        std::string dtorName = "~" + Name;
        detail::NotifyTypeBuilders<T>().NewDestructor(dtorName, ptr);
        methods.emplace_back(std::move(dtorName), ptr);
      }
      Add<T>(std::forward<Args>(args)...);
    }

    // Adds a static function given its name and pointer.
    template <class T, class RetT, class... FuncArgs, class... Args>
    void Add(std::string name, RetT(*ptr)(FuncArgs...), Args&&... args)
    {
      detail::NotifyTypeBuilders<T>().NewStaticFunction(name, ptr);
      methods.emplace_back(move(name), ptr);
      Add<T>(std::forward<Args>(args)...);
    }

    // Adds a non-const member function given its name and pointer.
    template <class T, class ClassT, class RetT, class... FuncArgs, class... Args>
    void Add(std::string name, RetT(ClassT::*ptr)(FuncArgs...), Args&&... args)
    {
      detail::NotifyTypeBuilders<T>().NewMemberFunction(name, ptr);
      methods.emplace_back(move(name), ptr);
      Add<T>(std::forward<Args>(args)...);
    }

    // Adds a const member function given its name and pointer.
    template <class T, class ClassT, class RetT, class... FuncArgs, class... Args>
    void Add(std::string name, RetT(ClassT::*ptr)(FuncArgs...) const, Args&&... args)
    {
      detail::NotifyTypeBuilders<T>().NewMemberFunction(name, ptr);
      methods.emplace_back(move(name), ptr);
      Add<T>(std::forward<Args>(args)...);
    }

    template <class T, class FieldT1, class T1, class FieldT2, class T2, class... Args>
    void Add(std::string name, FieldT1(T1::*getter)() const, void(T2::*setter)(FieldT2), Args&&... args)
    {
      using DataType = detail::common<detail::decayed<FieldT1>, detail::decayed<FieldT2>>;

      // Notify the plugin of the new field.
      detail::NotifyTypeBuilders<T>().NewMemberProperty(name, getter, setter);

      // Create the field.
      fields.emplace_back(move(name), getter, setter);

      // Perfect-forward the rest of the arguments to Add.
      Add<T>(std::forward<Args>(args)...);
    }

    template <class T, class FieldT, class ClassT, class... Args>
    void Add(std::string name, FieldT(ClassT::*getter)() const, struct ReadOnly_*, Args&&... args)
    {
      // Notify the plugin of the new field.
      detail::NotifyTypeBuilders<T>().NewMemberPropertyReadOnly(name, getter);

      // Create the field.
      fields.emplace_back(move(name), getter);

      // Perfect-forward the rest of the arguments to Add.
      Add<T>(std::forward<Args>(args)...);
    }

    // Extraction operator
    template <class T, class IStream, class Object, class... Args>
    void Add(IStream&(*ptr)(IStream&, Object), struct RightShift_*, Args&&... args)
    {
      std::string opName = "operator<<";
      detail::NotifyTypeBuilders<T>().NewStaticOperatorExtraction(name, ptr);
      methods.emplace_back(std::move(opName), ptr);
      Add<T>(std::forward<Args>(args)...);
    }

    template <class T, class FieldT1, class FieldT2, class... Args>
    void Add(std::string name, FieldT1(*getter)(), void(*setter)(FieldT2), Args&&... args)
    {
      using DataType = detail::common<detail::decayed<FieldT1>, detail::decayed<FieldT2>>;

      // Notify the plugin of the new property.
      detail::NotifyTypeBuilders<T>().NewStaticProperty(name, getter, setter);

      // Create the field.
      fields.emplace_back(move(name), getter, setter);

      // Perfect-forward the rest of the arguments to Add.
      Add<T>(std::forward<Args>(args)...);
    }

    template <class T, class FieldT1, class... Args>
    void Add(std::string name, FieldT1(*getter)(), struct ReadOnly_*, Args&&... args)
    {
      // Notify the plugin of the new property.
      detail::NotifyTypeBuilders<T>().NewStaticPropertyReadOnly(name, getter);

      // Create the field.
      fields.emplace_back(move(name), getter);

      // Perfect-forward the rest of the arguments to Add.
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const modulo method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct Modulo_*, Args&&... args)
    {
      std::string name = "operator%";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorModulo(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const AND method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct And_*, Args&&... args)
    {
      std::string name = "operator&";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorAnd(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const OR method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct Or_*, Args&&... args)
    {
      std::string name = "operator|";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorOr(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const XOR method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct Xor_*, Args&&... args)
    {
      std::string name = "operator^";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorXor(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const unary plus method to the type.
    template <class T, class Result, class Class, class... Args>
    void Add(Result(Class::*fn)() const, struct Plus_*, Args&&... args)
    {
      std::string name = "operator+";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorUnaryPlus(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const unary minus method to the type.
    template <class T, class Result, class Class, class... Args>
    void Add(Result(Class::*fn)() const, struct Minus_*, Args&&... args)
    {
      std::string name = "operator-";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorUnaryMinus(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const addition method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct Plus_*, Args&&... args)
    {
      std::string name = "operator+";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorAddition(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const subtraction method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct Minus_*, Args&&... args)
    {
      std::string name = "operator-";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorSubtraction(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const multiplication method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct Multiplication_*, Args&&... args)
    {
      std::string name = "operator*";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorMultiplication(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a const division method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result(Class::*fn)(Param) const, struct Division_*, Args&&... args)
    {
      std::string name = "operator/";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorDivision(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a assignment addition method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result&(Class::*fn)(Param), struct Plus_*, Args&&... args)
    {
      std::string name = "operator+=";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorAssignAddition(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a assignment subtraction method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result&(Class::*fn)(Param), struct Minus_*, Args&&... args)
    {
      std::string name = "operator-=";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorAssignSubtraction(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a assignment multiplication method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result&(Class::*fn)(Param), struct Multiplication_*, Args&&... args)
    {
      std::string name = "operator*=";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorAssignMultiplication(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Binds a assignment division method to the type.
    template <class T, class Result, class Class, class Param, class... Args>
    void Add(Result&(Class::*fn)(Param), struct Division_*, Args&&... args)
    {
      std::string name = "operator/=";
      detail::NotifyTypeBuilders<T>().NewMemberOperatorAssignDivision(name, fn);
      methods.emplace_back(move(name), fn);
      Add<T>(std::forward<Args>(args)...);
    }

    // Calls Add functions recursively to initialize the type.
    template <class T, class... Args>
    void Bind(std::string const& fullName, Args&&... args)
    {
      size_t scope = fullName.rfind("::");

      cppType = &typeid(T);
      name = (scope == std::string::npos ? fullName : fullName.substr(scope + 2));
      namespaceName = (scope == std::string::npos ? std::string() : fullName.substr(0, scope));

      detail::NotifyTypeBuilders<T>().Begin(name, namespaceName);

      Add<T>(std::forward<Args>(args)...);

      detail::NotifyTypeBuilders<T>().End(name, namespaceName);
    }

    // Initializes this type as a reference-type.
    template <class T>
    void InitializeReferenceType()
    {
      isReference = true;
      valueType = &TypeOf<T>();
    }

    // Formats the type info into an ostream.
    template <class OStream>
    friend std::ostream& operator<<(OStream& os, const TypeInfo& ti)
    {
      os << ti.Name;
    }
  };

  namespace detail
  {
    // Stores the static data for a type info.
    template <class T>
    TypeInfo& TypeOf()
    {
      static TypeInfo type;
      return type;
    }
  } // namespace detail

  // Returns a TypeInfo given its C++ type.
  template <class T>
  const TypeInfo& TypeOf()
  {
    return detail::TypeOf<detail::decayed<T>>();
  }
} // namespace reflect
