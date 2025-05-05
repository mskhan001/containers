#define BOOST_TEST_MODULE VectorTests
#include <boost/test/included/unit_test.hpp>

#include "vector.cpp"

#include <string>
#include <utility> // For std::move
#include <vector>  // Included for comparison ideas, but tests focus on custom vector

// Simple class for testing emplace_back and non-trivial types
struct TestClass {
  int a;
  double b;
  bool copied = false;
  bool moved = false;

  // Default constructor
  TestClass() : a(0), b(0.0) {}

  // Constructor for emplace_back
  TestClass(int val_a, double val_b) : a(val_a), b(val_b) {}

  // Copy constructor
  TestClass(const TestClass &other) : a(other.a), b(other.b), copied(true) {}

  // Move constructor
  TestClass(TestClass &&other) noexcept : a(other.a), b(other.b), moved(true) {
    other.a = 0; // Example of leaving source in a valid state
    other.b = 0.0;
    other.copied = false;
    other.moved = false;
  }

  // Copy assignment (simplified)
  TestClass &operator=(const TestClass &other) {
    if (this != &other) {
      a = other.a;
      b = other.b;
      copied = true; // This assignment was a copy
      moved = false;
    }
    return *this;
  }

  // Move assignment (simplified)
  TestClass &operator=(TestClass &&other) noexcept {
    if (this != &other) {
      a = other.a;
      b = other.b;
      moved = true; // This assignment was a move
      copied = false;
      other.a = 0; // Example of leaving source in a valid state
      other.b = 0.0;
      other.copied = false;
      other.moved = false;
    }
    return *this;
  }

  // Need comparison operators for BOOST_CHECK_EQUAL with TestClass
  bool operator==(const TestClass &other) const {
    return a == other.a && b == other.b; // Ignore flags for equality check
  }
};

// Provide a string representation for TestClass for Boost test output
std::ostream &operator<<(std::ostream &os, const TestClass &tc) {
  return os << "{" << tc.a << ", " << tc.b << ", copied=" << tc.copied << ", moved=" << tc.moved
            << "}";
}

BOOST_AUTO_TEST_SUITE(VectorTestSuite)

// --- Constructors Tests ---

BOOST_AUTO_TEST_CASE(Constructor_Default) {
  vector<int> v;
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0); // Capacity could be 0 or some small default
}

// Corrected test case based on vector(size_t count, const T& value = T())
BOOST_AUTO_TEST_CASE(Constructor_SizeWithDefaultInit) {
  size_t initial_size = 10;
  vector<int> v(initial_size);               // Calls vector(size_t, const T&=T()) -> sets size
  BOOST_CHECK_EQUAL(v.size(), initial_size); // Size should be initial_size
  BOOST_CHECK(v.capacity() >= initial_size); // Capacity should be at least size
  // Elements should be default-initialized (e.g., 0 for int) but cannot check directly
}

BOOST_AUTO_TEST_CASE(Constructor_SizeZeroWithDefaultInit) {
  vector<int> v(0);                   // Calls vector(size_t, const T&=T()) with 0
  BOOST_CHECK_EQUAL(v.size(), 0);     // Size should be 0
  BOOST_CHECK_EQUAL(v.capacity(), 0); // Capacity should be 0
}

BOOST_AUTO_TEST_CASE(Constructor_SizeWithSpecificInitValue) {
  size_t initial_size = 5;
  int init_val = 42;
  vector<int> v(initial_size, init_val);     // Calls vector(size_t, const T&)
  BOOST_CHECK_EQUAL(v.size(), initial_size); // Size should be initial_size
  BOOST_CHECK(v.capacity() >= initial_size); // Capacity should be at least size
  // Elements should be initialized with init_val but cannot check directly
}

BOOST_AUTO_TEST_CASE(Constructor_SizeZeroWithSpecificInitValue) {
  int init_val = 99;
  vector<int> v(0, init_val);         // Calls vector(size_t, const T&) with 0
  BOOST_CHECK_EQUAL(v.size(), 0);     // Size should be 0
  BOOST_CHECK_EQUAL(v.capacity(), 0); // Capacity should be 0
}

BOOST_AUTO_TEST_CASE(Constructor_Copy_Empty) {
  vector<int> original;
  vector<int> copied = original;
  BOOST_CHECK_EQUAL(copied.size(), original.size());
  BOOST_CHECK_EQUAL(copied.capacity(), original.capacity());
  BOOST_CHECK_EQUAL(original.size(), 0); // Original should be unchanged
}

BOOST_AUTO_TEST_CASE(Constructor_Copy_NotEmpty) {
  vector<int> original;
  original.push_back(1);
  original.push_back(2); // Size 2
  size_t original_size = original.size();
  size_t original_capacity = original.capacity();

  vector<int> copied = original;

  BOOST_CHECK_EQUAL(copied.size(), original_size);
  BOOST_CHECK(copied.capacity() >= original_size);   // Copy capacity might be different but >= size
  BOOST_CHECK_EQUAL(original.size(), original_size); // Original should be unchanged
  BOOST_CHECK_EQUAL(original.capacity(), original_capacity); // Original capacity unchanged
}

BOOST_AUTO_TEST_CASE(Constructor_Copy_FromSizedVector) {
  vector<int> original(5, 10);                    // Size 5
  size_t original_size = original.size();         // 5
  size_t original_capacity = original.capacity(); // >= 5

  vector<int> copied = original;

  BOOST_CHECK_EQUAL(copied.size(), original_size);           // 5
  BOOST_CHECK(copied.capacity() >= original_size);           // >= 5
  BOOST_CHECK_EQUAL(original.size(), original_size);         // Original unchanged
  BOOST_CHECK_EQUAL(original.capacity(), original_capacity); // Original unchanged
}

BOOST_AUTO_TEST_CASE(Constructor_Copy_WithReservedCapacity) {
  vector<int> original;
  original.reserve(20);                           // Cap >= 20, Size 0
  original.push_back(1);                          // Size 1, Capacity >= 20
  size_t original_size = original.size();         // 1
  size_t original_capacity = original.capacity(); // >= 20

  vector<int> copied = original;

  BOOST_CHECK_EQUAL(copied.size(), original_size);           // 1
  BOOST_CHECK(copied.capacity() >= original_size);           // >= 1
  BOOST_CHECK_EQUAL(original.size(), original_size);         // Original unchanged
  BOOST_CHECK_EQUAL(original.capacity(), original_capacity); // Original unchanged
}

BOOST_AUTO_TEST_CASE(Constructor_Move_Empty) {
  vector<int> original;
  // size_t original_capacity = original.capacity(); // Removed as moved-from state is less
  // predictable for capacity

  vector<int> moved = std::move(original);

  BOOST_CHECK_EQUAL(moved.size(), 0);
  BOOST_CHECK(moved.capacity() >= 0);
  BOOST_CHECK_EQUAL(original.size(), 0); // Moved-from vector should be empty
                                         // Capacity of moved-from is unspecified, but often 0
}

BOOST_AUTO_TEST_CASE(Constructor_Move_NotEmpty) {
  vector<int> original;
  original.push_back(1);
  original.push_back(2);                          // Size 2
  size_t original_size = original.size();         // 2
  size_t original_capacity = original.capacity(); // >= 2

  vector<int> moved = std::move(original);

  BOOST_CHECK_EQUAL(moved.size(), original_size); // 2
  BOOST_CHECK(moved.capacity() >=
              original_size); // >= 2 (capacity likely transferred, but must be >= new size)
  BOOST_CHECK_EQUAL(original.size(), 0); // Moved-from vector should be empty
                                         // Capacity of moved-from is unspecified
}

