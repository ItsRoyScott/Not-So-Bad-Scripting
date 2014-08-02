#pragma once

#include "PluginHelper.hpp"

namespace reflect
{
  // Meta information on a method in C++.
  class FunctionInfo
  {
  private: // data

    std::vector<const TypeInfo*> argumentTypes;
    std::type_index              cFunctionType = typeid(void);
    std::function<void()>        func;
    std::string                  name;
    const TypeInfo*         returnType;

  public: // properties

    // Array of TypeInfo objects describing the argument types.
    const std::vector<const TypeInfo*>& ArgumentTypes = argumentTypes;

    // Name of the method.
    const std::string& Name = name;

    // Return type as a TypeInfo.
    const TypeInfo* const& ReturnType = returnType;

  public: // methods

    FunctionInfo() = delete;

    FunctionInfo(FunctionInfo&& b) :
      argumentTypes(move(b.argumentTypes)),
      cFunctionType(move(b.cFunctionType)),
      func(move(b.func)),
      name(move(b.name)),
      returnType(b.returnType)
    {}
    
    FunctionInfo& operator=(FunctionInfo&& b)
    {
      argumentTypes = move(b.argumentTypes);
      cFunctionType = move(b.cFunctionType);
      func = move(b.func);
      name = move(b.name);
      returnType = b.returnType;
    
      return *this;
    }

    FunctionInfo(const FunctionInfo&) = delete;
    FunctionInfo& operator=(const FunctionInfo&) = delete;

    // Constructs given the name and function pointer.
    template <class FuncPtr>
    FunctionInfo(std::string name_, FuncPtr func_)
    {
      typedef FunctionTraits<FuncPtr> Traits;

      argumentTypes = Traits::RuntimeArguments();
      cFunctionType = typeid(typename Traits::CFunctionType);
      func = Traits::GenerateTypelessFunction(func_);
      name = move(name_);
      returnType = &TypeOf<typename Traits::ReturnType>();
    }

    ~FunctionInfo() = default;

    // Retrieves the typed function object.
    template <class CallT>
    std::function<CallT> AsFunction() const
    {
      typedef FunctionTraits<CallT> Traits;

      // Verify the call type is correct.
      if (cFunctionType != typeid(typename Traits::CFunctionType)) return{};

      // Reinterpret cast to the requested function type.
      return detail::CastFunction<void(), CallT>(func);
    }
  };

  template <class RetT, class ClassT, class... Args>
  auto Const(RetT(ClassT::*fn)(Args...) const) -> decltype(fn)
  {
    return fn;
  }

  template <class ClassT, class RetT, class... Args>
  auto NonConst(RetT(ClassT::*fn)(Args...)) -> decltype(fn)
  {
    return fn;
  }

  template <class... Args>
  struct Overloaded
  {
    template <class RetT, class ClassT>
    static auto Const(RetT(ClassT::*fn)(Args...) const) -> decltype(fn)
    {
      return fn;
    }
    template <class RetT>
    static auto NonConst(RetT(*fn)(Args...)) -> decltype(fn)
    {
      return fn;
    }
    template <class RetT, class ClassT>
    static auto NonConst(RetT(ClassT::*fn)(Args...)) -> decltype(fn)
    {
      return fn;
    }
  };

  template <class RetT>
  static auto Overloaded0(RetT(*fn)()) -> decltype(fn)
  {
    return fn;
  }
  template <class RetT, class ClassT>
  static auto Overloaded0(RetT(ClassT::*fn)()) -> decltype(fn)
  {
    return fn;
  }
  template <class RetT, class ClassT>
  static auto Overloaded0(RetT(ClassT::*fn)() const) -> decltype(fn)
  {
    return fn;
  }

  template <class RetT, class Arg0>
  static auto Overloaded1(RetT(*fn)(Arg0)) -> decltype(fn)
  {
    return fn;
  }
  template <class RetT, class ClassT, class Arg0>
  static auto Overloaded1(RetT(ClassT::*fn)(Arg0)) -> decltype(fn)
  {
    return fn;
  }
  template <class RetT, class ClassT, class Arg0>
  static auto Overloaded1(RetT(ClassT::*fn)(Arg0) const) -> decltype(fn)
  {
    return fn;
  }

  template <class RetT, class Arg0, class Arg1>
  static auto Overloaded2(RetT(*fn)(Arg0, Arg1)) -> decltype(fn)
  {
    return fn;
  }
  template <class RetT, class ClassT, class Arg0, class Arg1>
  static auto Overloaded2(RetT(ClassT::*fn)(Arg0, Arg1)) -> decltype(fn)
  {
    return fn;
  }
  template <class RetT, class ClassT, class Arg0, class Arg1>
  static auto Overloaded2(RetT(ClassT::*fn)(Arg0, Arg1) const) -> decltype(fn)
  {
    return fn;
  }
} // namespace reflect
