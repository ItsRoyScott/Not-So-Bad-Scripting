#pragma once

// Flags
#define REFLECT_USE_LUA

#include "ReflectionUtility.hpp"

#ifdef REFLECT_USE_LUA
#define REFLECT_LUA_PLUGIN_LIST_ENTRY ,Lua::ReflectionPlugin
#else
#define REFLECT_LUA_PLUGIN_LIST_ENTRY
#endif

// Includes for all plugins: include your custom plugin here.
#include "DefaultPlugin.hpp"
#ifdef REFLECT_USE_LUA
#include "lua/ReflectionPlugin.hpp"
#endif

// List of plugin types: add a backslash to the previous line
//  and `,MyPluginType` to hook in `MyPluginType`.
#define REFLECT_PLUGIN_LIST reflect::DefaultPlugin \
  REFLECT_LUA_PLUGIN_LIST_ENTRY

#include "PluginHelper.hpp"
