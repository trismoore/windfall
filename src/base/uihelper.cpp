#include "uihelper.hpp"
#include "console.hpp"
#include "shader.hpp"

UIHelper::UIHelper()
{

}

UIHelper::~UIHelper()
{

}

AWESOME_FUNC(UIHelper::JSshaders_list)
{
	Shader::listShaders();
}

AWESOME_FUNC(UIHelper::JSshaders_setUniform1i)
{
	if (args.size() < 3 || !args[0].isString() || !args[1].isString() || !args[2].isNumber()) { 
        	console.error("usage: setUniform1i(shader name [string], shader attribute [string], value [number]);"); return; }

	std::wstring a0 = args[0].toString();
        std::string name(a0.begin(), a0.end());
	std::wstring a1 = args[1].toString();
        std::string attr(a1.begin(), a1.end());
	Shader::seti(name,attr,args[2].toInteger());
}

AWESOME_FUNC(UIHelper::JSshaders_setUniform1f)
{
	if (args.size() < 3 || !args[0].isString() || !args[1].isString() || !args[2].isNumber()) { 
        	console.error("usage: setUniform1f(shader name [string], shader attribute [string], value [number]);"); return; }

	std::wstring a0 = args[0].toString();
        std::string name(a0.begin(), a0.end());
	std::wstring a1 = args[1].toString();
        std::string attr(a1.begin(), a1.end());
	Shader::setf(name,attr,args[2].toDouble());
}

void UIHelper::setupCallbacks(Awesome* awesome)
{
	awesome->registerCallbackFunction( L"shaders", L"list", Awesomium::JSDelegate(this, &UIHelper::JSshaders_list));
	awesome->registerCallbackFunction( L"shaders", L"setUniform1i", Awesomium::JSDelegate(this, &UIHelper::JSshaders_setUniform1i));
	awesome->registerCallbackFunction( L"shaders", L"setUniform1f", Awesomium::JSDelegate(this, &UIHelper::JSshaders_setUniform1f));
}

