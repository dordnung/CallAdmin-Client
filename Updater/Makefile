# -----------------------------------------------------
# File        Makefile
# Authors     David Ordnung, Impact
# License     GPLv3
# Web         http://dordnung.de, http://gugyclan.eu
# -----------------------------------------------------
#
# Copyright (C) 2013-2017 David Ordnung, Impact
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

BINARY = calladmin-client-updater

OBJECTS += calladmin_client_updater.cpp update_frame.cpp resource.cpp
LINK = `wx-config --libs` `curl-config --libs`

CFLAGS += `wx-config --cxxflags` `curl-config --cflags` -O3 -DCURL_STATICLIB -DWX_PRECOMP -D__WXGTK__ -D_LINUX -DNDEBUG -D_UNICODE \
				-Wall -Woverloaded-virtual -Os -fpermissive -fno-strict-aliasing

OBJ_BIN := $(OBJECTS:%.cpp=%.o)

%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all: to_prog

to_prog: $(OBJ_BIN)
	$(CPP) $(INCLUDE) $(OBJ_BIN) $(LINK) -o $(BINARY)

default: all

clean: 
	find . -type f -name '*.o' -delete
	find . -type f -name BINARY -delete