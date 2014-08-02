#pragma once

#include "Config.hpp"

namespace reflect
{
  namespace detail
  {
    template <size_t Idx>
    struct Plugin
    {
      typedef typename std::tuple_element<Idx, std::tuple<REFLECT_PLUGIN_LIST>>::type PluginType;
      template <class T, bool IsClass>
      using TypeBuilder = typename PluginType::template TypeBuilder<T, IsClass>;

      static PluginType& GetPlugin()
      {
        static PluginType plugin;
        return plugin;
      }

      template <class T, bool IsClass>
      static TypeBuilder<T, IsClass>& GetTypeBuilder()
      {
        static TypeBuilder<T, IsClass> builder(GetPlugin());
        return builder;
      }
    };

    template <class T, size_t N = std::tuple_size<std::tuple<REFLECT_PLUGIN_LIST>>::value>
    struct NotifyTypeBuilders
    {
      //
      // Helper functions which invoke callbacks on the TypeBuilder objects
      //  for a given type 'T'.
      //

      static bool const IsClass = std::is_class<T>::value;

      void Begin(std::string const& name, std::string const& namespaceName)
      {
        NotifyTypeBuilders<T, N - 1>().Begin(name, namespaceName);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().Begin(name, namespaceName);
      }
      void End(std::string const& name, std::string const& namespaceName)
      {
        NotifyTypeBuilders<T, N - 1>().End(name, namespaceName);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().End(name, namespaceName);
      }
      template <class Func>
      void NewConstructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewConstructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewConstructor(name, fn);
      }
      template <class Func>
      void NewCopyAssign(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewCopyAssign(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewCopyAssign(name, fn);
      }
      template <class Func>
      void NewCopyConstructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewCopyConstructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewCopyConstructor(name, fn);
      }
      template <class Func>
      void NewDefaultConstructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewDefaultConstructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewDefaultConstructor(name, fn);
      }
      template <class Func>
      void NewDestructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewDestructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewDestructor(name, fn);
      }
      template <class DataPtr>
      void NewMemberData(std::string const& name, DataPtr const& ptr)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberData(name, ptr);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberData(name, ptr);
      }
      template <class Func>
      void NewMemberFunction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberFunction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberFunction(name, fn);
      }
      template <class Func>
      void NewMemberOperatorAssignAddition(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorAssignAddition(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorAssignAddition(name, fn);
      }
      template <class Func>
      void NewMemberOperatorAssignDivision(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorAssignDivision(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorAssignDivision(name, fn);
      }
      template <class Func>
      void NewMemberOperatorAssignMultiplication(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorAssignMultiplication(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorAssignMultiplication(name, fn);
      }
      template <class Func>
      void NewMemberOperatorAssignSubtraction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorAssignSubtraction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorAssignSubtraction(name, fn);
      }
      template <class Func>
      void NewMemberOperatorAddition(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorAddition(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorAddition(name, fn);
      }
      template <class Func>
      void NewMemberOperatorDivision(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorDivision(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorDivision(name, fn);
      }
      template <class Func>
      void NewMemberOperatorModulo(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorModulo(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorModulo(name, fn);
      }
      template <class Func>
      void NewMemberOperatorMultiplication(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorMultiplication(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorMultiplication(name, fn);
      }
      template <class Func>
      void NewMemberOperatorSubtraction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorSubtraction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorSubtraction(name, fn);
      }
      template <class Func>
      void NewMemberOperatorUnaryMinus(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorUnaryMinus(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorUnaryMinus(name, fn);
      }
      template <class Func>
      void NewMemberOperatorUnaryPlus(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorUnaryPlus(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorUnaryPlus(name, fn);
      }
      template <class Func>
      void NewMemberOperatorXor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberOperatorXor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberOperatorXor(name, fn);
      }
      template <class Getter, class Setter>
      void NewMemberProperty(std::string const& name, Getter const& getter, Setter const& setter)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberProperty(name, getter, setter);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberProperty(name, getter, setter);
      }
      template <class Getter>
      void NewMemberPropertyReadOnly(std::string const& name, Getter const& getter)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberPropertyReadOnly(name, getter);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberPropertyReadOnly(name, getter);
      }
      template <class DataPtr>
      void NewStaticData(std::string const& name, DataPtr const& ptr)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticData(name, ptr);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticData(name, ptr);
      }
      template <class Func>
      void NewStaticFunction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticFunction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticFunction(name, fn);
      }
      template <class Func>
      void NewStaticOperatorExtraction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticOperatorExtraction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticOperatorExtraction(name, fn);
      }
      template <class Func>
      void NewStaticOperatorInsertion(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticOperatorInsertion(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticOperatorInsertion(name, fn);
      }
      template <class Getter, class Setter>
      void NewStaticProperty(std::string const& name, Getter const& getter, Setter const& setter)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticProperty(name, getter, setter);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticProperty(name, getter, setter);
      }
      template <class Func>
      void NewStaticPropertyReadOnly(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticPropertyReadOnly(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticPropertyReadOnly(name, fn);
      }
    };

    template <class T>
    struct NotifyTypeBuilders<T, 0>
    {
      //
      // Base case helper functions which do nothing
      //

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
      void NewMemberFunction(std::string const&, Result(Class::*)(Args...)) {}
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
      template <class GetResult, class GetClass, class SetArg, class SetClass>
      void NewMemberProperty(std::string const&, GetResult(GetClass::*)() const, void (SetClass::*)(SetArg)) {}
      template <class Result, class Class>
      void NewMemberPropertyReadOnly(std::string const&, Result(Class::*)() const) {}
      template <class Pointer>
      void NewStaticData(std::string const&, Pointer) {}
      template <class Result, class... Args>
      void NewStaticFunction(std::string const&, Result(*)(Args...)) {}
      template <class Class, class IStream>
      void NewStaticOperatorExtraction(std::string const&, IStream&(*)(IStream&, Class&)) {}
      template <class Class, class OStream>
      void NewStaticOperatorInsertion(std::string const&, OStream&(*)(OStream&, Class&)) {}
      template < class GetResult, class SetArg>
      void NewStaticProperty(std::string const&, GetResult(*)(), void(*)(SetArg)) {}
      template <class GetResult>
      void NewStaticPropertyReadOnly(std::string const&, GetResult(*)()) {}
    };

  } // namespace detail
} // namespace reflect