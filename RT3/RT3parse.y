/* RT3.y - parser for RT3 scene files */
%{
#include <malloc.h>
#include <stdlib.h>
#include <stack>
#include "Parser.h"

#define YYDEBUG 1
#define YYMAXDEPTH 100000000

int yylex( void );
int yyerror( char* );

using namespace std;
stack<void*> toDelete;

void clear_stack();
bool exists(string str);
bool getInteger(int *i, string str);
bool getFloat(float *f, string str);
bool getVector(Vector **v, string str);
bool getSpectrum(Spectrum **s, string str);
bool getMaterial(Material **m, string str);
bool getObject(Object3d **o, string str);
bool getLight(Light **l, string str);
bool getCamera(Camera **c, string str);
Variable* getVariable(string str);
char buf[256];
%}

%error-verbose

/* BISON Declarations */
%union {
	Image* image;
	Camera* camera;
	Material* material;
	Texture<Spectrum>* spectrum_texture;
	Object3d* object;
	vector<Object3d*>* objects;
	Transform* transform;
	Light* light;
	float real;
	int integer;
	string* str;
	Vector* vector;
	Spectrum* spectrum;
}
%token NEWLINE PLUS MINUS ASTERISK SLASH LPAREN RPAREN LANG RANG LCURL RCURL LSQ RSQ COMMA XVECTOR YVECTOR ZVECTOR EQ

%token <integer> NUMBER_INTEGER
%type <integer> number_integer expr term factor

%token <real> NUMBER_FLOAT
%type <real> number_expr number number_float float_expr float_term float_factor

%type <vector> vector vector_expr vector_term vector_factor vector_declaration
%type <spectrum> spectrum spectrum_expr spectrum_term spectrum_factor

%token <str> STRING IDENTIFIER
%type <str> filename

%token FILM FILM_WIDTH FILM_HEIGHT FILM_SAMPLES FILM_BACKGROUND
%type <image> film

%token PERSPECTIVE_CAMERA ORTHOGRAPHIC_CAMERA CAMERA_ORIGIN CAMERA_UP CAMERA_RIGHT CAMERA_ANGLE CAMERA_LOOKAT
%type<camera> camera camera_declaration perspective_camera orthographic_camera

%token MATERIAL_KD MATERIAL_KA MATERIAL_REFLECTED_COLOR MATERIAL_TRANSMITTED_COLOR MATERIAL_IOR
%type <material> material material_declaration diffuse_material ambient_material reflective_material transparent_material

%token TEXTURE_CHECKER TEXTURE_IMAGE
%type <spectrum_texture> spectrum_texture

%token SPHERE PLANE BOX DISC CYLINDER TRIANGLE MESH KDTREE KDTREE_LEAFSIZE GROUP TRANSFORMED_OBJECT
%type <object> object sphere plane box disc cylinder triangle mesh kdtree group
%type <objects> objects

%token TRANSLATE ROTATE SCALE
%type <transform> transform transforms

%token POINTLIGHT TRIANGLELIGHT
%type <light> light pointlight trianglelight

%token INTEGER_TYPE FLOAT_TYPE VECTOR_TYPE SPECTRUM_TYPE MATERIAL_TYPE OBJECT_TYPE LIGHT_TYPE CAMERA_TYPE

/* Grammar follows */
%%
input:              /* empty string */
    | input declaration
    ;
declaration: 
	film
	{
		clear_stack();
	}
	| camera
	{
		camera = $1;
		clear_stack();
	}
	| object
	{
		objects.push_back($1);
		clear_stack();
	}
	| light
	{
		lights.push_back($1);
		clear_stack();
	}
	| integervar | floatvar | vectorvar | spectrumvar | materialvar | objectvar | lightvar | cameravar
    ;
number_expr: float_expr | expr { $$ = (float)$1 }
expr: expr PLUS term  { $$ = $1 + $3; }
    | expr MINUS term { $$ = $1 + $3; }
    | term
    ;
