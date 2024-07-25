#ifndef VECTOR3D_H
#define VECTOR3D_H

namespace PT_CLOUD
{
    class Vector3D
    {
    private:
        int GenerateRunningId();
    public:
        int Id = 0;

        // coordinate
        double X, Y, Z;

        // original coordinate
        double m_X, m_Y, m_Z;

        // color
        double R, G, B;
        double m_dScaleFactor;

        bool IsVisited = false;
        bool IsAuxiliaryDot = false;

        Vector3D(double x, double y, double z, double r = 255, double g = 248, double b = 220);
        Vector3D(double x, double y, double z, bool isAuxiliaryDot, double r = 255, double g = 248, double b = 220);
        Vector3D(Vector3D* dot, double lengthAfterProjection);
        ~Vector3D();

        bool IsCoincidentWith(Vector3D* dot);
    };
}

#endif