##
## EPITECH PROJECT, 2024
## Makefile
## File description:
## Makefile
##

.PHONY: all clean fclean re tests_run vg cs linter format

%.o: %.cpp
	- g++ -c $< -o $@ $(CPPFLAGS)

BINARY_NAME			=	plazza

MAIN_SRC			=	./src/Main.cpp

SRC					=	./src/CLI.cpp							\
						./src/Utils.cpp							\
						./src/Parser.cpp						\
						./src/NamedPipe.cpp						\
						./src/ThreadPool.cpp					\
						./src/plazza/kitchen/Kitchen.cpp		\
						./src/plazza/Pizza.cpp					\
						./src/plazza/reception/Reception.cpp	\

OBJ					=	$(SRC:.cpp=.o)

MAIN_OBJ			=	$(MAIN_SRC:.cpp=.o)

# Tests sources ---------------------------------------------------------------
SRC_TESTS			=

# Flags -----------------------------------------------------------------------
INCLUDES			=	-I./include/ -I./src/

CPPFLAGS			+=	-std=c++17 -Wall -Wextra -Werror $(INCLUDES) 		\

CPPTESTFLAGS		=	--coverage -lcriterion $(CPPFLAGS)

VALGRIND_FLAGS		=														\
	--leak-check=full														\
	--show-leak-kinds=definite												\
	--track-origins=yes														\
	--errors-for-leak-kinds=definite										\
	--log-file="$(VALGRIND_LOG)"											\

CPPLINT_FLAGS		=														\
	--root=./include														\
	--repository=. 															\
	--filter=-build/include_subdir,-runtime/references,-build/c++17,-build/$\
c++11,-legal/copyright,-whitespace/indent_namespace							\
	--recursive																\

VALGRIND_LOG		=	valgrind.log

# Rules -----------------------------------------------------------------------
all: $(BINARY_NAME)

$(BINARY_NAME):	$(OBJ) $(MAIN_OBJ)
	- g++ -o $(BINARY_NAME) $(OBJ) $(MAIN_OBJ) $(CPPFLAGS)

vg: $(BINARY_NAME) $(CLIENT_BINARY_NAME)
	valgrind $(VALGRIND_FLAGS) ./$(BINARY_NAME)
	cat $(VALGRIND_LOG)

tests_run:
	g++ -o unit_tests $(SRC) $(SRC_TESTS) $(CPPTESTFLAGS)
	./unit_tests
	gcovr --exclude tests/
	gcovr -e tests --branch

clean:
	rm -f $(OBJ) $(MAIN_OBJ) *.gcda *.gcno vgcore.* *.log

fclean: clean
	rm -f $(BINARY_NAME) unit_tests

re: fclean all

cs:	clean
	coding-style . .
	cat coding-style-reports.log
	rm -f coding-style-reports.log

linter: clean
	cpplint $(CPPLINT_FLAGS) ./src/

format: clean
	find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.tpp" \) ! \
	-path "./tests/*" -exec clang-format -i {} +
