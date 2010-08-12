COMPILE_STATUS = $(shell ./compile.sh)
COMPILE_RESULTS = $(shell cat ./compile_results.txt)

tree_vis: 
	$(cat ./compile_results.txt $(COMPILE_STATUS) $(COMPILE_RESULTS)) 
