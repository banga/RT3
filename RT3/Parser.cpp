#include "Parser.h"

Scene* scene = NULL;
Camera* camera = NULL;
vector<Object3d*> objects;
vector<Light*> lights;
vector<Material*> materials;
Spectrum background;
int samplesPerPixel = 4, Width = 200, Height = 200;
int line_no = 1;

map<string,Variable*> variables;

bool Parse(const char* fname)
{
	if(fname == NULL)
	{
		yyin = stdin;
	}
	else
	{
		if(fopen_s(&yyin, fname, "r"))
		{
			printf ("\nError opening %s for input", fname);
			return false;
		}
	}

	// Parse scene file
	printf ("\nParsing %s", fname);
	clock_t t = clock();
	bool success = (yyparse() == 0);
	printf ("\nParsing completed in %.3fs\n", (clock()-t) / (float)CLOCKS_PER_SEC);

	// Close scene file
	if(yyin != stdin)
		fclose(yyin);

	for(vector<Material*>::iterator it = materials.begin(); it != materials.end(); it++)
		printf("\n%s at 0x%x", (*it)->toString().c_str(), (*it));

	// Delete variables
	for(map<string,Variable*>::iterator it = variables.begin(); it != variables.end(); it++)
		delete it->second;

	if(success)
	{
		scene = new Scene(samplesPerPixel, Width, Height, 4, 4, onRenderComplete, camera, objects, lights, materials);
		scene->Background = background;
		printf("\nScene contains:\n\t%d objects\n\t%d lights\n\t%d materials", Object3d::count, lights.size(), materials.size());
	}

	printf("\n");
	system("PAUSE");
	return success;
}