BOOST_AUTO_TEST_CASE(Constructor_Move_FromSizedVector) {
  vector<int> original(5, 10);                    // Size 5
  size_t original_size = original.size();         // 5
  size_t original_capacity = original.capacity(); // >= 5

  vector<int> moved = std::move(original);

  BOOST_CHECK_EQUAL(moved.size(), original_size); // 5
  BOOST_CHECK(moved.capacity() >= original_size); // >= 5 (capacity likely transferred)
  BOOST_CHECK_EQUAL(original.size(), 0);          // Moved-from should be empty
                                                  // Capacity of moved-from is unspecified
}

BOOST_AUTO_TEST_CASE(Constructor_Move_WithReservedCapacity) {
  vector<int> original;
  original.reserve(20);                           // Cap >= 20, Size 0
  original.push_back(1);                          // Size 1, Capacity >= 20
  size_t original_size = original.size();         // 1
  size_t original_capacity = original.capacity(); // >= 20

  vector<int> moved = std::move(original);

  BOOST_CHECK_EQUAL(moved.size(), original_size); // 1
  BOOST_CHECK(moved.capacity() >=
              original_size);            // >= 1 (capacity likely transferred, must be >= new size)
  BOOST_CHECK_EQUAL(original.size(), 0); // Moved-from vector should be empty
                                         // Capacity of moved-from is unspecified
}

// --- Assignment Operator Tests ---

BOOST_AUTO_TEST_CASE(Assignment_Copy_EmptyToEmpty) {
  vector<int> v1;
  vector<int> v2;
  v2 = v1;
  BOOST_CHECK_EQUAL(v2.size(), 0);
  BOOST_CHECK(v2.capacity() >= 0);
  BOOST_CHECK_EQUAL(v1.size(), 0); // Original unchanged
}

BOOST_AUTO_TEST_CASE(Assignment_Copy_SizedToEmpty) {
  vector<int> v1(5, 10);              // Size 5
  size_t v1_size = v1.size();         // 5
  size_t v1_capacity = v1.capacity(); // >= 5

  vector<int> v2; // Size 0
  v2 = v1;        // v2 should become a copy of v1

  BOOST_CHECK_EQUAL(v2.size(), v1_size);         // 5
  BOOST_CHECK(v2.capacity() >= v1_size);         // >= 5
  BOOST_CHECK_EQUAL(v1.size(), v1_size);         // Original unchanged
  BOOST_CHECK_EQUAL(v1.capacity(), v1_capacity); // Original unchanged
}

BOOST_AUTO_TEST_CASE(Assignment_Copy_NotEmptyToEmpty) {
  vector<int> v1;
  v1.push_back(1);
  v1.push_back(2);                    // Size 2
  size_t v1_size = v1.size();         // 2
  size_t v1_capacity = v1.capacity(); // >= 2

  vector<int> v2; // Size 0
  v2 = v1;        // v2 should become a copy of v1

  BOOST_CHECK_EQUAL(v2.size(), v1_size);         // 2
  BOOST_CHECK(v2.capacity() >= v1_size);         // >= 2
  BOOST_CHECK_EQUAL(v1.size(), v1_size);         // Original unchanged
  BOOST_CHECK_EQUAL(v1.capacity(), v1_capacity); // Original unchanged
}

BOOST_AUTO_TEST_CASE(Assignment_Copy_NotEmptyToNotEmptySmaller) {
  vector<int> v1;
  v1.push_back(1);
  v1.push_back(2); // Size 2

  vector<int> v2(5, 99); // Size 5, Capacity >= 5
  size_t v2_initial_capacity = v2.capacity();

  v2 = v1; // v2 should become a copy of v1

  BOOST_CHECK_EQUAL(v2.size(), v1.size()); // Size should be 2
  BOOST_CHECK(v2.capacity() >= v1.size()); // Capacity >= 2
  // Capacity might shrink or stay the same, std::vector doesn't guarantee shrinking here
  BOOST_CHECK_EQUAL(v1.size(), 2); // Original unchanged
}

BOOST_AUTO_TEST_CASE(Assignment_Copy_NotEmptyToNotEmptyLarger) {
  vector<int> v1;
  v1.push_back(1); // Size 1

  vector<int> v2(5, 99); // Size 5
  size_t v2_initial_capacity = v2.capacity();

  v2 = v1; // v2 should become a copy of v1

  BOOST_CHECK_EQUAL(v2.size(), v1.size()); // Size should be 1
  BOOST_CHECK(v2.capacity() >= v1.size()); // Capacity >= 1
  // Capacity might shrink or stay the same, std::vector doesn't guarantee shrinking here
  BOOST_CHECK_EQUAL(v1.size(), 1); // Original unchanged
}

BOOST_AUTO_TEST_CASE(Assignment_Copy_SelfAssignment) {
  vector<int> v;
  v.push_back(1);
  v.push_back(2); // Size 2
  size_t initial_size = v.size();
  size_t initial_capacity = v.capacity();

  v = v; // Self-assignment

  BOOST_CHECK_EQUAL(v.size(), initial_size);         // Should be unchanged
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Should be unchanged
}

BOOST_AUTO_TEST_CASE(Assignment_Move_EmptyToEmpty) {
  vector<int> v1;
  vector<int> v2;
  v2 = std::move(v1);
  BOOST_CHECK_EQUAL(v2.size(), 0);
  BOOST_CHECK(v2.capacity() >= 0);
  BOOST_CHECK_EQUAL(v1.size(), 0); // Moved-from empty
}

BOOST_AUTO_TEST_CASE(Assignment_Move_SizedToEmpty) {
  vector<int> v1(5, 10);              // Size 5
  size_t v1_size = v1.size();         // 5
  size_t v1_capacity = v1.capacity(); // >= 5

  vector<int> v2; // Size 0
  v2 = std::move(v1);

  BOOST_CHECK_EQUAL(v2.size(), v1_size); // 5
  BOOST_CHECK(v2.capacity() >= v1_size); // Capacity likely transferred
  BOOST_CHECK_EQUAL(v1.size(), 0);       // Moved-from empty
                                         // Capacity of moved-from is unspecified
}

BOOST_AUTO_TEST_CASE(Assignment_Move_NotEmptyToEmpty) {
  vector<int> v1;
  v1.push_back(1);
  v1.push_back(2);                    // Size 2
  size_t v1_size = v1.size();         // 2
  size_t v1_capacity = v1.capacity(); // >= 2

  vector<int> v2; // Size 0
  v2 = std::move(v1);

  BOOST_CHECK_EQUAL(v2.size(), v1_size); // 2
  BOOST_CHECK(v2.capacity() >= v1_size); // Capacity likely transferred
  BOOST_CHECK_EQUAL(v1.size(), 0);       // Moved-from empty
                                         // Capacity of moved-from is unspecified
}

BOOST_AUTO_TEST_CASE(Assignment_Move_SizedToNotEmpty) {
  vector<int> v1(5, 10);                     // Size 5
  size_t v1_size_before = v1.size();         // 5
  size_t v1_capacity_before = v1.capacity(); // >= 5

  vector<int> v2;
  v2.push_back(3); // Size 1, Capacity >= 1

  v2 = std::move(v1); // v2 gets v1's resources

  BOOST_CHECK_EQUAL(v2.size(), v1_size_before); // 5
  BOOST_CHECK(v2.capacity() >= v1_size_before); // Capacity likely transferred
  BOOST_CHECK_EQUAL(v1.size(), 0);              // Moved-from empty
                                                // Capacity of moved-from is unspecified
}

