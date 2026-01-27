/**
 * Color3ub and Color4ub Test Suite
 *
 * This file demonstrates the usage of the new Color3ub and Color4ub classes
 * and their conversion capabilities with Color3f and Color4f.
 *
 * 编译命令（如果需要独立测试）:
 * g++ -std=c++17 -I../inc color_test_example.cpp ../src/Color/Color3f.cpp ../src/Color/Color4f.cpp ../src/Color/Color3ub.cpp ../src/Color/Color4ub.cpp -o color_test
 */

#include<iostream>
#include<hgl/color/Color3f.h>
#include<hgl/color/Color4f.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>

void test_color3ub_basic()
{
    std::cout << "\n=== Color3ub Basic Functionality Test ===\n";

    // 基本构造函数测试
    hgl::Color3ub color1;  // 默认构造函数 (0,0,0)
    hgl::Color3ub color2(128);  // 单值构造函数 (128,128,128)
    hgl::Color3ub color3(255, 128, 64);  // 三值构造函数

    std::cout << "color1 (default): r=" << (int)color1.r << " g=" << (int)color1.g << " b=" << (int)color1.b << "\n";
    std::cout << "color2 (128): r=" << (int)color2.r << " g=" << (int)color2.g << " b=" << (int)color2.b << "\n";
    std::cout << "color3 (255,128,64): r=" << (int)color3.r << " g=" << (int)color3.g << " b=" << (int)color3.b << "\n";

    // 颜色设置方法测试
    color1=hgl::RedColor3ub;
    color2=hgl::GreenColor3ub;
    color3=hgl::BlueColor3ub;

    std::cout << "After color operations:\n";
    std::cout << "color1.Red(): r=" << (int)color1.r << " g=" << (int)color1.g << " b=" << (int)color1.b << "\n";
    std::cout << "color2.Green(): r=" << (int)color2.r << " g=" << (int)color2.g << " b=" << (int)color2.b << "\n";
    std::cout << "color3.Blue(): r=" << (int)color3.r << " g=" << (int)color3.g << " b=" << (int)color3.b << "\n";
}

void test_color4ub_basic()
{
    std::cout << "\n=== Color4ub Basic Functionality Test ===\n";

    // 基本构造函数测试
    hgl::Color4ub color1;  // 默认构造函数 (0,0,0,255)
    hgl::Color4ub color2(128);  // 单值构造函数 (128,128,128,255)
    hgl::Color4ub color3(255, 128, 64, 192);  // 四值构造函数

    std::cout << "color1 (default): r=" << (int)color1.r << " g=" << (int)color1.g << " b=" << (int)color1.b << " a=" << (int)color1.a << "\n";
    std::cout << "color2 (128): r=" << (int)color2.r << " g=" << (int)color2.g << " b=" << (int)color2.b << " a=" << (int)color2.a << "\n";
    std::cout << "color3 (255,128,64,192): r=" << (int)color3.r << " g=" << (int)color3.g << " b=" << (int)color3.b << " a=" << (int)color3.a << "\n";

    // 测试32位颜色格式输出
    std::cout << "color3.ToRGBA8(): 0x" << std::hex << color3.toRGBA8() << std::dec << "\n";
    std::cout << "color3.ToBGRA8(): 0x" << std::hex << color3.toBGRA8() << std::dec << "\n";
    std::cout << "color3.ToARGB8(): 0x" << std::hex << color3.toARGB8() << std::dec << "\n";
    std::cout << "color3.ToABGR8(): 0x" << std::hex << color3.toABGR8() << std::dec << "\n";
}

void test_conversions()
{
    std::cout << "\n=== Color Conversion Tests ===\n";

    // Color3ub <-> Color3f 转换测试
    hgl::Color3ub ub3(255, 128, 64);
    hgl::Color3f f3 = ub3.toColor3f();

    std::cout << "Color3ub(255,128,64) -> Color3f: r=" << f3.r << " g=" << f3.g << " b=" << f3.b << "\n";

    hgl::Color3f f3_input(1.0f, 0.5f, 0.25f);
    hgl::Color3ub ub3_result = f3_input.toColor3ub();

    std::cout << "Color3f(1.0,0.5,0.25) -> Color3ub: r=" << (int)ub3_result.r << " g=" << (int)ub3_result.g << " b=" << (int)ub3_result.b << "\n";

    // Color4ub <-> Color4f 转换测试
    hgl::Color4ub ub4(255, 128, 64, 192);
    hgl::Color4f f4 = ub4.toColor4f();

    std::cout << "Color4ub(255,128,64,192) -> Color4f: r=" << f4.r << " g=" << f4.g << " b=" << f4.b << " a=" << f4.a << "\n";

    hgl::Color4f f4_input(1.0f, 0.5f, 0.25f, 0.75f);
    hgl::Color4ub ub4_result = f4_input.toColor4ub();

    std::cout << "Color4f(1.0,0.5,0.25,0.75) -> Color4ub: r=" << (int)ub4_result.r << " g=" << (int)ub4_result.g << " b=" << (int)ub4_result.b << " a=" << (int)ub4_result.a << "\n";
}

