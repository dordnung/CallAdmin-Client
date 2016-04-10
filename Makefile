# -----------------------------------------------------
# File        Makefile
# Authors     David O., Impact
# License     GPLv3
# Web         http://popoklopsi.de, http://gugyclan.eu
# -----------------------------------------------------
#
# Copyright (C) 2013-2016 David O., Impact
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

OPENSTEAMWORKS = ../opensteamworks

BINARY = calladmin-client

OBJECTS += about_panel.cpp call_dialog.cpp call_panel.cpp calladmin-client.cpp config.cpp config_panel.cpp curl_util.cpp log_panel.cpp main_frame.cpp main_panel.cpp \
				notebook.cpp opensteam.cpp opensteam_helper.cpp resource.cpp taskbar.cpp timer.cpp tracker_panel.cpp tinyxml2/tinyxml2.cpp

INCLUDE += -I"$(OPENSTEAMWORKS)/Open Steamworks" -I./tinyxml2
LINK = `wx-config --libs` `curl-config --libs` $(OPENSTEAMWORKS)/Resources/Libs/Linux32/steamclient.a -ldl

CFLAGS += `wx-config --cxxflags` `curl-config --cflags` -O3 -DSTEAMWORKS_CLIENT_INTERFACES -DCURL_STATICLIB -DWX_PRECOMP -D__WXGTK__ -D_LINUX -DNDEBUG -D_UNICODE \
				-Wall -Woverloaded-virtual -std=c++11 -Wno-unknown-pragmas -Wno-deprecated-declarations -Wno-overloaded-virtual -Os -fpermissive -fno-strict-aliasing

OBJ_BIN := $(OBJECTS:%.cpp=%.o)

%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all: to_prog

to_prog: $(OBJ_BIN)
	$(CPP) $(INCLUDE) $(OBJ_BIN) $(LINK) -o $(BINARY)

default: all

clean: 
	find . -type f -name '*.o' -delete
	find . -type f -name '$(BINARY)' -delete