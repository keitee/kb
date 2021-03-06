# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make clean  - removes all files generated by make.

GTEST_DIR = $(HOME)/works/googletest/googletest
GMOCK_DIR = $(HOME)/works/googletest/googlemock

TARGET1 = child
TARGET2 = parent
TARGET3 = child_gbo

# Where to find user code.
USER_DIR = .

CPPFLAGS += -isystem $(GTEST_DIR)/include -isystem $(GMOCK_DIR)/include 

# CXXFLAGS += -std=c99 -g -Wall -Wextra -pthread -D_GLIBCXX_DEBUG
CXXFLAGS += -std=c99 -g -Wall -Wextra -pthread

TESTS = $(TARGET1)_out $(TARGET2)_out \
				$(TARGET3)_out

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

GMOCK_HEADERS = $(GMOCK_DIR)/include/gmock/*.h \
                $(GMOCK_DIR)/include/gmock/internal/*.h

# House-keeping build targets.
all : $(TESTS)

clean :
	rm -f $(TESTS) *.a *.o

# Builds gtest.a and gtest_main.a.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

# Builds gtest.a and gtest_main.a.
GMOCK_SRCS_ = $(GMOCK_DIR)/src/*.cc $(GMOCK_HEADERS)

# For simplicity and to avoid depending on Google Test's
gmock-all.o : $(GMOCK_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GMOCK_DIR) $(CXXFLAGS) -c \
            $(GMOCK_DIR)/src/gmock-all.cc

gmock.a : gmock-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

#
# $(TARGET).o : $(TARGET).cpp $(GTEST_HEADERS)
# 	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(TARGET).cpp -o $(TARGET).o

# $(TARGET)_test : $(TARGET).o gtest.a gmock.a
# 	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

# c version

$(TARGET1).o : $(TARGET1).c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $(TARGET1).c -o $(TARGET1).o

$(TARGET1)_out : $(TARGET1).o
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(TARGET2).o : $(TARGET2).c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $(TARGET2).c -o $(TARGET2).o

$(TARGET2)_out : $(TARGET2).o
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

# gbo client
$(TARGET3)_out : $(TARGET3).c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -fsanitize=address $(TARGET3).c -o $@

# $(TARGET3)_out : $(TARGET3).o
# 	$(CC) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
