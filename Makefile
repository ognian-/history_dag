OUTDIR := out
SOURCES := $(wildcard src/*.cpp) $(wildcard test/*.cpp)
PROTO_DEFS := $(wildcard proto/*.proto)
PRODUCT := history-dag-test
CXX = g++
CXXFLAGS += -std=c++17 \
	-Werror -Wall -Wextra -pedantic -pedantic-errors \
	-Wold-style-cast \
	-O0 -g3 \
	-Iinclude -Itest -I$(OUTDIR)/proto/.. \
	$$(pkg-config --cflags protobuf)

ifneq ($(SANITIZE),no)
ifneq ($(MEMCHECK),yes)
	CXXFLAGS += -fsanitize=address -fsanitize=undefined
	LDFLAGS += -lasan -lubsan
endif
endif

LDFLAGS += -lz $$(pkg-config --libs protobuf)

RUN_COMMAND := $(OUTDIR)/$(PRODUCT)
ifeq ($(MEMCHECK),yes)
	RUN_COMMAND := valgrind --tool=memcheck --track-origins=yes $(OUTDIR)/$(PRODUCT)
endif

PROTO_SOURCES := $(patsubst %.proto,%.pb.cc,$(patsubst %,$(OUTDIR)/%,$(PROTO_DEFS)))
PROTO_HEADERS := $(patsubst %.proto,%.pb.h,$(patsubst %,$(OUTDIR)/%,$(PROTO_DEFS)))

SOURCE_OBJECTS := $(patsubst %.cpp,%.o,$(patsubst %,$(OUTDIR)/%,$(SOURCES)))
PROTO_OBJECTS := $(patsubst %.pb.cc,%.pb.o,$(PROTO_SOURCES))

OBJECTS := $(SOURCE_OBJECTS) $(PROTO_OBJECTS)

.SECONDARY: $(PROTO_SOURCES) $(PROTO_HEADERS)

all: $(OUTDIR)/$(PRODUCT)
	@echo Running $(OUTDIR)/$(PRODUCT)
	@$(RUN_COMMAND)

$(OUTDIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) -MM -MF $@ -MT '$(@:.d=.o)' $< $(CXXFLAGS)

-include $(OBJECTS:.o=.d)

$(OUTDIR)/%.pb.cc $(OUTDIR)/%.pb.h: %.proto
	@mkdir -p $(dir $@)
	@echo Generating protobuf $<
	@protoc --cpp_out=$(OUTDIR) $<

$(OUTDIR)/%.o: %.cpp $(OUTDIR)/%.d $(PROTO_HEADERS)
	@mkdir -p $(dir $@)
	@echo Compiling C++ $<
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

%.pb.o: %.pb.cc %.pb.h
	@mkdir -p $(dir $@)
	@echo Compiling Protobuf C++ $<
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

$(OUTDIR)/$(PRODUCT): $(OBJECTS)
	@echo Linking $@
	@$(CXX) -o $@ $^ $(LDFLAGS)
	@echo Build successful

.PHONY: clean

clean:
	@rm -rf $(OUTDIR)