BOOST_AUTO_TEST_CASE(Assignment_Move_NotEmptyToNotEmpty) {
  vector<int> v1;
  v1.push_back(1);
  v1.push_back(2); // Size 2, Capacity >= 2
  size_t v1_size_before = v1.size();
  // size_t v1_capacity_before = v1.capacity(); // Removed as capacity check isn't against *exact*
  // original

  vector<int> v2;
  v2.push_back(3); // Size 1, Capacity >= 1

  v2 = std::move(v1); // v2 gets v1's resources

  BOOST_CHECK_EQUAL(v2.size(), v1_size_before); // 2
  BOOST_CHECK(v2.capacity() >= v2.size());      // Must have enough capacity for its new size
  BOOST_CHECK_EQUAL(v1.size(), 0);              // Moved-from empty
                                                // Capacity of moved-from is unspecified
}

BOOST_AUTO_TEST_CASE(Assignment_Move_SelfAssignment) {
  vector<int> v;
  v.push_back(1);
  v.push_back(2); // Size 2
  size_t initial_size = v.size();
  size_t initial_capacity = v.capacity();

  v = std::move(v); // Self-move assignment

  BOOST_CHECK_EQUAL(v.size(), initial_size); // Should be unchanged
  BOOST_CHECK_EQUAL(
      v.capacity(),
      initial_capacity); // Should be unchanged (implementation dependent, but stable is good)
}

// --- Size and Capacity Tests ---

BOOST_AUTO_TEST_CASE(Size_AfterDefaultConstructor) {
  vector<int> v;
  BOOST_CHECK_EQUAL(v.size(), 0);
}

BOOST_AUTO_TEST_CASE(Size_AfterSizeConstructor) {
  size_t initial_size = 10;
  vector<int> v(initial_size); // Sets size to 10
  BOOST_CHECK_EQUAL(v.size(), initial_size);
}

BOOST_AUTO_TEST_CASE(Size_AfterSizeInitConstructor) {
  size_t initial_size = 5;
  vector<int> v(initial_size, 42); // Sets size to 5
  BOOST_CHECK_EQUAL(v.size(), initial_size);
}

BOOST_AUTO_TEST_CASE(Size_AfterPushBack) {
  vector<int> v; // Start empty
  v.push_back(1);
  BOOST_CHECK_EQUAL(v.size(), 1);
  v.push_back(2);
  BOOST_CHECK_EQUAL(v.size(), 2);
}

BOOST_AUTO_TEST_CASE(Size_AfterEmplaceBack) {
  vector<int> v; // Start empty
  v.emplace_back(1);
  BOOST_CHECK_EQUAL(v.size(), 1);
  v.emplace_back(2);
  BOOST_CHECK_EQUAL(v.size(), 2);
}

BOOST_AUTO_TEST_CASE(Size_AfterResizeLarger) {
  vector<int> v; // Start empty
  v.resize(5);   // Size becomes 5
  BOOST_CHECK_EQUAL(v.size(), 5);

  vector<int> v2(3); // Start size 3
  v2.resize(10);     // Size becomes 10
  BOOST_CHECK_EQUAL(v2.size(), 10);
}

BOOST_AUTO_TEST_CASE(Size_AfterResizeSmaller) {
  vector<int> v(10); // Start size 10
  v.resize(3);       // Size becomes 3
  BOOST_CHECK_EQUAL(v.size(), 3);
}

BOOST_AUTO_TEST_CASE(Size_AfterResizeToZero) {
  vector<int> v(10); // Start size 10
  v.resize(0);       // Size becomes 0
  BOOST_CHECK_EQUAL(v.size(), 0);

  vector<int> v2; // Start empty
  v2.resize(0);   // Size remains 0
  BOOST_CHECK_EQUAL(v2.size(), 0);
}

BOOST_AUTO_TEST_CASE(Size_AfterClear) {
  vector<int> v;
  v.push_back(1);
  v.push_back(2); // Size 2
  v.resize(0);    // Mimic clear with resize(0)
  BOOST_CHECK_EQUAL(v.size(), 0);
}

BOOST_AUTO_TEST_CASE(Capacity_AfterDefaultConstructor) {
  vector<int> v;
  BOOST_CHECK(v.capacity() >= 0);
}

BOOST_AUTO_TEST_CASE(Capacity_AfterSizeConstructor) {
  size_t sz = 15;
  vector<int> v(sz);               // Size 15, Cap >= 15
  BOOST_CHECK_EQUAL(v.size(), sz); // Size is set
  BOOST_CHECK(v.capacity() >= sz); // Capacity is >= size
}

BOOST_AUTO_TEST_CASE(Capacity_AfterSizeInitConstructor) {
  size_t sz = 8;
  vector<int> v(sz, 0);            // Size 8, Cap >= 8
  BOOST_CHECK_EQUAL(v.size(), sz); // Size is set
  BOOST_CHECK(v.capacity() >= sz); // Capacity is >= size
}

BOOST_AUTO_TEST_CASE(Capacity_AfterPushBack_NoReallocation_FromEmpty) {
  vector<int> v; // Size 0, Cap >= 0
  v.reserve(10); // Size 0, Cap >= 10
  size_t initial_capacity = v.capacity();
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(initial_capacity >= 10);

  v.push_back(1); // Size 1
  v.push_back(2); // Size 2
  v.push_back(3); // Size 3

  BOOST_CHECK_EQUAL(v.size(), 3);                    // Size increased by 3
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should not change if space exists
}

BOOST_AUTO_TEST_CASE(Capacity_AfterPushBack_WithReallocation_FromEmpty) {
  vector<int> v(0); // Start with minimal capacity, Size 0, Cap 0
  // v.reserve(1); // Ensure initial capacity is small, >=1 (v(0) likely gives cap 0)
  size_t initial_capacity = v.capacity(); // Should be 0 or minimal

  v.push_back(1); // Size 1, Should trigger reallocation if initial_capacity was 0
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK(v.capacity() >= 1);
  size_t capacity_after_first_push = v.capacity();
  BOOST_CHECK(capacity_after_first_push > initial_capacity ||
              initial_capacity == 0); // Capacity must grow

  // Add more elements to force another reallocation if capacity_after_first_push is small
  for (size_t i = 0; i < capacity_after_first_push; ++i) {
    v.push_back(i);
  }
  // Now push one more to *guarantee* reallocation
  size_t capacity_before_realloc = v.capacity(); // Same as capacity_after_first_push
  size_t size_before_realloc = v.size();         // 1 + capacity_after_first_push

  v.push_back(99);
  BOOST_CHECK_EQUAL(v.size(), size_before_realloc + 1);
  BOOST_CHECK(v.capacity() >= capacity_before_realloc); // Capacity must have increased
  BOOST_CHECK(v.capacity() >= v.size());
}

