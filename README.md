## ComputerGraphicsProject_1
This is a  project for a Computer Graphics course in NCTU


#Build Environment : 

- OS: Ubuntu 20.04.1 LTS

- Install libraries:

```bash
sudo apt-get install build-essential libgl1-mesa-dev 
# install build tool and drivers

sudo apt-get install libglew-dev libsdl2-dev libsdl2-image-dev libglm-dev libfreetype6-dev
# install GLFW, SDL2, GLM, FreeType

```

- Check OpenGL version (I am working with version 4.6)
```bash
glxinfo | grep OpenGL
# You should find followings lines:
# OpenGL version string: 4.6 (Compatibility Profile) Mesa 20.0.8
# OpenGL shading language version string: 4.60
```
- More Information see [OpenGL Programming Wikibook](https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/Linux) 





