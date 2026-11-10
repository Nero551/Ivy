// test_meta.cpp — Tests for the Ivy::Meta reflection system.
//
// Link with: gtest, gtest_main
// Include path: project root (so "Protos/Meta/Meta.hpp" resolves).

#include <gtest/gtest.h>

#include <cstddef>
#include <string_view>
#include <utility>

#include "Reflect/Basic.hpp"
#include "Reflect/ReflectedInstance.hpp"
#include "Reflect/ReflectedReference.hpp"
#include "Reflect/Registry.hpp"

using namespace Ivy::Reflect;

// ============================================================================
// Fixture types
// ============================================================================
namespace Ivy::UnitTests
{

struct Point
{
    int X = 0;
    int Y = 0;
    int* Ptr = nullptr;
    const int Fixed = 100;

    Point() = default;
    Point(int x, int y) : X(x), Y(y) {}
    Point(int v) : X(v), Y(v) {}

    int Sum() const
    {
        return X + Y;
    }
    void SetX(int v)
    {
        X = v;
    }
    int& XRef()
    {
        return X;
    }
    int& YRef()
    {
        return Y;
    }
    int Get() const
    {
        return X;
    }
    int AddOffset(int offset) const
    {
        return X + offset;
    }
    float AddFloat(float offset) const
    {
        return static_cast<float>(X) + offset;
    }
};

struct Simple
{
    int Value = 5;
    Simple() = default;
    Simple(int v) : Value(v) {}
};

struct Counter
{
    static int Alive;
    int Value = 0;

    Counter()
    {
        ++Alive;
    }
    Counter(int v) : Value(v)
    {
        ++Alive;
    }
    Counter(const Counter& o) : Value(o.Value)
    {
        ++Alive;
    }
    ~Counter()
    {
        --Alive;
    }
};

int Counter::Alive = 0;

// The names actually stored in the registry — matches what REFLECT / Create /
// Inspect will look up.
constexpr std::string_view kPointName = GetTypeName<Point>();
constexpr std::string_view kSimpleName = GetTypeName<Simple>();
constexpr std::string_view kCounterName = GetTypeName<Counter>();

// ============================================================================
// One-time registration
// ============================================================================
//
// Registration mutates the type descriptor (appends to vectors), so it must be
// performed exactly once. SetUpTestSuite gives us exactly one shot per run.

void RegisterAllTypes()
{
    // ---- Point -----------------------------------------------------------
    REFLECT(Point);
    FIELD(Point, X);
    FIELD(Point, Y);
    FIELD(Point, Ptr);
    FIELD(Point, Fixed);
    CONSTRUCTOR(Point);
    CONSTRUCTOR(Point, int, int);
    CONSTRUCTOR(Point, int);
    METHOD(Point, Sum, int);
    METHOD(Point, SetX, void, int);
    METHOD(Point, XRef, int&);
    METHOD(Point, YRef, int&);
    METHOD(Point, Get, int);
    METHOD(Point, AddOffset, int, int);
    METHOD(Point, AddFloat, float, float);

    // ---- Simple ----------------------------------------------------------
    REFLECT(Simple);
    FIELD(Simple, Value);
    CONSTRUCTOR(Simple);
    CONSTRUCTOR(Simple, int);

    // ---- Counter ---------------------------------------------------------
    REFLECT(Counter);
    FIELD(Counter, Value);
    CONSTRUCTOR(Counter);
    CONSTRUCTOR(Counter, int);
}

// ============================================================================
// Test suite
// ============================================================================

class ReflectionTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite()
    {
        RegisterAllTypes();
    }
};

// ============================================================================
// Type registration
// ============================================================================

TEST_F(ReflectionTest, TypeDescriptorIsStable)
{
    auto& a = REFLECT(Point);
    auto& b = REFLECT(Point);
    EXPECT_EQ(&a, &b);
    EXPECT_EQ(a.TypeId, b.TypeId);
}

TEST_F(ReflectionTest, TypeIdsAreDistinct)
{
    EXPECT_NE(REFLECT(Point).TypeId, REFLECT(Simple).TypeId);
    EXPECT_NE(REFLECT(int).TypeId, REFLECT(float).TypeId);
    EXPECT_NE(REFLECT(int).TypeId, REFLECT(unsigned).TypeId);
    EXPECT_NE(REFLECT(Point).TypeId, REFLECT(int).TypeId);
}

TEST_F(ReflectionTest, TypeMetadata)
{
    auto& t = REFLECT(Point);
    EXPECT_EQ(t.Name, kPointName);
    EXPECT_EQ(t.Size, sizeof(Point));
    EXPECT_EQ(t.Alignment, alignof(Point));
}

TEST_F(ReflectionTest, VoidTypeIsRegisterable)
{
    auto& v = REFLECT(void);
    EXPECT_EQ(v.Name, "void");
    EXPECT_EQ(v.Size, 0u);
}

// ============================================================================
// Field registration
// ============================================================================

