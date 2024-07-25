#include "pch.h"

#include <string>
#include "Vector3D.h"
#include "Triangle.h"

using namespace std;
using namespace PT_CLOUD;

Triangle::Triangle(Vector3D* v0, Vector3D* v1, Vector3D* v2)
{
    Id = GenerateRunningId();
    Vertex[0] = v0;
    Vertex[1] = v1;
    Vertex[2] = v2;
}

Triangle::~Triangle()
{
}

int Triangle::GenerateRunningId()
{
    static int id = 0;
    return id++;
}

bool Triangle::HasVertexCoincidentWith(Vector3D* dot)
{
    return Vertex[0]->IsCoincidentWith(dot)
        || Vertex[1]->IsCoincidentWith(dot)
        || Vertex[2]->IsCoincidentWith(dot);
}

void Triangle::AssignNeighbors(Triangle* n0, Triangle* n1, Triangle* n2)
{
    Neighbor[0] = n0;
    Neighbor[1] = n1;
    Neighbor[2] = n2;
}