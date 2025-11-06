/**
 * @file test_base.cpp
 * @brief Test cases for Base module (Position, SimpleOptional, utility functions)
 */

#include "../include/Base.h"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace FeEGE;

// Test helper function to compare doubles
bool doubleEquals(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
}

void test_position_constructor() {
    std::cout << "Testing Position constructor..." << std::endl;
    
    // Default constructor
    Position p1;
    assert(doubleEquals(p1.x, 0.0));
    assert(doubleEquals(p1.y, 0.0));
    
    // Parameterized constructor
    Position p2(3.5, 4.5);
    assert(doubleEquals(p2.x, 3.5));
    assert(doubleEquals(p2.y, 4.5));
    
    std::cout << "  ✓ Position constructor tests passed" << std::endl;
}

void test_position_operators() {
    std::cout << "Testing Position operators..." << std::endl;
    
    Position p1(3.0, 4.0);
    Position p2(1.0, 2.0);
    
    // Addition
    Position sum = p1 + p2;
    assert(doubleEquals(sum.x, 4.0));
    assert(doubleEquals(sum.y, 6.0));
    
    // Subtraction
    Position diff = p1 - p2;
    assert(doubleEquals(diff.x, 2.0));
    assert(doubleEquals(diff.y, 2.0));
    
    // Scalar multiplication
    Position scaled = p1 * 2.0;
    assert(doubleEquals(scaled.x, 6.0));
    assert(doubleEquals(scaled.y, 8.0));
    
    std::cout << "  ✓ Position operator tests passed" << std::endl;
}

void test_position_methods() {
    std::cout << "Testing Position methods..." << std::endl;
    
    Position p1(3.0, 4.0);
    
    // Length test (3-4-5 triangle)
    double len = p1.length();
    assert(doubleEquals(len, 5.0));
    
    // Dot product
    Position p2(2.0, 1.0);
    double dot = p1.dot(p2);
    assert(doubleEquals(dot, 10.0)); // 3*2 + 4*1 = 10
    
    // Normalize
    Position normalized = p1.normalize();
    assert(doubleEquals(normalized.x, 0.6)); // 3/5
    assert(doubleEquals(normalized.y, 0.8)); // 4/5
    assert(doubleEquals(normalized.length(), 1.0));
    
    // Perpendicular
    Position perp = p1.perpendicular();
    assert(doubleEquals(perp.x, -4.0));
    assert(doubleEquals(perp.y, 3.0));
    // Perpendicular should be orthogonal (dot product = 0)
    assert(doubleEquals(p1.dot(perp), 0.0));
    
    std::cout << "  ✓ Position method tests passed" << std::endl;
}

void test_simple_optional() {
    std::cout << "Testing SimpleOptional..." << std::endl;
    
    // Empty optional
    SimpleOptional<int> opt1;
    assert(!opt1.hasValue());
    assert(!opt1);
    
    // Optional with value
    SimpleOptional<int> opt2(42);
    assert(opt2.hasValue());
    assert(opt2);
    assert(opt2.value() == 42);
    
    // value_or test
    assert(opt1.value_or(100) == 100);
    assert(opt2.value_or(100) == 42);
    
    // Test with string
    SimpleOptional<std::string> opt3("hello");
    assert(opt3.hasValue());
    assert(opt3.value() == "hello");
    
    std::cout << "  ✓ SimpleOptional tests passed" << std::endl;
}

void test_clamp() {
    std::cout << "Testing clamp function..." << std::endl;
    
    // Normal clamping
    assert(clamp(5, 0, 10) == 5);
    assert(clamp(-5, 0, 10) == 0);
    assert(clamp(15, 0, 10) == 10);
    
    // Edge cases
    assert(clamp(0, 0, 10) == 0);
    assert(clamp(10, 0, 10) == 10);
    
    // Double values
    assert(doubleEquals(clamp(5.5, 0.0, 10.0), 5.5));
    assert(doubleEquals(clamp(-1.5, 0.0, 10.0), 0.0));
    assert(doubleEquals(clamp(15.5, 0.0, 10.0), 10.0));
    
    std::cout << "  ✓ clamp function tests passed" << std::endl;
}

void test_fixed() {
    std::cout << "Testing fixed function..." << std::endl;
    
    assert(fixed(3.2) == 3);
    assert(fixed(3.7) == 3);
    assert(fixed(-3.2) == -3);
    assert(fixed(-3.7) == -3);
    assert(fixed(0.0) == 0);
    
    std::cout << "  ✓ fixed function tests passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running Base Module Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_position_constructor();
        test_position_operators();
        test_position_methods();
        test_simple_optional();
        test_clamp();
        test_fixed();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "All Base Module Tests Passed! ✓" << std::endl;
        std::cout << "========================================" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
