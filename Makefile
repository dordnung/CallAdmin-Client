# -----------------------------------------------------
# File        makefile
# Authors     David Ordnung, Impact
# License     GPLv3
# Web         http://dordnung.de, http://gugyclan.eu
# -----------------------------------------------------
#
########## ATTENTION #####
# Linux support isn't given, yet.
# But you can use this makefile to test it
#
# Copyright (C) 2013 David Ordnung, Impact
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>

CPP = g++

WX = /home/david/Windows/Users/witch_000/Documents/Scripting/SDKS/wxwidgets
OPENSTEAMWORKS = /home/david/Windows/Users/witch_000/Documents/Scripting/SDKS/opensteamworks
CURL = /home/david/Windows/Users/witch_000/Documents/Scripting/SDKS/curl

BINARY = calladmin_client

OBJECTS += about.cpp calladmin-client.cpp call.cpp config.cpp log.cpp main.cpp opensteam.cpp taskbar.cpp tinyxml2/tinyxml2.cpp
INCLUDE += -I$(WX)/include -I$(WX)/lib/gcc_lib -I$(OPENSTEAMWORKS)/include -I$(CURL) -I./ -I./tinyxml2
LINK = -L$(WX)/lib/gcc_lib -L$(CURL) $(OPENSTEAMWORKS)/libs/steamclient.a -lcurl -lwx_gtk2u_adv-2.9 -lwx_gtk2u_core-2.9 -lwx_baseu-2.9 -lwxpng-2.9 -lwxjpeg-2.9 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgthread-2.0 -lrt -lglib-2.0 -lX11 -lXxf86vm -lSM -m32 -lrt -ldl -lm

CFLAGS += -O3 -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -DCURL_STATICLIB -DWX_PRECOMP -DSTEAMWORKS_CLIENT_INTERFACES -D_LINUX -DSI_CONVERT_ICU -DNDEBUG -D_UNICODE -DUNICODE \
				-Wall -Wno-unused-parameter -Wno-unused-result -Wno-unknown-pragmas -Woverloaded-virtual -Wnonnull -pthread -Os -fpermissive -fno-strict-aliasing -m32

OBJ_BIN := $(OBJECTS:%.cpp=%.o)

%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all: to_prog

to_prog: $(OBJ_BIN)
	$(CPP) $(INCLUDE) $(OBJ_BIN) $(LINK) -o $(BINARY)

default: all

clean: 
	find . -type f -name '*.o' -delete