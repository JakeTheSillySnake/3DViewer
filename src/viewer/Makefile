CC = g++ -Wall -Wextra -Werror -std=c++17
LIBS = -I/usr/local/include/ -L/usr/local/lib -lgtest -lpthread

PROJECT_DIR = SimpleViewer
DIST_DIR = SimpleViewer_1.0
BACK = backend
FRONT = frontend
TESTS = ../tests


all: clean install test gcov_report

test: clean
	$(CC) $(BACK)/*.cpp $(TESTS)/*.cpp -o test.out $(LIBS)
	./test.out

gcov_report: clean
	$(CC) --coverage $(BACK)/*.cpp $(TESTS)/*.cpp -lstdc++ -o test.out $(LIBS)
	./test.out
	lcov -t "./test.out" --exclude='*.h' -exclude='*tests*' --ignore-errors source -o report.info --no-external -c -d .
	genhtml --ignore-errors source -o report report.info
	open ./report/index.html

valgrind:
	$(CC) $(BACK)/*.cpp tests/snake* -o test.out $(LIBS)
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes ./test.out
	make clean
	
clang-fix:
	clang-format -i *.h *.cpp $(BACK)/* $(FRONT)/*.cpp $(FRONT)/*.h $(TESTS)/*

install:
	mkdir $(PROJECT_DIR)
	cd $(FRONT); make viewer
	@echo "\Simple Viewer installed successfully.\n"
	cd $(PROJECT_DIR); ./viewer

uninstall:
	rm -rf $(PROJECT_DIR)
	@echo "\Simple Viewer uninstalled.\n"

reinstall: uninstall install

dvi:
	texi2dvi documentation/games_man.tex
	@echo "\nManual was converted to dvi.\n"	

dist:
	mkdir $(DIST_DIR)
	cp -R  brick_game documentation gui tests Makefile $(DIST_DIR)
	tar -czvf $(DIST_DIR).tar.gz $(DIST_DIR)
	rm -rf $(DIST_DIR)
	@echo "\nDistribution directory 'SimpleViewer_1.0' is ready.\n"

rebuild: clean all

clean: rm -f *.o *.a *.gcno *.gcda *.info *.out *.gcov *.gch
	rm -rf *.aux *.dvi *.log *.toc *.tar *.gz *.txt 
	rm -rf ./report *.dSYM
	@echo "\nAll clean!\n"
	
