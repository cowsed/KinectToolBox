#ifndef MATH_H
#define MATH_H

struct vec3 {
    float x, y, z;
};

// Heatmap color lookup table generator, inspired from:
// http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
// https://stackoverflow.com/questions/58509179/qimage-how-to-convert-a-gray-image-to-a-rgb-heatmap
#include <QColor>
class ColorGradient {
private:
    struct ColorPoint {
        float r, g, b;
        float val; // Position of our color along the gradient (between 0 and 1).
        ColorPoint(float red, float green, float blue, float value)
            : r(red)
            , g(green)
            , b(blue)
            , val(value)
        {
        }
    };
    std::vector<ColorPoint> mColors; // An array of color points in ascending value.
    uint mTableSize = 0;
    QVector<QRgb> mColorMap;

    // hidden
    ColorGradient();

public:
    inline auto getColorMap()
    {
        return mColorMap;
    }

    ColorGradient(uint table_size)
    {
        createDefaultHeatMapGradient(table_size);
    }

    //-- Places a 6 color heapmap gradient into the "color" vector
#define CF64(val) ((float)(val) / 64.0)
#define CF256(val) ((float)(val) / 256.0)
    void createDefaultHeatMapGradient(uint table_size)
    {
        mTableSize = table_size;
        mColorMap.clear();
        mColors.clear();

        // ascending order
        mColors.push_back(ColorPoint(0.0, 0.0, 1.5, 0.001)); // Dark Blue
        mColors.push_back(ColorPoint(0.0, 0.0, 1.0, 0.05)); // Blue
        mColors.push_back(ColorPoint(0.0, 1.0, 1.0, 0.1)); // Cyan
        mColors.push_back(ColorPoint(1.0, 1.0, 0.0, 0.15)); // Yellow
        mColors.push_back(ColorPoint(1.0, 0.0, 0.0, 0.2)); // Red
        mColors.push_back(ColorPoint(0.5, 0.0, 0.0, 0.5)); // Dark Red

        QColor color;

        // Generate the color table
        for (uint n = 0; n < table_size; n++) {
            float value = (float)n / (float)table_size;
            bool found = false;

            for (int i = 0; i < mColors.size(); i++) {
                ColorPoint& currC = mColors[i];
                if (value < currC.val) {
                    ColorPoint& prevC = mColors[std::max(0, i - 1)];
                    float valueDiff = (prevC.val - currC.val);
                    float fractBetween = (valueDiff == 0) ? 0 : (value - currC.val) / valueDiff;

                    float r = (prevC.r - currC.r) * fractBetween + currC.r;
                    float g = (prevC.g - currC.g) * fractBetween + currC.g;
                    float b = (prevC.b - currC.b) * fractBetween + currC.b;
                    color.setRgbF(r, g, b);
                    mColorMap.append(color.rgb());
                    found = true;
                    break;
                }
            }

            if (!found) {
                color.setRgbF(mColors.back().r, mColors.back().g, mColors.back().b);
                mColorMap.append(color.rgb());
            }
        }
    }
};
#endif // MATH_H
