# ft_printf tester

This is currently in alpha pre-release.

This program is a tester for 42's project **ft_printf**.
It runs a series of tests against `ft_printf` and compares the output with 
the original `printf`. It works on both linux and mac. On linux, it forces the
output of the original `printf` to equal the mac's one.

If a test detects that the output is wrong, it will print information about
what should've been print.

### RTFM

Clone the test repository inside the folder where you generate the `libftprintf.a`
file, then `cd` into it.

The tests run mainly through `make`. Here are all the commands:

* `make`: run all the mandatory tests
* `make d`: run all the tests related to the `%d` specifier. Currently, only the
`cspdiuxX%` specifiers are supported
* `make nosan run`: run all the mandatory tests without the AddressSanitizer
* `make nosan p`: run all tests related to the `%p` without the AddressSanitizer
* `make 1088`: run only the 1088th test

### Limiting errors output

If there are so many errors that you can't even see the whole output, you can
limit the error tolerance by changing the `ERROR_LIMIT` variable in the Makefile


### TODO:
- [ ] Print useful information in case of errors
    - [x] print the diff between `printf` and `ft_printf`
    - [x] print the function that was called on that specific test
    - [ ] give information about when the returned value was wrong
