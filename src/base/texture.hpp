#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <map>
#include <string>

#include "ogl.hpp"

class Texture {

	GLuint id;
	GLuint boundTo;
	std::string filename;
	static std::map< GLuint, Texture* > bound;

	unsigned char *keptData;
	GLuint width,height;

public:

	void bind(int unit=-1);
	void loadDDS(const std::string& f);
	void loadTGA(const std::string& f, bool keepData);

	void getMinMaxRed(const float startX, const float startY, const float endX, const float endY, float *mn, float *mx);

	Texture();
	Texture(const std::string& f, bool keepData=false);
	~Texture();

	void filterNearest();
	void filterLinear();
	void filterMipmap();

	void wrapRepeat() { wrap(GL_REPEAT); }
	void wrapMirror() { wrap(GL_MIRRORED_REPEAT); }
	void wrapClamp()  { wrap(GL_CLAMP); }
	void wrap(GLenum wrap);
};

#endif//TEXTURE_HPP
