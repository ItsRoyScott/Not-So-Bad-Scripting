#include "reflect/Reflection.hpp"
#include <iostream>
#include <sstream>

using namespace reflect;
using namespace std;

namespace ns
{
  struct Foo
  {
    Foo() = default;
    Foo(int i_) : i(i_) {}
    Foo(const Foo& b) = default;
    Foo& operator=(const Foo& b) = default;
    ~Foo() = default;

    int i = 0;

    int Add(int j) { return i + j; }
    int I() const { return i; }
    void I(int i_) { i = i_; }
    int IReadOnly() const { return i; }

    Foo operator+(Foo const& b) const { return i + b.i; }

    friend istream& operator>>(istream& is, Foo& f) { return is >> f.i; }
    friend ostream& operator<<(ostream& os, Foo const& f) { return os << f.i; }

    static int si;

    static int Subtract(int j) { return si - j; }
    static int SI() { return si; }
    static void SI(int si_) { si = si_; }
    static int SIReadOnly() { return si; }
  };
  int Foo::si = 0;
} // namespace ns

namespace reflect
{
  template <class CallT>
  auto CFunction(typename std::add_pointer<CallT>::type fn) -> decltype(fn)
  {
    return std::move(fn);
  }

  template<>
  struct Binding<ns::Foo> : BindingBase<ns::Foo>
  {
    Binding()
    {
      Bind("ns::Foo",
        "i", &T::i,
        "__add", &T::operator+,
        "I", Const(&T::I), NonConst(&T::I),
        "IReadOnly", &T::IReadOnly, ReadOnly,
        "si", &T::si,
        "Subtract", &T::Subtract,
        "SI", Overloaded0(&T::SI), Overloaded1(&T::SI),
        "SIReadOnly", CFunction<int()>([]() -> int { return T::SIReadOnly(); }), ReadOnly
      );
    }
  };

  template<>
  struct Binding<int> : BindingBase<int>
  {
    Binding()
    {
      Bind("int");
    }
  };

} // namespace reflect

extern "C" __declspec(dllexport) inline void* reflect_GetAssembly(void)
{
  return &Reflection::Instance();
}

int main()
{
  cout << Reflection::Instance() << endl;

  Lua::DoString(1 + R"_LuaScript_(
      local foo = ns.Foo()
      print("foo.i = " .. foo.i)
      local foo2 = ns.Foo()
      foo2.i = 7
      print("foo + foo2 = " .. (foo + foo2).i)
      local i = foo.I
      print("foo.I = " .. i)
      foo.I = 42
      print("foo.i = " .. foo.i)
      i = foo.IReadOnly
      print("foo.IReadOnly = " .. i)
      print("ns.Foo.si = " .. ns.Foo.si)
      i = ns.Foo.SI
      print("ns.Foo.SI = " .. i)
      print("ns.Foo.Subtract(10) = " .. ns.Foo.Subtract(10))
      ns.Foo.SI = 100
      print("ns.Foo.SI = " .. ns.Foo.SI)
      print("ns.Foo.SIReadOnly = " .. ns.Foo.SIReadOnly)
    )_LuaScript_");

  return 0;
}