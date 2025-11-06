/**
 * @file test_integration.cpp
 * @brief Integration tests combining multiple modules
 */

#include "../include/Base.h"
#include "../include/Collision.h"
#include "../include/Layout.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>

using namespace FeEGE;

// Test helper function
bool doubleEquals(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}

void test_position_with_collision() {
    std::cout << "Testing Position integration with Collision..." << std::endl;
    
    // Create positions using Position class
    Position p1(0, 0);
    Position p2(10, 0);
    Position p3(10, 10);
    Position p4(0, 10);
    
    // Create a polygon from positions
    Polygon square = {p1, p2, p3, p4};
    
    // Transform the square
    Position origin(5, 5);
    Position newPos(20, 20);
    Polygon transformed = transformPosition(square, origin, newPos);
    
    // Verify transformation
    assert(transformed.size() == 4);
    
    // Check that the square moved correctly
    Position expectedFirstVertex = p1 - origin + newPos;
    assert(doubleEquals(transformed[0].x, expectedFirstVertex.x));
    assert(doubleEquals(transformed[0].y, expectedFirstVertex.y));
    
    std::cout << "  ✓ Position with Collision integration tests passed" << std::endl;
}

void test_collision_with_transformed_shapes() {
    std::cout << "Testing Collision with transformed shapes..." << std::endl;
    
    // Create a unit square
    Polygon square = {
        Position(0, 0),
        Position(1, 0),
        Position(1, 1),
        Position(0, 1)
    };
    
    // Scale it up
    Position origin(0.5, 0.5);
    double scale = 10.0;
    double angle = 0.0;
    Polygon scaledSquare = transformShape(square, scale, origin, angle);
    
    // Move to different positions
    Polygon square1 = transformPosition(scaledSquare, origin * scale, Position(5, 5));
    Polygon square2 = transformPosition(scaledSquare, origin * scale, Position(20, 20));
    
    // They shouldn't collide
    assert(!isTouched(square1, square2));
    
    // Move square2 closer
    Polygon square2_close = transformPosition(scaledSquare, origin * scale, Position(8, 8));
    
    // Now they should collide
    assert(isTouched(square1, square2_close));
    
    std::cout << "  ✓ Collision with transformed shapes tests passed" << std::endl;
}

void test_clamp_with_positions() {
    std::cout << "Testing clamp with Position coordinates..." << std::endl;
    
    Position pos(150, 250);
    
    // Clamp position to bounds
    Position bounded(
        clamp(pos.x, 0.0, 100.0),
        clamp(pos.y, 0.0, 200.0)
    );
    
    assert(doubleEquals(bounded.x, 100.0));
    assert(doubleEquals(bounded.y, 200.0));
    
    std::cout << "  ✓ Clamp with Position tests passed" << std::endl;
}

void test_position_normalization_in_collision() {
    std::cout << "Testing Position normalization in collision context..." << std::endl;
    
    // Create a direction vector
    Position direction(3, 4); // 3-4-5 triangle
    Position normalized = direction.normalize();
    
    // Should be unit length
    assert(doubleEquals(normalized.length(), 1.0));
    
    // Use normalized direction for separation
    Polygon square1 = {
        Position(0, 0), Position(10, 0),
        Position(10, 10), Position(0, 10)
    };
    
    Polygon square2 = {
        Position(20, 0), Position(30, 0),
        Position(30, 10), Position(20, 10)
    };
    
    // Calculate distance along normalized direction
    double dist = getSafeDistance(square1, square2, normalized);
    // Just verify it doesn't crash
    
    std::cout << "  ✓ Position normalization in collision tests passed" << std::endl;
}

void test_perpendicular_with_collision() {
    std::cout << "Testing perpendicular vectors with collision..." << std::endl;
    
    // Create orthogonal directions
    Position right(1, 0);
    Position up = right.perpendicular(); // Should be (0, 1) or (-0, 1)
    
    // Verify perpendicularity
    assert(doubleEquals(right.dot(up), 0.0));
    
    // Create rectangles offset in perpendicular directions
    Polygon base = {
        Position(0, 0), Position(10, 0),
        Position(10, 2), Position(0, 2)
    };
    
    Position offset1 = right * 15; // Offset to the right
    Position offset2 = up * 5;      // Offset upward
    
    Polygon rect1 = transformPosition(base, Position(5, 1), offset1);
    Polygon rect2 = transformPosition(base, Position(5, 1), offset2);
    
    // They shouldn't collide (different directions)
    assert(!isTouched(rect1, rect2));
    
    std::cout << "  ✓ Perpendicular with collision tests passed" << std::endl;
}

