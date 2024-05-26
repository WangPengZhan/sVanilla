# Develop

## Build

### install tools and  dependency

tools：git cmake python
dependency: qt5 or qt6


### get code

```sh
git clone https://github.com/WangPengZhan/sVanilla.git
cd sVanilla
git submodule update --init --recursive
```

### configurate

```sh
mkdir build
cmake -B build
```

### build target

```sh
 cmake --build build
```

## Project Structure

```
TOP
 |
 |-- README.md
 |-- CMakeLists.txt
 |
 |-- include
 |      |
 |      |-- <*.h files>
 |
 |-- cmake
 |    |
 |    |-- <*.cmake files>
 |
 |-- doc
 |    |
 |    |-- <*.md files>
 |
 |--sVanilla
 |      |
 |      |-- src
 |      |    |
 |      |    |-- <*.cpp/*.h/*.ui files>
 |      |  
 |      |-- resource
 |      |       |
 |      |       |-- <*.qrc files>
 |      |
 |      |-- translate 
 |      |       |
 |      |       |-- <*.ts files>
 |
 |-- ThirdParty  // ThirdParty depend submodule
 |       |
 |       |-- [vcpkg] // optional 3rd party package manager git sub-module
 |       |
 |       |-- *** // optional any other 3rd party sub-module
 |
 |-- *** // other classification folder

```