term: term ASTERISK number_integer { $$ = $1 * $3; }
    | term SLASH number_integer    { $$ = $1 / $3; }
	| factor
    ;
factor: LPAREN expr RPAREN  { $$ = $2 }
      | number_integer
      ;
number_integer: NUMBER_INTEGER 
	| INTEGER_TYPE IDENTIFIER
	{
		if(!getInteger(&$$,*$2)) YYABORT;
	};
float_expr: float_expr PLUS float_term  { $$ = $1 + $3 }
    | float_expr MINUS float_term { $$ = $1 + $3 }
    | float_term
    ;
float_term: float_term ASTERISK number { $$ = $1 * $3 }
	| float_term SLASH number { $$ = $1 / $3 }
    | float_factor
    ;
float_factor: LPAREN float_expr RPAREN  { $$ = $2 }
    | number
    ;
number_float: NUMBER_FLOAT
	| FLOAT_TYPE IDENTIFIER
	{
		if(!getFloat(&$$,*$2)) YYABORT;
	};
number: number_float | number_integer { $$ = (float)$1 }
vector_expr: vector_expr PLUS vector_term
	{
		$$ = new Vector(*$1 + *$3);
		toDelete.push($$);
	}
    | vector_expr MINUS vector_term
	{
		$$ = new Vector(*$1 - *$3);
		toDelete.push($$);
	}
    | vector_term
    ;
vector_term: vector_term ASTERISK number_expr
	{
		$$ = new Vector((*$1) * $3);
		toDelete.push($$);
	}
    | vector_factor
    ;
vector_factor: LPAREN vector_expr RPAREN  { $$ = $2 }
    | vector
    ;
vector: vector_declaration
vector_declaration: LANG number_expr COMMA number_expr COMMA number_expr RANG
	{
		$$ = new Vector($2,$4,$6);
		toDelete.push($$);
	}
	| LANG number_expr number_expr number_expr RANG
	{
		$$ = new Vector($2,$3,$4);
		toDelete.push($$);
	}
	| XVECTOR { $$ = new Vector(1,0,0); toDelete.push($$); }
	| YVECTOR { $$ = new Vector(0,1,0); toDelete.push($$); }
	| ZVECTOR { $$ = new Vector(0,0,1); toDelete.push($$); }
	| LANG number_expr RANG {$$ = new Vector($2,$2,$2); toDelete.push($$);}
	| LANG IDENTIFIER RANG
	{
		if(!getVector(&$$,*$2)) YYABORT;
	};
spectrum_expr: spectrum_expr PLUS spectrum_term
	{
		$$ = new Spectrum(*$1 + *$3);
		toDelete.push($$);
	}
    | spectrum_expr MINUS spectrum_term
	{
		$$ = new Spectrum(*$1 - *$3);
		toDelete.push($$);
	}
    | spectrum_term
    ;
spectrum_term: spectrum_term ASTERISK number_expr
	{
		$$ = new Spectrum((*$1) * $3);
		toDelete.push($$);
	}
    | spectrum_factor
    ;
spectrum_factor: LPAREN spectrum_expr RPAREN  { $$ = $2 }
    | spectrum
    ;
spectrum: LSQ number_expr COMMA number_expr COMMA number_expr RSQ
	{
		float c[3] = {$2,$4,$6};
		$$ = new Spectrum(c);
		toDelete.push($$);
	}
	| LSQ number_expr number_expr number_expr RSQ
	{
		float c[3] = {$2,$3,$4};
		$$ = new Spectrum(c);
		toDelete.push($$);
	}
	| LSQ number_expr RSQ
	{
		float c[3] = {$2,$2,$2};
		$$ = new Spectrum(c);
		toDelete.push($$);
	}
	| LSQ IDENTIFIER RSQ
	{
		printf("\n%s", $2->c_str());
		if(!getSpectrum(&$$,*$2)) YYABORT;
	};