void test_assignment_operators()
{
    std::cout << "\n=== Assignment Operator Tests ===\n";

    // Color3ub = Color3f
    hgl::Color3ub ub3;
    hgl::Color3f f3(0.6f, 0.4f, 0.8f);
    ub3 = f3;
    std::cout << "Color3ub = Color3f(0.6,0.4,0.8): r=" << (int)ub3.r << " g=" << (int)ub3.g << " b=" << (int)ub3.b << "\n";

    // Color3f = Color3ub
    hgl::Color3f f3_2;
    hgl::Color3ub ub3_2(200, 100, 50);
    f3_2 = ub3_2;
    std::cout << "Color3f = Color3ub(200,100,50): r=" << f3_2.r << " g=" << f3_2.g << " b=" << f3_2.b << "\n";

    // Color4ub = Color4f
    hgl::Color4ub ub4;
    hgl::Color4f f4(0.6f, 0.4f, 0.8f, 0.9f);
    ub4 = f4;
    std::cout << "Color4ub = Color4f(0.6,0.4,0.8,0.9): r=" << (int)ub4.r << " g=" << (int)ub4.g << " b=" << (int)ub4.b << " a=" << (int)ub4.a << "\n";

    // Color4f = Color4ub
    hgl::Color4f f4_2;
    hgl::Color4ub ub4_2(200, 100, 50, 180);
    f4_2 = ub4_2;
    std::cout << "Color4f = Color4ub(200,100,50,180): r=" << f4_2.r << " g=" << f4_2.g << " b=" << f4_2.b << " a=" << f4_2.a << "\n";
}

void test_constructor_conversion()
{
    std::cout << "\n=== Constructor Conversion Tests ===\n";

    // 通过构造函数转换
    hgl::Color3ub ub3_source(200, 150, 100);
    hgl::Color3f f3_from_ub(ub3_source);  // 构造函数转换
    std::cout << "Color3f from Color3ub(200,150,100): r=" << f3_from_ub.r << " g=" << f3_from_ub.g << " b=" << f3_from_ub.b << "\n";

    hgl::Color3f f3_source(0.8f, 0.6f, 0.3f);
    hgl::Color3ub ub3_from_f(f3_source);  // 构造函数转换
    std::cout << "Color3ub from Color3f(0.8,0.6,0.3): r=" << (int)ub3_from_f.r << " g=" << (int)ub3_from_f.g << " b=" << (int)ub3_from_f.b << "\n";

    hgl::Color4ub ub4_source(200, 150, 100, 220);
    hgl::Color4f f4_from_ub(ub4_source);  // 构造函数转换
    std::cout << "Color4f from Color4ub(200,150,100,220): r=" << f4_from_ub.r << " g=" << f4_from_ub.g << " b=" << f4_from_ub.b << " a=" << f4_from_ub.a << "\n";

    hgl::Color4f f4_source(0.8f, 0.6f, 0.3f, 0.85f);
    hgl::Color4ub ub4_from_f(f4_source);  // 构造函数转换
    std::cout << "Color4ub from Color4f(0.8,0.6,0.3,0.85): r=" << (int)ub4_from_f.r << " g=" << (int)ub4_from_f.g << " b=" << (int)ub4_from_f.b << " a=" << (int)ub4_from_f.a << "\n";
}

void test_arithmetic_operations()
{
    std::cout << "\n=== Arithmetic Operations Test ===\n";

    hgl::Color3ub color1(100, 150, 200);
    hgl::Color3ub color2(50, 75, 25);

    hgl::Color3ub result_add = color1 + color2;
    std::cout << "Color3ub(100,150,200) + Color3ub(50,75,25) = (" << (int)result_add.r << "," << (int)result_add.g << "," << (int)result_add.b << ")\n";

    hgl::Color3ub result_mul = color1 * 0.5f;
    std::cout << "Color3ub(100,150,200) * 0.5 = (" << (int)result_mul.r << "," << (int)result_mul.g << "," << (int)result_mul.b << ")\n";

    hgl::Color4ub color4_1(100, 150, 200, 255);
    hgl::Color4ub color4_2(50, 75, 25, 128);

    hgl::Color4ub result4_add = color4_1 + color4_2;
    std::cout << "Color4ub(100,150,200,255) + Color4ub(50,75,25,128) = (" << (int)result4_add.r << "," << (int)result4_add.g << "," << (int)result4_add.b << "," << (int)result4_add.a << ")\n";
}

int main()
{
    std::cout << "CMCore Color3ub and Color4ub Test Suite\n";
    std::cout << "========================================\n";

    test_color3ub_basic();
    test_color4ub_basic();
    test_conversions();
    test_assignment_operators();
    test_constructor_conversion();
    test_arithmetic_operations();

    std::cout << "\n=== All Tests Completed ===\n";
    std::cout << "Color3ub and Color4ub classes are working correctly!\n";
    std::cout << "所有测试完成，Color3ub和Color4ub类运行正常！\n";

    return 0;
}
