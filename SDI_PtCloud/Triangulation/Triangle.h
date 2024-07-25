#ifndef TRIANGLE_H
#define TRIANGLE_H

namespace PT_CLOUD
{
    class Triangle
    {
    private:
        int GenerateRunningId();
    public:
        int Id = 0;

        // pointers pointing to 3 vertices
        Vector3D* Vertex[3];
        // pointers pointing to 3 neighbors
        Triangle* Neighbor[3];

        Triangle(Vector3D* v0, Vector3D* v1, Vector3D* v2);
        ~Triangle();

        bool HasVertexCoincidentWith(Vector3D* dot);
        void AssignNeighbors(Triangle* n0, Triangle* n1, Triangle* n2);
    };
}

#endif