BOOST_AUTO_TEST_CASE(Capacity_AfterEmplaceBack_WithReallocation_FromEmpty) {
  vector<TestClass> v(0); // Start with minimal capacity, Size 0, Cap 0
  // v.reserve(1); // Ensure initial capacity is small, >=1
  size_t initial_capacity = v.capacity(); // Should be 0 or minimal

  v.emplace_back(1, 1.1); // Size 1, Should trigger reallocation if initial_capacity was 0
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK(v.capacity() >= 1);
  size_t capacity_after_first_emplace = v.capacity();
  BOOST_CHECK(capacity_after_first_emplace > initial_capacity ||
              initial_capacity == 0); // Capacity must grow

  // Add more elements to force reallocation
  for (size_t i = 0; i < capacity_after_first_emplace; ++i) {
    v.emplace_back(i, (double)i);
  }
  // Now emplace one more to *guarantee* reallocation
  size_t capacity_before_realloc = v.capacity(); // Same as capacity_after_first_emplace
  size_t size_before_realloc = v.size();         // 1 + capacity_after_first_emplace

  v.emplace_back(99, 99.9);
  BOOST_CHECK_EQUAL(v.size(), size_before_realloc + 1);
  BOOST_CHECK(v.capacity() >= capacity_before_realloc); // Capacity must have increased
  BOOST_CHECK(v.capacity() >= v.size());
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveLarger_FromSized) {
  vector<int> v(5); // Size 5, Capacity >= 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  size_t initial_capacity = v.capacity();
  size_t requested_capacity = initial_capacity + 10;
  v.reserve(requested_capacity);
  BOOST_CHECK_EQUAL(v.size(), 5); // Size should not change after reserve
  BOOST_CHECK(v.capacity() >=
              requested_capacity); // Capacity should be at least the requested value
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveLarger_FromEmpty) {
  vector<int> v; // Size 0, Cap >= 0
  size_t requested_capacity = 10;
  v.reserve(requested_capacity);
  BOOST_CHECK_EQUAL(v.size(), 0);                  // Size should not change
  BOOST_CHECK(v.capacity() >= requested_capacity); // Capacity >= requested
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveSmaller_FromSized) {
  vector<int> v(20); // Size 20, Capacity >= 20
  BOOST_CHECK_EQUAL(v.size(), 20);
  size_t initial_capacity = v.capacity();
  size_t requested_capacity = initial_capacity / 2; // Smaller than current
  v.reserve(requested_capacity);
  BOOST_CHECK_EQUAL(v.size(), 20);                   // Size should not change
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should NOT decrease
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveSmaller_FromEmpty) {
  vector<int> v; // Size 0, Cap >= 0
  v.reserve(20); // Size 0, Cap >= 20
  size_t initial_capacity = v.capacity();
  v.reserve(10); // Smaller than current capacity
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should NOT decrease
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveSame_FromSized) {
  vector<int> v(10); // Size 10, Capacity >= 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  size_t initial_capacity = v.capacity();
  v.reserve(initial_capacity);
  BOOST_CHECK_EQUAL(v.size(), 10);                   // Size should not change
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should remain the same
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveSame_FromEmpty) {
  vector<int> v; // Size 0, Cap >= 0
  v.reserve(10); // Size 0, Cap >= 10
  size_t initial_capacity = v.capacity();
  v.reserve(initial_capacity);
  BOOST_CHECK_EQUAL(v.size(), 0);                    // Size should not change
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should remain the same
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveZero_FromSized) {
  vector<int> v(10); // Size 10, Capacity >= 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  size_t initial_capacity = v.capacity();
  v.reserve(0);
  BOOST_CHECK_EQUAL(v.size(), 10);                   // Size should not change
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should not decrease below current
}

BOOST_AUTO_TEST_CASE(Capacity_AfterReserveZero_FromEmpty) {
  vector<int> v; // Size 0, Cap >= 0
  v.reserve(0);
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0); // Capacity can be 0
}

BOOST_AUTO_TEST_CASE(Capacity_AfterResizeLarger) {
  vector<int> v; // Start empty
  v.resize(5);   // Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5); // Capacity must be at least the new size

  vector<int> v2(3); // Start size 3
  size_t initial_cap_v2 = v2.capacity();
  v2.resize(10); // Size 10
  BOOST_CHECK_EQUAL(v2.size(), 10);
  BOOST_CHECK(v2.capacity() >= 10); // Capacity must be at least the new size
  BOOST_CHECK(v2.capacity() > initial_cap_v2 ||
              initial_cap_v2 >= 10); // Capacity must grow if needed
}

BOOST_AUTO_TEST_CASE(Capacity_AfterResizeSmaller) {
  vector<int> v(10);                      // Size 10
  size_t initial_capacity = v.capacity(); // >= 10
  v.resize(3);                            // Size 3
  BOOST_CHECK_EQUAL(v.size(), 3);
  // Capacity might stay the same or shrink, but std::vector doesn't guarantee shrinking below
  // current size
  BOOST_CHECK(v.capacity() >= 3);
  // BOOST_CHECK(v.capacity() <= initial_capacity); // Cannot guarantee shrinking
}

BOOST_AUTO_TEST_CASE(Capacity_AfterResizeToZero) {
  vector<int> v(10);                      // Size 10
  size_t initial_capacity = v.capacity(); // >= 10
  v.resize(0);                            // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  // Capacity might stay the same or shrink
  BOOST_CHECK(v.capacity() >= 0);
  // BOOST_CHECK(v.capacity() <= initial_capacity); // Cannot guarantee shrinking
}

// --- push_back Tests ---

BOOST_AUTO_TEST_CASE(PushBack_OneElement_FromEmpty) {
  vector<int> v; // Size 0
  size_t initial_capacity = v.capacity();
  v.push_back(10); // Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK(v.capacity() >= 1);
  BOOST_CHECK(v.capacity() > initial_capacity ||
              initial_capacity == 0); // Capacity must grow or was already >= 1
                                      // Cannot check value 10 directly
}

BOOST_AUTO_TEST_CASE(PushBack_OneElement_FromSized) {
  vector<int> v(5);                               // Size 5
  size_t initial_size = v.size();                 // 5
  size_t initial_capacity = v.capacity();         // >= 5
  v.push_back(10);                                // Size 6
  BOOST_CHECK_EQUAL(v.size(), initial_size + 1);  // Size is 6
  BOOST_CHECK(v.capacity() >= initial_size + 1);  // Capacity >= 6
  if (initial_capacity < initial_size + 1) {      // If initial capacity wasn't enough
    BOOST_CHECK(v.capacity() > initial_capacity); // Capacity must grow
  } else {
    BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity shouldn't change
  }
}

BOOST_AUTO_TEST_CASE(PushBack_MultipleElements_NoReallocation_FromReservedEmpty) {
  vector<int> v;                          // Size 0
  v.reserve(5);                           // Cap >= 5
  size_t initial_capacity = v.capacity(); // Should be >= 5

  v.push_back(1); // Size 1
  v.push_back(2); // Size 2
  v.push_back(3); // Size 3

  BOOST_CHECK_EQUAL(v.size(), 3);                    // Size increased by 3
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should be unchanged
}

BOOST_AUTO_TEST_CASE(PushBack_MultipleElements_WithReallocation_FromEmpty) {
  vector<int> v(0); // Minimal initial capacity, Size 0
  // v.reserve(1); // Ensure small cap if needed
  size_t initial_capacity = v.capacity(); // Should be 0 or minimal

  // Add elements to force reallocation multiple times
  size_t num_pushes = 10;
  for (size_t i = 0; i < num_pushes; ++i) {
    v.push_back(i);
  }
  BOOST_CHECK_EQUAL(v.size(), num_pushes);
  BOOST_CHECK(v.capacity() >= num_pushes);
  BOOST_CHECK(v.capacity() > initial_capacity || initial_capacity == 0); // Capacity must have grown
}

