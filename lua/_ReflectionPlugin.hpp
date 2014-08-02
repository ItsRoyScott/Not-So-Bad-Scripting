#pragma once

#include <cstdlib>
#include "lua/lua.hpp"
#include "lua/LuaBridge.h"
#include "reflect/DefaultPlugin.hpp"
#include <sstream>

namespace Lua
{
  using namespace luabridge;
  using namespace reflect;

  inline lua_State* CreateGlobalState()
  {
    static lua_State* state = luaL_newstate();
    luaL_openlibs(state);
    std::atexit([] { lua_close(state); });
    return state;
  }

  inline lua_State* L()
  {
    static lua_State* state = CreateGlobalState();
    return state;
  }

  inline std::string GetLastLuaError(lua_State* state)
  {
    std::stringstream ss;
    for (int level = 1; level <= lua_gettop(state); level++)
    {
      const char* errorMessage = lua_tostring(state, level);
      if (errorMessage)
      {
        ss << "Error: " << errorMessage << std::endl;
      }
    }

    lua_Debug debugInfo;
    for (int level = 0; lua_getstack(state, level, &debugInfo); level++)
    {
      lua_getinfo(state, "nSlf", &debugInfo);
      ss << "Line: " << debugInfo.currentline << std::endl;
      ss << "Source: " << debugInfo.source << std::endl;

      const char* function = lua_tostring(state, -1);
      if (function)
      {
        ss << "Function: " << function << std::endl;
      }
    }

    return ss.str();
  }

  inline bool CheckLuaResult()
  {
    std::string errors = GetLastLuaError(L());
    if (errors.size())
    {
      printf("%s\n", errors.c_str());
    }
    return errors.empty();
  }

  inline bool DoString(std::string const& str)
  {
    luaL_dostring(L(), str.c_str());
    return CheckLuaResult();
  }

  struct ReflectionPlugin : DefaultPlugin
  {
    template <class T, bool IsClass>
    struct TypeBuilder : DefaultPlugin::TypeBuilder<T, IsClass>
    {
      TypeBuilder(ReflectionPlugin&) {}
    };

    template <class T>
    struct TypeBuilder<T, true> : DefaultPlugin::TypeBuilder<T, true>
    {
    private: // data

      typedef luabridge::Namespace::Class<T> Class;

      Class* class_ = nullptr;
      luabridge::Namespace* namespace_ = nullptr;

    public: // methods

      TypeBuilder(ReflectionPlugin&) {}

      void Begin(std::string const& className, std::string const& namespaceName)
      {
        namespace_ = new luabridge::Namespace(luabridge::getGlobalNamespace(L()).beginNamespace(namespaceName.c_str()));
        class_ = new Class(namespace_->beginClass<T>(className.c_str()));
      }

      void End(std::string const&, std::string const&)
      {
        class_->endClass();
        delete class_;
        delete namespace_;
      }

      void NewDefaultConstructor(std::string const&, void(*)(void*))
      {
        class_->template addConstructor<void(*)()>();
      }

      template <class DataPtr>
      void NewMemberData(std::string const& name, DataPtr const& data)
      {
        class_->addData(name.c_str(), data);
      }

      template <class FuncPtr>
      void NewMemberFunction(std::string const& name, FuncPtr const& fn)
      {
        class_->addFunction(name.c_str(), fn);
      }

      template <class Func>
      void NewMemberOperatorAddition(std::string const&, Func const& fn)
      {
        class_->addFunction("__add", fn);
      }

      template <class Func>
      void NewMemberOperatorDivision(std::string const&, Func const& fn)
      {
        class_->addFunction("__div", fn);
      }

      template <class Func>
      void NewMemberOperatorModulo(std::string const&, Func const& fn)
      {
        class_->addFunction("__mod", fn);
      }

      template <class Func>
      void NewMemberOperatorMultiplication(std::string const&, Func const& fn)
      {
        class_->addFunction("__mul", fn);
      }

      template <class Func>
      void NewMemberOperatorSubtraction(std::string const&, Func const& fn)
      {
        class_->addFunction("__sub", fn);
      }

      template <class Func>
      void NewMemberOperatorXor(std::string const&, Func const& fn)
      {
        class_->addFunction("__pow", fn);
      }

      template <class Getter, class Setter>
      void NewMemberProperty(std::string const& name, Getter const& getter, Setter const& setter)
      {
        class_->addProperty(name.c_str(), getter, setter);
      }

      template <class Getter>
      void NewMemberPropertyReadOnly(std::string const& name, Getter const& getter)
      {
        class_->addProperty(name.c_str(), getter);
      }

      template <class DataPtr>
      void NewStaticData(std::string const& name, DataPtr const& data)
      {
        class_->addStaticData(name.c_str(), data);
      }

      template <class FuncPtr>
      void NewStaticFunction(std::string const& name, FuncPtr const& fn)
      {
        class_->addStaticFunction(name.c_str(), fn);
      }

      template <class Getter, class Setter>
      void NewStaticProperty(std::string const& name, Getter const& getter, Setter const& setter)
      {
        class_->addStaticProperty(name.c_str(), getter, setter);
      }

      template <class Getter>
      void NewStaticPropertyReadOnly(std::string const& name, Getter const& getter)
      {
        class_->addStaticProperty(name.c_str(), getter);
      }
    };
  };
} // namespace Lua