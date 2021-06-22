# ft_printf tester

This is currently in beta.

This program is a tester for 42's project **ft_printf**.
It runs a series of tests against `ft_printf` and compares the output with 
the original `printf`.

If one test detects that the output is wrong, it will print information about
what should've been print.

TODO:
- [ ] Print useful information in case of errors
    - [x] print the diff between `printf` and `ft_printf`
	- [ ] print the function that was called on that specific test