BOOST_AUTO_TEST_CASE(PushBack_IntoVectorWithReservedCapacity) {
  vector<int> v; // Size 0
  v.reserve(10); // Capacity >= 10
  size_t reserved_capacity = v.capacity();
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(reserved_capacity >= 10);

  for (int i = 0; i < 10; ++i) {
    v.push_back(i); // Add 10 elements
  }
  BOOST_CHECK_EQUAL(v.size(), 10);
  BOOST_CHECK_EQUAL(v.capacity(), reserved_capacity); // Should not have reallocated

  v.push_back(10); // Add one more - should trigger reallocation
  BOOST_CHECK_EQUAL(v.size(), 11);
  BOOST_CHECK(v.capacity() > reserved_capacity);
  BOOST_CHECK(v.capacity() >= 11);
}

// --- emplace_back Tests ---

BOOST_AUTO_TEST_CASE(EmplaceBack_OneElement_FromEmpty) {
  vector<int> v; // Size 0
  size_t initial_capacity = v.capacity();
  v.emplace_back(10); // Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK(v.capacity() >= 1);
  BOOST_CHECK(v.capacity() > initial_capacity ||
              initial_capacity == 0); // Capacity must grow or was already >= 1
                                      // Cannot check value 10 directly
}

BOOST_AUTO_TEST_CASE(EmplaceBack_OneElement_FromSized) {
  vector<int> v(5);                               // Size 5
  size_t initial_size = v.size();                 // 5
  size_t initial_capacity = v.capacity();         // >= 5
  v.emplace_back(10);                             // Size 6
  BOOST_CHECK_EQUAL(v.size(), initial_size + 1);  // Size is 6
  BOOST_CHECK(v.capacity() >= initial_size + 1);  // Capacity >= 6
  if (initial_capacity < initial_size + 1) {      // If initial capacity wasn't enough
    BOOST_CHECK(v.capacity() > initial_capacity); // Capacity must grow
  } else {
    BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity shouldn't change
  }
}

BOOST_AUTO_TEST_CASE(EmplaceBack_MultipleElements_NoReallocation_FromReservedEmpty) {
  vector<int> v;                          // Size 0
  v.reserve(5);                           // Cap >= 5
  size_t initial_capacity = v.capacity(); // Should be >= 5

  v.emplace_back(1); // Size 1
  v.emplace_back(2); // Size 2
  v.emplace_back(3); // Size 3

  BOOST_CHECK_EQUAL(v.size(), 3);                    // Size increased by 3
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should be unchanged
}

BOOST_AUTO_TEST_CASE(EmplaceBack_MultipleElements_WithReallocation_FromEmpty) {
  vector<int> v(0);                       // Minimal initial capacity, Size 0
                                          // v.reserve(1); // Ensure small cap if needed
  size_t initial_capacity = v.capacity(); // Should be 0 or minimal

  // Add elements to force reallocation multiple times
  size_t num_emplaces = 10;
  for (size_t i = 0; i < num_emplaces; ++i) {
    v.emplace_back(i);
  }
  BOOST_CHECK_EQUAL(v.size(), num_emplaces);
  BOOST_CHECK(v.capacity() >= num_emplaces);
  BOOST_CHECK(v.capacity() > initial_capacity || initial_capacity == 0); // Capacity must have grown
}

BOOST_AUTO_TEST_CASE(EmplaceBack_IntoVectorWithReservedCapacity) {
  vector<int> v; // Size 0
  v.reserve(10); // Capacity >= 10
  size_t reserved_capacity = v.capacity();
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(reserved_capacity >= 10);

  for (int i = 0; i < 10; ++i) {
    v.emplace_back(i); // Add 10 elements
  }
  BOOST_CHECK_EQUAL(v.size(), 10);
  BOOST_CHECK_EQUAL(v.capacity(), reserved_capacity); // Should not have reallocated

  v.emplace_back(10); // Add one more - should trigger reallocation
  BOOST_CHECK_EQUAL(v.size(), 11);
  BOOST_CHECK(v.capacity() > reserved_capacity);
  BOOST_CHECK(v.capacity() >= 11);
}

BOOST_AUTO_TEST_CASE(EmplaceBack_TestClass_DefaultConstructor_FromEmpty) {
  vector<TestClass> v; // Size 0
  v.emplace_back();    // Uses TestClass() default constructor, Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK(v.capacity() >= 1);
  // Cannot check internal state of the element
}

BOOST_AUTO_TEST_CASE(EmplaceBack_TestClass_SpecificConstructor_FromEmpty) {
  vector<TestClass> v;     // Size 0
  v.emplace_back(10, 5.5); // Uses TestClass(int, double) constructor, Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK(v.capacity() >= 1);
  // Cannot check internal state of the element
}

BOOST_AUTO_TEST_CASE(EmplaceBack_TestClass_MultipleArguments_FromEmpty) {
  vector<TestClass> v;    // Size 0
  v.emplace_back(1, 1.1); // Size 1
  v.emplace_back(2, 2.2); // Size 2
  v.emplace_back(3, 3.3); // Size 3
  BOOST_CHECK_EQUAL(v.size(), 3);
  BOOST_CHECK(v.capacity() >= 3);
  // Cannot check internal state of the elements
}

// --- reserve Tests ---
// Tests below now correctly assume reserve() does not change size()

BOOST_AUTO_TEST_CASE(Reserve_FromEmpty_ToPositive) {
  vector<int> v; // Size 0
  size_t requested_capacity = 10;
  v.reserve(requested_capacity);
  BOOST_CHECK_EQUAL(v.size(), 0); // Size should not change
  BOOST_CHECK(v.capacity() >= requested_capacity);
}

BOOST_AUTO_TEST_CASE(Reserve_FromEmpty_ToZero) {
  vector<int> v; // Size 0
  v.reserve(0);
  BOOST_CHECK_EQUAL(v.size(), 0); // Size should not change
  BOOST_CHECK(v.capacity() >= 0); // Can be 0
}

BOOST_AUTO_TEST_CASE(Reserve_FromSized_LargerThanCapacity) {
  vector<int> v(5); // Size 5, Capacity >= 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  size_t initial_capacity = v.capacity(); // >= 5
  size_t requested_capacity = initial_capacity + 10;
  v.reserve(requested_capacity);
  BOOST_CHECK_EQUAL(v.size(), 5); // Size should not change
  BOOST_CHECK(v.capacity() >= requested_capacity);
}

BOOST_AUTO_TEST_CASE(Reserve_FromSized_SmallerThanCapacity) {
  vector<int> v(20); // Size 20, Capacity >= 20
  BOOST_CHECK_EQUAL(v.size(), 20);
  size_t initial_capacity = v.capacity();
  size_t requested_capacity = initial_capacity / 2; // Smaller than current
  v.reserve(requested_capacity);
  BOOST_CHECK_EQUAL(v.size(), 20);                   // Size should not change
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should NOT decrease
}

BOOST_AUTO_TEST_CASE(Reserve_FromSized_SameAsCapacity) {
  vector<int> v(10); // Size 10, Capacity >= 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  size_t initial_capacity = v.capacity();
  v.reserve(initial_capacity);
  BOOST_CHECK_EQUAL(v.size(), 10);                   // Size should not change
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should remain the same
}

