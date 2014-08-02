#pragma once

#include "DataInfo.hpp"
#include "ReflectionUtility.hpp"
#include "TypeInfo.hpp"

namespace reflect
{
  class NamespaceInfo
  {
  private: // data

    std::unordered_map<std::string, DataInfo> data;
    std::unordered_map<std::string, FunctionInfo> functions;
    std::string name;
    std::string namespaceName;
    std::unordered_map<std::string, NamespaceInfo*> namespaces;
    std::unordered_map<std::string, TypeInfo const*> types;

  public: // data

    // Global data stored in the namespace.
    ConstMapWrapper<std::string, DataInfo> Data = data;

    // Global functions existing in the namespace.
    ConstMapWrapper<std::string, FunctionInfo> Functions = functions;

    // Name of the namespace.
    std::string const& Name = name;

    // Name of the owning namespace name.
    std::string const& NamespaceName = namespaceName;

    // Child namespaces. 
    ConstMapWrapper<std::string, NamespaceInfo*> Namespaces = namespaces;

    // Types stored within the namespace.
    ConstMapWrapper<std::string, TypeInfo const*> Types = types;

  public: // methods

    NamespaceInfo() = delete;
    NamespaceInfo(NamespaceInfo const&) = delete;
    NamespaceInfo& operator=(NamespaceInfo const&) = delete;

    NamespaceInfo(NamespaceInfo&& b) : 
      data(std::move(b.data)),
      functions(std::move(b.functions)),
      name(std::move(b.name)),
      namespaceName(std::move(b.namespaceName)),
      namespaces(std::move(b.namespaces)),
      types(std::move(b.types))
    {}

    NamespaceInfo& operator=(NamespaceInfo&& b) 
    {
      data = std::move(b.data);
      functions = std::move(b.functions);
      name = std::move(b.name);
      namespaceName = std::move(b.namespaceName);
      namespaces = std::move(b.namespaces);
      types = std::move(b.types);
      return *this;
    }

    void Add() {}

    template <class Result, class... FuncArgs, class... Args>
    void Add(std::string const& name, Result(*fn)(FuncArgs...), Args&&... args)
    {
      functions.emplace(name, FunctionInfo(name, fn));
      Add(std::forward<Args>(args)...);
    }

    template <class Data, class... Args>
    void Add(std::string const& name, Data dataPtr, Args&&... args)
    {
      data.emplace(name, DataInfo(name, dataPtr));
      Add(std::forward<Args>(args)...);
    }

    // Adds a child namespace to the namespace.
    void AddNamespace(NamespaceInfo* ns)
    {
      namespaces.emplace(ns->Name, ns);
    }

    // Adds a type to the namespace.
    void AddType(TypeInfo* type)
    {
      types.emplace(type->Name, type);
    }

    template <class... Args>
    void Bind(std::string const& fullName, Args&&... args)
    {
      std::size_t scope = name.rfind("::");

      name = (scope == std::string::npos ? fullName : fullName.substr(scope + 2));
      namespaceName = (scope == std::string::npos ? std::string() : fullName.substr(0, scope));

      Add(std::forward<Args>(args)...);
    }

    NamespaceInfo& GetNamespace(std::string const& name)
    {
      auto it = namespaces.find(name);
      if (it != namespaces.end()) return *it->second;

      std::string newNamespace = NamespaceName.size() ? NamespaceName + "::" : "";
      newNamespace += this->name;

      NamespaceInfo* info = new NamespaceInfo(std::move(newNamespace), name);
      namespaces.emplace(name, info);

      return *info;
    }

  private: // methods

    NamespaceInfo(std::string namespaceName_, std::string name_) :
      namespaceName(std::move(namespaceName_)),
      name(std::move(name_))
    {}

    ~NamespaceInfo()
    {
      for (auto& namespacePair : namespaces)
      {
        delete namespacePair.second;
      }
    }

    friend class Reflection;
  };
} // namespace reflect
