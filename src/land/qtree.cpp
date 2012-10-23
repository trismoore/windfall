#include "qtree.hpp"
#include "console.hpp"

#include "vbo.hpp"
#include "shader.hpp"
#include "camera.hpp"

#include "landpatch.hpp"

extern Camera* g_camera;

int nextPowerOfTwo(int n)
{
	int p = 0;
	while ((1<<p) < n) ++p;
	return (1<<p);
}

const float QTree::drawColours[10][3] = { 1,1,1, 1,0,0, 0,1,0, 0,0,1, 1,1,0, 0,1,1, 1,0,1, 0.5,1,0.3, 0.7,0.1,0.9, 0.9,0.6,0.7 };
const float QTree::vertexPositions[8*3] = { 0,0,0, 1,0,0, 0,0,1, 1,0,1, 0,1,0, 1,1,0, 0,1,1, 1,1,1 };
const unsigned int QTree::vertexIndices[8*2] = { 0,4, 1,5, 2,6, 3,7, 4,5, 4,6, 5,7, 6,7 };
VBO* QTree::vbo =0;
Shader* QTree::shader =0;

int QTree::qTreeNumberTotal=0, QTree::qTreeNumberVisible=0;

QTree::QTree(int startX, int startZ, int endX, int endZ, int maxSize, int level) 
	: level(level), startX(startX), startZ(startZ), endX(endX), endZ(endZ)
{
	++qTreeNumberTotal;

	if (!vbo) {
		vbo=new VBO();
		shader=new Shader("qtree.shader");
		vbo->pushData("vertex", 3, 8, vertexPositions);
		vbo->pushIndices(8*2, vertexIndices);
		vbo->create();
	}

//	console.logf("%d QTree(%d,%d - %d,%d max %d)",level, startX,startZ,endX,endZ,maxSize).indent();
	//lod=-1;
	visible = true;

	sizeX = endX-startX, sizeZ = endZ-startZ;

	int midX = startX + (sizeX+1) / 2; // round up
	int midZ = startZ + (sizeZ+1) / 2;

	// it is possible to just split in half, but it's probably more efficient to get to a square as soon as possible
	midX = startX + nextPowerOfTwo(midX - startX);
	midZ = startZ + nextPowerOfTwo(midZ - startZ);

	bool splitHoriz=false, splitVert=false;
	
	if (endX - startX > maxSize) splitHoriz=true;
	if (endZ - startZ > maxSize) splitVert =true;

	// 4 cases: no split (we are leaf), split horiz, split vert, split both
	for (int i=0; i<4; ++i) children[i]=0;
	landPatch=0;

	if (splitHoriz && splitVert) {
		children[0] = new QTree(startX, startZ, midX, midZ, maxSize, level+1); // NW
		children[1] = new QTree(  midX, startZ, endX, midZ, maxSize, level+1); // NE
		children[2] = new QTree(startX,   midZ, midX, endZ, maxSize, level+1); // SW
		children[3] = new QTree(  midX,   midZ, endX, endZ, maxSize, level+1); // SE
		boundingBoxMin = children[0]->boundingBoxMin;
		boundingBoxMax = children[0]->boundingBoxMax;
		for (int i=1; i<4; ++i) {
			if (children[i]->boundingBoxMin.x < boundingBoxMin.x) boundingBoxMin.x = children[i]->boundingBoxMin.x;
			if (children[i]->boundingBoxMin.y < boundingBoxMin.y) boundingBoxMin.y = children[i]->boundingBoxMin.y;
			if (children[i]->boundingBoxMin.z < boundingBoxMin.z) boundingBoxMin.z = children[i]->boundingBoxMin.z;
			if (children[i]->boundingBoxMax.x > boundingBoxMax.x) boundingBoxMax.x = children[i]->boundingBoxMax.x;
			if (children[i]->boundingBoxMax.y > boundingBoxMax.y) boundingBoxMax.y = children[i]->boundingBoxMax.y;
			if (children[i]->boundingBoxMax.z > boundingBoxMax.z) boundingBoxMax.z = children[i]->boundingBoxMax.z;
		}			
	} else if (splitHoriz) {
		children[0] = new QTree(startX, startZ, midX, endZ, maxSize, level+1); // W
		children[1] = new QTree(  midX, startZ, endX, endZ, maxSize, level+1); // E
		boundingBoxMin = children[0]->boundingBoxMin;
		boundingBoxMax = children[0]->boundingBoxMax;
		int i=1; {
			if (children[i]->boundingBoxMin.x < boundingBoxMin.x) boundingBoxMin.x = children[i]->boundingBoxMin.x;
			if (children[i]->boundingBoxMin.y < boundingBoxMin.y) boundingBoxMin.y = children[i]->boundingBoxMin.y;
			if (children[i]->boundingBoxMin.z < boundingBoxMin.z) boundingBoxMin.z = children[i]->boundingBoxMin.z;
			if (children[i]->boundingBoxMax.x > boundingBoxMax.x) boundingBoxMax.x = children[i]->boundingBoxMax.x;
			if (children[i]->boundingBoxMax.y > boundingBoxMax.y) boundingBoxMax.y = children[i]->boundingBoxMax.y;
			if (children[i]->boundingBoxMax.z > boundingBoxMax.z) boundingBoxMax.z = children[i]->boundingBoxMax.z;
		}			
	} else if (splitVert) {
		children[0] = new QTree(startX, startZ, endX, midZ, maxSize, level+1); // N
		children[2] = new QTree(startX,   midZ, endX, endZ, maxSize, level+1); // S
		boundingBoxMin = children[0]->boundingBoxMin;
		boundingBoxMax = children[0]->boundingBoxMax;
		int i=2; {
			if (children[i]->boundingBoxMin.x < boundingBoxMin.x) boundingBoxMin.x = children[i]->boundingBoxMin.x;
			if (children[i]->boundingBoxMin.y < boundingBoxMin.y) boundingBoxMin.y = children[i]->boundingBoxMin.y;
			if (children[i]->boundingBoxMin.z < boundingBoxMin.z) boundingBoxMin.z = children[i]->boundingBoxMin.z;
			if (children[i]->boundingBoxMax.x > boundingBoxMax.x) boundingBoxMax.x = children[i]->boundingBoxMax.x;
			if (children[i]->boundingBoxMax.y > boundingBoxMax.y) boundingBoxMax.y = children[i]->boundingBoxMax.y;
			if (children[i]->boundingBoxMax.z > boundingBoxMax.z) boundingBoxMax.z = children[i]->boundingBoxMax.z;
		}			
	}
	else {
//		console.debugf("%d,%d - %d,%d, %d", startX,startZ, endX,endZ, level);
		landPatch = new LandPatch(startX, startZ, 1, 1);
		boundingBoxMin = landPatch->boundingBoxMin;
		boundingBoxMax = landPatch->boundingBoxMax;
	}

	// only 1x1 landpatches should calculate their bounding boxes, bigger qtrees should work it out from their children
	// (calculating bounding boxes is *slow*, especially on big landscape texture sizes)
	if (!landPatch) {
		landPatch = new LandPatch(startX, startZ, endX-startX, endZ-startZ);
	}

	centre = 0.5f * (boundingBoxMin + boundingBoxMax);
	radius = glm::distance(centre, boundingBoxMin);
//	console.debugf("%d,%d - %d,%d, %d centre:%.3f,%.3f,%.3f, rad %.3f", startX,startZ, endX,endZ, level, centre.x,centre.y,centre.z, radius);

//	console.outdent();
}