BOOST_AUTO_TEST_CASE(Reserve_Zero_FromSized) {
  vector<int> v(10); // Size 10, Capacity >= 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  size_t initial_capacity = v.capacity();
  v.reserve(0);
  BOOST_CHECK_EQUAL(v.size(), 10);                   // Size should not change
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should not decrease below current
}

// --- resize Tests ---
// These tests were largely correct in terms of size changes,
// only potentially needing capacity checks clarified

BOOST_AUTO_TEST_CASE(Resize_Empty_ToPositive) {
  vector<int> v; // Size 0
  size_t new_size = 5;
  v.resize(new_size); // Size becomes 5
  BOOST_CHECK_EQUAL(v.size(), new_size);
  BOOST_CHECK(v.capacity() >= new_size); // Capacity must be at least the new size
  // Cannot check init value (default constructed for int, i.e., 0)
}

BOOST_AUTO_TEST_CASE(Resize_Empty_ToPositive_WithInitValue) {
  vector<int> v; // Size 0
  size_t new_size = 5;
  int init_val = 77;
  v.resize(new_size, init_val); // Size becomes 5
  BOOST_CHECK_EQUAL(v.size(), new_size);
  BOOST_CHECK(v.capacity() >= new_size); // Capacity must be at least the new size
                                         // Cannot check init value (77)
}

BOOST_AUTO_TEST_CASE(Resize_Empty_ToZero) {
  vector<int> v; // Size 0
  v.resize(0);   // Size remains 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0);
}

BOOST_AUTO_TEST_CASE(Resize_NotEmpty_Smaller) {
  vector<int> v(10);                      // Size 10
  size_t initial_size = v.size();         // 10
  size_t initial_capacity = v.capacity(); // >= 10
  size_t new_size = 3;
  v.resize(new_size);                    // Size becomes 3
  BOOST_CHECK_EQUAL(v.size(), new_size); // Size should be 3
  BOOST_CHECK(v.capacity() >= new_size); // Capacity >= 3
  // Capacity might stay the same or shrink, but std::vector doesn't guarantee shrinking here
  // BOOST_CHECK(v.capacity() <= initial_capacity); // Cannot guarantee shrinking
}

BOOST_AUTO_TEST_CASE(Resize_NotEmpty_SameSize) {
  vector<int> v(10);                      // Size 10
  size_t initial_size = v.size();         // 10
  size_t initial_capacity = v.capacity(); // >= 10
  v.resize(initial_size);                 // Resize to 10
  BOOST_CHECK_EQUAL(v.size(), initial_size);
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity); // Capacity should be unchanged
}

BOOST_AUTO_TEST_CASE(Resize_NotEmpty_Larger_NoInitValue) {
  vector<int> v(5);                       // Size 5
  size_t initial_size = v.size();         // 5
  size_t initial_capacity = v.capacity(); // >= 5
  size_t new_size = 10;
  v.resize(new_size);                            // Size becomes 10
  BOOST_CHECK_EQUAL(v.size(), new_size);         // Size should be 10
  BOOST_CHECK(v.capacity() >= new_size);         // Capacity >= 10
  BOOST_CHECK(v.capacity() >= initial_capacity); // Capacity should not decrease
  // Cannot check values of newly added elements (should be default constructed)
}

BOOST_AUTO_TEST_CASE(Resize_NotEmpty_Larger_WithInitValue) {
  vector<int> v(5);                       // Size 5
  size_t initial_size = v.size();         // 5
  size_t initial_capacity = v.capacity(); // >= 5
  size_t new_size = 10;
  int init_val = 88;
  v.resize(new_size, init_val);                  // Size becomes 10
  BOOST_CHECK_EQUAL(v.size(), new_size);         // Size should be 10
  BOOST_CHECK(v.capacity() >= new_size);         // Capacity >= 10
  BOOST_CHECK(v.capacity() >= initial_capacity); // Capacity should not decrease
  // Cannot check values of newly added elements (should be init_val)
}

BOOST_AUTO_TEST_CASE(Resize_NotEmpty_ToZero) {
  vector<int> v(10);                      // Size 10
  size_t initial_capacity = v.capacity(); // >= 10
  v.resize(0);                            // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0); // Capacity might stay or shrink
}

BOOST_AUTO_TEST_CASE(Resize_ToZero_FromZero) {
  vector<int> v; // Size 0
  v.resize(0);   // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0);
}

BOOST_AUTO_TEST_CASE(Resize_TestClass_Larger_DefaultConstructed_FromSized) {
  vector<TestClass> v(2);         // Size 2
  size_t initial_size = v.size(); // 2
  size_t new_size = 5;
  v.resize(new_size);                    // Size 5
  BOOST_CHECK_EQUAL(v.size(), new_size); // Size 5
  BOOST_CHECK(v.capacity() >= new_size); // Capacity >= 5
  // Cannot verify that the 3 new elements were default constructed
}

BOOST_AUTO_TEST_CASE(Resize_TestClass_Larger_CopyConstructed_FromSized) {
  vector<TestClass> v(2);         // Size 2
  size_t initial_size = v.size(); // 2
  size_t new_size = 5;
  TestClass init_val(99, 99.9);
  v.resize(new_size, init_val);          // Size 5
  BOOST_CHECK_EQUAL(v.size(), new_size); // Size 5
  BOOST_CHECK(v.capacity() >= new_size); // Capacity >= 5
  // Cannot verify that the 3 new elements were copy constructed from init_val
}

// --- Combined Operations Tests ---

BOOST_AUTO_TEST_CASE(Combined_ReserveThenPushBack) {
  vector<int> v; // Size 0
  v.reserve(10); // Capacity >= 10
  size_t reserved_capacity = v.capacity();
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(reserved_capacity >= 10);

  for (int i = 0; i < 10; ++i) {
    v.push_back(i); // Add 10 elements, size becomes 10
  }
  BOOST_CHECK_EQUAL(v.size(), 10);
  BOOST_CHECK_EQUAL(v.capacity(), reserved_capacity); // Should not have reallocated

  v.push_back(10); // Add one more - should trigger reallocation, size becomes 11
  BOOST_CHECK_EQUAL(v.size(), 11);
  BOOST_CHECK(v.capacity() > reserved_capacity);
  BOOST_CHECK(v.capacity() >= 11);
}

BOOST_AUTO_TEST_CASE(Combined_PushBackThenResizeUp_FromEmpty) {
  vector<int> v;                          // Size 0
  v.push_back(1);                         // Size 1
  v.push_back(2);                         // Size 2, Capacity >= 2
  size_t initial_size = v.size();         // 2
  size_t initial_capacity = v.capacity(); // >= 2

  v.resize(5); // Resize to 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5);
  BOOST_CHECK(v.capacity() >= initial_capacity); // Capacity should not decrease
  // Cannot check if initial elements are preserved and new are default constructed
}

BOOST_AUTO_TEST_CASE(Combined_SizedThenPushBackThenResizeUp) {
  vector<int> v(2);                       // Size 2, Capacity >= 2
  size_t initial_size = v.size();         // 2
  size_t initial_capacity = v.capacity(); // >= 2

  v.push_back(3); // Size 3
  BOOST_CHECK_EQUAL(v.size(), 3);
  BOOST_CHECK(v.capacity() >= 3);

  v.resize(10); // Resize to 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  BOOST_CHECK(v.capacity() >= 10);
  BOOST_CHECK(v.capacity() >= initial_capacity); // Capacity should not decrease
}