film: FILM LCURL FILM_WIDTH expr FILM_HEIGHT expr FILM_SAMPLES expr FILM_BACKGROUND spectrum_expr RCURL 
	{
		Width = $4;
		Height = $6;
		samplesPerPixel = $8;
		background = *$10;
		printf("\n%d %d %d", $4, $6, $8);
	};
camera: CAMERA_TYPE LCURL camera_declaration RCURL { $$ = $3; }
    | CAMERA_TYPE LCURL camera_declaration CAMERA_LOOKAT vector_expr RCURL
	{
		$3->lookAt(*$5);
		$$ = $3;
	};
camera_declaration: perspective_camera | orthographic_camera
	| CAMERA_TYPE IDENTIFIER
	{
		if(!getCamera(&$$,*$2)) YYABORT;
	};
perspective_camera:
	PERSPECTIVE_CAMERA LCURL CAMERA_ORIGIN vector_expr CAMERA_UP vector_expr CAMERA_RIGHT vector_expr CAMERA_ANGLE number_expr RCURL
	{
		$$ = new PerspectiveCamera(*$4,*$6,*$8,$10);
	};
orthographic_camera: ORTHOGRAPHIC_CAMERA LCURL CAMERA_ORIGIN vector_expr CAMERA_UP vector_expr CAMERA_RIGHT vector_expr RCURL
	{
		$$ = new OrthographicCamera(*$4,*$6,*$8);
	};
spectrum_texture: spectrum_expr { $$ = new ConstantTexture<Spectrum> (*$1); }
	| TEXTURE_CHECKER LCURL spectrum_expr spectrum_expr transforms RCURL
	{
		$$ = new CheckerTexture<Spectrum> (*$3,*$4,$5);
	}
	| TEXTURE_IMAGE LCURL filename transforms RCURL
	{
		$$ = new ImageTexture ($3->c_str(),$4);
	}
	| TEXTURE_IMAGE LCURL filename number_expr transforms RCURL
	{
		$$ = new ImageTexture ($3->c_str(),$5,$4);
	}
material: material_declaration
material_declaration: diffuse_material | ambient_material | reflective_material | transparent_material
	| MATERIAL_TYPE IDENTIFIER
	{
		if(!getMaterial(&$$,*$2)) YYABORT;
	};
diffuse_material: MATERIAL_TYPE LCURL MATERIAL_KD spectrum_texture RCURL
	{
		Material *m = new Material($4);
		materials.push_back(m);
		$$ = m;
		Intersection i;
	};
ambient_material: MATERIAL_TYPE LCURL MATERIAL_KD spectrum_texture MATERIAL_KA spectrum_texture RCURL
	{
		Material *m = new Material($4,$6);
		materials.push_back(m);
		$$ = m;
	};
reflective_material: MATERIAL_TYPE LCURL MATERIAL_KD spectrum_texture MATERIAL_KA spectrum_texture MATERIAL_REFLECTED_COLOR spectrum_texture RCURL
	{
		Material *m = new Material($4,$6,$8);
		materials.push_back(m);
		$$ = m;
	};
transparent_material: MATERIAL_TYPE LCURL MATERIAL_KD spectrum_texture MATERIAL_KA spectrum_texture MATERIAL_REFLECTED_COLOR spectrum_texture MATERIAL_TRANSMITTED_COLOR spectrum_texture MATERIAL_IOR number_expr RCURL
	{
		Material *m = new Material($4,$6,$8,$10,$12);
		materials.push_back(m);
		$$ = m;
	};
object: sphere | plane | box | disc | cylinder | triangle | mesh | kdtree | group
	| OBJECT_TYPE IDENTIFIER
	{
		if(!getObject(&$$,*$2)) YYABORT;
	};
sphere: SPHERE LCURL vector_expr number_expr material transforms RCURL
	{
		$$ = new Sphere(*$3,$4,$5);
		$$->applyTransform($6);
	};
