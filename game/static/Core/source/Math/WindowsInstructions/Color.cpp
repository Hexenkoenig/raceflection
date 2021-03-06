#if OS_WINDOWS

#include <Math/WindowsInstructions/Instructions.h>

BEGINNAMESPACE

// some color functions
DX_IMPL_NAMEALIAS(ColorSRGBToRGB);
DX_IMPL_NAMEALIAS(ColorRGBToSRGB);
DX_IMPL_NAMEALIAS(ColorAdjustContrast);
DX_IMPL_NAMEALIAS(ColorAdjustSaturation);
DX_IMPL_NAMEALIAS(ColorEqual);
DX_IMPL_NAMEALIAS(ColorGreater);
DX_IMPL_NAMEALIAS(ColorGreaterOrEqual);
DX_IMPL_NAMEALIAS(ColorHSLToRGB);
DX_IMPL_NAMEALIAS(ColorHSVToRGB);
DX_IMPL_NAMEALIAS(ColorIsInfinite);
DX_IMPL_NAMEALIAS(ColorIsNaN);
DX_IMPL_NAMEALIAS(ColorLess);
DX_IMPL_NAMEALIAS(ColorLessOrEqual);
DX_IMPL_NAMEALIAS(ColorModulate);
DX_IMPL_NAMEALIAS(ColorNegative);
DX_IMPL_NAMEALIAS(ColorNotEqual);
DX_IMPL_NAMEALIAS(ColorRGBToHSL);
DX_IMPL_NAMEALIAS(ColorRGBToHSV);
DX_IMPL_NAMEALIAS(ColorRGBToXYZ);
DX_IMPL_NAMEALIAS(ColorRGBToYUV);
DX_IMPL_NAMEALIAS(ColorRGBToYUV_HD);
DX_IMPL_NAMEALIAS(ColorSRGBToXYZ);
DX_IMPL_NAMEALIAS(ColorXYZToRGB);
DX_IMPL_NAMEALIAS(ColorXYZToSRGB);
DX_IMPL_NAMEALIAS(ColorYUVToRGB);
DX_IMPL_NAMEALIAS(ColorYUVToRGB_HD);

ENDNAMESPACE

#endif