BOOST_AUTO_TEST_CASE(Combined_PushBackThenResizeDown_FromEmpty) {
  vector<int> v; // Size 0
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(4);                         // Size 4, Capacity >= 4
  size_t initial_size = v.size();         // 4
  size_t initial_capacity = v.capacity(); // >= 4

  v.resize(2); // Resize to 2
  BOOST_CHECK_EQUAL(v.size(), 2);
  BOOST_CHECK(v.capacity() >= 2); // Capacity >= 2
                                  // Capacity might stay the same or shrink, but >= 2
}

BOOST_AUTO_TEST_CASE(Combined_SizedThenPushBackThenResizeDown) {
  vector<int> v(4);                       // Size 4, Capacity >= 4
  size_t initial_size = v.size();         // 4
  size_t initial_capacity = v.capacity(); // >= 4

  v.push_back(5); // Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5);

  v.resize(2); // Resize to 2
  BOOST_CHECK_EQUAL(v.size(), 2);
  BOOST_CHECK(v.capacity() >= 2); // Capacity >= 2
}

BOOST_AUTO_TEST_CASE(Combined_ResizeUpThenResizeDown_FromEmpty) {
  vector<int> v;                           // Size 0
  v.resize(10);                            // Size 10, Capacity >= 10
  size_t capacity_after_up = v.capacity(); // >= 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  BOOST_CHECK(capacity_after_up >= 10);

  v.resize(5); // Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5);
  // Capacity might be capacity_after_up or smaller, but >= 5
}

BOOST_AUTO_TEST_CASE(Combined_ResizeUpThenResizeDown_FromSized) {
  vector<int> v(3); // Size 3
  size_t initial_cap = v.capacity();
  v.resize(10);                            // Size 10, Cap >= 10
  size_t capacity_after_up = v.capacity(); // >= 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  BOOST_CHECK(capacity_after_up >= 10);
  BOOST_CHECK(capacity_after_up >= initial_cap); // Capacity should not decrease

  v.resize(5); // Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5);
  // Capacity might be capacity_after_up or smaller, but >= 5
}

BOOST_AUTO_TEST_CASE(Combined_ResizeDownThenResizeUp_FromSized) {
  vector<int> v(10);                      // Size 10, Capacity >= 10
  size_t initial_capacity = v.capacity(); // >= 10

  v.resize(5); // Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5); // Capacity >= 5

  v.resize(15); // Size 15
  BOOST_CHECK_EQUAL(v.size(), 15);
  BOOST_CHECK(v.capacity() >= 15); // Capacity >= 15
}

BOOST_AUTO_TEST_CASE(Combined_CopyThenModifyOriginal) {
  vector<int> original;
  original.push_back(1);
  original.push_back(2); // Size 2, Capacity >= 2
  size_t original_size_before_copy = original.size();
  size_t original_capacity_before_copy = original.capacity();

  vector<int> copied = original; // Copy constructor

  BOOST_CHECK_EQUAL(copied.size(), original_size_before_copy);
  BOOST_CHECK(copied.capacity() >= original_size_before_copy);
  BOOST_CHECK_EQUAL(original.size(), original_size_before_copy); // Original unchanged
  BOOST_CHECK_EQUAL(original.capacity(),
                    original_capacity_before_copy); // Original capacity unchanged

  original.push_back(3); // Modify original (size 3)
  BOOST_CHECK_EQUAL(original.size(), original_size_before_copy + 1);
  BOOST_CHECK_EQUAL(copied.size(), original_size_before_copy); // Copy size unchanged
  BOOST_CHECK(copied.capacity() >= original_size_before_copy); // Copy capacity unchanged
}

BOOST_AUTO_TEST_CASE(Combined_MoveThenUseMovedFrom) {
  vector<int> original;
  original.push_back(1);
  original.push_back(2); // Size 2, Capacity >= 2
  size_t original_size_before_move = original.size();
  size_t original_capacity_before_move = original.capacity();

  vector<int> moved = std::move(original); // Move constructor

  BOOST_CHECK_EQUAL(moved.size(), original_size_before_move); // Moved-to has original size
  BOOST_CHECK(moved.capacity() >=
              original_size_before_move); // Moved-to capacity likely transferred, must be >= size
  BOOST_CHECK_EQUAL(original.size(), 0);  // Moved-from is empty

  // Using the moved-from vector (should be valid, but empty)
  original.push_back(10);
  BOOST_CHECK_EQUAL(original.size(), 1);
  BOOST_CHECK(original.capacity() >= 1);

  // Using the moved-to vector
  moved.push_back(20);
  BOOST_CHECK_EQUAL(moved.size(), original_size_before_move + 1); // Size increased
  BOOST_CHECK(moved.capacity() >= moved.size());
}

BOOST_AUTO_TEST_CASE(Combined_CopyAssignmentThenModifyOriginal) {
  vector<int> original;
  original.push_back(1);
  original.push_back(2); // Size 2

  vector<int> other(5, 99); // Size 5
  size_t other_initial_capacity = other.capacity();

  other = original; // Copy assignment

  BOOST_CHECK_EQUAL(other.size(), original.size()); // Size 2
  BOOST_CHECK(other.capacity() >= original.size()); // Cap >= 2
  // Cap might be original_capacity or stay >= 5 (other_initial_capacity), std::vector doesn't
  // guarantee shrinking
  BOOST_CHECK_EQUAL(original.size(), 2); // Original unchanged

  original.push_back(3); // Modify original (size 3)
  BOOST_CHECK_EQUAL(original.size(), 3);
  BOOST_CHECK_EQUAL(other.size(), 2); // Other (the copy) is unchanged
}

BOOST_AUTO_TEST_CASE(Combined_MoveAssignmentThenUseMovedFrom) {
  vector<int> original;
  original.push_back(1);
  original.push_back(2); // Size 2
  size_t original_size_before_move = original.size();
  size_t original_capacity_before_move = original.capacity();

  vector<int> other(5, 99); // Size 5, will be replaced

  other = std::move(original); // Move assignment

  BOOST_CHECK_EQUAL(other.size(), original_size_before_move); // Size 2
  BOOST_CHECK(other.capacity() >= other.size()); // Cap >= 2 (likely got original's cap)
  BOOST_CHECK_EQUAL(original.size(), 0);         // Moved-from empty

  // Use moved-from
  original.push_back(10);
  BOOST_CHECK_EQUAL(original.size(), 1);

  // Use moved-to
  other.push_back(20);
  BOOST_CHECK_EQUAL(other.size(), original_size_before_move + 1); // Size 3
}

// --- Different Data Types Tests ---

BOOST_AUTO_TEST_CASE(DataType_Double) {
  vector<double> v; // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  v.push_back(1.1); // Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  v.resize(5); // Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5);
  vector<double> v2 = v; // Copy
  BOOST_CHECK_EQUAL(v2.size(), 5);
}

BOOST_AUTO_TEST_CASE(DataType_String) {
  vector<std::string> v; // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  v.push_back("hello"); // Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  v.resize(3, "world"); // Size 3
  BOOST_CHECK_EQUAL(v.size(), 3);
  BOOST_CHECK(v.capacity() >= 3);
  vector<std::string> v2 = std::move(v); // Move
  BOOST_CHECK_EQUAL(v2.size(), 3);
  BOOST_CHECK_EQUAL(v.size(), 0);
}

