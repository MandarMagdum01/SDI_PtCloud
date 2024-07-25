#include "pch.h"

#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include "DelaunayTriangulation.h"

using namespace std;
using namespace PT_CLOUD;

DelaunayTriangulation::DelaunayTriangulation()
{
    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        m_AuxiliaryDots[i] = new Vector3D(
            (i % 2 == 0 ? 1 : -1) * (i / 2 == 0 ? VECTOR_LENGTH : 0),
            (i % 2 == 0 ? 1 : -1) * (i / 2 == 1 ? VECTOR_LENGTH : 0),
            (i % 2 == 0 ? 1 : -1) * (i / 2 == 2 ? VECTOR_LENGTH : 0),
            true, 0, 0, 0
        );
    }

    m_ProjectedDots = new vector<Vector3D*>();
    m_Mesh = new vector<Triangle*>();

}

DelaunayTriangulation::~DelaunayTriangulation()
{
    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        delete m_AuxiliaryDots[i];
    }

    vector<Vector3D*>::iterator itDots;
    for (itDots = m_ProjectedDots->begin(); itDots != m_ProjectedDots->end(); itDots++)
    {
        delete *itDots;
    }

    vector<Triangle*>::iterator itMesh;
    for (itMesh = m_Mesh->begin(); itMesh != m_Mesh->end(); itMesh++)
    {
        delete *itMesh;
    }

    delete m_ProjectedDots;
    delete m_Mesh;
}

vector<Triangle*> DelaunayTriangulation::GetTriangulationResult(vector<Vector3D*> &dots)
{

    m_ProjectedDots->reserve(dots.size());

    // N random dots can form 8+(N-6)*2 triangles based on the algorithm
    m_Mesh->reserve(8 + (dots.size() - 6) * 2);

    // project dots to an unit shpere for triangulation
    vector<Vector3D*>::iterator itDots;
    for (itDots = dots.begin(); itDots != dots.end(); itDots++)
    {
        Vector3D* projectedDot = new Vector3D((*itDots), VECTOR_LENGTH);
        m_ProjectedDots->push_back(projectedDot);
    }

    // prepare initial convex hull with 6 vertices and 8 triangle faces
    BuildInitialHull(m_ProjectedDots);

    for (itDots = m_ProjectedDots->begin(); itDots != m_ProjectedDots->end(); itDots++)
    {
        Vector3D* dot = *itDots;
        if (!dot->IsVisited)
        {
            InsertDot(dot);
        }
    }

    // remove trianges connected with auxiliary dots
    RemoveExtraTriangles();

    // generate output
    vector<Triangle*>::iterator itMesh;
    for (itMesh = m_Mesh->begin(); itMesh != m_Mesh->end(); itMesh++)
    {
        Triangle* triangle = *itMesh;
        triangle->Vertex[0]->X = triangle->Vertex[0]->m_X ;
        triangle->Vertex[0]->Y = triangle->Vertex[0]->m_Y;
        triangle->Vertex[0]->Z = triangle->Vertex[0]->m_Z ;

        triangle->Vertex[1]->X = triangle->Vertex[1]->m_X;
        triangle->Vertex[1]->Y = triangle->Vertex[1]->m_Y;
        triangle->Vertex[1]->Z = triangle->Vertex[1]->m_Z;

        triangle->Vertex[2]->X = triangle->Vertex[2]->m_X;
        triangle->Vertex[2]->Y = triangle->Vertex[2]->m_Y;
        triangle->Vertex[2]->Z = triangle->Vertex[2]->m_Z;
    }


    return vector<Triangle*>(*m_Mesh);
}


