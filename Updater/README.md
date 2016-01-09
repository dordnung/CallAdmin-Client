CallAdmin Client Updater
==========

The Updater for the Call Admin Client

## How to install: ##

### On Linux: ###
- **Install requirements**
  1. `apt-get install build-essential`
  2. `apt-get install libcurl4-openssl-dev`
  3. `apt-get install libwxgtk3.0-dev`
    - Install wxWidgets with [this guide](https://wiki.wxwidgets.org/Compiling_and_getting_started) (But use option: `--disable-shared`), if wxWidgets 3.0 is not available on your system (There is no support for wxWidgets 2)

- **Build calladmin-client-updater**
  1. `wget https://github.com/popoklopsi/CallAdmin-Client/archive/master.zip && unzip CallAdmin-Client-master.zip`
  2. `cd CallAdmin-Client-master/Updater`
  3. `make`

### On Windows (Visual Studio 2013): ###
- **Use Prebuild version**
  - Download prebuild file from `http://www.xxx.com`
  
- **Or build on your own: Build wxwidgets 3.0.2**
  1. Download wxwidgets 3.0.2 from `https://github.com/wxWidgets/wxWidgets/releases/download/v3.0.2/wxWidgets-3.0.2.zip` and unzip
  2. Add VS to the system PATH if not done yet:
    - For example: `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin` 
  3. Open command line at `wxWidgets-3.0.2/build/msw/`
  4. Type `vcvars32.bat` and press ENTER
  5. Type `nmake.exe -f makefile.vc SHARED=0 BUILD=release UNICODE=1` and press ENTER
  6. Add a new system variable named `WXWIDGETS302` with the path to the wxWidgets-3.0.2 folder
  
- **Build libcurl**
  1. Download curl from `http://curl.haxx.se/download/curl-7.46.0.zip` and unzip
  2. Add VS to the system PATH if not done yet:
    - For example: `C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin` 
  3. Open command line at `curl-7.46.0/winbuild`
  4. Type `vcvars32.bat` and press ENTER
  5. Type `nmake /f Makefile.vc mode=static VC=12 MACHINE=x86` and press ENTER
  6. Move the `libcurl_a.lib` and the `include` folder from `curl-7.46.0/builds/libcurl-XXX` to some folder
  7. Add a new system variable named `CURL` with the path to the .lib and the include folder

- **Build calladmin-client-updater**
  1. Download CallAdmin Client from `https://github.com/popoklopsi/CallAdmin-Client/archive/master.zip` and unzip
  2. Open `CallAdmin-Client-master/Updater/msvc13/calladmin-client-updater.sln` 
  3. Build the project.