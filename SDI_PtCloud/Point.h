#pragma once
class Point3D 
{
public:
	float x, y, z;
	float value1, value2, value3, value4;
    int m_nId;  // Unique identifier for each point
    Point3D() : x(0), y(0), z(0), value1(0), value2(0), value3(0), value4(0), m_nId(-1)
    {
        // Constructor initializer list initializes member variables
        // with the provided values.
    }

    Point3D(float xVal, float yVal, float zVal, float v1, float v2, float v3, float v4, int inId)
        : x(xVal), y(yVal), z(zVal), value1(v1), value2(v2), value3(v3), value4(v4), m_nId(inId){}

};