void DelaunayTriangulation::BuildInitialHull(vector<Vector3D*>* dots)
{
    Vector3D* initialVertices[INIT_VERTICES_COUNT];
    Triangle* initialHullFaces[INIT_FACES_COUNT];

    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        initialVertices[i] = m_AuxiliaryDots[i];
    }

    // if close enough, use input dots to replace auxiliary dots so won't be removed in the end
    double minDistance[INIT_VERTICES_COUNT] = { 0, 0, 0, 0, 0, 0 };
    vector<Vector3D*>::iterator it;
    for (it = dots->begin(); it != dots->end(); it++)
    {
        double distance[INIT_VERTICES_COUNT];
        for (int i = 0; i < INIT_VERTICES_COUNT; i++)
        {
            distance[i] = GetDistance(m_AuxiliaryDots[i], *it);
            if (minDistance[i] == 0 || distance[i] < minDistance[i])
            {
                minDistance[i] = distance[i];
            }
        }

        for (int i = 0; i < INIT_VERTICES_COUNT; i++)
        {
            if (minDistance[i] == distance[i] && IsMinimumValueInArray(distance, INIT_VERTICES_COUNT, i))
            {
                initialVertices[i] = *it;
            }
        }
    }

    int vertex0Index[] = { 0, 0, 0, 0, 1, 1, 1, 1 };
    int vertex1Index[] = { 4, 3, 5, 2, 2, 4, 3, 5 };
    int vertex2Index[] = { 2, 4, 3, 5, 4, 3, 5, 2 };

    for (int i = 0; i < INIT_FACES_COUNT; i++)
    {
        Vector3D* v0 = initialVertices[vertex0Index[i]];
        Vector3D* v1 = initialVertices[vertex1Index[i]];
        Vector3D* v2 = initialVertices[vertex2Index[i]];

        Triangle* triangle = new Triangle(v0, v1, v2);
        initialHullFaces[i] = triangle;

        m_Mesh->push_back(triangle);
    }

    int neighbor0Index[] = { 1, 2, 3, 0, 7, 4, 5, 6 };
    int neighbor1Index[] = { 4, 5, 6, 7, 0, 1, 2, 3 };
    int neighbor2Index[] = { 3, 0, 1, 2, 5, 6, 7, 4 };

    for (int i = 0; i < INIT_FACES_COUNT; i++)
    {
        Triangle* n0 = initialHullFaces[neighbor0Index[i]];
        Triangle* n1 = initialHullFaces[neighbor1Index[i]];
        Triangle* n2 = initialHullFaces[neighbor2Index[i]];
        initialHullFaces[i]->AssignNeighbors(n0, n1, n2);
    }

    // dot already in the mesh, avoid being visited by InsertDot() again
    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        initialVertices[i]->IsVisited = true;
    }
}

void DelaunayTriangulation::InsertDot(Vector3D* dot)
{
    double det[] = { 0, 0, 0 };

    vector<Triangle*>::iterator it;
    it = m_Mesh->begin();
    Triangle* triangle = *it;

    while (it != m_Mesh->end())
    {
        det[0] = GetDeterminant(triangle->Vertex[0], triangle->Vertex[1], dot);
        det[1] = GetDeterminant(triangle->Vertex[1], triangle->Vertex[2], dot);
        det[2] = GetDeterminant(triangle->Vertex[2], triangle->Vertex[0], dot);

        // if this dot projected into an existing triangle, split the existing triangle to 3 new ones
        if (det[0] >= 0 && det[1] >= 0 && det[2] >= 0)
        {
            if (!triangle->HasVertexCoincidentWith(dot))
            {
                SplitTriangle(triangle, dot);
            }

            return;
        }

        // on one side, search neighbors
        else if (det[1] >= 0 && det[2] >= 0)
            triangle = triangle->Neighbor[0];
        else if (det[0] >= 0 && det[2] >= 0)
            triangle = triangle->Neighbor[1];
        else if (det[0] >= 0 && det[1] >= 0)
            triangle = triangle->Neighbor[2];

        // cannot determine effectively 
        else if (det[0] >= 0)
            triangle = triangle->Neighbor[1];
        else if (det[1] >= 0)
            triangle = triangle->Neighbor[2];
        else if (det[2] >= 0)
            triangle = triangle->Neighbor[0];
        else
            triangle = *it++;
    }
}

void DelaunayTriangulation::RemoveExtraTriangles()
{
    vector<Triangle*>::iterator it;
    for (it = m_Mesh->begin(); it != m_Mesh->end();)
    {
        Triangle* triangle = *it;
        bool isExtraTriangle = false;
        for (int i = 0; i < 3; i++)
        {
            if (triangle->Vertex[i]->IsAuxiliaryDot)
            {
                isExtraTriangle = true;
                break;
            }
        }

        if (isExtraTriangle)
        {
            delete *it;
            it = m_Mesh->erase(it);
        }
        else
        {
            it++;
        }
    }
}

void DelaunayTriangulation::SplitTriangle(Triangle* triangle, Vector3D* dot)
{
    Triangle* newTriangle1 = new Triangle(dot, triangle->Vertex[1], triangle->Vertex[2]);
    Triangle* newTriangle2 = new Triangle(dot, triangle->Vertex[2], triangle->Vertex[0]);

    triangle->Vertex[2] = triangle->Vertex[1];
    triangle->Vertex[1] = triangle->Vertex[0];
    triangle->Vertex[0] = dot;

    newTriangle1->AssignNeighbors(triangle, triangle->Neighbor[1], newTriangle2);
    newTriangle2->AssignNeighbors(newTriangle1, triangle->Neighbor[2], triangle);
    triangle->AssignNeighbors(newTriangle2, triangle->Neighbor[0], newTriangle1);

    FixNeighborhood(newTriangle1->Neighbor[1], triangle, newTriangle1);
    FixNeighborhood(newTriangle2->Neighbor[1], triangle, newTriangle2);

    m_Mesh->push_back(newTriangle1);
    m_Mesh->push_back(newTriangle2);

    // optimize triangles according to delaunay triangulation definition
    DoLocalOptimization(triangle, triangle->Neighbor[1]);
    DoLocalOptimization(newTriangle1, newTriangle1->Neighbor[1]);
    DoLocalOptimization(newTriangle2, newTriangle2->Neighbor[1]);
}

