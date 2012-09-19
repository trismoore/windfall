#include <iostream>

#include "awesome.hpp"
#include "shader.hpp"
#include "vbo.hpp"
#include "useful.h"

Awesomium::WebView* Awesome::webView;
Awesomium::WebCore* Awesome::webCore;

void Awesome::render(double dT)
{
  webCore->update();
  shader->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  if (webView->isDirty())// && !webView->isResizing())
  {
    const Awesomium::RenderBuffer* m_buffer = webView->render();
    if (m_buffer->rowSpan != m_buffer->width * 4)
      console.warning("Awesomuim: rowSpan != width*4 - expect weird results");
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
                 m_buffer->width, m_buffer->height,
                 0,GL_BGRA,GL_UNSIGNED_BYTE,m_buffer->buffer);
  }
  vbo->bind(shader);
  vbo->drawArrays(GL_TRIANGLE_STRIP);
  logOpenGLErrors();
}

Awesome::Awesome(Config* config)
{
  console.log("Awesome starting!").indent();

  windowwidth = config->getInt("window.width",1024);
  windowheight = config->getInt("window.height",768);

  Awesomium::WebCoreConfig conf;
  conf.setEnablePlugins(true);
  conf.setSaveCacheAndCookies(false);
  conf.setLogLevel(Awesomium::LOG_VERBOSE);
  conf.setCustomCSSFromFile("data/html/css/custom.css");

  console.log("Starting Awesome Core");
  webCore = new Awesomium::WebCore(conf);
  webCore->setBaseDirectory(DATA_DIR);

  // Create a new WebView instance with a certain width and height, using the
  // WebCore we just created
  console.log("Creating an Awesome View");
  webView = webCore->createWebView(windowwidth, windowheight);
  webView->setListener(this);
  webView->setResourceInterceptor(this);
  webView->setTransparent(true);
  webView->focus();

  console.log("Allocating GL resources").indent();

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  console.debugf("Texture -> %d", texture);

  const GLfloat GVertices[] = {  -1, 1,  -1,-1,   1, 1,   1,-1 };
  const GLfloat GTextures[] = {   0, 0,   0, 1,   1, 0,   1, 1 };

  vbo = new VBO();
  vbo->pushData("vertexPos", 2, 4, GVertices);
  vbo->pushData("vertexUV", 2, 4, GTextures);
  vbo->create();

  shader = new Shader("awesome.shader");
  textureSampler = shader->getUniformLocation("myTextureSampler");
  glUniform1i(textureSampler, 0);

  console.outdent();

  console.log("Finished Awesome Startup").outdent();
}

Awesome::~Awesome()
{
  // Destroy our WebView instance
  webView->destroy();
  delete webCore;
  glDeleteTextures(1, &texture);
  delete shader;
  delete vbo;
}

void Awesome::resize(int w, int h)
{
  windowwidth = w;
  windowheight = h;
  webView->resize(w,h);
  glViewport(0,0,w,h);
  console.logf("Awesome::resize %dx%d", w,h);
}

void Awesome::loadFile(std::string file)
{
  reloadString = file;
  reloadType = 1;
//  std::string f = "data/html/" + file;
 // WatchFileRelative(f);
  webView->loadFile(file);
}

void Awesome::loadURL(std::string url)
{
  reloadString = url;
  reloadType = 2;
  webView->loadURL(url);
}

void Awesome::loadHTML(std::string html)
{
  reloadString = html;
  reloadType = 3;
  webView->loadHTML(html);
}

void Awesome::reload()
{
  switch(reloadType) {
    case 1: loadFile(reloadString); break;
    case 2: loadURL(reloadString); break;
    case 3: loadHTML(reloadString); break;
    default: break;
  }
}

void Awesome::registerCallbackFunction(std::wstring object,
                                       std::wstring function,
                                       Awesomium::JSDelegate delegate)
{
//  std::wcout << "Registering " << object << "." << function << "\n";
  console.logf("Registering callback %ls.%ls", object.c_str(), function.c_str());
  webView->createObject(object);
  delegateMap[function] = delegate;
  webView->setObjectCallback(object, function);
  callbackFunctions[function] = object;
}

