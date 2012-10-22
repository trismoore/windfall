#include "texture.hpp"
#include "useful.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#include <stdio.h>
#include "console.hpp"

std::map< GLuint, Texture* > Texture::bound;

Texture::Texture()
  : id(0),boundTo(0),keptData(0)
{

}

Texture::Texture(const std::string& f, bool keepData)
  : id(0),boundTo(0),keptData(0)
{
	std::string ext = substr(f,-3);
	if (ext.compare("dds") == 0) {
		if (keepData) throw "can't keep DDS data (it's compressed)";
		loadDDS(f);
	}
	else if (ext.compare("tga") == 0) {
		loadTGA(f,keepData);
	} else
		throw "can only load TGA or DDS formats.";

}

Texture::~Texture()
{
	glDeleteTextures(1,&id);
	delete keptData;
}

void Texture::bind(int unit/*=-1*/)
{
	if (unit==-1) unit=boundTo;
	if (bound[unit] == this) return;
	console.debugf("Texture::bind %d to %d",id,unit);
	boundTo = unit;
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D, id);
	logOpenGLErrors();
	bound[unit]=this;
}

void Texture::wrap(GLenum wrap)
{
	//bind();
	assert(boundTo>0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

void Texture::filterNearest()
{
	//bind();
	assert(boundTo>0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Texture::filterLinear()
{
	//bind();
	assert(boundTo>0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::filterMipmap()
{
	//bind();
	assert(boundTo>0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void Texture::loadDDS(const std::string& f)
{
	filename = std::string(DATA_DIR) + f;

	// much of this from www.opengl-tutorial.org
	console.logf("Loading DDS from %s", filename.c_str()).indent();

	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(filename.c_str(), "rb");
	if (fp == NULL) {
		console.error("Can't open file").outdent();
		throw "loadDDS: Can't open file";
	}

	/* verify the type of file */
	char filecode[4];
	size_t r;
	r = fread(filecode, 1, 4, fp);
	if (4!=r) {
		console.error("Can't read magic number of file!").outdent();
		fclose(fp);
		throw "loadDDS: Can't read first 4 bytes (magic number)";
	}
	assert(4==r);
	if (filecode[0]!='D' || filecode[1]!='D' || filecode[2]!='S' || filecode[3]!=' ') {
		fclose(fp);
		console.error("File isn't DDS format!").outdent();
		throw "loadDDS: Not DDS format";
	}

	/* get the surface desc */
	r = fread(&header, 124, 1, fp);
	if (r!=1) {
		console.error("Can't read header of file!").outdent();
		fclose(fp);
		throw "loadDDS: Can't read header";
	}

	height      = *(unsigned int*)&(header[8 ]);
	width       = *(unsigned int*)&(header[12]);
	unsigned int linearSize     = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	r = fread(buffer, 1, bufsize, fp);
	if (r!=bufsize) {
		console.errorf("only read %d bytes (wanted %d)",r,bufsize).outdent();
		fclose(fp);
		throw "loadDDS: Not enough data read";
	}
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch(fourCC)
	{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			console.error("file is in unknown format!").outdent();
			free(buffer);
			throw "loadDDS: Wrong format!";
	}

	console.logf("File is type %s",
			(format==GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ?
			 "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT" :
			 (format==GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ?
			  "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT" :
			  (format==GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ?
			   "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT" :
			   "unknown")
			 )
			)
		    );

	if (id) glDeleteTextures(1, &id);
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	logOpenGLErrors();

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		console.logf("MipMap level %d (%dx%d)",level,width,height);
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
				0, size, buffer + offset);
		offset += size;
		width  /= 2;
		height /= 2;
	}
	free(buffer);

	// if texture didn't have any mip maps, get GL to generate them for us
	if (mipMapCount == 1)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,0);

	logOpenGLErrors();
	console.logf("OK, loaded -> %d", id).outdent();
}

void Texture::loadTGA(const std::string& f, bool keepData)
{
	FILE *fp;
	std::string filename = DATA_DIR + f;
	console.logf("Texture::loadTGA %s", filename.c_str());
	console.indent();

	unsigned char *buf;
	fp = fopen(filename.c_str(), "rb");
	if (!fp) console.throwOut("Can't open file to read!");
	fseek(fp,0,SEEK_END);
	long length = ftell(fp);
	rewind(fp);
	buf = new unsigned char[length];
	long r = fread(buf,1,length,fp);
	fclose(fp);
	console.logf("Read %ld/%ld bytes", r,length);

        short st;
        bool need2flip = false;
	short m_height, m_width;

        int bpp = buf[16];
	if (buf[2] != 2) console.errorf("TGA isn't a type 2 (it is a type %d)!  (type 3 is greyscale, type 10 is RLE-compressed.  both haven't got any code written for them yet.)", buf[2]);
        if (buf[2] != 2)                         console.throwOut("Not a type 2 TGA file.");
        if (buf[1] != 0 || (bpp!=32 && bpp!=24)) console.throwOut("Not 24 or 32 bits TGA");

        m_width  = *(short*)(buf+12);
        m_height = *(short*)(buf+14);

        st = *(short*)(buf+10);
        if (st==0) { need2flip=!need2flip; }  // if start is 0 that means origin in bottom-left

        console.logf("Loading %dx%d image, %d BPP%s",m_width,m_height,bpp,need2flip?" origin bottom-left":" origin top-left");

	if (id) glDeleteTextures(1, &id);
        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        logOpenGLErrors();

	unsigned char *buf2 = buf+18+buf[0];
        keptData = new unsigned char[m_width*m_height*4];
	
	// upgrade 24bpp to 32bpp by inserting alpha channel (at full opacity).  
	// this makes pixel lookups easier (one code-path), and probably quicker
	if (bpp==24) {
                for (unsigned int o=0; o<m_width*m_height; ++o) {
                        keptData[o*4+0] = buf2[o*3+0];
                        keptData[o*4+1] = buf2[o*3+1];
                        keptData[o*4+2] = buf2[o*3+2];
                        keptData[o*4+3] = 255;
                }
	} else {
		// just copy data
		memcpy(keptData, buf2, m_width*m_height*4);
	}

	if (need2flip) {
//		unsigned char *row = new unsigned char[m_width];
		unsigned char *row=buf; // just borrow this bit of memory, we don't need it anymore
		int y,y2;
		for (y=0,y2=m_height-1; y<m_height/2; ++y,--y2) {
			memcpy(row, &keptData[y*m_width*4], m_width*4);
			memcpy(&keptData[y*m_width*4], &keptData[y2*m_width*4], m_width*4);
			memcpy(&keptData[y2*m_width*4], row, m_width*4);
		}
//		delete[] row;
	}

	// send data to OpenGL
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_width,m_height,0,GL_BGRA,GL_UNSIGNED_BYTE,keptData);

	glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);

	if (!keepData) {
		delete[] keptData; keptData=0;
	}
	delete[] buf;
	width=m_width;
	height=m_height;

        logOpenGLErrors();
        console.logf("OK, loaded -> %d", id).outdent();

}

// return the min/max value of red channel (0..1): X,Y should be normalized coords (0..1).
void Texture::getMinMaxRed(const float startX, const float startY, const float endX, const float endY, float *mn, float *mx)
{
	if (!keptData) { console.errorf("Can't get Min/Max for texture because the data wasn't kept!"); return; }

	int sX = startX * width;
	int sY = startY * height;
	int eX = min(int(endX*width+0.5),(int)width);
	int eY = min(int(endY*height+0.5),(int)height);

	for (int y=sY; y<eY; ++y) {
		for (int x=sX; x<eX; ++x) {
			float r = keptData[(y*width + x) * 4 + 0] / 255.f;
			if (r < *mn) *mn = r;
			if (r > *mx) *mx = r;
		}
	}
//	console.debugf("Texture::minMaxRed of %.3f,%.3f - %.3f,%.3f (%d,%d - %d,%d) = %.3f/%.3f", startX,startY, endX,endY, sX,sY,eX,eY, *mn, *mx);
}
