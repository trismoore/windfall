# Windfall


## Dependancies

*   OpenGL

*   GLFW 

    makes opening opengl contexts easy

    needs to be compiled & installed, uses cmake to find

*   GLEW (The OpenGL Extension Wrangler Library)

    needs to be compiled & installed, uses cmake to find

*   GLM (GL mathematics library)

    header-only, just copied into src

*   Awesomium

    web page rendering code, based on Chromium

    uses cmake to find, but I wrote that myself so untested elsewhere

*   Fonts

    from Google's web fonts 

    * [Average](http://www.google.com/webfonts/specimen/Average), SIL Open Font license 1.1
    * [Asap](http://www.google.com/webfonts/specimen/Asap), SIL Open Font license 1.1

    are in `data/html/styles`

*   KendoUI 

    I'm using the open-source version from [kendoui.com](http://kendoui.com)

*   Simplex noise functions

    [webgl-noise](https://github.com/ashima/webgl-noise)

## Config

Config can be specified on the command line:

`$ ./Windfall window.width=1024 window.height=768 window.fullscreen=0`

or in a file, default is `data/config`, but setting `configfile=data/somethingelse` will load a different config file instead.
