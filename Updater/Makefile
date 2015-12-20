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

WX = ../wxwidgets
OPENSSL = ../openssl
CURL = ../curl

BINARY = calladmin-client-updater

OBJECTS += calladmin-client-updater.cpp update_dialog.cpp
INCLUDE += -I. -I$(CURL)/include
LINK = `wx-config --libs` $(CURL)/lib/.libs/libcurl.a $(OPENSSL)/libssl.a $(OPENSSL)/libcrypto.a -lz -m32

CFLAGS += `wx-config --cxxflags` -O3 -DCURL_STATICLIB -DWX_PRECOMP -D_LINUX -DNDEBUG -D_UNICODE \
				-Wall -Woverloaded-virtual -Os -fpermissive -fno-strict-aliasing -m32

OBJ_BIN := $(OBJECTS:%.cpp=%.o)

%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all: to_prog

to_prog: $(OBJ_BIN)
	$(CPP) $(INCLUDE) $(OBJ_BIN) $(LINK) -o $(BINARY)

default: all

clean: 
	find . -type f -name '*.o' -delete