TEST_F(ReflectionTest, FieldsRegistered)
{
    auto& t = REFLECT(Point);
    EXPECT_TRUE(t.Fields.Contains("X"));
    EXPECT_TRUE(t.Fields.Contains("Y"));
    EXPECT_TRUE(t.Fields.Contains("Ptr"));
    EXPECT_TRUE(t.Fields.Contains("Fixed"));
    EXPECT_FALSE(t.Fields.Contains("DoesNotExist"));
}

TEST_F(ReflectionTest, FieldOffsets)
{
    auto& t = REFLECT(Point);
    EXPECT_EQ(t.Fields.At("X").Offset, offsetof(Point, X));
    EXPECT_EQ(t.Fields.At("Y").Offset, offsetof(Point, Y));
    EXPECT_EQ(t.Fields.At("Ptr").Offset, offsetof(Point, Ptr));
    EXPECT_EQ(t.Fields.At("Fixed").Offset, offsetof(Point, Fixed));
}

TEST_F(ReflectionTest, FieldTraits)
{
    auto& t = REFLECT(Point);

    auto& x = t.Fields.At("X");
    EXPECT_FALSE(x.IsConst);
    EXPECT_FALSE(x.IsPointer);
    EXPECT_FALSE(x.IsReference);
    EXPECT_FALSE(x.IsRValue);

    EXPECT_TRUE(t.Fields.At("Fixed").IsConst);
    EXPECT_TRUE(t.Fields.At("Ptr").IsPointer);
    EXPECT_FALSE(t.Fields.At("Fixed").IsPointer);
}

TEST_F(ReflectionTest, FieldTypeInfo)
{
    auto& x = REFLECT(Point).Fields.At("X");
    EXPECT_EQ(x.Type->TypeId, REFLECT(int).TypeId);
    EXPECT_EQ(x.Type->Size, sizeof(int));

    // The reflection system uses CleanType, so a pointer field shares the
    // pointee's TypeId (int* collapses to int).
    auto& ptr = REFLECT(Point).Fields.At("Ptr");
    EXPECT_EQ(ptr.Type->TypeId, GetTypeId<int>());
    EXPECT_TRUE(ptr.IsPointer);
    EXPECT_FALSE(ptr.IsConst);
}

// ============================================================================
// Get / Set via ReflectedReference
// ============================================================================

TEST_F(ReflectionTest, GetFieldByReference)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);

    EXPECT_EQ(ref.GetField("X").As<int>(), 10);
    EXPECT_EQ(ref.GetField("Y").As<int>(), 20);
    EXPECT_EQ(ref.GetField("Fixed").As<const int>(), 100);
}

TEST_F(ReflectionTest, SetFieldByReference)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);

    ref.SetField("X", 99);
    EXPECT_EQ(p.X, 99);

    ref.SetField("Y", -5);
    EXPECT_EQ(p.Y, -5);
}

TEST_F(ReflectionTest, GetPointerField)
{
    int target = 42;
    Point p{1, 2};
    p.Ptr = &target;

    auto ref = Inspect(&p, kPointName);
    auto& ptr = ref.GetField("Ptr").As<int*>();
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*ptr, 42);
}

TEST_F(ReflectionTest, ReferenceDoesNotOwnObject)
{
    Point p{10, 20};
    {
        auto ref = Inspect(&p, kPointName);
        ref.SetField("X", 555);
    }
    // p must still be alive after ref goes out of scope.
    EXPECT_EQ(p.X, 555);
}

// ============================================================================
// Construction
// ============================================================================

TEST_F(ReflectionTest, DefaultConstruct)
{
    auto instance = Create(kPointName);
    instance.SetField("X", 7);
    instance.SetField("Y", 8);
    EXPECT_EQ(instance.GetField("X").As<int>(), 7);
    EXPECT_EQ(instance.GetField("Y").As<int>(), 8);
}

TEST_F(ReflectionTest, TwoArgConstruct)
{
    auto instance = Create(kPointName, 10, 20);
    EXPECT_EQ(instance.GetField("X").As<int>(), 10);
    EXPECT_EQ(instance.GetField("Y").As<int>(), 20);
}

TEST_F(ReflectionTest, OneArgConstructSelectsCorrectOverload)
{
    auto instance = Create(kPointName, 42);
    // The single-int ctor sets both X and Y to the argument.
    EXPECT_EQ(instance.GetField("X").As<int>(), 42);
    EXPECT_EQ(instance.GetField("Y").As<int>(), 42);
}

TEST_F(ReflectionTest, CopyConstruct)
{
    auto original = Create(kPointName, 10, 20);
    auto copy = original;

    EXPECT_EQ(copy.GetField("X").As<int>(), 10);
    EXPECT_EQ(copy.GetField("Y").As<int>(), 20);

    copy.SetField("X", 99);
    EXPECT_EQ(copy.GetField("X").As<int>(), 99);
    EXPECT_EQ(original.GetField("X").As<int>(), 10); // original is unaffected
}

