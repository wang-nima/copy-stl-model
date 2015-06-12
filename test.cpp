#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>      // std::ifstream
#include <cfloat>
#include <cstring>
using namespace std;

const float gap = 5;

class v3
{
public:
	v3() {
		x = y = z = 0;
	}
    v3(char* bin);
    float x, y, z;
	v3 const operator + (const v3 &new_vector) const {
		v3 ret;
		ret.x = x + new_vector.x;
		ret.y = y + new_vector.y;
		ret.z = z + new_vector.z;
		return ret;
	}
};

v3::v3(char* facet)
{
    char f1[4] = {facet[0],facet[1],facet[2],facet[3]};
    char f2[4] = {facet[4],facet[5],facet[6],facet[7]};
    char f3[4] = {facet[8],facet[9],facet[10],facet[11]};
 
    x = *((float*) f1 );
    y = *((float*) f2 );
    z = *((float*) f3 );
}


class tri
{
public:
	tri() {}
	tri(v3 normal, v3 p1, v3 p2, v3 p3) {
		this->normal = normal;
		this->p1 = p1;
		this->p2 = p2;
		this->p3 = p3;
	}
	tri const operator + (const v3 &translation_vector) const {
		tri ret;
		ret.normal = normal;
		ret.p1 = p1 + translation_vector;
		ret.p2 = p2 + translation_vector;
		ret.p3 = p3 + translation_vector;
		return ret;
	}
    v3 normal, p1, p2, p3;
};


void read_stl(const string &fname, vector<tri> &v, float &min_x, float &max_x, float &min_y, float &max_y){
 
    //!!
    //don't forget ios::binary
    //!!
    ifstream myFile (fname.c_str(), ios::in | ios::binary);
 
    char header_info[80] = "";
    char nTri[4];
    unsigned int nTriLong;
 
    //read 80 byte header
    if (myFile) {
        myFile.read (header_info, 80);
        cout <<"header: " << header_info << endl;
    }
    else{
        cout << "error" << endl;
    }
 
    //read 4-byte ulong
    if (myFile) {
        myFile.read (nTri, 4);
        nTriLong = *((unsigned int*)nTri) ;
        cout <<"n Tri: " << nTriLong << endl;
		v.reserve(nTriLong);
    }
    else{
        cout << "error" << endl;
    }
 
    //now read in all the triangles
    for(int i = 0; i < nTriLong; i++){
 
        char facet[50];
 
        if (myFile) {
 
        	// read one 50-byte triangle
            myFile.read (facet, 50);
 
			v3 normal(facet);
            v3 p1(facet+12);
            v3 p2(facet+24);
            v3 p3(facet+36);

			float x1 = p1.x;
			float x2 = p2.x;
			float x3 = p3.x;
			float y1 = p1.y;
			float y2 = p2.y;
			float y3 = p3.y;

			min_x = min(min_x, min(min(x1, x2), x3));
			min_y = min(min_y, min(min(y1, y2), y3));
			max_x = max(max_x, max(max(x1, x2), x3));
			max_y = max(max_y, max(max(y1, y2), y3));

            // add a new triangle to the array
            v.push_back( tri(normal,p1,p2,p3) );
        }
    }
    return;
}

vector<tri> make_copies(const vector<tri> &v, int copies, float bounding_box_length, float bounding_box_width) {
	vector<tri> ret(v.size() * copies);
	for (int i = 0; i < copies; i++) {

		v3 translation_vector;
		translation_vector.x = i * (bounding_box_length + gap);
		translation_vector.y = 0;
		translation_vector.z = 0;

		for (int j = 0; j < v.size(); j++) {
			ret.push_back(v[j] + translation_vector);
		}
	}
	return ret;
}

void save_stl_file(const vector<tri> &v) {
	ofstream f("out.stl", ios::out | ios::binary);

	char header[80];
	memset(header, 0, sizeof(header));
	f.write(header, 80);
	int size = v.size();
	f.write((char*)&size, sizeof(int));

	char empty[2];
	memset(empty, 0, sizeof(empty));

	// 50 byte for one surface
	for (int i = 0; i < v.size(); i++) {
		tri temp = v[i];
		f.write((char*)&temp, sizeof(tri));
		f.write(empty, sizeof(empty));
	}

}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage: %s input_stl_file\n", argv[0]);
		return 0;
	}
	string file_name(argv[1]);
	vector<tri> v;
	float min_x = FLT_MAX;
	float max_x = FLT_MIN;
	float min_y = FLT_MAX;
	float max_y = FLT_MIN;
	read_stl(file_name, v, min_x, max_x, min_y, max_y);
	cout << "min x: " << min_x << "\nmax x: " << max_x << "\nmin y: " << min_y << "\nmax y: " << max_y  << endl;
	float bounding_box_length = max_x - min_x;
	float bounding_box_width = max_y - min_y;
	cout << "bounding box length = " << bounding_box_length << "\nbounding box width = " << bounding_box_width << endl;
	v = make_copies(v, 5, bounding_box_length, bounding_box_width);
	save_stl_file(v);
	return 0;
}
