#pragma once

#include "ReflectionUtility.hpp"

namespace reflect
{
  // A reflection plugin houses a TypeBuilder class.
  struct DefaultPlugin
  {
    // Contains numerous functions called when reflection is retrieving meta
    //  information on a type.
    template <class T, bool IsClass>
    struct TypeBuilder
    {
      TypeBuilder() {}
      TypeBuilder(DefaultPlugin&) {}

      void Begin(std::string const&, std::string const&) {}
      void End(std::string const&, std::string const&) {}
      template <class Arg0, class... Args>
      void NewConstructor(std::string const&, void(*)(void*, Arg0, Args...)) {}
      void NewCopyAssign(std::string const&, T&(*)(T&, T const&)) {}
      void NewCopyConstructor(std::string const&, void(*)(void*, T const&)) {}
      void NewDefaultConstructor(std::string const&, void(*)(void*)) {}
      void NewDestructor(std::string const&, void(*)(void*)) {}
      template <class Class, class Data>
      void NewMemberData(std::string const&, Data Class::*) {}
      template <class Result, class Class, class... Args>
      void NewMemberFunction(std::string const&, Result (Class::*)(Args...)) {}
      template <class Result, class Class, class... Args>
      void NewMemberFunction(std::string const&, Result(Class::*)(Args...) const) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorAssignAddition(std::string const&, Result(Class::*)(Arg)) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorAssignDivision(std::string const&, Result(Class::*)(Arg)) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorAssignMultiplication(std::string const&, Result(Class::*)(Arg)) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorAssignSubtraction(std::string const&, Result(Class::*)(Arg)) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorAddition(std::string const&, Result(Class::*)(Arg) const) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorDivision(std::string const&, Result(Class::*)(Arg) const) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorModulo(std::string const&, Result(Class::*)(Arg) const) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorMultiplication(std::string const&, Result(Class::*)(Arg) const) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorSubtraction(std::string const&, Result(Class::*)(Arg) const) {}
      template <class Result, class Class>
      void NewMemberOperatorUnaryMinus(std::string const&, Result(Class::*)() const) {}
      template <class Result, class Class>
      void NewMemberOperatorUnaryPlus(std::string const&, Result(Class::*)() const) {}
      template <class Result, class Class, class Arg>
      void NewMemberOperatorXor(std::string const&, Result(Class::*)(Arg) const) {}
      template <class GetResult , class GetClass, class SetArg, class SetClass>
      void NewMemberProperty(std::string const&, GetResult (GetClass::*)() const, void (SetClass::*)(SetArg)) {}
      template <class Result, class Class>
      void NewMemberPropertyReadOnly(std::string const&, Result (Class::*)() const) {}
      template <class Pointer>
      void NewStaticData(std::string const&, Pointer) {}
      template <class Result, class... Args>
      void NewStaticFunction(std::string const&, Result (*)(Args...)) {}
      template <class Class, class IStream>
      void NewStaticOperatorExtraction(std::string const&, IStream&(*)(IStream&, Class&)) {}
      template <class Class, class OStream>
      void NewStaticOperatorInsertion(std::string const&, OStream&(*)(OStream&, Class&)) {}
      template < class GetResult, class SetArg>
      void NewStaticProperty(std::string const&, GetResult(*)(), void(*)(SetArg)) {}
      template <class GetResult>
      void NewStaticPropertyReadOnly(std::string const&, GetResult(*)()) {}
    };
  };
} // namespace reflect
