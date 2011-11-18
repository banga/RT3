#include "Mesh.h"
#include <fstream>
#include <time.h>
#include "OBJParse.h"

class TriangleData
{
public:
	int a, b, c;

	TriangleData():a(-1),b(-1),c(-1)
	{
	}
	TriangleData(int _a, int _b, int _c):a(_a),b(_b),c(_c)
	{
	}
};

Mesh::Mesh(string fname, Material* m):Object3d(m)
{
	clock_t t = clock();
	printf("\nParsing %s...", fname.c_str());
	string ext = fname.substr(fname.find_last_of('.') + 1);

	vector<Object3d*> objects;

	if(ext.compare("obj") == 0 || ext.compare("OBJ") == 0)
	{
		OBJParser parser;
		if(parser.ParseOBJ(fname,m))
		{
			objects = parser.getObjects();
		}
		else
		{
			printf("\nERROR parsing %s",fname.c_str());
			return;
		}
	} else {
		ifstream is(fname);
		if(!is.good()) {
			printf("\nERROR parsing %s",fname.c_str());
			return;
		} else {
			string line;
			char buf[256];

			if(ext.compare("ply") == 0) {
				int nTriangles, nVertices;
				bool isHeader = true;
				while(isHeader && (is.good())) {
					string word;
					is >> word;
					if(word.compare("element") == 0) {
						is >> word;
						if(word.compare("vertex") == 0) {
							is >> nVertices;
						} else if(word.compare("face") == 0) {
							is >> nTriangles;
						}
					} else if(word.compare("end_header") == 0) {
						isHeader = false;
					}
				}

				Vector* vertices = new Vector[nVertices];
				Vector* normals = new Vector[nVertices];
				float x, y, z;
				for(int i = 0; i < nVertices && is.good(); i++) {
					is >> x >> y >> z;
					vertices[i] = Vector(x,y,z);
					normals[i] = Vector(0,0,0);
				}

				TriangleData* triangles = new TriangleData[nTriangles];

				int a, b, c;
				for(int i = 0; i < nTriangles && is.good(); i++) {
					is >> a >> b >> c;
					Vector va = vertices[a], vb = vertices[b], vc = vertices[c];
					objects.push_back(new Triangle(va, vb, vc, m));
				}

				delete[] vertices;
				delete[] normals;
				delete[] triangles;
			} else if(ext.compare("tri") == 0) {
				while(is.good()) {
					string word;
					is >> word;
					if(word.compare("Triangle") == 0) {
						is.getline(buf,256);
						Vector a, b, c, n1, n2, n3;

						is >> a.x >> a.y >> a.z;
						is >> n1.x >> n1.y >> n1.z;
						is >> b.x >> b.y >> b.z;
						is >> n2.x >> n2.y >> n2.z;
						is >> c.x >> c.y >> c.z;
						is >> n3.x >> n3.y >> n3.z;

						objects.push_back(new MeshTriangle(a, b, c, n1, n2, n3, m));
					}
				}
			}
			printf("done in %.3fs.", (clock()-t) / (float)CLOCKS_PER_SEC);
		}
	}
	mesh = new KDTreeObject(objects);
}

void Mesh::GetBounds(Vector &min, Vector &max)
{
	mesh->GetBounds(min, max);
	if(transform) transform->TransformBounds(min,max);
}

bool Mesh::Intersect(Ray& r, Intersection& i)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);

	return mesh->Intersect(ray,i);
}

bool Mesh::IntersectP(Ray& r)
{
	Ray ray = (transform ? transform->InvTransformRay(r) : r);
	return mesh->IntersectP(ray);
}

Mesh::~Mesh()
{
}