void Awesome::runJS(std::string js)
{
  console.logf("AwesomeJS> %s", js.c_str());
  webView->executeJavascript(js);
}

// webview listener functions
void Awesome::onBeginNavigation(Awesomium::WebView* caller, const std::string& url,
                                    const std::wstring& frameName)
{
  //std::cout << "[WebViewListener::onBeginNavigation]\n\tURL: " << url << std::endl;
}

void Awesome::onBeginLoading(Awesomium::WebView* caller,
                                 const std::string& url,
                                 const std::wstring& frameName,
                                 int statusCode, const std::wstring& mimeType)
{
  //std::cout << "[WebViewListener::onBeginLoading]\n\tURL: " << url << "\n\tSTATUS CODE: " <<
  //statusCode << "\n\tMIME TYPE: ";
  //std::wcout << mimeType << std::endl;
  console.logf("BeginLoading %d (%ls) <a href='%s'>%s</a>", statusCode, mimeType.c_str(), url.c_str(),url.c_str());
}

void Awesome::onFinishLoading(Awesomium::WebView* caller)
{
  //std::cout << "[WebViewListener::onFinishLoading]" << std::endl;
  //caller->requestScrollData(L"");
}

void Awesome::onCallback(Awesomium::WebView* caller,
                         const std::wstring& object,
                         const std::wstring& callback,
                         const Awesomium::JSArguments& args)
{
  AwesomeDelegateMap::iterator i = delegateMap.find(callback);
  if ( i != delegateMap.end() ) {
    //std::wcout << "Found, calling!\n";
    i->second(caller,args);
  } else {
    //std::wcout << "onCallback(" << object << "." << callback << ") not found\n";
    console.errorf("Callback(%ls.%ls) not found!", object.c_str(), callback.c_str());
  }
}

void Awesome::onReceiveTitle(Awesomium::WebView* caller,
                                 const std::wstring& title,
                                 const std::wstring& frameName)
{
  //std::wcout << L"[WebViewListener::onReceiveTitle]\n\tTITLE: " << title << std::endl;
}

void Awesome::onChangeTooltip(Awesomium::WebView* caller,
                                  const std::wstring& tooltip)
{
}

void Awesome::onChangeCursor(Awesomium::WebView* caller,
                                 Awesomium::CursorType cursor)
{
}

void Awesome::onChangeKeyboardFocus(Awesomium::WebView* caller,
                                        bool isFocused)
{
  //std::cout << "[WebViewListener::onChangeKeyboardFocus]\n\tFOCUSED: " << isFocused << std::endl;
}

void Awesome::onChangeTargetURL(Awesomium::WebView* caller,
                                    const std::string& url)
{
}

void Awesome::onOpenExternalLink(Awesomium::WebView* caller,
                                     const std::string& url,
                                     const std::wstring& source)
{
//  addWebTileWithURL(url, windowwidth, windowheight);
//  std::cout << "[WebViewListener::onOpenExternalLink]\n\tURL: " << url << std::endl;
//  animateTo(webTiles.size() - 1);
}

void Awesome::onRequestDownload(Awesomium::WebView* caller,
                                    const std::string& url)
{
//  std::cout << "[WebViewListener::onRequestDownload]\n\tURL: " << url << std::endl;
}

void Awesome::onWebViewCrashed(Awesomium::WebView* caller)
{
//  std::cout << "[WebViewListener::onWebViewCrashed]" << std::endl;
  console.error("Awesome WebView crashed!");

  console.log("Reloading WebView.  Try not to break this one...");
  webView->destroy();
  webView = webCore->createWebView(windowwidth, windowheight);
  webView->setListener(this);
  webView->setTransparent(true);

  console.log("Re-registering the callbacks now");
  std::map<std::wstring,std::wstring>::iterator it;
  for (it = callbackFunctions.begin(); it != callbackFunctions.end(); ++it) {
    webView->createObject( (*it).second );
    webView->setObjectCallback( (*it).second, (*it).first );
  }

  console.log("Reloading page");
  reload();
}

