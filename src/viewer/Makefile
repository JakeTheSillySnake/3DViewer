PROJECT_DIR = SimpleViewer
DIST_DIR = SimpleViewer_1.0
BACK = backend
FRONT = frontend
TESTS = tests


all: clean install test gcov_report

test: clean
	cd $(FRONT); make test
	cd $(PROJECT_DIR); ./test.out

gcov_report: clean
	cd $(FRONT); make gcov
	open ./report/index.html

valgrind: 
	cd $(FRONT); make test
	cd $(PROJECT_DIR); valgrind --tool=memcheck --leak-check=yes --track-origins=yes ./test.out
	
clang-fix:
	clang-format -i $(BACK)/* $(FRONT)/*.cpp $(FRONT)/*.h $(TESTS)/*

install:
	mkdir $(PROJECT_DIR)
	cd $(FRONT); make viewer
	@echo "\nSimple Viewer installed successfully.\n"
	cd $(PROJECT_DIR); ./SimpleViewer

uninstall: clean
	rm -rf $(PROJECT_DIR)
	@echo "\nSimple Viewer uninstalled.\n"

reinstall: uninstall install

dvi:
	texi2dvi documentation/games_man.tex
	@echo "\nManual was converted to dvi.\n"	

dist:
	mkdir $(DIST_DIR)
	cp -R  backend frontend tests main.cpp ../assets ../external ../documentation Makefile $(DIST_DIR)
	tar -czvf $(DIST_DIR).tar.gz $(DIST_DIR)
	rm -rf $(DIST_DIR)
	@echo "\nDistribution directory 'SimpleViewer_1.0' is ready.\n"

rebuild: clean all

clean: 
	rm -rf *.aux *.dvi *.log *.toc *.tar *.gz *.txt 
	rm -rf ./report *.dSYM *.info
	@echo "\nAll clean!\n"
	
