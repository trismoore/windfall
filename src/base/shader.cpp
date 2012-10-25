#include <iostream>
#include <fstream>
#include <sstream>
#include "shader.hpp"
#include "camera.hpp"
#include "console.hpp"
#include "useful.h"
#include "awesome.hpp"

Shader* Shader::root = 0;
GLuint Shader::activeProgram = -1;

Shader::Shader(const std::string& file) : fragment(0),vertex(0),program(0)
{
	load(file);

	// add ourselves to the linked list
	Shader *p = root;
	if (!root) root=this;
	else {
		while (p->next)
			p=p->next;
		p->next = this;
	}
	next = 0;
/*
	// list
	p=root;
	console.debugf("Shaders, after adding this 0x%08x",this);
	while (p) {
		console.debugf("-> 0x%08x %s",p,p->name.c_str());
		p=p->next;
	}
*/
}

Shader::~Shader()
{
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteProgram(program);
	// find ourselves in the linked list and remove self
	Shader *p = root;
	if (root==this) {
		root = p->next;
	} else {
		while (p->next != this)
			p=p->next;
		p->next = next;
	}
}

// read a file in, if any :includes are encountered, then recursively getFile them too
void Shader::getFile(std::string& source, const std::string &file)
{
	std::string filename = std::string(DATA_DIR) + "shaders/" + file;
	console.logf("Shader::getFile(%s)", filename.c_str());

	filenames.push_back(filename);
	int fileNumber = filenames.size();
	int lineNumber = 0;
	std::ifstream f;
	f.open(filename.c_str());
	if (f.fail()) { console.errorf("Can't open shader file '%s'!", filename.c_str()); return; }

	std::string tmp, line;
	while (std::getline(f,tmp)) {
		++lineNumber;
		line = tmp; trim(line);
		if (line.size() == 0) continue;
		// i didn't use #include because
		// i wanted to make it clear that 
		// this is NOT an official preprocessor directive.
		if (line.substr(0,8).compare(":include") == 0) {
			char temp[1000];
			sprintf(temp,"/* %s -------------------------------------------- */\n", line.c_str());
			source += temp;
//printf("--> %s\n", line.c_str());
			console.indent();
			getFile(source, substr(line,9,0));
			console.outdent();
			sprintf(temp,"/* ------------ end of %s ------------------------ */\n", line.c_str());
			source += temp;
//printf("^ %s\n", line.c_str());
		} else {
			if (substr(line,-1).compare(":") != 0 && line[0]!='#') { // not a special line (of the sort "vertex:" or #version) - prepend #line tag
				char temp[1000];
				sprintf(temp, "#line %d %d\n", lineNumber, fileNumber);
				source += temp;
//printf("%s", temp);
			}
			source += line;
//printf("%s\n", line.c_str());
			source += '\n';
		}
	}
	f.close();
}

// read file, parse, compile
void Shader::load(const std::string &file)
{
	std::string source;
	getFile(source, file);
	loadSource(file, source);
}

// read string, parse into vert/frag and then compile each section & link
void Shader::loadSource(const std::string& shadername, const std::string &source)
{
//std::cout << "Shader::loadSource " << shadername << ": \n" << source << " ---- \n";
	name = shadername;
	uniform_cache.clear();

	std::stringstream s(source);
	std::string line;
	std::string type="global"; // anything at the top of the file (#version etc) gets put in ALL shaders.

	while (std::getline(s,line)) {
		if (substr(line,-1).compare(":")==0) {
			type = substr(line,0,-1);
		} else {
			line += '\n';
			shaders[type] += line;
			//      if (line[0]!='#' && line[1]!='l') // don't output #line lines
			//        console.debugf("<code>%8s: %s</code>", type.c_str(), line.c_str());
		}
	}

	// create shaders
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	vertex = glCreateShader(GL_VERTEX_SHADER);
	compile(vertex, shaders["vertex"], shaders["global"]);
	compile(fragment, shaders["fragment"], shaders["global"]);
	// attach them
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	// and link it all up
	glLinkProgram(program);
	GLint result = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &result);

	if (!result) { 
		printErrors(program);
		std::string shadersToOutput[3] = { "global", "vertex", "fragment" };
		for (int i=0; i<3; ++i) {
			console.logf("--- %s shader ---", shadersToOutput[i].c_str());
			std::stringstream s(shaders[shadersToOutput[i]]);
			std::string line;
			while (std::getline(s,line)) {
				console.logf("%s", line.c_str());
			}
		}
		throw "Shader didn't compile, stopping now.";
	}

	logOpenGLErrors();
}

void Shader::compile(GLuint shader, const std::string& source, const std::string& addToTop)
{
	std::string added = addToTop + source;
	const char *p = added.c_str();
	//  console.logf("Compiling source:\n%s\n",p);
	glShaderSource(shader, 1, &p, NULL);
	glCompileShader(shader);
	GLint result = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result) printErrors(shader);
}