void Awesome::onPluginCrashed(Awesomium::WebView* caller,
                                  const std::wstring& pluginName)
{
//  std::cout << "[WebViewListener::onPluginCrashed]" << std::endl;
}

void Awesome::onRequestMove(Awesomium::WebView* caller, int x, int y)
{
}

void Awesome::onGetPageContents(Awesomium::WebView* caller,
                                    const std::string& url,
                                    const std::wstring& contents)
{
}

void Awesome::onDOMReady(Awesomium::WebView* caller)
{
  //std::cout << "[WebViewListener::onDOMReady]" << std::endl;
}

void Awesome::onRequestFileChooser(Awesomium::WebView* caller,
                                       bool selectMultipleFiles,
                                       const std::wstring& title,
                                       const std::wstring& defaultPath)
{
//  std::cout << "[WebViewListener::onRequestFileChooser]" << std::endl;
}

void Awesome::onGetScrollData(Awesomium::WebView* caller,
                                  int contentWidth,
                                  int contentHeight,
                                  int preferredWidth,
                                  int scrollX,
                                  int scrollY)
{
  //std::cout << "[WebViewListener::onGetScrollData]\n\tPAGE WIDTH: " << contentWidth << "\n\tPAGE HEIGHT: " << contentHeight << std::endl;
}

void Awesome::onJavascriptConsoleMessage(Awesomium::WebView* caller,
                                             const std::wstring& message,
                                             int lineNumber,
                                             const std::wstring& source)
{
  //std::wcout << "[WebViewListener::onJavascriptConsoleMessage]\n\tMESSAGE: " << message << L"\n\tLINE: " << lineNumber << L"\n\tSOURCE: " << source << std::endl;
  //std::wcout << L"[" << source << L":" << lineNumber << L"] " << message << std::endl;
  console.logf("[%ls:%d] %ls", source.c_str(),lineNumber, message.c_str());
}

void Awesome::onGetFindResults(Awesomium::WebView* caller,
                                   int requestID,
                                   int numMatches,
                                   const Awesomium::Rect& selection,
                                   int curMatch,
                                   bool finalUpdate)
{
}

void Awesome::onUpdateIME(Awesomium::WebView* caller,
                              Awesomium::IMEState imeState,
                              const Awesomium::Rect& caretRect)
{
}

void Awesome::onShowContextMenu(Awesomium::WebView* caller,
                                    int mouseX,
                                    int mouseY,
                                    Awesomium::MediaType type,
                                    int mediaState,
                                    const std::string& linkURL,
                                    const std::string& srcURL,
                                    const std::string& pageURL,
                                    const std::string& frameURL,
                                    const std::wstring& selectionText,
                                    bool isEditable,
                                    int editFlags)
{
}

void Awesome::onRequestLogin(Awesomium::WebView* caller,
                                 int requestID,
                                 const std::string& requestURL,
                                 bool isProxy,
                                 const std::wstring& hostAndPort,
                                 const std::wstring& scheme,
                                 const std::wstring& realm)
{
}

void Awesome::onChangeHistory(Awesomium::WebView* caller,
                                  int backCount,
                                  int forwardCount)
{
}

void Awesome::onFinishResize(Awesomium::WebView* caller,
                                 int width,
                                 int height)
{
}

void Awesome::onShowJavascriptDialog(Awesomium::WebView* caller,
                                         int requestID,
                                         int dialogFlags,
                                         const std::wstring& message,
                                         const std::wstring& defaultPrompt,
                                         const std::string& frameURL)
{
}

Awesomium::ResourceResponse * Awesome::onRequest (Awesomium::WebView *caller,
                                                 Awesomium::ResourceRequest *request)
{
  return NULL;
}

void Awesome::onResponse (Awesomium::WebView *caller,
                         const std::string &url,
                         int statusCode,
                         const Awesomium::ResourceResponseMetrics &metrics)
{
  if (statusCode != 200) // 200 = OK
    console.logf("%d %s", statusCode, url.c_str());
//  else
//    LOG_PRINTF("%d %s", statusCode, url.c_str());
}