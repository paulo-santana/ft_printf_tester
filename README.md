# ft_printf tester
Updated for the new specification (2021 2nd semester)

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

The tests are managed maily by a shell script. Here are all the commands:

* `sh test`: run all the tests
* `sh test m`: run all the mandatory tests
* `sh test b1`: run the first bonus tests (`'-0.'` and widths)
* `sh test b2`: run all second bonus tests (`'# +'`)
* `sh test d`: run only the `mandatory` tests related to the `%d` specifier. Currently, only the
`cspdiuxX%` specifiers are supported
* `sh test b1 c`: run the `b1` tests related to the `%c` specifier.
* `sh test b2 X`: run the `b2` tests related to the `%X` specifier.
* `sh test b1 b2`: run all the bonus tests
* `sh test nosan`: run all the tests without the AddressSanitizer
* `sh test nosan m`: run all the mandatory tests without the AddressSanitizer
* `sh test nosan b1`: run the first bonus tests without the AddressSanitizer
* `sh test nosan p`: run all tests related to the `%p` without the AddressSanitizer
* `sh test 1088`: run only the 1088th test

### Limiting errors output

If there are so many errors that you can't even see the whole output, you can
limit the error tolerance by changing the `ERROR_LIMIT` variable in the Makefile.
By default, it is set to 0, which means no limit.

You can also pipe the output to `less`:
`sh test | less -r`


### TODO:
- [x] Print useful information in case of errors
    - [x] print the diff between `printf` and `ft_printf`
    - [x] print the function that was called on that specific test
    - [x] give information about when the returned value was wrong
