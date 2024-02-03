default:
	gcc stack.c -m32 -g -o stack

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

ddt:
	gcc stack_ddt.c -m32 -g -o stack

testddt: ddt
	for i in `ls tests`;\
		do a=`./stack tests/$$i`;\
		if [[ $$a == `cat expect/$$i` ]]; then\
			echo TEST PASSED: $$i;\
		else\
			echo == TEST FAILED: $$i ==;\
		fi;\
	done

testddtisr: ddt
	for i in `ls tests_isr`;\
		do a=`./stack tests_isr/$$i`;\
		if [[ $$a == `cat expect_isr/$$i` ]]; then\
			echo TEST PASSED: $$i;\
		else\
			echo == TEST FAILED: $$i ==;\
		fi;\
	done