plane: PLANE LCURL vector_expr vector_expr material transforms RCURL
	{
		$$ = new Plane(*$3,*$4,$5);
		$$->applyTransform($6);
	};
box: BOX LCURL vector_expr vector_expr material transforms RCURL
	{
		$$ = new Box(*$3,*$4,$5);
		$$->applyTransform($6);
	};
disc: DISC LCURL vector_expr vector_expr number_expr material transforms RCURL
	{
		$$ = new Disc(*$3,*$4,$5,$6);
		$$->applyTransform($7);
	};
cylinder: CYLINDER LCURL vector_expr vector_expr number_expr material transforms RCURL
	{
		$$ = new Cylinder(*$3,*$4,$5,$6);
		$$->applyTransform($7);
	};
triangle: TRIANGLE LCURL vector_expr vector_expr vector_expr material transforms RCURL
	{
		$$ = new Triangle(*$3,*$4,*$5,$6);
		$$->applyTransform($7);
	}
	| TRIANGLE LCURL vector_expr vector_expr number_expr number_expr vector_expr vector_expr number_expr number_expr vector_expr vector_expr number_expr number_expr material transforms RCURL
	{
		float u[3] = {$5,$9,$13};
		float v[3] = {$6,$10,$14};
		$$ = new MeshTriangle(*$3,*$7,*$11,*$4,*$8,*$12,u,v,$15);
		$$->applyTransform($16);
	}
	;
mesh: MESH LCURL filename material transforms RCURL
	{
		$$ = new Mesh($3->c_str(),$4);
		delete($3);
		$$->applyTransform($5);
	}
	| MESH LCURL KDTREE_LEAFSIZE expr filename material transforms RCURL
	{
		KDTreeObject::MINOBJS = (int)$4;
		$$ = new Mesh($5->c_str(),$6);
		delete($5);
		$$->applyTransform($7);
	};
filename: STRING
kdtree: KDTREE LCURL objects transforms RCURL
	{
		$$ = new KDTreeObject(*$3);
		delete $3;
		$$->applyTransform($4);
	}
	| KDTREE LCURL KDTREE_LEAFSIZE expr objects transforms RCURL
	{
		KDTreeObject::MINOBJS = (int)$4;
		$$ = new KDTreeObject(*$5);
		delete $5;
		$$->applyTransform($6);
	};
group: GROUP LCURL objects transforms RCURL
	{
		$$ = new ListObject(*$3);
		delete $3;
		$$->applyTransform($4);
	};
objects: objects object { $1->push_back($2); }
	| object
	{
		$$ = new vector<Object3d*>();
		$$->push_back($1);
	};
transforms: /* empty */ { $$ = NULL; }
	| transforms transform
	{
		$$ = new Transform((*$2) * (*$1));
		toDelete.push($$);
	};
	| transform
transform: 
	TRANSLATE vector_expr 
	{
		$$ = new Transform(Transform::Translate($2->x,$2->y,$2->z));
		toDelete.push($$);
	}
	| ROTATE vector_expr
	{
		$$ = new Transform(Transform::RotateX($2->x)*Transform::RotateY($2->y)*Transform::RotateZ($2->z));
		toDelete.push($$);
	}
	| SCALE vector_expr
	{
		$$ = new Transform(Transform::Scale($2->x,$2->y,$2->z));
		toDelete.push($$);
	};
light: pointlight | trianglelight
	| LIGHT_TYPE IDENTIFIER
	{
		if(!getLight(&$$,*$2)) YYABORT;
	};
pointlight: POINTLIGHT LCURL vector_expr spectrum_expr RCURL
	{
		$$ = new PointLight(*$3,*$4);
	};
trianglelight: TRIANGLELIGHT LCURL vector_expr vector_expr vector_expr spectrum_expr RCURL
	{
		$$ = new TriangleLight(*$3,*$4,*$5,*$6);
	};
