#ifndef UIHELPER_HPP
#define UIHELPER_HPP

#include "awesome.hpp"

#define LOADING_STATE_UI_LOADING     0
#define LOADING_STATE_ENGINE_LOADING 1
#define LOADING_STATE_READY          9
extern int g_loadingState;

class UIHelper {

public:

  UIHelper();
  ~UIHelper();

  void setupCallbacks(Awesome* awesome);
  AWESOME_FUNC(JSshaders_setUniform1i);
  AWESOME_FUNC(JSshaders_setUniform1f);
  AWESOME_FUNC(JSshaders_list);

  AWESOME_FUNC(JSui_loaded);
};

#endif//UIHELPER_HPP
