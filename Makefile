OUTDIR := out
SOURCES := $(wildcard src/*.cpp) $(wildcard test/*.cpp)
PRODUCT := history-dag-test
CXX = g++
CXXFLAGS += -std=c++17 \
	-Werror -Wall -Wextra -pedantic -pedantic-errors \
	-Wold-style-cast \
	-Og -g3 \
	-Iinclude -Itest
LDFLAGS +=

ifeq ($(SANITIZE),yes)
	CXXFLAGS += -fsanitize=address -fsanitize=undefined
	LDFLAGS += -lasan -lubsan
endif

OBJECTS := $(patsubst %.cpp,%.o,$(patsubst %,$(OUTDIR)/%,$(SOURCES)))

all: $(OUTDIR)/$(PRODUCT)
	@echo Running $(OUTDIR)/$(PRODUCT)
	@$(OUTDIR)/$(PRODUCT)

$(OUTDIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) -MM -MF $@ -MT '$(@:.d=.o)' $< $(CXXFLAGS)

-include $(OBJECTS:.o=.d)

$(OUTDIR)/%.o: %.cpp $(OUTDIR)/%.d
	@mkdir -p $(dir $@)
	@echo Compiling C++ $<
	@$(CXX) -c -o $@ $< $(CXXFLAGS)

$(OUTDIR)/$(PRODUCT): $(OBJECTS)
	@echo Linking $@
	@$(CXX) -o $@ $^ $(LDFLAGS)
	@echo Success

.PHONY: clean

clean:
	@rm -rf $(OUTDIR)
