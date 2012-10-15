#ifndef UIHELPER_HPP
#define UIHELPER_HPP

#include "awesome.hpp"

class UIHelper {

public:

  UIHelper();
  ~UIHelper();

  void setupCallbacks(Awesome* awesome);
  AWESOME_FUNC(JSshaders_setUniform1i);
  AWESOME_FUNC(JSshaders_setUniform1f);
  AWESOME_FUNC(JSshaders_list);
};

#endif//UIHELPER_HPP
