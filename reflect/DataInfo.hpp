#pragma once

#include "ReflectionUtility.hpp"

namespace reflect
{
  // Meta information on a field in C++.
  class DataInfo
  {
  private: // data

    std::function<void(void*, void*)>  getter;
    std::function<void*(void*)>        getFieldPointer;
    std::string                        name;
    const TypeInfo*                    ownerType = nullptr;
    std::function<void(void*, void*)>  setter;
    const TypeInfo*                    type = nullptr;

  public: // properties

    // Name of the field.
    const std::string& Name = name;

    // Class that owns this field (may be null for global/static objects).
    const TypeInfo* const& OwnerType = ownerType;

    // Type of the field.
    const TypeInfo* const& Type = type;

  public: // methods

    DataInfo() = delete;

    DataInfo(DataInfo&& b) :
      getFieldPointer(move(b.getFieldPointer)),
      getter(move(b.getter)),
      name(move(b.name)),
      ownerType(b.ownerType),
      setter(move(b.setter)),
      type(b.type)
    {}

    DataInfo& operator=(DataInfo&& b)
    {
      getFieldPointer = move(b.getFieldPointer);
      getter = move(b.getter);
      name = move(b.name);
      ownerType = b.ownerType;
      setter = move(b.setter);
      type = b.type;

      return *this;
    }

    DataInfo(const DataInfo&) = delete;
    DataInfo& operator=(const DataInfo&) = delete;

    // Constructs given the field's name and pointer (global object).
    template <class FieldT, class = typename std::enable_if<IsField<FieldT*>::value>::type>
    DataInfo(std::string name_, FieldT* fieldPtr) :
      name(move(name_)),
      getFieldPointer([=](void*) -> void* { return fieldPtr; }),
      type(&TypeOf<FieldT>())
    {
    }

    // Constructs given the field's name and member pointer.
    template <class FieldT, class T>
    DataInfo(std::string name_, FieldT T::*fieldPtr) :
      name(move(name_)),
      ownerType(&TypeOf<T>()),
      type(&TypeOf<detail::decayed<FieldT>>())
    {
      getFieldPointer = [=](void* this_) -> void*
      {
        FieldT& field = (reinterpret_cast<T*>(this_)->*fieldPtr);
        return &field;
      };

      getter = [=](void* this_, void* data)
      {
        *reinterpret_cast<FieldT*>(data) = (reinterpret_cast<T*>(this_)->*fieldPtr);
      };

      setter = [=](void* this_, void* other)
      {
        FieldT& field = (reinterpret_cast<T*>(this_)->*fieldPtr);
        field = *reinterpret_cast<FieldT*>(other);
      };
    }

    // Constructs a field info from a static getter.
    template <class FieldT1>
    DataInfo(std::string name_, FieldT1(*getter)()) :
      name(move(name_)),
      type(&TypeOf<detail::decayed<FieldT1>>())
    {
      this->getter = [=](void* this_, void* data)
      {
        *reinterpret_cast<FieldT1*>(data) = getter();
      };
    }

    // Constructs a field info from a getter.
    template <class FieldT1, class T1>
    DataInfo(std::string name_, FieldT1(T1::*getter)() const) :
      name(move(name_)),
      ownerType(&TypeOf<T1>()),
      type(&TypeOf<detail::decayed<FieldT1>>())
    {
      // Create the generic getter function. Takes the 'this' pointer as a void*
      //  and casts it to the class type. It calls the getter function and returns
      //  the result as a variant.
      this->getter = [=](void* this_, void* data)
      {
        *reinterpret_cast<FieldT1*>(data) = (reinterpret_cast<T1*>(this_)->*getter)();
      };
    }