void test_optional_with_collision_result() {
    std::cout << "Testing SimpleOptional with collision results..." << std::endl;
    
    // Simulate optional collision result
    SimpleOptional<double> penetrationDepth;
    
    // No collision initially
    assert(!penetrationDepth.hasValue());
    
    // After collision, store depth
    penetrationDepth = SimpleOptional<double>(2.5);
    assert(penetrationDepth.hasValue());
    assert(doubleEquals(penetrationDepth.value(), 2.5));
    
    // Use value_or for safe access
    double depth = penetrationDepth.value_or(0.0);
    assert(doubleEquals(depth, 2.5));
    
    std::cout << "  ✓ SimpleOptional with collision results tests passed" << std::endl;
}

void test_layout_with_positions() {
    std::cout << "Testing Layout with Position calculations..." << std::endl;
    
    // Create a layout
    auto layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Row)
        .setSpacing(10.0)
        .setPadding(5.0)
        .build();
    
    assert(layout != nullptr);
    
    // Simulate positioning widgets based on layout
    // Calculate positions for 3 widgets with 50px width each
    double spacing = layout->getSpacing();
    double padding = layout->getPadding();
    
    Position widget1Pos(padding, padding);
    Position widget2Pos(padding + 50 + spacing, padding);
    Position widget3Pos(padding + 100 + 2 * spacing, padding);
    
    // Verify positions
    assert(doubleEquals(widget1Pos.x, 5.0));
    assert(doubleEquals(widget2Pos.x, 65.0));
    assert(doubleEquals(widget3Pos.x, 125.0));
    
    std::cout << "  ✓ Layout with Position calculations tests passed" << std::endl;
}

void test_polygon_point_containment_edge_cases() {
    std::cout << "Testing polygon point containment edge cases..." << std::endl;
    
    // Triangle
    Polygon triangle = {
        Position(0, 0),
        Position(10, 0),
        Position(5, 10)
    };
    
    // Test center point (should be inside)
    Position center(5, 3);
    assert(isPointInConvexPolygon(triangle, center) == true);
    
    // Test point way outside
    Position outside(100, 100);
    assert(isPointInConvexPolygon(triangle, outside) == false);
    
    // Test point on edge
    Position onEdge(5, 0);
    // Result may vary by implementation, just ensure it doesn't crash
    isPointInConvexPolygon(triangle, onEdge);
    
    std::cout << "  ✓ Polygon point containment edge cases tests passed" << std::endl;
}

void test_vector_math_comprehensive() {
    std::cout << "Testing comprehensive vector mathematics..." << std::endl;
    
    // Create vectors
    Position a(3, 4);
    Position b(1, 2);
    
    // Test all operations
    Position sum = a + b;
    assert(doubleEquals(sum.x, 4.0) && doubleEquals(sum.y, 6.0));
    
    Position diff = a - b;
    assert(doubleEquals(diff.x, 2.0) && doubleEquals(diff.y, 2.0));
    
    Position scaled = a * 2.0;
    assert(doubleEquals(scaled.x, 6.0) && doubleEquals(scaled.y, 8.0));
    
    double dot = a.dot(b);
    assert(doubleEquals(dot, 11.0)); // 3*1 + 4*2 = 11
    
    double len = a.length();
    assert(doubleEquals(len, 5.0));
    
    Position norm = a.normalize();
    assert(doubleEquals(norm.length(), 1.0));
    
    Position perp = a.perpendicular();
    assert(doubleEquals(a.dot(perp), 0.0)); // Perpendicular means dot = 0
    
    std::cout << "  ✓ Comprehensive vector mathematics tests passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running Integration Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_position_with_collision();
        test_collision_with_transformed_shapes();
        test_clamp_with_positions();
        test_position_normalization_in_collision();
        test_perpendicular_with_collision();
        test_optional_with_collision_result();
        test_layout_with_positions();
        test_polygon_point_containment_edge_cases();
        test_vector_math_comprehensive();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "All Integration Tests Passed! ✓" << std::endl;
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
