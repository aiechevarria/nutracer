<div align="center">
  <img src="./assets/logo.png" width="200" alt="NuTracegen Logo" />
</div>
<h1 align="center">NuTracegen</h1>
<h2 align="center">A simple trace generator for NuCachis</h2>
<h3 align="center"><a>https://www.atc.unican.es</a></h3>

## About the project
A simple trace generator for NuCachis.

## Getting started
### Prerequisites
The GUI requires OpenGL and SDL2. For Debian based systems, install the following dependencies:
```
apt-get install cmake gcc git libsdl2-dev libgl1-mesa-dev libglu1-mesa-dev
```

### Installation
1. Clone the repository:
```
git clone https://github.com/aiechevarria/nutracegen.git
```

2. Run cmake:
```
cd nutracegen
mkdir build
cd build
cmake ..
cmake --build .
```

3. Execute with:
```
./nutracegen
```

## Usage
By default NuTracegen will run in GUI mode.

Output of `./nutracegen -h`:
TODO