    // Constructs a field info from a member getter returning a reference.
    template <class FieldT1, class T1>
    DataInfo(std::string name_, FieldT1&(T1::*getter)() const) :
      name(move(name_)),
      ownerType(&TypeOf<T1>()),
      type(&TypeOf<detail::decayed<FieldT1>>())
    {
      // Create the generic getter function. Takes the 'this' pointer as a void*
      //  and casts it to the class type. It calls the getter function and returns
      //  the result as a variant.
      this->getter = [=](void* this_, void* data)
      {
        *reinterpret_cast<FieldT1*>(data) = &(reinterpret_cast<T1*>(this_)->*getter)();
      };
    }

    // Constructs a field info from a getter / setter pair.
    template <class FieldT1, class FieldT2, class T1, class T2>
    DataInfo(std::string name_, FieldT1(T1::*getter)() const, void(T2::*setter)(FieldT2)) :
      name(move(name_)),
      ownerType(&TypeOf<T1>()),
      type(&TypeOf<detail::decayed<FieldT1>>())
    {
      // Create the generic getter function. Takes the 'this' pointer as a void*
      //  and casts it to the class type. It calls the getter function and returns
      //  the result as a variant.
      this->getter = [=](void* this_, void* data)
      {
        *reinterpret_cast<FieldT1*>(data) = (reinterpret_cast<T1*>(this_)->*getter)();
      };

      // Create the generic setter function. Takes the 'this' pointer as a void*
      //  and the value as a void* and casts the pointers to their appropriate
      //  types prior to calling the set function.
      this->setter = [=](void* this_, void* value)
      {
        (reinterpret_cast<T2*>(this_)->*setter)(*reinterpret_cast<typename std::decay<FieldT2>::type*>(value));
      };
    }

    // Constructs a field info from a static getter / setter pair.
    template <class FieldT1, class FieldT2>
    DataInfo(std::string name_, FieldT1(*getter)(), void(*setter)(FieldT2)) :
      name(std::move(name_)),
      type(&TypeOf<detail::decayed<FieldT1>>())
    {
      // Create the generic getter function. Takes the 'this' pointer as a void*
      //  and casts it to the class type. It calls the getter function and returns
      //  the result as a variant.
      this->getter = [=](void* this_, void* data)
      {
        *reinterpret_cast<FieldT1*>(data) = getter();
      };

      // Create the generic setter function. Takes the 'this' pointer as a void*
      //  and the value as a void* and casts the pointers to their appropriate
      //  types prior to calling the set function.
      this->setter = [=](void* this_, void* value)
      {
        setter(*reinterpret_cast<detail::decayed<FieldT2>*>(value));
      };
    }

    // Initializes a field info from a std::function getter.
    template <class GetResult>
    DataInfo(std::string name_, std::function<GetResult()> getter) :
      name(std::move(name_)),
      type(&TypeOf<detail::decayed<GetResult>>())
    {
      this->getter = [=](void* this_, void* data)
      {
        *reinterpret_cast<detail::decayed<GetResult>*>(data) = getter();
      };
    }

    ~DataInfo() = default;

    // Returns a pointer to the field given its type.
    template <class FieldT>
    FieldT* Address() const
    {
      if (ownerType != nullptr) return nullptr;
      if (&TypeOf<FieldT>() != type) return nullptr;
      return reinterpret_cast<FieldT*>(getFieldPointer(nullptr));
    }

    // Returns a pointer to the field given its type
    //  and valid reference to the owning class.
    template <class FieldT, class T>
    FieldT* Address(T& this_) const
    {
      if (&TypeOf<T>() != ownerType) return nullptr;
      if (&TypeOf<FieldT>() != type) return nullptr;
      return reinterpret_cast<FieldT*>(getFieldPointer(&this_));
    }
  };

  static struct Addition_     * const TagAddition    = nullptr;
  static struct Destructor_   * const TagDestructor  = nullptr;
  static struct LeftShift_    * const TagLeftShift   = nullptr;
  static struct ReadOnly_     * const TagReadOnly    = nullptr;
  static struct RightShift_   * const TagRightShift  = nullptr;
  static struct WriteOnly_    * const TagWriteOnly   = nullptr;
} // namespace reflect