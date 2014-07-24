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

      // Begin
      void Begin(std::string const& name, std::string const& namespaceName)
      {
        NotifyTypeBuilders<T, N - 1>().Begin(name, namespaceName);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().Begin(name, namespaceName);
      }
      // End
      void End(std::string const& name, std::string const& namespaceName)
      {
        NotifyTypeBuilders<T, N - 1>().End(name, namespaceName);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().End(name, namespaceName);
      }
      // NewConstructor
      template <class Func>
      void NewConstructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewConstructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewConstructor(name, fn);
      }
      // NewCopyAssign
      template <class Func>
      void NewCopyAssign(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewCopyAssign(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewCopyAssign(name, fn);
      }
      // NewCopyConstructor
      template <class Func>
      void NewCopyConstructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewCopyConstructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewCopyConstructor(name, fn);
      }
      // NewDefaultConstructor
      template <class Func>
      void NewDefaultConstructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewDefaultConstructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewDefaultConstructor(name, fn);
      }
      // NewDestructor
      template <class Func>
      void NewDestructor(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewDestructor(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewDestructor(name, fn);
      }
      // NewMemberData
      template <class DataPtr>
      void NewMemberData(std::string const& name, DataPtr const& ptr)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberData(name, ptr);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberData(name, ptr);
      }
      // NewMemberFunction
      template <class Func>
      void NewMemberFunction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberFunction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberFunction(name, fn);
      }
      // NewMemberProperty
      template <class Getter, class Setter>
      void NewMemberProperty(std::string const& name, Getter const& getter, Setter const& setter)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberProperty(name, getter, setter);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberProperty(name, getter, setter);
      }
      // NewMemberPropertyReadOnly
      template <class Getter>
      void NewMemberPropertyReadOnly(std::string const& name, Getter const& getter)
      {
        NotifyTypeBuilders<T, N - 1>().NewMemberPropertyReadOnly(name, getter);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewMemberPropertyReadOnly(name, getter);
      }
      // NewOperatorExtraction
      template <class Func>
      void NewOperatorExtraction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewOperatorExtraction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewOperatorExtraction(name, fn);
      }
      // NewOperatorInsertion
      template <class Func>
      void NewOperatorInsertion(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewOperatorInsertion(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewOperatorInsertion(name, fn);
      }
      // NewStaticData
      template <class DataPtr>
      void NewStaticData(std::string const& name, DataPtr const& ptr)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticData(name, ptr);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticData(name, ptr);
      }
      // NewStaticFunction
      template <class Func>
      void NewStaticFunction(std::string const& name, Func const& fn)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticFunction(name, fn);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticFunction(name, fn);
      }
      // NewStaticProperty
      template <class Getter, class Setter>
      void NewStaticProperty(std::string const& name, Getter const& getter, Setter const& setter)
      {
        NotifyTypeBuilders<T, N - 1>().NewStaticProperty(name, getter, setter);
        Plugin<N - 1>::template GetTypeBuilder<T, IsClass>().NewStaticProperty(name, getter, setter);
      }
      // NewStaticPropertyReadOnly
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
      void NewConstructor(std::string const&, void(*)(This, Arg0, Args...)) {}
      void NewCopyAssign(std::string const&, T&(*)(T&, T const&)) {}
      void NewCopyConstructor(std::string const&, void(*)(This, T const&)) {}
      void NewDefaultConstructor(std::string const&, void(*)(This)) {}
      void NewDestructor(std::string const&, void(*)(This)) {}
      template <class Class, class Data>
      void NewMemberData(std::string const&, Data Class::*) {}
      template <class Result, class Class, class... Args>
      void NewMemberFunction(std::string const&, Result(Class::*)(Args...)) {}
      template <class Result, class Class, class... Args>
      void NewMemberFunction(std::string const&, Result(Class::*)(Args...) const) {}
      template <class GetResult, class GetClass, class SetArg, class SetClass>
      void NewMemberProperty(std::string const&, GetResult(GetClass::*)() const, void (SetClass::*)(SetArg)) {}
      template <class Result, class Class>
      void NewMemberPropertyReadOnly(std::string const&, Result(Class::*)() const) {}
      template <class Class, class IStream>
      void NewOperatorExtraction(std::string const&, IStream&(*)(IStream&, Class&)) {}
      template <class Class, class OStream>
      void NewOperatorInsertion(std::string const&, OStream&(*)(OStream&, Class&)) {}
      template <class Pointer>
      void NewStaticData(std::string const&, Pointer) {}
      template <class Result, class... Args>
      void NewStaticFunction(std::string const&, Result(*)(Args...)) {}
      template <class GetResult, class SetArg>
      void NewStaticProperty(std::string const&, GetResult(*)(), void(*)(SetArg)) {}
      template <class Result>
      void NewStaticPropertyReadOnly(std::string const&, Result(*)()) {}
      template <class GetResult>
      void NewStaticPropertyReadOnly(std::string const&, std::function<GetResult()> const&) {}
    };

  } // namespace detail
} // namespace reflect