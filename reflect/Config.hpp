#pragma once

#include "ReflectionUtility.hpp"

// Remove Lua if the user requests so.
#ifdef REFLECT_NO_LUA
  #define REFLECT_LUA_PLUGIN_LIST_ENTRY
#else
  #define REFLECT_LUA_PLUGIN_LIST_ENTRY ,Lua::ReflectionPlugin
#endif

// Includes for all plugins: include your custom plugin here.
#include "DefaultPlugin.hpp"
#ifndef REFLECT_NO_LUA
  #include "lua/ReflectionPlugin.hpp"
#endif

// List of plugin types: add a backslash to the previous line,
//  then write `,MyPluginType` to hook in `MyPluginType`.
#define REFLECT_PLUGIN_LIST \
  REFLECT_LUA_PLUGIN_LIST_ENTRY

#include "PluginHelper.hpp"
