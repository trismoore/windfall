#include "console.hpp"
#include "config.hpp"
#include "ogl.hpp"
#include "awesome.hpp"
#include "useful.h"
#include "camera.hpp"
#include "landscape.hpp"
#include "debugrenderer.hpp"
#include "uihelper.hpp"

double gTime, gdT;
extern const char* g_versionString;

Camera * g_camera = 0;

int main(int argc, char ** argv)
{
	console.logf("Starting Windfall (%s)", g_versionString);
	console.logf("- %s, %s -", __DATE__, __TIME__);

	Config *config = new Config();
	console.log("Parsing configuration").indent();
	console.debug("Setting default config").indent();
	config->set("window.width","1024");
	config->set("window.height","768");
	config->set("window.fullscreen",0);
	config->set("window.title", std::string("Windfall (") + g_versionString + ")");
	console.outdent();
	console.debug("Getting command line overrides").indent();
	config->parseArgs(argc,argv);
	console.outdent();
	std::string configfile = DATA_DIR + config->getString("configfile", "config");
	console.debugf("Reading config from file %s",configfile.c_str()).indent();
	config->loadFile(configfile.c_str());
	console.outdent();
	console.outdent();

	console.log("Starting OpenGL").indent();
	ogl.init();
	ogl.openWindow(config);
	console.outdent();

	Awesome * awesome = new Awesome(config);
	awesome->loadFile(config->getString("UI.html", "html/ui.html"));
	console.setupCallbacks(awesome);

	UIHelper *uihelper = new UIHelper;
	uihelper->setupCallbacks(awesome);

	console.outdent();
	console.log("Looping until UI calls UI.loaded()");
	console.indent();
	while (OGL::isRunning() && g_loadingState==LOADING_STATE_UI_LOADING) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		awesome->render();
		glfwSwapBuffers();
	}

	Landscape* landscape;
	Camera * camera;
	DebugRenderer *debug;

	int component = 0;
	float loadedAmount = 0;
	std::string loadingComponent = "Engine";

	console.outdent();
	console.log("UI loaded, looping and loading engine components");
	console.indent();

	while (OGL::isRunning() && g_loadingState==LOADING_STATE_ENGINE_LOADING) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (loadedAmount >= 1) {	
			++component;
			loadedAmount = 0;
		}
		awesome->runJSf("loadingTextAndFraction('%s', %f);", loadingComponent.c_str(), loadedAmount);

		awesome->render();
		awesome->render();
		awesome->render(); // plenty of time to alter UI
	
		switch (component) {
			case 0:
				loadingComponent="Camera";
				camera = new Camera(awesome);
				g_camera = camera;
				loadedAmount = 1;
				break;
			case 1: loadedAmount += 0.1;
				//loadedAmount = 1;
				break;
			case 2:
				loadingComponent="Landscape";
				landscape = new Landscape(config, awesome);
				loadedAmount = 1;
				break;
			case 3:
				// load a bit of landscape
				loadedAmount = landscape->load();
				break;
			case 4:
				loadingComponent="DebugRenderer";
				debug = new DebugRenderer(config);
				loadedAmount = 1;
				break;
			case 5: loadedAmount += 0.2;
				//loadedAmount = 1;
				break;
			default:
				g_loadingState = LOADING_STATE_READY;
				awesome->runJS("onEngineFinishedLoading();");
				break;
		}

		glfwSwapBuffers();
		glfwSwapBuffers(); // make sure any UI progress is displayed (not double-buffered)
	}

	console.outdent();
	console.log("Finished setup, entering loop...");
	console.indent();
	
	gTime = glfwGetTime();
	double timeLastFrame = gTime;
	double timeEndFrame = gTime;
	int frames = 0;
	double oneSecondTimer = 1.0;
	gdT = 0;

	while (OGL::isRunning()) {
		gTime = glfwGetTime();
		gdT = gTime - timeLastFrame;
		timeLastFrame = gTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//camera->look( glm::vec3(0.5,-0.5,0.6), glm::vec3(0.5,0.5,0.0));
		camera->update();

		landscape->render();
		awesome->render();

		// debug all texture units
		debug->render();

		timeEndFrame = glfwGetTime();
		oneSecondTimer -= gdT;
		++frames;
		if (oneSecondTimer < 0) {
			oneSecondTimer = 1.0;
			//      awesome.runJSf("UI.message('<strong>%d</strong> last second, [last frame <i>dTime</i>=%.3fms (%.1f fps), <i>actual</i> = %.3fms (<strong>%.1f</strong> fps)]; AwesomeLastSec=%d');",
//			printf("%d last second, [last dTime=%.3fms (%.1f fps), actual = %.3fms (%.1f fps)]; AwesomeLastSec=%d');\n",
//			           frames, 1000.f*gdT, 1.f/gdT, 1000.f*(timeEndFrame-gTime), 1.f/(timeEndFrame-gTime), Awesome::updatesLastSecond);
			Awesome::updatesLastSecond = 0;
			frames=0;
		}

		glfwSwapBuffers();
	}

	console.outdent();
	console.log("Finished loop, shutting down");
	console.indent();
	delete landscape;
	delete awesome;
	delete uihelper;
	delete camera;
	delete debug;
	delete config;
	ogl.closeWindow();
	ogl.terminate();
	console.outdent();

	return 0;
}
