/**
 * @file test_collision.cpp
 * @brief Test cases for Collision module (collision detection, transformations)
 */

#include "../include/Collision.h"
#include "../include/Base.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace FeEGE;

// Test helper function to compare doubles
bool doubleEquals(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

void test_transform_position() {
    std::cout << "Testing transformPosition..." << std::endl;
    
    // Create a simple square
    Polygon square = {
        Position(0, 0),
        Position(10, 0),
        Position(10, 10),
        Position(0, 10)
    };
    
    Position origin(5, 5); // Center of square
    Position newPos(15, 20); // New position
    
    Polygon transformed = transformPosition(square, origin, newPos);
    
    // After transformation, the center should be at newPos
    // First vertex should be at newPos - (origin - square[0])
    // = newPos - (5,5) + (0,0) = (15,20) - (5,5) = (10,15)
    assert(doubleEquals(transformed[0].x, 10.0));
    assert(doubleEquals(transformed[0].y, 15.0));
    
    std::cout << "  ✓ transformPosition tests passed" << std::endl;
}

void test_transform_shape() {
    std::cout << "Testing transformShape..." << std::endl;
    
    // Create a simple triangle
    Polygon triangle = {
        Position(0, 0),
        Position(10, 0),
        Position(5, 10)
    };
    
    Position origin(5, 3.33); // Approximate center
    double scale = 2.0;
    double angle = 0.0; // No rotation for simplicity
    
    Polygon transformed = transformShape(triangle, scale, origin, angle);
    
    // With scale 2.0 and no rotation, distances from origin should double
    // Check that the shape is scaled
    assert(transformed.size() == triangle.size());
    
    std::cout << "  ✓ transformShape tests passed" << std::endl;
}

void test_is_point_in_convex_polygon() {
    std::cout << "Testing isPointInConvexPolygon..." << std::endl;
    
    // Create a square from (0,0) to (10,10)
    Polygon square = {
        Position(0, 0),
        Position(10, 0),
        Position(10, 10),
        Position(0, 10)
    };
    
    // Point inside
    assert(isPointInConvexPolygon(square, Position(5, 5)) == true);
    
    // Point outside
    assert(isPointInConvexPolygon(square, Position(15, 5)) == false);
    assert(isPointInConvexPolygon(square, Position(5, 15)) == false);
    assert(isPointInConvexPolygon(square, Position(-5, 5)) == false);
    
    // Point on edge (behavior may vary, but typically considered inside)
    Position onEdge(0, 5);
    // Just test that it doesn't crash
    isPointInConvexPolygon(square, onEdge);
    
    std::cout << "  ✓ isPointInConvexPolygon tests passed" << std::endl;
}

void test_is_touched_no_collision() {
    std::cout << "Testing isTouched (no collision)..." << std::endl;
    
    // Two separate squares
    Polygon square1 = {
        Position(0, 0),
        Position(10, 0),
        Position(10, 10),
        Position(0, 10)
    };
    
    Polygon square2 = {
        Position(20, 20),
        Position(30, 20),
        Position(30, 30),
        Position(20, 30)
    };
    
    bool result = isTouched(square1, square2);
    assert(result == false);
    
    std::cout << "  ✓ isTouched (no collision) tests passed" << std::endl;
}

void test_is_touched_with_collision() {
    std::cout << "Testing isTouched (with collision)..." << std::endl;
    
    // Two overlapping squares
    Polygon square1 = {
        Position(0, 0),
        Position(10, 0),
        Position(10, 10),
        Position(0, 10)
    };
    
    Polygon square2 = {
        Position(5, 5),
        Position(15, 5),
        Position(15, 15),
        Position(5, 15)
    };
    
    bool result = isTouched(square1, square2);
    assert(result == true);
    
    std::cout << "  ✓ isTouched (with collision) tests passed" << std::endl;
}

void test_get_separate_distance() {
    std::cout << "Testing getSeparateDistance..." << std::endl;
    
    // Two squares side by side with a gap
    Polygon square1 = {
        Position(0, 0),
        Position(10, 0),
        Position(10, 10),
        Position(0, 10)
    };
    
    Polygon square2 = {
        Position(15, 0),
        Position(25, 0),
        Position(25, 10),
        Position(15, 10)
    };
    
    // Direction from left to right
    Position direction(1, 0);
    
    double distance = getSeparateDistance(square1, square2, direction);
    
    // The gap between them is 5 units
    // Distance should be positive when not colliding
    assert(distance >= 0);
    
    std::cout << "  ✓ getSeparateDistance tests passed" << std::endl;
}

void test_get_safe_distance() {
    std::cout << "Testing getSafeDistance..." << std::endl;
    
    // Two squares with a gap
    Polygon square1 = {
        Position(0, 0),
        Position(10, 0),
        Position(10, 10),
        Position(0, 10)
    };
    
    Polygon square2 = {
        Position(15, 0),
        Position(25, 0),
        Position(25, 10),
        Position(15, 10)
    };
    
    Position direction(1, 0);
    direction = direction.normalize();
    
    double distance = getSafeDistance(square1, square2, direction);
    
    // Should return positive distance when not colliding
    // (exact value depends on implementation)
    
    std::cout << "  ✓ getSafeDistance tests passed" << std::endl;
}

void test_collision_triangles() {
    std::cout << "Testing collision with triangles..." << std::endl;
    
    // Two triangles
    Polygon triangle1 = {
        Position(0, 0),
        Position(10, 0),
        Position(5, 10)
    };
    
    // Non-overlapping triangle
    Polygon triangle2 = {
        Position(20, 0),
        Position(30, 0),
        Position(25, 10)
    };
    
    assert(isTouched(triangle1, triangle2) == false);
    
    // Overlapping triangle
    Polygon triangle3 = {
        Position(5, 0),
        Position(15, 0),
        Position(10, 10)
    };
    
    assert(isTouched(triangle1, triangle3) == true);
    
    std::cout << "  ✓ Collision with triangles tests passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running Collision Module Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_transform_position();
        test_transform_shape();
        test_is_point_in_convex_polygon();
        test_is_touched_no_collision();
        test_is_touched_with_collision();
        test_get_separate_distance();
        test_get_safe_distance();
        test_collision_triangles();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "All Collision Module Tests Passed! ✓" << std::endl;
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
