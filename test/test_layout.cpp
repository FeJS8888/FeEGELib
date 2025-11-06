/**
 * @file test_layout.cpp
 * @brief Test cases for Layout module (FlexLayout)
 */

#include "../include/Layout.h"
#include "../include/Base.h"
#include <cassert>
#include <iostream>
#include <memory>

void test_flex_layout_builder() {
    std::cout << "Testing FlexLayoutBuilder..." << std::endl;
    
    // Create a FlexLayout using builder pattern
    auto layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Row)
        .setAlign(LayoutAlign::Center)
        .setSpacing(10.0)
        .setPadding(5.0)
        .build();
    
    assert(layout != nullptr);
    assert(layout->getDirection() == LayoutDirection::Row);
    assert(layout->getAlign() == LayoutAlign::Center);
    assert(layout->getSpacing() == 10.0);
    assert(layout->getPadding() == 5.0);
    
    std::cout << "  ✓ FlexLayoutBuilder tests passed" << std::endl;
}

void test_flex_layout_chaining() {
    std::cout << "Testing FlexLayout chaining..." << std::endl;
    
    // Create a FlexLayout using chaining
    FlexLayout layout;
    layout.direction(LayoutDirection::Column)
          .spacing(20.0)
          .padding(10.0)
          .align(LayoutAlign::Start);
    
    assert(layout.getDirection() == LayoutDirection::Column);
    assert(layout.getAlign() == LayoutAlign::Start);
    assert(layout.getSpacing() == 20.0);
    assert(layout.getPadding() == 10.0);
    
    std::cout << "  ✓ FlexLayout chaining tests passed" << std::endl;
}

void test_flex_layout_defaults() {
    std::cout << "Testing FlexLayout defaults..." << std::endl;
    
    FlexLayout layout;
    
    // Test default values
    assert(layout.getDirection() == LayoutDirection::Row);
    assert(layout.getAlign() == LayoutAlign::Start);
    assert(layout.getSpacing() == 0.0);
    assert(layout.getPadding() == 0.0);
    
    std::cout << "  ✓ FlexLayout defaults tests passed" << std::endl;
}

void test_flex_layout_setters() {
    std::cout << "Testing FlexLayout setters..." << std::endl;
    
    FlexLayout layout;
    
    layout.setDirection(LayoutDirection::Column);
    layout.setAlign(LayoutAlign::End);
    layout.setSpacing(15.0);
    layout.setPadding(7.5);
    
    assert(layout.getDirection() == LayoutDirection::Column);
    assert(layout.getAlign() == LayoutAlign::End);
    assert(layout.getSpacing() == 15.0);
    assert(layout.getPadding() == 7.5);
    
    std::cout << "  ✓ FlexLayout setters tests passed" << std::endl;
}

void test_layout_direction_enum() {
    std::cout << "Testing LayoutDirection enum..." << std::endl;
    
    // Test that we can use the enum values
    LayoutDirection row = LayoutDirection::Row;
    LayoutDirection col = LayoutDirection::Column;
    
    assert(row != col);
    
    std::cout << "  ✓ LayoutDirection enum tests passed" << std::endl;
}

void test_layout_align_enum() {
    std::cout << "Testing LayoutAlign enum..." << std::endl;
    
    // Test that we can use the enum values
    LayoutAlign start = LayoutAlign::Start;
    LayoutAlign center = LayoutAlign::Center;
    LayoutAlign end = LayoutAlign::End;
    LayoutAlign spaceBetween = LayoutAlign::SpaceBetween;
    
    assert(start != center);
    assert(center != end);
    assert(end != spaceBetween);
    
    std::cout << "  ✓ LayoutAlign enum tests passed" << std::endl;
}

void test_flex_layout_builder_all_aligns() {
    std::cout << "Testing FlexLayoutBuilder with all align types..." << std::endl;
    
    auto layout1 = FlexLayoutBuilder().setAlign(LayoutAlign::Start).build();
    assert(layout1->getAlign() == LayoutAlign::Start);
    
    auto layout2 = FlexLayoutBuilder().setAlign(LayoutAlign::Center).build();
    assert(layout2->getAlign() == LayoutAlign::Center);
    
    auto layout3 = FlexLayoutBuilder().setAlign(LayoutAlign::End).build();
    assert(layout3->getAlign() == LayoutAlign::End);
    
    auto layout4 = FlexLayoutBuilder().setAlign(LayoutAlign::SpaceBetween).build();
    assert(layout4->getAlign() == LayoutAlign::SpaceBetween);
    
    std::cout << "  ✓ FlexLayoutBuilder all align types tests passed" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Running Layout Module Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        test_flex_layout_builder();
        test_flex_layout_chaining();
        test_flex_layout_defaults();
        test_flex_layout_setters();
        test_layout_direction_enum();
        test_layout_align_enum();
        test_flex_layout_builder_all_aligns();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "All Layout Module Tests Passed! ✓" << std::endl;
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
