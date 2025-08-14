# Color3ub and Color4ub Classes

This document describes the new Color3ub and Color4ub classes added to the CMCore color system.

## Overview

The CMCore color system now includes integer-based color classes alongside the existing float-based ones:

- **Color3f/Color4f**: Float-based RGB/RGBA colors (0.0 - 1.0 range)
- **Color3ub/Color4ub**: Integer-based RGB/RGBA colors (0 - 255 range) - **NEW**

## Class Descriptions

### Color3ub - RGB Color with uint8 Components

```cpp
class Color3ub
{
public:
    uint8 r, g, b;  // Red, Green, Blue components (0-255)
    
    // Constructors
    Color3ub();                           // Default: (0,0,0)
    Color3ub(uint8 l);                    // Gray: (l,l,l)
    Color3ub(uint8 r, uint8 g, uint8 b);  // RGB
    Color3ub(const Color3f &v);           // From Color3f
    
    // Conversion methods
    Color3f ToColor3f() const;
    uint8 ToGrey() const;
    
    // Standard color methods
    void Red(), Green(), Blue();
    void Black(), White();
    void Yellow(), Purple();
    void Zero(), One();
    
    // Arithmetic operations
    Color3ub operator+(const Color3ub &v);
    Color3ub operator*(float v);
    // ... and more
};
```

### Color4ub - RGBA Color with uint8 Components

```cpp
class Color4ub
{
public:
    uint8 r, g, b, a;  // Red, Green, Blue, Alpha components (0-255)
    
    // Constructors
    Color4ub();                                    // Default: (0,0,0,255)
    Color4ub(uint8 l);                            // Gray: (l,l,l,255)
    Color4ub(uint8 r, uint8 g, uint8 b, uint8 a); // RGBA
    Color4ub(const Color3ub &v, uint8 a = 255);   // From Color3ub
    Color4ub(const Color4f &v);                   // From Color4f
    
    // Conversion methods
    Color4f ToColor4f() const;
    uint32 ToRGBA8() const;
    uint32 ToBGRA8() const;
    uint32 ToARGB8() const;
    uint32 ToABGR8() const;
    
    // Standard color and arithmetic operations
    // ... similar to Color3ub
};
```

## Conversion Support

Full bidirectional conversion is supported between float and integer color classes:

### Automatic Conversions

```cpp
// Assignment operators
Color3ub byteColor;
Color3f floatColor(0.5f, 0.8f, 0.2f);
byteColor = floatColor;  // Automatic conversion

Color3f anotherFloat;
Color3ub anotherByte(128, 200, 50);
anotherFloat = anotherByte;  // Automatic conversion
```

### Constructor Conversions

```cpp
Color3f floatFromByte(Color3ub(255, 128, 64));
Color3ub byteFromFloat(Color3f(1.0f, 0.5f, 0.25f));
```

### Explicit Conversions

```cpp
Color3ub byteColor(200, 150, 100);
Color3f floatColor = byteColor.ToColor3f();

Color3f someFloat(0.8f, 0.6f, 0.4f);
Color3ub someBytes = someFloat.ToColor3ub();
```

## Usage Examples

### Basic Usage

```cpp
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>

// Create colors
Color3ub red(255, 0, 0);
Color3ub green(0, 255, 0);
Color4ub semiTransparentBlue(0, 0, 255, 128);

// Color operations
Color3ub purple = red + blue;
Color3ub dimmedRed = red * 0.5f;

// Get 32-bit color values
uint32 rgba = semiTransparentBlue.ToRGBA8();
uint32 bgra = semiTransparentBlue.ToBGRA8();
```

### Conversion Examples

```cpp
// Float to integer
Color3f floatColor(0.6f, 0.8f, 0.4f);
Color3ub intColor = floatColor.ToColor3ub();  // (153, 204, 102)

// Integer to float  
Color3ub byteColor(200, 100, 50);
Color3f convertedFloat = byteColor.ToColor3f();  // (0.784, 0.392, 0.196)

// Mixed operations
Color3f result = Color3ub(255, 128, 64).ToColor3f() * 0.8f;
```

## Integration

The new classes integrate seamlessly with the existing CMCore color system:

- Follow the same naming conventions and method patterns
- Automatically detected by the CMake build system
- Compatible with existing Vector3u8/Vector4u8 type aliases
- Support all standard arithmetic and comparison operations

## Files Added

- `inc/hgl/color/Color3ub.h` - Color3ub class definition
- `inc/hgl/color/Color4ub.h` - Color4ub class definition  
- `src/Color/Color3ub.cpp` - Color3ub implementation
- `src/Color/Color4ub.cpp` - Color4ub implementation

## Files Modified

- `inc/hgl/color/Color3f.h` - Added Color3ub conversion support
- `inc/hgl/color/Color4f.h` - Added Color4ub conversion support
- `src/Color/Color3f.cpp` - Added conversion implementations
- `src/Color/Color4f.cpp` - Added conversion implementations