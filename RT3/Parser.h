#include "Scene.h"
#include "Sphere.h"
#include "Box.h"
#include "Plane.h"
#include "Disc.h"
#include "Cylinder.h"
#include "Mesh.h"
#include "PointLight.h"
#include "TriangleLight.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "ConstantTexture.h"
#include "CheckerTexture.h"
#include "ImageTexture.h"
#include "Variable.h"

extern FILE* yyin;
extern Scene* scene;
extern Camera* camera;
extern vector<Object3d*> objects;
extern vector<Light*> lights;
extern vector<Material*> materials;
extern Spectrum background;
extern int samplesPerPixel, Width, Height, line_no;

extern void(*onRenderComplete)();
extern int yyparse(void);
extern int yyerror(char*);
bool Parse(const char*);

extern map<string,Variable*> variables;