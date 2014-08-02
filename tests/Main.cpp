#include "reflect/Reflection.hpp"
#include <cmath>
#include <iostream>
#include <sstream>

using namespace reflect;
using namespace std;

namespace ns
{
  struct Foo
  {
  public: // data

    int i = 0;
    static int si;

  public: // methods

    Foo() = default;
    Foo(int i_) : i(i_) {}
    Foo(const Foo& b) = default;
    Foo& operator=(const Foo& b) = default;
    ~Foo() = default;

    int Add(int j) { return i + j; }
    int I() const { return i; }
    void I(int i_) { i = i_; }
    int IReadOnly() const { return i; }
    static int Subtract(int j) { return si - j; }
    static int SI() { return si; }
    static void SI(int si_) { si = si_; }
    static int SIReadOnly() { return si; }

  public: // operators

    Foo operator+() const { return +i; }
    Foo operator-() const { return -i; }

    Foo operator+(Foo const& b) const { return i + b.i; }
    Foo operator-(Foo const& b) const { return i - b.i; }
    Foo operator*(Foo const& b) const { return i * b.i; }
    Foo operator/(Foo const& b) const { return i / b.i; }

    Foo operator%(Foo const& b) const { return i % b.i; }
    Foo operator^(Foo const& b) const { return (int)std::pow(i, b.i); }

    Foo& operator+=(Foo const& b)
    {
      i += b.i;
      return *this;
    }
    Foo& operator-=(Foo const& b)
    {
      i -= b.i;
      return *this;
    }
    Foo& operator*=(Foo const& b)
    {
      i *= b.i;
      return *this;
    }
    Foo& operator/=(Foo const& b)
    {
      i /= b.i;
      return *this;
    }

    friend istream& operator>>(istream& is, Foo& f) { return is >> f.i; }
    friend ostream& operator<<(ostream& os, Foo const& f) { return os << f.i; }
  };
  int Foo::si = 0;

  namespace sub
  {
    float Data = 1;
    inline float Function(float x, float y) { return x * y; }
  }
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
        "I", Const(&T::I), NonConst(&T::I),
        "IReadOnly", &T::IReadOnly, TagReadOnly,
        "si", &T::si,
        "Subtract", &T::Subtract,
        "SI", Overloaded0(&T::SI), Overloaded1(&T::SI),
        "SIReadOnly", CFunction<int()>([]() -> int { return T::SIReadOnly(); }), TagReadOnly,
        Overloaded1(&T::operator+), TagPlus,
        Overloaded1(&T::operator-), TagMinus,
        &T::operator*, TagMultiplication,
        &T::operator/, TagDivision,
        &T::operator+=, TagPlus,
        &T::operator-=, TagMinus,
        &T::operator*=, TagMultiplication,
        &T::operator/=, TagDivision,
        Overloaded0(&T::operator+), TagPlus,
        Overloaded0(&T::operator-), TagMinus,
        &T::operator%, TagModulo,
        &T::operator^, TagXor
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

  struct Namespace {};
  template<>
  struct Binding<Namespace> : BindingBase<Namespace>
  {
    Binding()
    {
      BindNamespace("ns::sub",
        "Data", &ns::sub::Data,
        "Function", &ns::sub::Function);
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

  NamespaceInfo& nsInfo = Reflection::Instance().GetNamespace("ns::sub");
  nsInfo.Data["Data"].Set<float>(27.0f);
  assert(ns::sub::Data == 27);
  float f = nsInfo.Functions["Function"].AsFunction<float(float, float)>()(3, 4);
  assert(f == 12);

  Lua::DoString(1 + R"_LuaScript_(
      -- functions
      local foo = ns.Foo()
      print("foo.i = " .. foo.i)
      local foo2 = ns.Foo()
      foo2.i = 7
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

      -- operators
      foo.i = 7
      foo2.i = 3
      print("\nTesting operators with foo(" .. foo.i .. ") and foo2(" .. foo2.i .. ")")
      print("foo + foo2 = " .. (foo + foo2).i)
      print("foo - foo2 = " .. (foo - foo2).i)
      print("foo * foo2 = " .. (foo * foo2).i)
      print("foo / foo2 = " .. (foo / foo2).i)
      print("Foo % foo2 = " .. (foo % foo2).i)
      print("Foo ^ foo2 = " .. (foo ^ foo2).i)
    )_LuaScript_");

  return 0;
}