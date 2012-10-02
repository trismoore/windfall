#ifndef AWESOME_HPP
#define AWESOME_HPP

#include <Awesomium/WebCore.h>
#include <Awesomium/JSDelegate.h>

#include "config.hpp"
#include "renderer.hpp"
#include "ogl.hpp"

class Shader;
class VBO;

typedef std::map<std::wstring, Awesomium::JSDelegate> AwesomeDelegateMap;

class Awesome : public Renderer, public Awesomium::WebViewListener, public Awesomium::ResourceInterceptor
{
  int windowheight, windowwidth;
  GLuint texture, textureSampler;
  Shader * shader;
  VBO * vbo;

  AwesomeDelegateMap delegateMap;
  std::string reloadString;
  int reloadType; // 1==file, 2==url, 3==html
  // function, object map
  std::map<std::wstring, std::wstring> callbackFunctions;

public:
  static int updatesLastSecond;

  Awesome(Config* config);
  ~Awesome();

  // functions that can be called from Javascript
  void JSgetFile(Awesomium::WebView* caller, const Awesomium::JSArguments& args);
  void JSsaveStringToFile(Awesomium::WebView* caller, const Awesomium::JSArguments& args);
  void JSloadStringFromFile(Awesomium::WebView* caller, const Awesomium::JSArguments& args);
  void JSquit(Awesomium::WebView* caller, const Awesomium::JSArguments& args);

  static Awesomium::WebView* webView;
  static Awesomium::WebCore* webCore;

  void render();
  void onResize(int w,int h);
  void onKey(int key, int action);
  void onChar(int character, int action);
  void onMouseButton(int button, int action);
  void onMouseMove(int x, int y);
  void onMouseWheel(int pos);

  void loadFile(std::string file);
  void loadURL(std::string url);
  void loadHTML(std::string html);
  void reload();

  void runJS(std::string js);
  void runJSf(const char *format, ...);
  void callJS(std::string object, std::string func, int arg);
  void callJS(std::string object, std::string func, std::string arg);
  const int getJSInt(std::string js);

  void registerCallbackFunction(std::wstring object,
                                std::wstring function,
                                Awesomium::JSDelegate delegate);

  // ** The following methods are inherited from WebViewListener:
  virtual void onBeginNavigation(Awesomium::WebView* caller,
                                 const std::string& url,
                                 const std::wstring& frameName);

  virtual void onBeginLoading(Awesomium::WebView* caller,
                              const std::string& url,
                              const std::wstring& frameName,
                              int statusCode, const std::wstring& mimeType);

  virtual void onFinishLoading(Awesomium::WebView* caller);

  virtual void onCallback(Awesomium::WebView* caller,
                          const std::wstring& objectName,
                          const std::wstring& callbackName,
                          const Awesomium::JSArguments& args);

  virtual void onReceiveTitle(Awesomium::WebView* caller,
                              const std::wstring& title,
                              const std::wstring& frameName);

  virtual void onChangeTooltip(Awesomium::WebView* caller,
                               const std::wstring& tooltip);

  virtual void onChangeCursor(Awesomium::WebView* caller,
                              Awesomium::CursorType cursor);

  virtual void onChangeKeyboardFocus(Awesomium::WebView* caller,
                                     bool isFocused);

  virtual void onChangeTargetURL(Awesomium::WebView* caller,
                                 const std::string& url);

  virtual void onOpenExternalLink(Awesomium::WebView* caller,
                                  const std::string& url,
                                  const std::wstring& source);

  virtual void onRequestDownload(Awesomium::WebView* caller,
                                 const std::string& url);

  virtual void onWebViewCrashed(Awesomium::WebView* caller);

  virtual void onPluginCrashed(Awesomium::WebView* caller,
                               const std::wstring& pluginName);

  virtual void onRequestMove(Awesomium::WebView* caller, int x, int y);

  virtual void onGetPageContents(Awesomium::WebView* caller,
                                 const std::string& url,
                                 const std::wstring& contents);

  virtual void onDOMReady(Awesomium::WebView* caller);

  virtual void onRequestFileChooser(Awesomium::WebView* caller,
                                    bool selectMultipleFiles,
                                    const std::wstring& title,
                                    const std::wstring& defaultPath);

  virtual void onGetScrollData(Awesomium::WebView* caller,
                               int contentWidth,
                               int contentHeight,
                               int preferredWidth,
                               int scrollX,
                               int scrollY);

  virtual void onJavascriptConsoleMessage(Awesomium::WebView* caller,
                                          const std::wstring& message,
                                          int lineNumber,
                                          const std::wstring& source);

  virtual void onGetFindResults(Awesomium::WebView* caller,
                                int requestID,
                                int numMatches,
                                const Awesomium::Rect& selection,
                                int curMatch,
                                bool finalUpdate);

  virtual void onUpdateIME(Awesomium::WebView* caller,
                           Awesomium::IMEState imeState,
                           const Awesomium::Rect& caretRect);

  virtual void onShowContextMenu(Awesomium::WebView* caller,
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
                                 int editFlags);

  virtual void onRequestLogin(Awesomium::WebView* caller,
                              int requestID,
                              const std::string& requestURL,
                              bool isProxy,
                              const std::wstring& hostAndPort,
                              const std::wstring& scheme,
                              const std::wstring& realm);

  virtual void onChangeHistory(Awesomium::WebView* caller,
                               int backCount,
                               int forwardCount);

  virtual void onFinishResize(Awesomium::WebView* caller,
                              int width,
                              int height);

  virtual void onShowJavascriptDialog(Awesomium::WebView* caller,
                                      int requestID,
                                      int dialogFlags,
                                      const std::wstring& message,
                                      const std::wstring& defaultPrompt,
                                      const std::string& frameURL);


  virtual Awesomium::ResourceResponse * onRequest (Awesomium::WebView *caller,
                                                   Awesomium::ResourceRequest *request);
  virtual void onResponse (Awesomium::WebView *caller,
                           const std::string &url,
                           int statusCode,
                           const Awesomium::ResourceResponseMetrics &metrics);
};

#endif//AWESOME_HPP
