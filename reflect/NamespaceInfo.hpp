#pragma once

#include "DataInfo.hpp"
#include "ReflectionUtility.hpp"

namespace reflect
{
  class NamespaceInfo
  {
  private: // data

    std::unordered_map<std::string, NamespaceInfo*> childNamespaces;

    std::unordered_map<std::string, DataInfo> data;

    std::unordered_map<std::string, FunctionInfo> functions;

    std::unordered_map<std::string, TypeInfo const*> types;

  public: // data

    std::unordered_map<std::string, DataInfo> const& Data = data;

    std::unordered_map<std::string, FunctionInfo> const& Functions = functions;

    std::unordered_map<std::string, NamespaceInfo*> const& Namespaces = childNamespaces;

    std::unordered_map<std::string, TypeInfo const*> const& Types = types;

  public: // methods

    NamespaceInfo() = delete;
    NamespaceInfo(NamespaceInfo&&) {}
    NamespaceInfo& operator=(NamespaceInfo&&) {}
    NamespaceInfo(NamespaceInfo const&) = delete;
    NamespaceInfo& operator=(NamespaceInfo const&) = delete;
    ~NamespaceInfo() = default;
  };
} // namespace reflect
