CXX=$(CROSS_COMPILE)g++
RM=rm

CXXSOURCES=main.cpp

INCLUDE =  -I$(INSTALL_DIR)/include
INCLUDE += -I$(INSTALL_DIR)/include/api2
INCLUDE += -I$(INSTALL_DIR)/include/apiApp
INCLUDE += -I$(INSTALL_DIR)/rp_sdk
INCLUDE += -I$(INSTALL_DIR)/rp_sdk/libjson
INCLUDE += -I/usr/include/python3.5m

LIBS = -L$(INSTALL_DIR)/lib
LIBS += -L$(INSTALL_DIR)/rp_sdk
LIBS += -L/usr/lib/python3.5/config-3.5m-arm-linux-gnueabihf
LIBS += -L/usr/lib

COMMON_FLAGS+=-Wall -fPIC -Os -s
CXXFLAGS+=$(COMMON_FLAGS) -std=c++11 $(INCLUDE)
CXXFLAGS+= -Wno-unused-result -Wsign-compare -g -fstack-protector-strong -Wformat -Werror=format-security -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes
LDFLAGS = -shared $(COMMON_FLAGS) $(LIBS)
LDFLAGS+= -Wl,--whole-archive,--no-as-needed
LDFLAGS+= -lcryptopp -lrpapp -lrp -lrp_sdk
LDFLAGS+= -Wl,--no-whole-archive
LDFLAGS+= -lpython3.5m -lpthread -ldl -lutil -lm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions

CXXOBJECTS=$(CXXSOURCES:.cpp=.o)
OBJECTS=$(CXXOBJECTS)

OUT_DIR=../
C_OUT_NAME=$(OUT_DIR)controllerhf.so

all: make_c_app

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

make_c_app: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(C_OUT_NAME) $(LDFLAGS)

clean:
	-$(RM) -f $(C_OUT_NAME) $(OBJECTS)
