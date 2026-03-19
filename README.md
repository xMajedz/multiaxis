# Depends on
* Luau ([luau-lang/luau](https://github.com/luau-lang/luau))
* Raylib ([raysan5/raylib](https://github.com/raysan5/raylib))
* Open Dynamics Engine ([odedevs/ode](https://bitbucket.org/odedevs/ode)) 
# Building
clone this repo
```
git clone https://github.com/xMajedz/multiaxis.git
cd muliaxis
```
update submodules
```
git submodule init
git submodule update
```
clone Luau
```
git clone https://github.com/luau-lang/luau.git
```
build Luau
```
make -C luau config=release luau
```
clone Raylib
```
git clone https://github.com/raysan5/raylib.git
```
build Raylib
```
make -C raylib/src
```
clone ODE
```
git clone https://bitbucket.org/odedevs/ode.git
```
build ODE
```
cd ode
./bootstrap
./configure
make
```
build tobas
```
cd build
make -C ..
```
run tobas
```
./tobas
```
# Downloads
check the [releases](https://github.com/xMajedz/tobas/releases) page for updates 