QTree::~QTree()
{
	for (int i=0; i<4; ++i) delete children[i];
	delete landPatch;
}

void QTree::debugRender()
{
	shader->setCamera(g_camera);
	vbo->bind(shader);

	qTreeNumberVisible=0;

	glDepthFunc(GL_ALWAYS);
	debugRenderRecurse();
	glDepthFunc(GL_LESS);

//printf("QTree::rendered %d / %d\n", qTreeNumberVisible, qTreeNumberTotal);
}

void QTree::debugRenderRecurse()
{
	if (!visible) return;

	if (lod >= 0)
	{
//printf("%d;",lod);
		++qTreeNumberVisible;

		shader->set3f("bbMin", boundingBoxMin);
		shader->set3f("bbMax", boundingBoxMax);

		shader->set3f("colour",drawColours[lod]);

		vbo->drawElements(GL_LINES, 8*2, 0);
	}
	else // we didn't get drawn, draw the children instead.
	{
		for (int i=0; i<4; ++i)
			if (children[i]) 
				children[i]->debugRenderRecurse();	
	}
}

void QTree::render()
{
	if (!visible) return;

	if (lod >= 0)
	{
		landPatch->draw(lod);
	}
	else
	{
		for (int i=0; i<4; ++i)
			if (children[i])
				children[i]->render();
	}
}

void QTree::calculateLOD(Camera* camera)
{
	calculateLODRecursive(camera);
	// merge any branches that have the same LOD.
//	mergeLODsRecursive();
}

void QTree::mergeLODsRecursive()
{
	if (!children[0]) return; // we are leaf node - our LOD is OK.

	// merge children first
	for (int i=0; i<4; ++i) if (children[i]) children[i]->mergeLODsRecursive();

	// if all children that exist have the same LOD, then merge them into us.
	lod = -1;
	int childrenLODs = children[0]->lod; // children[0] guarenteed to exist
	for (int i=1; i<4; ++i) {
		if (children[i] && children[i]->lod < childrenLODs) return;
	}
	lod = childrenLODs;
//	lod = childrenLODs-1; // merge 4 children together means our detail is increased
//	if (lod<0) lod=0;
//for (int l=0;l<level;++l) printf(" ");
//printf("%.3f,%.3f,%.3fr%.3f vis%d dist%.3f lod%d c[0]%d\n",centre.x,centre.y,centre.z,radius, visible,distance,lod,children[0]->lod);
}

void QTree::calculateLODRecursive(Camera* camera)
{
	visible = camera->isSphereVisible(centre,radius);

	// v2: start from the top and split when lod < 1
	if (visible) {
		distance = glm::distance(centre, camera->pos);
		lod = int(sqrtf(distance / radius));
		if (lod < 1) {
			if (children[0]) {
				lod = -1;
				for (int i=0; i<4; ++i)
					if (children[i])
						children[i]->calculateLODRecursive(camera);
			} else lod=0;
		}
	} else lod=-1;

	// v1: assign lod, then merge equal lods into larger patches
/*
	if (visible) { //lod>=0) {
		distance = glm::distance(centre, camera->pos);
		lod = int(sqrtf(distance));
		if (lod>=10) lod=9;
		for (int i=0; i<4; ++i)
			if (children[i])
				children[i]->calculateLODRecursive(camera);
	} else {
		lod = -1;
	}
*/
//for (int l=0;l<level;++l) printf(" ");
//printf("%.3f,%.3f,%.3fr%.3f vis%d dist%.3f r%.3f d2/r%.3f lod%d\n",centre.x,centre.y,centre.z,radius, visible,distance,radius,distance*distance/radius,lod);
}