void DelaunayTriangulation::FixNeighborhood(Triangle* target, Triangle* oldNeighbor, Triangle* newNeighbor)
{
    for (int i = 0; i < 3; i++)
    {
        if (target->Neighbor[i] == oldNeighbor)
        {
            target->Neighbor[i] = newNeighbor;
            break;
        }
    }
}

void DelaunayTriangulation::DoLocalOptimization(Triangle* t0, Triangle* t1)
{

    for (int i = 0; i < 3; i++)
    {
        if (t1->Vertex[i] == t0->Vertex[0] ||
            t1->Vertex[i] == t0->Vertex[1] ||
            t1->Vertex[i] == t0->Vertex[2])
        {
            continue;
        }

        double matrix[] = {
            t1->Vertex[i]->X - t0->Vertex[0]->X,
            t1->Vertex[i]->Y - t0->Vertex[0]->Y,
            t1->Vertex[i]->Z - t0->Vertex[0]->Z,

            t1->Vertex[i]->X - t0->Vertex[1]->X,
            t1->Vertex[i]->Y - t0->Vertex[1]->Y,
            t1->Vertex[i]->Z - t0->Vertex[1]->Z,

            t1->Vertex[i]->X - t0->Vertex[2]->X,
            t1->Vertex[i]->Y - t0->Vertex[2]->Y,
            t1->Vertex[i]->Z - t0->Vertex[2]->Z
        };

        if (GetDeterminant(matrix) <= 0)
        {
            // terminate after optimized
            break;
        }

        if (TrySwapDiagonal(t0, t1))
        {
            return;
        }
    }
}

bool DelaunayTriangulation::TrySwapDiagonal(Triangle* t0, Triangle* t1)
{
    for (int j = 0; j < 3; j++)
    {
        for (int k = 0; k < 3; k++)
        {
            if (t0->Vertex[j] != t1->Vertex[0] &&
                t0->Vertex[j] != t1->Vertex[1] &&
                t0->Vertex[j] != t1->Vertex[2] &&
                t1->Vertex[k] != t0->Vertex[0] &&
                t1->Vertex[k] != t0->Vertex[1] &&
                t1->Vertex[k] != t0->Vertex[2])
            {
                t0->Vertex[(j + 2) % 3] = t1->Vertex[k];
                t1->Vertex[(k + 2) % 3] = t0->Vertex[j];

                t0->Neighbor[(j + 1) % 3] = t1->Neighbor[(k + 2) % 3];
                t1->Neighbor[(k + 1) % 3] = t0->Neighbor[(j + 2) % 3];
                t0->Neighbor[(j + 2) % 3] = t1;
                t1->Neighbor[(k + 2) % 3] = t0;

                FixNeighborhood(t0->Neighbor[(j + 1) % 3], t1, t0);
                FixNeighborhood(t1->Neighbor[(k + 1) % 3], t0, t1);

                DoLocalOptimization(t0, t0->Neighbor[j]);
                DoLocalOptimization(t0, t0->Neighbor[(j + 1) % 3]);
                DoLocalOptimization(t1, t1->Neighbor[k]);
                DoLocalOptimization(t1, t1->Neighbor[(k + 1) % 3]);

                return true;
            }
        }
    }

    return false;
}

bool DelaunayTriangulation::IsMinimumValueInArray(double arr[], int length, int index)
{
    for (int i = 0; i < length; i++)
    {
        if (arr[i] < arr[index])
        {
            return false;
        }
    }

    return true;
}

double DelaunayTriangulation::GetDistance(Vector3D* v0, Vector3D* v1)
{
    return sqrt(pow((v0->X - v1->X), 2) +
        pow((v0->Y - v1->Y), 2) +
        pow((v0->Z - v1->Z), 2));
}

double DelaunayTriangulation::GetDeterminant(Vector3D* v0, Vector3D* v1, Vector3D* v2)
{
    double matrix[] = {
        v0->X, v0->Y, v0->Z,
        v1->X, v1->Y, v1->Z,
        v2->X, v2->Y, v2->Z
    };

    return GetDeterminant(matrix);
}

double DelaunayTriangulation::GetDeterminant(double matrix[])
{
    // inversed for left handed coordinate system
    double determinant = matrix[2] * matrix[4] * matrix[6]
        + matrix[0] * matrix[5] * matrix[7]
        + matrix[1] * matrix[3] * matrix[8]
        - matrix[0] * matrix[4] * matrix[8]
        - matrix[1] * matrix[5] * matrix[6]
        - matrix[2] * matrix[3] * matrix[7];

    // adjust result based on float number accuracy, otherwise causing deadloop
    return abs(determinant) <= DBL_EPSILON ? 0 : determinant;
}