integervar: INTEGER_TYPE IDENTIFIER EQ expr
	{
		if(exists(*$2)) YYABORT;
		variables[*$2] = new Variable($4);
		delete $2;
	};
floatvar: FLOAT_TYPE IDENTIFIER EQ number_expr
	{
		if(exists(*$2)) YYABORT;
		variables[*$2] = new Variable($4);
		delete $2;
	};
vectorvar: VECTOR_TYPE IDENTIFIER EQ vector_expr
	{
		if(exists(*$2)) YYABORT;
		printf("\nVector %s = ", $2->c_str());
		$4->print();
		variables[*$2] = new Variable($4);
		delete $2;
	};
spectrumvar: SPECTRUM_TYPE IDENTIFIER EQ spectrum_expr
	{
		if(exists(*$2)) YYABORT;
		printf("\nSpectrum %s = ", $2->c_str());
		$4->print();
		variables[*$2] = new Variable($4);
		delete $2;
	};
materialvar: MATERIAL_TYPE IDENTIFIER EQ material
	{
		if(exists(*$2)) YYABORT;
		variables[*$2] = new Variable($4);
		delete $2;
	};
objectvar: OBJECT_TYPE IDENTIFIER EQ object
	{
		if(exists(*$2)) YYABORT;
		variables[*$2] = new Variable($4);
		delete $2;
	};
lightvar: LIGHT_TYPE IDENTIFIER EQ light
	{
		if(exists(*$2)) YYABORT;
		variables[*$2] = new Variable($4);
		delete $2;
	};
cameravar: CAMERA_TYPE IDENTIFIER EQ camera
	{
		if(exists(*$2)) YYABORT;
		variables[*$2] = new Variable($4);
		delete $2;
	};
%%
/*--------------------------------------------------------*/
/* Additional C code */
/* Error processor for yyparse */
#include <stdio.h>
int yyerror(char *s)        /* called by yyparse on error */
{
    printf("\nERROR: %s (%d)\n", s, line_no);
    return(0);
}
void clear_stack()
{
	while(!toDelete.empty())
	{
		delete toDelete.top();
		toDelete.pop();
	}
}
bool exists(string str)
{
	bool b = (variables.find(str) != variables.end());
	if(b)
	{
		sprintf_s(buf,256,"Variable %s was already declared",str.c_str());
		yyerror(buf);
	}
	return b;
}
bool getInteger(int *i, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getInteger(i))
	{
		sprintf_s(buf,256,"%s is not of Integer type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}
bool getFloat(float *f, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getFloat(f))
	{
		sprintf_s(buf,256,"%s is not of Float type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}
bool getVector(Vector **vec, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getVector(vec))
	{
		sprintf_s(buf,256,"%s is not of Vector type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}
bool getSpectrum(Spectrum **s, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getSpectrum(s))
	{
		sprintf_s(buf,256,"%s is not of Spectrum type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}
bool getMaterial(Material **m, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getMaterial(m))
	{
		sprintf_s(buf,256,"%s is not of Material type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}
bool getObject(Object3d **o, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getObject(o))
	{
		sprintf_s(buf,256,"%s is not of Object type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}
bool getCamera(Camera **c, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getCamera(c))
	{
		sprintf_s(buf,256,"%s is not of Camera type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}
bool getLight(Light **l, string str)
{
	Variable *v = getVariable(str);
	if(!v) return false;
	if(!v->getLight(l))
	{
		sprintf_s(buf,256,"%s is not of Light type",str.c_str());
		yyerror(buf);
		return false;
	}
	return true;
}

Variable* getVariable(string str)
{
	map<string,Variable*>::iterator it = variables.find(str);	
	if(it == variables.end())
	{
		sprintf_s(buf,256,"%s not declared",str.c_str());
		yyerror(buf);
		return NULL;
	}
	return it->second;
}