TEST_F(ReflectionTest, MoveConstruct)
{
    auto original = Create(kPointName, 3, 4);
    auto moved = std::move(original);
    EXPECT_EQ(moved.GetField("X").As<int>(), 3);
    EXPECT_EQ(moved.GetField("Y").As<int>(), 4);
}

TEST_F(ReflectionTest, InstanceProperlyDestructsObject)
{
    int base = Counter::Alive;
    {
        auto instance = Create(kCounterName, 123);
        EXPECT_EQ(Counter::Alive, base + 1);
        EXPECT_EQ(instance.GetField("Value").As<int>(), 123);
    }
    EXPECT_EQ(Counter::Alive, base);
}

// ============================================================================
// Method invocation
// ============================================================================

TEST_F(ReflectionTest, VoidMethodByReference)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);
    ref.InvokeMethod("SetX", 77);
    EXPECT_EQ(p.X, 77);
}

TEST_F(ReflectionTest, VoidMethodByInstance)
{
    auto instance = Create(kPointName, 1, 2);
    instance.InvokeMethod("SetX", 42);
    EXPECT_EQ(instance.GetField("X").As<int>(), 42);
}

TEST_F(ReflectionTest, ValueReturnMethod)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);
    auto result = ref.InvokeMethod("Sum");
    EXPECT_EQ(result.As<int>(), 30);
    EXPECT_FALSE(result.Description.IsReference);
}

TEST_F(ReflectionTest, ReferenceReturnMethodMutatesOriginal)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);
    auto result = ref.InvokeMethod("XRef");
    EXPECT_EQ(result.As<int>(), 10);
    EXPECT_TRUE(result.Description.IsReference);

    result.As<int>() = 555;
    EXPECT_EQ(p.X, 555);
}

TEST_F(ReflectionTest, SecondReferenceReturn)
{
    Point p{1, 2};
    auto ref = Inspect(&p, kPointName);
    auto result = ref.InvokeMethod("YRef");
    result.As<int>() = 888;
    EXPECT_EQ(p.Y, 888);
}

TEST_F(ReflectionTest, MethodWithZeroArgs)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);
    EXPECT_EQ(ref.InvokeMethod("Get").As<int>(), 10);
}

TEST_F(ReflectionTest, MethodWithIntArg)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);
    EXPECT_EQ(ref.InvokeMethod("AddOffset", 5).As<int>(), 15);
}

TEST_F(ReflectionTest, MethodReturningFloat)
{
    Point p{10, 20};
    auto ref = Inspect(&p, kPointName);
    EXPECT_FLOAT_EQ(ref.InvokeMethod("AddFloat", 0.5f).As<float>(), 10.5f);
}

// ============================================================================
// Method metadata
// ============================================================================

TEST_F(ReflectionTest, MethodParametersRegistered)
{
    auto& t = REFLECT(Point);
    bool found = false;
    for (auto& m : t.Methods)
    {
        if (m.Name == "SetX")
        {
            found = true;
            ASSERT_EQ(m.Parameters.size(), 1u);
            EXPECT_EQ(m.Parameters[0].Type->TypeId, REFLECT(int).TypeId);
            EXPECT_EQ(m.Return.Type->TypeId, REFLECT(void).TypeId);
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ReflectionTest, MethodCountMatchesRegistered)
{
    // Sum, SetX, XRef, YRef, Get, AddOffset, AddFloat
    EXPECT_EQ(REFLECT(Point).Methods.size(), 7u);
}

// ============================================================================
// Constructor metadata
// ============================================================================

TEST_F(ReflectionTest, ConstructorMetadata)
{
    auto& t = REFLECT(Point);
    // Registered: Point(), Point(int,int), Point(int)
    EXPECT_EQ(t.Constructors.size(), 3u);

    bool foundTwoInt = false;
    for (auto& c : t.Constructors)
    {
        if (c.Parameters.size() == 2 && c.Parameters[0].Type->TypeId == REFLECT(int).TypeId &&
            c.Parameters[1].Type->TypeId == REFLECT(int).TypeId)
        {
            foundTwoInt = true;
        }
    }
    EXPECT_TRUE(foundTwoInt);
}

// ============================================================================
// Multi-type independence
// ============================================================================

TEST_F(ReflectionTest, MultipleTypesDoNotInterfere)
{
    auto p = Create(kPointName, 5);
    auto s = Create(kSimpleName, 7);

    EXPECT_EQ(p.GetField("X").As<int>(), 5);
    EXPECT_EQ(s.GetField("Value").As<int>(), 7);

    EXPECT_FALSE(REFLECT(Simple).Fields.Contains("X"));
    EXPECT_FALSE(REFLECT(Point).Fields.Contains("Value"));
    EXPECT_NE(p.GetType().TypeId, s.GetType().TypeId);
}

} // namespace Ivy::UnitTests
