#include "texture.hpp"
#include "useful.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#include <stdio.h>
#include "console.hpp"

std::map< GLuint, Texture* > Texture::bound;

Texture::Texture()
  : id(0),boundTo(0)
{

}

Texture::Texture(const char *f)
  : id(0),boundTo(0)
{
  if (f) loadDDS(f);
}

Texture::~Texture()
{
  glDeleteTextures(1,&id);
}

void Texture::bind(int unit)
{
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

void Texture::loadDDS(const char* f)
{
  assert(f);
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

  unsigned int height      = *(unsigned int*)&(header[8 ]);
  unsigned int width         = *(unsigned int*)&(header[12]);
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