GLint Shader::getUniformLocation(const std::string& name)
{
	std::map< std::string, GLint >::iterator it;
	it = uniform_cache.find(name);
	if (it != uniform_cache.end()) {
		//LOG_PRINTF("Uniform location for %s: cached %d", name.c_str(), it->second);
		return it->second;
	} else {
		use();
		GLuint l = glGetUniformLocation(program, name.c_str());
		uniform_cache[name] = l;
		console.logf("Program %02d '%s' UniformLocation for %s: not cached %d", program, this->name.c_str(), name.c_str(), l);
		logOpenGLErrors();
		return l;
	}
}

GLint Shader::getAttribLocation(const std::string& name)
{
	std::map< std::string, GLint >::iterator it;
	it = attribute_cache.find(name);
	if (it != attribute_cache.end()) {
		//LOG_PRINTF("Attribute location for %s: cached %d", name.c_str(), it->second);
		return it->second;
	} else {
		use();
		GLuint l = glGetAttribLocation(program, name.c_str());
		attribute_cache[name] = l;
		console.logf("Program %02d '%s'  AttribLocation for %s: not cached %d", program, this->name.c_str(), name.c_str(), l);
		logOpenGLErrors();
		return l;
	}
}

void Shader::use()
{
	if (activeProgram != program) {
		glUseProgram(program);
		activeProgram = program;
	}
	if (logOpenGLErrors())
		console.errorf("Shader::Use errors in program %d", program);
}

void Shader::printErrors(GLuint ID)
{
	// GL returns a massive string of errors.
	// chop them into \n terminated strings and LOG
	char tmp[1024];
	int length = 0;
	glGetShaderInfoLog(ID, 1024, &length, tmp);
	std::stringstream ss(tmp);
	std::string i;
	while (std::getline(ss, i)) {
		// substitute a filename for the fileNumber in the error
		std::string precolon;
		std::string error;
		int colonPos = i.find(":");
		precolon = substr(i,0,colonPos);
		error = substr(i,colonPos+2,0);
		int fileNumber=0, lineNumber=0;
		sscanf(precolon.c_str(), "%d(%d)", &fileNumber, &lineNumber);
		std::string filename;
		if (fileNumber > 0) filename=filenames[fileNumber-1];
		else filename="<no file>";
		console.errorf("Shader %d Error at [%d=%s:%d] %s", ID, fileNumber, filename.c_str(), lineNumber, error.c_str());
		//    console.errorf("ShaderError in %d: %s",ID,i.c_str());
	}
}

void Shader::setCamera(Camera* camera)
{
	use();
	logOpenGLErrors();
	GLuint MatrixID = getUniformLocation("VP");
	logOpenGLErrors();
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &camera->VP[0][0]);
	logOpenGLErrors();
}

void Shader::seti(const std::string& name, const int i) 
{ 
	//console.debugf("Shader::seti %d,%s=%d",program,name.c_str(),i);
	glUniform1i(getUniformLocation(name), i); 
}

void Shader::setf(const std::string& name, const float f)
{
	//console.debugf("Shader::setf %d,%s=%f",program,name.c_str(),f);
	glUniform1f(getUniformLocation(name), f);
}

void Shader::set2f(const std::string& name, const float f1, const float f2)
{
	glUniform2f(getUniformLocation(name), f1, f2);
}

void Shader::set3f(const std::string& name, const float f1, const float f2, const float f3)
{
	glUniform3f(getUniformLocation(name), f1,f2,f3);
}

void Shader::set3f(const std::string& name, const float *p)
{
	set3f(name,p[0],p[1],p[2]);
}

void Shader::set3f(const std::string& name, const glm::vec3& v)
{
	set3f(name, v.x,v.y,v.z);
}

Shader* Shader::findShader(const std::string& shaderName) {
	Shader *p = root;
	while (p) {
		if (p->name.compare(shaderName)==0) return p;
		p=p->next;
	}
	console.errorf("Can't find shader named %s! (try 'list' to find shader's name)", shaderName.c_str());
	return 0;
}

void Shader::seti(const std::string& shaderName, const std::string& attribute, const int i) {
	Shader *s = findShader(shaderName);
	if (s) s->seti(attribute, i);
}

void Shader::setf(const std::string& shaderName, const std::string& attribute, const float f) {
	Shader *s = findShader(shaderName);
	if (s) s->setf(attribute, f);
}

void Shader::listShaders()
{
	Shader * p = root;
	while (p) {
		console.logf("Shader '%s', frag:%d, vert:%d, program:%d", p->name.c_str(), p->fragment,p->vertex,p->program);
		p=p->next;
	}
}

