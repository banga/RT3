#include "Defines.h"
#include "Object3d.h"
#include <string>
#include "Material.h"
#include "ConstantTexture.h"

using namespace std;

const string whitespace = " \n\r\t";
void split(const string& str, vector<string>& words, const string delimiters = whitespace, bool skip = true)
{
	words.clear();

    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        words.push_back(str.substr(lastPos, pos - lastPos));
		if(skip)
		{
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
		}
		else
		{
			if(string::npos == pos) break;
			lastPos = pos+1;
		}
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

class OBJParser
{
private:
	ifstream is, mis;
	vector<Vector> vertices;
	vector<Vector> normals;
	vector<Object3d*> objects;
	vector<float> tu, tv;
	map<string,Material*> materials;
	Material *currentMaterial;
	char buf[256];

	void ParseVertex()
	{
		float x, y, z;
		is >> x >> y >> z;
		vertices.push_back(Vector(x,y,z));
	}
	void ParseNormal()
	{
		float x, y, z;
		is >> x >> y >> z;
		normals.push_back(Vector(x,y,z));
	}
	void ParseUV()
	{
		float u, v;
		is >> u >> v;
		tu.push_back(u);
		tv.push_back(v);
	}
	int ParseFaceCoords(string s, int v[3])
	{
		vector<string> words;
		split(s,words,"/",false);
		v[0] = v[1] = v[2] = -1;
		for(string::size_type i = 0; i < words.size(); i++)
		{
			s = words[i];
			if(s.length() > 0)
				v[i] = atoi(s.c_str()) - 1;
		}
		return words.size();
	}
	void addPolygon(vector<Vector> vs, vector<float> _u, vector<float> _v, vector<Vector> vns)
	{
		bool hasNormals = vns.size() > 0, hasTextureCoords = _u.size() > 0;
		int n = vs.size();
		Vector a = vs[0], b = vs[1], c;
		float uv_u[3], uv_v[3];
		if(hasTextureCoords)
		{
			uv_u[0] = _u[0]; uv_v[0] = _v[0];
			uv_u[1] = _u[1]; uv_v[1] = _v[1];
		}
		Vector na, nb, nc;
		if(hasNormals)
		{
			na = vns[0];
			nb = vns[1];
		}
		for(int i = 2; i < n; i++)
		{
			c = vs[i];
			if(hasTextureCoords)
			{
				uv_u[2] = _u[i]; uv_v[2] = _v[i];
				if(hasNormals)
				{
					nc = vns[i];
					objects.push_back(new MeshTriangle(a, b, c, na, nb, nc, uv_u, uv_v, currentMaterial));
					nb = nc;
				}
				else
				{
					objects.push_back(new MeshTriangle(a, b, c, uv_u, uv_v, currentMaterial));
				}
				uv_u[1] = uv_u[2]; uv_v[1] = uv_v[2];
			}
			else if(hasNormals)
			{
				nc = vns[i];
				objects.push_back(new MeshTriangle(a, b, c, na, nb, nc, currentMaterial));
				nb = nc;
			}
			else
			{
				objects.push_back(new Triangle(a, b, c, currentMaterial));
			}
			b = c;
		}
	}
	void ParseFace()
	{
		is.getline(buf,256);
		vector<string> words;
		split(string(buf),words);

		vector<Vector> vs, vns;
		vector<float> _u, _v;
		for(vector<string>::iterator it = words.begin(); it < words.end(); it++)
		{
			int v[3];
			int i = ParseFaceCoords(*it,v);
			if(v[0] >= 0) vs.push_back(vertices[v[0]]);

			if(v[1] >= 0)
			{
				_u.push_back(tu[v[1]]);
				_v.push_back(tv[v[1]]);
			}

			if(v[2] >= 0) vns.push_back(normals[v[2]]);
		}
		addPolygon(vs,_u,_v,vns);
	}
	Spectrum ParseSpectrum()
	{
		float f[3];
		mis >> f[0] >> f[1] >> f[2];
		return Spectrum(f);
	}
	void ParseMaterial()
	{
		string name, word;
		mis >> name;
		Spectrum Ka, Kd, Ks, Tf(1.f);
		float Ns = 0.f, Ni = 1.f, d = 1.f, Tr = 1.f, sharpness = 60.f;
		int illum = 1;
		while(mis.good())
		{
			mis >> word;
			if(word.compare("newmtl") == 0)
			{
				mis.seekg(-6,ios_base::cur);
				break;
			}
			else if(word.compare("Ka") == 0)
			{
				Ka = ParseSpectrum();
			}
			else if(word.compare("Kd") == 0)
			{
				Kd = ParseSpectrum();
			}
			else if(word.compare("Ks") == 0)
			{
				Ks = ParseSpectrum();
			}
			else if(word.compare("Ns") == 0)
			{
				mis >> Ns;
			}
			else if(word.compare("Ni") == 0)
			{
				mis >> Ni;
			}
			else if(word.compare("d") == 0)
			{
				mis >> d;
			}
			else if(word.compare("Tr") == 0)
			{
				mis >> Tr;
			}
			else if(word.compare("sharpness") == 0)
			{
				mis >> Tr;
			}
			else if(word.compare("illum") == 0)
			{
				mis >> illum;
			}
			else if(word.compare("Tf") == 0)
			{
				Tf = ParseSpectrum();
			}
			else
			{
				mis.getline(buf,256);
			}
		}
		if(materials.find(name) == materials.end())
		{
			switch(illum)
			{
				case 0:
					materials[name] = new Material(new ConstantTexture<Spectrum>(Spectrum(0.)),new ConstantTexture<Spectrum>(Ka)); break;
				case 1:
					materials[name] = new Material(new ConstantTexture<Spectrum>(Kd),new ConstantTexture<Spectrum>(Ka)); break;
				case 2:
					// TODO: Phong
					materials[name] = new Material(new ConstantTexture<Spectrum>(Kd),new ConstantTexture<Spectrum>(Ka),new ConstantTexture<Spectrum>(Ks),new ConstantTexture<Spectrum>(Tf),Ni);
				default:
					materials[name] = new Material(new ConstantTexture<Spectrum>(Kd),new ConstantTexture<Spectrum>(Ka),new ConstantTexture<Spectrum>(Ks),new ConstantTexture<Spectrum>(Tf),Ni);
			}
		}
	}
	bool ParseMaterialLibrary(const string& fname)
	{
		mis.open(fname);
		if(!mis.good())
			return false;

		string word;
		while(mis.good())
		{
			mis >> word;
			if(word.compare("newmtl") == 0)
			{
				ParseMaterial();
			}
			else
			{
				mis.getline(buf,256);
			}
		}
		mis.close();
		return true;
	}
public:
	OBJParser()
	{
		currentMaterial = NULL;
	};
	bool ParseOBJ(const string& fname, Material *defaultMaterial)
	{
		currentMaterial = defaultMaterial;
		
		is.open(fname);
		if(!is.good()) return false;

		string basedir = fname.substr(0,fname.find_last_of("\\/")+1);
		string word;
		while(is.good())
		{
			is >> word;
			if(!is.good())
				break;
			if(word.compare("v") == 0)
			{
				ParseVertex();
			}
			else if(word.compare("vn") == 0)
			{
				ParseNormal();
			} 
			else if(word.compare("vt") == 0)
			{
				ParseUV();
			}
			else if(word.compare("f") == 0)
			{
				ParseFace();
			}
			else if(word.compare("mtllib") == 0)
			{
				is.getline(buf,256);
				vector<string> words;
				split(buf, words);
				for(vector<string>::iterator it = words.begin(); it < words.end(); it++)
				{
					word = *it;
					if(!ParseMaterialLibrary(basedir+word))
					{
						printf("\nERROR parsing material library %s. Using default material.", word.c_str());
					}
				}
			}
			else if(word.compare("usemtl") == 0)
			{
				is >> word;
				map<string,Material*>::iterator itmap = materials.find(word);
				if(itmap == materials.end())
				{
					currentMaterial = defaultMaterial;
				}
				else
				{
					currentMaterial = itmap->second;
				}
			}
			else
			{
				is.getline(buf,256);
			}
		}
		is.close();
		return true;
	}
	vector<Object3d*> getObjects()
	{
		return objects;
	}
};