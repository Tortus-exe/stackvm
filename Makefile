default:
	gcc stack.c -g -o stack

clean:
	rm stack

test: default
	for i in `ls tests`;\
		do a=`./stack tests/$$i`;\
		if [[ $$a == `cat expect/$$i` ]]; then\
			echo TEST PASSED: $$i;\
		else\
			echo == TEST FAILED: $$i ==;\
		fi;\
	done