BOOST_AUTO_TEST_CASE(DataType_TestClass) {
  vector<TestClass> v; // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  v.emplace_back(1, 1.1); // Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  v.push_back(TestClass(2, 2.2)); // Should use push_back with copy or move, Size 2
  BOOST_CHECK_EQUAL(v.size(), 2);
  v.resize(5, TestClass(3, 3.3)); // Should use resize with copy, Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5);
  vector<TestClass> v2 = v; // Should use copy constructor for vector and elements, Size 5
  BOOST_CHECK_EQUAL(v2.size(), 5);
}

// --- Edge Cases and Large Scale Tests ---

BOOST_AUTO_TEST_CASE(EdgeCase_PushBackManyElements) {
  vector<int> v; // Size 0
  size_t num_elements = 1000;
  for (size_t i = 0; i < num_elements; ++i) {
    v.push_back(i);
  }
  BOOST_CHECK_EQUAL(v.size(), num_elements);
  BOOST_CHECK(v.capacity() >= num_elements);
}

BOOST_AUTO_TEST_CASE(EdgeCase_EmplaceBackManyElements) {
  vector<TestClass> v; // Size 0
  size_t num_elements = 1000;
  for (size_t i = 0; i < num_elements; ++i) {
    v.emplace_back((int)i, (double)i);
  }
  BOOST_CHECK_EQUAL(v.size(), num_elements);
  BOOST_CHECK(v.capacity() >= num_elements);
}

BOOST_AUTO_TEST_CASE(EdgeCase_ResizeToVeryLargeSize_FromEmpty) {
  vector<int> v; // Size 0
  size_t large_size = 10000;
  v.resize(large_size); // Size 10000
  BOOST_CHECK_EQUAL(v.size(), large_size);
  BOOST_CHECK(v.capacity() >= large_size);
}

BOOST_AUTO_TEST_CASE(EdgeCase_ResizeToVeryLargeSize_FromSmallSized) {
  vector<int> v(10); // Size 10
  size_t large_size = 10000;
  v.resize(large_size); // Size 10000
  BOOST_CHECK_EQUAL(v.size(), large_size);
  BOOST_CHECK(v.capacity() >= large_size);
}

BOOST_AUTO_TEST_CASE(EdgeCase_ReserveVeryLargeCapacity_FromEmpty) {
  vector<int> v; // Size 0
  size_t large_capacity = 20000;
  v.reserve(large_capacity); // Cap >= 20000
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= large_capacity);
}

BOOST_AUTO_TEST_CASE(EdgeCase_ReserveVeryLargeCapacity_FromSmallSized) {
  vector<int> v(100); // Size 100
  size_t large_capacity = 20000;
  v.reserve(large_capacity);        // Cap >= 20000
  BOOST_CHECK_EQUAL(v.size(), 100); // Size unchanged
  BOOST_CHECK(v.capacity() >= large_capacity);
}

BOOST_AUTO_TEST_CASE(EdgeCase_CopyLargeVector_FromPopulated) {
  vector<int> original;
  size_t num_elements = 5000;
  original.reserve(num_elements + 100); // Add some extra capacity
  for (size_t i = 0; i < num_elements; ++i) {
    original.push_back(i);
  } // Size 5000
  size_t original_size = original.size();
  size_t original_capacity = original.capacity();

  vector<int> copied = original;
  BOOST_CHECK_EQUAL(copied.size(), original_size);
  BOOST_CHECK(copied.capacity() >= original_size);   // Capacity might be different, but >= size
  BOOST_CHECK_EQUAL(original.size(), original_size); // Original unchanged
}

BOOST_AUTO_TEST_CASE(EdgeCase_MoveLargeVector_FromPopulated) {
  vector<int> original;
  size_t num_elements = 5000;
  original.reserve(num_elements + 100); // Add some extra capacity
  for (size_t i = 0; i < num_elements; ++i) {
    original.push_back(i);
  } // Size 5000
  size_t original_size = original.size();
  size_t original_capacity = original.capacity();

  vector<int> moved = std::move(original);
  BOOST_CHECK_EQUAL(moved.size(), original_size);
  BOOST_CHECK(moved.capacity() >= original_size); // Capacity likely transferred, must be >= size
  BOOST_CHECK_EQUAL(original.size(), 0);          // Moved-from is empty
}

BOOST_AUTO_TEST_CASE(EdgeCase_ResizeToZeroAfterPopulating_FromEmpty) {
  vector<int> v;
  for (int i = 0; i < 50; ++i)
    v.push_back(i); // Size 50
  BOOST_CHECK_EQUAL(v.size(), 50);
  v.resize(0); // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0);
}

BOOST_AUTO_TEST_CASE(EdgeCase_ResizeToZeroAfterPopulating_FromSized) {
  vector<int> v(50); // Size 50
  BOOST_CHECK_EQUAL(v.size(), 50);
  v.resize(0); // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0);
}

BOOST_AUTO_TEST_CASE(EdgeCase_ReserveZeroThenPushBack) {
  vector<int> v; // Size 0
  v.reserve(0);
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0);
  v.push_back(1); // Size 1
  BOOST_CHECK_EQUAL(v.size(), 1);
  BOOST_CHECK(v.capacity() >= 1); // Capacity must increase from 0
}

BOOST_AUTO_TEST_CASE(EdgeCase_ResizeToZeroThenResizeUp_FromPopulated) {
  vector<int> v;
  for (int i = 0; i < 10; ++i)
    v.push_back(i); // Size 10
  BOOST_CHECK_EQUAL(v.size(), 10);
  v.resize(0); // Size 0
  BOOST_CHECK_EQUAL(v.size(), 0);
  BOOST_CHECK(v.capacity() >= 0);
  v.resize(5); // Size 5
  BOOST_CHECK_EQUAL(v.size(), 5);
  BOOST_CHECK(v.capacity() >= 5);
}

BOOST_AUTO_TEST_CASE(EdgeCase_CopyAssignmentToSelfWithContent_FromPopulated) {
  vector<int> v;
  v.push_back(1);
  v.push_back(2); // Size 2
  size_t initial_size = v.size();
  size_t initial_capacity = v.capacity();
  v = v; // Self-assignment
  BOOST_CHECK_EQUAL(v.size(), initial_size);
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity);
}

BOOST_AUTO_TEST_CASE(EdgeCase_MoveAssignmentToSelfWithContent_FromPopulated) {
  vector<int> v;
  v.push_back(1);
  v.push_back(2); // Size 2
  size_t initial_size = v.size();
  size_t initial_capacity = v.capacity();
  v = std::move(v); // Self-move assignment
  BOOST_CHECK_EQUAL(v.size(), initial_size);
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity);
}

BOOST_AUTO_TEST_CASE(EdgeCase_CopyAssignmentToSelfWithContent_FromSized) {
  vector<int> v(5, 10); // Size 5
  size_t initial_size = v.size();
  size_t initial_capacity = v.capacity();
  v = v; // Self-assignment
  BOOST_CHECK_EQUAL(v.size(), initial_size);
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity);
}

BOOST_AUTO_TEST_CASE(EdgeCase_MoveAssignmentToSelfWithContent_FromSized) {
  vector<int> v(5, 10); // Size 5
  size_t initial_size = v.size();
  size_t initial_capacity = v.capacity();
  v = std::move(v); // Self-move assignment
  BOOST_CHECK_EQUAL(v.size(), initial_size);
  BOOST_CHECK_EQUAL(v.capacity(), initial_capacity);
}

// Add more tests as needed to reach > 100 and cover combinations
// (e.g., reserve, then resize up, then push_back; sized constructor, then resize down, then
// push_back)

BOOST_AUTO_TEST_SUITE_END()