#include "pch.h"

#include <cmath>
#include <string>
#include "Vector3D.h"

using namespace std;
using namespace PT_CLOUD;

Vector3D::Vector3D(double x, double y, double z, double r, double g, double b)
{
    Id = GenerateRunningId();

    X = x;
    Y = y;
    Z = z;

    m_X = x;
    m_Y = y;
    m_Z = z;

    R = r;
    G = g;
    B = b;
    m_dScaleFactor = 1;
}

Vector3D::Vector3D(double x, double y, double z, bool isAuxiliaryDot, double r, double g, double b)
{
    Id = GenerateRunningId();

    IsAuxiliaryDot = isAuxiliaryDot;

    X = x;
    Y = y;
    Z = z;

    m_X = x;
    m_Y = y;
    m_Z = z;

    R = r;
    G = g;
    B = b;
    m_dScaleFactor = 1;
}

Vector3D::Vector3D(Vector3D* dot, double lengthAfterProjection)
{
    Id = dot->Id;
    IsVisited = dot->IsVisited;
    IsAuxiliaryDot = dot->IsAuxiliaryDot;

    double length = sqrt(pow(dot->X, 2) + pow(dot->Y, 2) + pow(dot->Z, 2));
    double scaleFactor = lengthAfterProjection / length;
    m_dScaleFactor = scaleFactor;

    m_X = dot->X;
    m_Y = dot->Y;
    m_Z = dot->Z;

    X = scaleFactor * dot->X;
    Y = scaleFactor * dot->Y;
    Z = scaleFactor * dot->Z;

    R = dot->R;
    G = dot->G;
    B = dot->B;
}

Vector3D::~Vector3D()
{
}

int Vector3D::GenerateRunningId()
{
    static int id = 0;
    return id++;
}

bool Vector3D::IsCoincidentWith(Vector3D* dot)
{
    return (X == dot->X && Y == dot->Y && Z == dot->Z);
}
