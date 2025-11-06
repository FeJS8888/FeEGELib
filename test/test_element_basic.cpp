/**
 * @file test_element_basic.cpp
 * @brief Basic test cases for Element module (without graphics initialization)
 */

#include "../include/Element.h"
#include "../include/Base.h"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace FeEGE;

// Test helper function to compare doubles
bool doubleEquals(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

void test_sgn_function() {
    std::cout << "Testing sgn function..." << std::endl;
    
    assert(sgn(5.0) == 1);
    assert(sgn(-5.0) == -1);
    assert(sgn(0.0) == 0);
    assert(sgn(0.1) == 1);
    assert(sgn(-0.1) == -1);
    
    std::cout << "  ✓ sgn function tests passed" << std::endl;
}

void test_position_global() {
    std::cout << "Testing global position functions..." << std::endl;
    
    // Test getLastPixel and getMousePos initialization
    Position& lastPixel = getLastPixel();
    Position& mousePos = getMousePos();
    
    // Just verify they exist and can be accessed
    lastPixel.x = 10.0;
    lastPixel.y = 20.0;
    
    assert(doubleEquals(lastPixel.x, 10.0));
    assert(doubleEquals(lastPixel.y, 20.0));
    
    std::cout << "  ✓ Global position function tests passed" << std::endl;
}

void test_animate_constructor() {
    std::cout << "Testing Animate constructor..." << std::endl;
    
    // Create a simple animation function
    auto animFunc = [](double t) -> Position {
        return Position(t * 10, t * 20);
    };
    
    Animate anim(animFunc, 1.0, 100.0);
    
    assert(doubleEquals(anim.speed, 1.0));
    assert(doubleEquals(anim.end, 100.0));
    
    // Test the function
    Position result = anim.function(5.0);
    assert(doubleEquals(result.x, 50.0));
    assert(doubleEquals(result.y, 100.0));
    
    std::cout << "  ✓ Animate constructor tests passed" << std::endl;
}

void test_ease_animations_exist() {
    std::cout << "Testing Ease animations existence..." << std::endl;
    
    // Just verify that the Ease animations are defined and accessible
    // We can't test them fully without graphics initialization
    
    // Test that they have valid properties
    assert(Ease::easeInOutCubicX.speed > 0);
    assert(Ease::easeInOutCubicY.speed > 0);
    assert(Ease::easeOutQuadX.speed > 0);
    assert(Ease::easeOutQuadY.speed > 0);
    
    std::cout << "  ✓ Ease animations existence tests passed" << std::endl;
}

void test_path_functions() {
    std::cout << "Testing path utility functions..." << std::endl;
    
    // Test resolvePath with simple paths
    std::string path1 = resolvePath("test.txt");
    assert(!path1.empty());
    
    std::string path2 = resolvePath("./test.txt");
    assert(!path2.empty());
    
    // Test getCurrentPath
    std::string currentPath = getCurrentPath();
    assert(!currentPath.empty());
    
    std::cout << "  ✓ Path utility function tests passed" << std::endl;
}

void test_utf8_detection() {
    std::cout << "Testing UTF-8 detection..." << std::endl;
    
    // Test with ASCII (valid UTF-8)
    std::string ascii = "Hello World";
    assert(isValidUTF8(ascii) == true);
    
    // Test with empty string
    std::string empty = "";
    assert(isValidUTF8(empty) == true);
    
    std::cout << "  ✓ UTF-8 detection tests passed" << std::endl;
}

void test_encoding_detection() {
    std::cout << "Testing encoding detection..." << std::endl;
    
    // Test with ASCII
    std::string ascii = "Hello World";
    std::string encoding = detectEncoding(ascii);
    assert(!encoding.empty());
    
    std::cout << "  ✓ Encoding detection tests passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running Element Basic Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_sgn_function();
        test_position_global();
        test_animate_constructor();
        test_ease_animations_exist();
        test_path_functions();
        test_utf8_detection();
        test_encoding_detection();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "All Element Basic Tests Passed! ✓" << std::endl;
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
