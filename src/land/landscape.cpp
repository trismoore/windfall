#include "landscape.hpp"
#include "console.hpp"
#include "config.hpp"
#include "shader.hpp"
#include "vbo.hpp"
#include "useful.h"
#include "camera.hpp"
#include "texture.hpp"
#include "qtree.hpp"
#include "landpatch.hpp"
#include "awesome.hpp"

extern Camera* g_camera;
extern double gdT;

Landscape::Landscape(Config* config, Awesome* awesome)
	: config(config), awesome(awesome)
{

}

float Landscape::load()
{
	static int C=0,R=0,cC=0,cR=0,mult=0;
	static int numPatchesX=0, numPatchesZ=0;

//	sleep(1);
	static int loadState=-1;
	++loadState;
	switch(loadState) {
		case 0:
			console.log("Landscape loading").indent();
			cameraDragging = false;
			awesome->runJS("window.landscape = {};");
			return 0.01;
		case 1:
			LandPatch::vbo = new VBO();

			LandPatch::numberOfLODLevels = config->getInt("land.numberOfLODLevels", 4);
			cC=config->getInt("land.columns",212), cR=config->getInt("land.rows",243);
			C=cC,R=cR;

			if (LandPatch::numberOfLODLevels < 1) throw "Incorrect number of LOD levels! (must be >= 1)";
			if (LandPatch::numberOfLODLevels > 10) console.warningf("land.lodlevels set quite high (%d) - takes a lot of memory!",LandPatch::numberOfLODLevels);

			mult=(1<<(LandPatch::numberOfLODLevels-1));
			while ( (C-4) % mult != 0 || C-4<=0 ) ++C;
			while ( ((R-3)/2) % mult != 0 || (R-3)/2<=0) ++R;
			if (C!=cC) console.logf("Changed land.columns %d -> %d: (C-4) must be multiple of %d", cC, C, mult);
			if (R!=cR) console.logf("Changed land.rows %d -> %d: (R-3)/2 must be multiple of %d", cR, R, mult);

			awesome->runJSf("window.landscape['columns']=%d;", C);
			awesome->runJSf("window.landscape['rows']=%d;", R);
			return 0.03;
		case 2: {
			console.log("Making hexGrid for landscape").indent();
			console.logf("Creating grid: %dx%d = %d vertices, %d triangles",R,C,R*C,(2*C-7)*(R-3));
			awesome->runJSf("window.landscape['triangles_per_patch']=%d;", (2*C-7)*(R-3));

			// we only need XZ, height (Y) is read from the texture
			float *gridXY=0;
			float *g=0;
			gridXY = new float[R*C*2];
			g = gridXY;
			for (int iy=0; iy<R; ++iy) {
				for (int ix=0; ix<C; ++ix) {
					//      *g++ = clamp( ((float(ix)+0.5f*(iy%2))/float(C-1)), 0,1); // X offset if Y is odd
					//      *g++ = float(iy)/float(R-1);
					// X offset if Y is odd
					*g++ = clamp(((float(ix)+0.5f*(iy%2)-1.5)/float(C-4)), 0,1);
					*g++ = float(iy-1)/float(R-3);
				}
			}
			LandPatch::vbo->pushData("vertex", 2, R*C, gridXY);
			delete[] gridXY;

			return 0.10;
		}
		case 3: {
			// now index into the grid for triangles
#define addIndex(_c,_r) \
			{ \
				index[i] = _c + (_r) * C; \
				if (index[i]>mx) mx=index[i]; \
				if (index[i]<mn) mn=index[i]; \
				if (index[i]>=R*C) console.errorf("Index Out Of Range: (%d+%d*%d) %d >= %d on i=%d lod=%d", _c,_r,C,index[i],R*C,i,lambda); \
				++i; \
			}
			//console.logf("%d: %d,%d = %d",i,_c,_r,index[i]); \

#define addIndex2(_c,_r) \
			{ addIndex(_c,_r); addIndex(_c,_r); }

			LandPatch::indicesLocations.resize(LandPatch::numberOfLODLevels+1);
			int offset = 0;
			int mn=999999,mx=0;

			console.logf("Creating %d LOD levels (0-%d)",LandPatch::numberOfLODLevels,LandPatch::numberOfLODLevels-1);
			awesome->runJSf("window.landscape['numberOfLODLevels']=%d;", LandPatch::numberOfLODLevels);

			for (int lambda = 0; lambda < LandPatch::numberOfLODLevels; ++lambda) {
				console.logf("LOD %d", lambda).indent();

				// STITCH LIST (not needed for LOD 0)
				if (lambda >= 1) {
					std::vector<GLuint> index;
					int i=0;
					//      int i = stitchList.size ();
					int numAdded = (3 * (R + C - 7)) / pow(2, lambda - 1);
					//      int N = index.size() + numAdded;
					//      stitchNumbersList[lambda] = N;
					index.resize(numAdded);
					int shift = (lambda == 1) ? 1 : 0;
					int halfstride = pow(2, lambda - 1);
					int stride = pow(2, lambda);

					for (int r = 1; r < R - 2; r += stride) {
						//West
						addIndex(1, r);
						addIndex(1, r + halfstride );
						addIndex(1, r + stride);
						//East
						addIndex(C - 3, r);
						addIndex(C - 3, r + stride);
						addIndex(C - 3 + shift, r + halfstride);
					}

					for (int c = 1; c < C - 3; c += stride) {
						//North
						addIndex(c, 1);
						addIndex(c + stride, 1);
						addIndex(c + halfstride, 1);
						//South
						addIndex(c, R - 2);
						addIndex(c + halfstride, R - 2);
						addIndex(c + stride, R - 2);
					}
					console.logf("%d indices in stitch",numAdded);
					LandPatch::vbo->pushIndices(index.size(), &index[0]);
					LandPatch::indicesLocations[lambda].stitchOffset = offset;
					LandPatch::indicesLocations[lambda].stitchCount = index.size();
					offset+=index.size() * sizeof(float);
				} else {
					// lod 0 gets no stitching
					LandPatch::indicesLocations[lambda].stitchOffset = offset;
					LandPatch::indicesLocations[lambda].stitchCount = 0;
				}
				// STRIP (all LODs)
				{
					int stride = pow(2, lambda);
					int shift = stride / 2;
					std::vector<GLuint> index;//& = stripArray[lambda];
					int M_strip = ((R - 3) / (stride * 2)) *
						((C - 4) * 4 / stride + 10) - 3;
					if (M_strip < 0) {
						console.error("There aren't enough vertices to do LOD on this patch!");
						console.errorf("index resize %d = ((%d-3)/(%d*2)) * ((%d-4)*4/%d+10) - 3", M_strip,R,stride,C,stride);
					}
					index.resize (M_strip);
					int colEnd = (lambda == 0) ? C - 2 : C - 3;
					int r = 1, i = 0;

					while (r <= R - 4) {
						// EVEN rows
						addIndex(colEnd, r + stride);
						for (int c = C - 3; c >= 1; c -= stride) {
							addIndex(c, r);
							if (c == 1) { addIndex(c, r + stride); }
							else        { addIndex(c - shift, r + stride ); }
						}
						// Degenerate triangles
						r += stride;
						addIndex2(1, r);
						// ODD rows
						for (int c = 1; c <= C - 3; c += stride) {
							addIndex(c, r + stride );
							if (lambda == 0)     { addIndex(c + 1, r); }
							else if (c == C - 3) { addIndex(c, r); }
							else                 { addIndex(c + shift, r ); }
						}
						// Degenerate triangles
						r += stride;
						if (r <= R - 4) {
							addIndex(colEnd, r - stride);
							addIndex2(colEnd, r + stride);
						}
					}
					console.logf("%d indices in strip", index.size());
					LandPatch::vbo->pushIndices(index.size(), &index[0]);
					LandPatch::indicesLocations[lambda].stripOffset = offset;
					LandPatch::indicesLocations[lambda].stripCount = index.size();
					offset+=index.size() * sizeof(float);
				}
				console.outdent();
			} // lambda

			console.logf("Min max of indices: %d %d (should be within 0 %d)",mn,mx,R*C);

			console.logf("indices:");
			for (int i=0; i<LandPatch::numberOfLODLevels; ++i) {
				console.logf("%d stitch %6d+%6d*%d<%6d strip %6d+%6d*%d<%6d",i,
						LandPatch::indicesLocations[i].stitchOffset,
						LandPatch::indicesLocations[i].stitchCount,sizeof(float),
						LandPatch::indicesLocations[i].stitchOffset +
						LandPatch::indicesLocations[i].stitchCount*sizeof(float),
						LandPatch::indicesLocations[i].stripOffset,
						LandPatch::indicesLocations[i].stripCount,sizeof(float),
						LandPatch::indicesLocations[i].stripOffset +
						LandPatch::indicesLocations[i].stripCount*sizeof(float)  );
			}

			return 0.3;
		}
		case 4:
			//  LandPatch::vbo->PushIndices(indices.size(), &indices[0]);
			LandPatch::vbo->create();

			console.outdent(); // hex grid
			return 0.4;
		case 5: {
			numPatchesX = config->getInt("land.numPatchesX", 4);
			numPatchesZ = config->getInt("land.numPatchesZ", 4);

			if (numPatchesX <= 0) { console.errorf("numPatchesX MUST be > 0"); throw "patch error"; }
			if (numPatchesZ <= 0) { console.errorf("numPatchesZ MUST be > 0"); throw "patch error"; }

			LandPatch::shader = new Shader("landpatch.shader");
			LandPatch::textureScaleX=1.f/numPatchesX;
			LandPatch::textureScaleZ=1.f/numPatchesZ;
			LandPatch::shader->set2f("textureScale", 1.f/numPatchesX, 1.f/numPatchesZ);
			float heightScale = config->getFloat("land.heightScale",1.f);
			LandPatch::heightScale = heightScale;
			LandPatch::shader->setf("heightScale", heightScale);

			awesome->runJSf("window.landscape['numPatchesX']=%d;", numPatchesX);
			awesome->runJSf("window.landscape['numPatchesZ']=%d;", numPatchesZ);
			awesome->runJSf("window.landscape['heightScale']=%f;", heightScale);
			return 0.50;
		}
		case 6: {
			console.log("Loading textures").indent();
			try {
			std::string heightmap(config->getString("land.heightmap","land/land.tga"));
			LandPatch::texHeights = new Texture(heightmap.c_str(), true);
			} catch (const char *s) {
				console.errorf("Can't load texture for heightmap: %s", s);
				throw s;
			}
			LandPatch::texHeights->bind(2);
			LandPatch::texHeights->wrapClamp();
			LandPatch::texHeights->filterLinear();
			LandPatch::shader->seti("heightsSampler",2);
			logOpenGLErrors();
			return 0.6;
		}
		case 7: {
			std::string groundtexture(config->getString("land.groundtexture","land/tex.dds"));
			LandPatch::texGround = new Texture(groundtexture.c_str());
			LandPatch::texGround->bind(3);
			LandPatch::texGround->wrapClamp();
			LandPatch::texGround->filterMipmap();
			LandPatch::shader->seti("groundSampler",3);
			return 0.70;
		}
		case 8: {
			std::string detailtexture(config->getString("land.detailtexture","land/detail.dds"));
			LandPatch::texGroundDetail = new Texture(detailtexture.c_str());
			LandPatch::texGroundDetail->bind(4);
			LandPatch::texGroundDetail->wrapRepeat();
			LandPatch::texGroundDetail->filterMipmap();
			LandPatch::shader->seti("groundDetail",4);
			return 0.80;
		}
		case 9:
			logOpenGLErrors();
			console.outdent(); // textures
			return 0.85;
		case 10:
			console.log("Creating QTree").indent();

			qtreeRoot = new QTree(0,0,numPatchesX,numPatchesZ, 1,0);

			console.logf("Made landscape patches %dx%d (so scale is %f,%f)", numPatchesX, numPatchesZ, 1.f/numPatchesX, 1.f/numPatchesZ);

			console.outdent();
			return 0.95;
		case 11:
			console.log("Landscape finished");
			console.outdent(); // land
			return 1;
		default:
			console.error("Landscape loading too much!");
			return 1;
	}
}

Landscape::~Landscape()
{
	console.log("Landscape closing");
	delete qtreeRoot;
	delete LandPatch::vbo;
	delete LandPatch::texHeights;
	delete LandPatch::texGround;
	delete LandPatch::texGroundDetail;
	delete LandPatch::shader;
}

void Landscape::onMouseButton(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
		cameraDragging = action == GLFW_PRESS;
}

void Landscape::onMouseMove(int x, int y)
{
	double cameraSens = 0.5;
	if (cameraDragging) {
		g_camera->yaw(cameraSens * (cameraMouseX - x));
		g_camera->pitch(cameraSens * (cameraMouseY - y));
	}
	cameraMouseX=x;
	cameraMouseY=y;
}

void Landscape::render()
{
	// move camera around: WASD (**FIXME** probably won't be good on other key layouts)
	double sens = 0.25 * max(0.1f,g_camera->pos.y); // speed depends on height
	if (glfwGetKey('W')) g_camera->forward(gdT * sens);
	if (glfwGetKey('S')) g_camera->backward(gdT * sens);
	if (glfwGetKey('A')) g_camera->left(gdT * sens);
	if (glfwGetKey('D')) g_camera->right(gdT * sens);
	if (glfwGetKey('Q')) g_camera->roll(gdT * -45);
	if (glfwGetKey('E')) g_camera->roll(gdT * +45);

	static int l=0; // LOD Level
	if (glfwGetKey('1')) l=0;
	if (glfwGetKey('2')) l=1;
	if (glfwGetKey('3')) l=2;
	if (glfwGetKey('4')) l=3;
	if (glfwGetKey('5')) l=4;
	if (glfwGetKey('6')) l=5;
	if (glfwGetKey('7')) l=6;
	if (glfwGetKey('8')) l=7;
	if (glfwGetKey('9')) l=8;
	if (glfwGetKey('0')) l=9;
	if (l>=LandPatch::numberOfLODLevels) l=LandPatch::numberOfLODLevels-1;

	static bool wireframe = false;
	if (glfwGetKey('O')) wireframe=true;
	if (glfwGetKey('P')) wireframe=false;

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}

/*
	LandPatch::vbo->drawArrays(GL_POINTS);

	texHeights->bind(2);
	texGround->bind(3);
	texGroundDetail->bind(4);
	shader->setCamera(g_camera);
	LandPatch::vbo->bind(shader);
	logOpenGLErrors();

	vbo->drawElements(GL_TRIANGLES,
                          indicesLocations[l].stitchCount,
                          indicesLocations[l].stitchOffset);

	vbo->drawElements(GL_TRIANGLE_STRIP,
                          indicesLocations[l].stripCount,
                          indicesLocations[l].stripOffset);
*/

	qtreeRoot->calculateLOD(g_camera);

	static int qtreeDebugRender = -1;
	if (qtreeDebugRender < 0) qtreeDebugRender = config->getInt("qtreeDebug", 0);
	else if (qtreeDebugRender == 1) qtreeRoot->debugRender();

	LandPatch::texHeights->bind();
	LandPatch::texGround->bind();
	LandPatch::texGroundDetail->bind();

	LandPatch::shader->setCamera(g_camera);
	LandPatch::vbo->bind(LandPatch::shader);
	qtreeRoot->render();

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
	logOpenGLErrors();
}
