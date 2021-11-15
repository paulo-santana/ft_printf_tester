# ft_printf tester
A tester for 42's **ft_printf** project.

## Disclaimer and an advice

Try to not use these tests (or any other tests you didn't code yourself) to develop your project.
**Write your own tests instead***.
Since the C01 piscine list, 42 expects us to use more time writing test code than actual
project code. And this comes with a bunch of somewhat "invisible" benefits, and if you don't practice it you won't notice
you're losing them.

To get better at programming, you need to practice. But the more tests written by other persons you use,
the less tests you'll write yourself. Writing fewer tests means writing less code overall. And blindly following tests
you didn't write means you won't be exercising creativity and could probably squeeze even more your tunnel vision.
And these tests might even be wrong or incomplete.

This whole project started as a "side job" when I was doing my printf. It was actually funnier to work on it and I learned
A LOT about processes, signals, forks and pipes. These concepts are the basis of the minishell project. There's no need for you 
to go this deep if you're a newbee in programming or just don't have enough time, but you shouldn't be neglecting the benefits of
writing more code and learning new stuff.

Do yourself a favor and write your own tests.

#### Why this?
After some intense arguments, I realised this tester could be harmful for people that misuse it, as it is an automation of a
huge part of what is expected by the project (for you to write a big load of tests). I could just destroy this repository and
go on my way, but I know it won't stop people from using it, there are backups everywhere, and I would just be erasing a cool project
from my github profile. So instead I'll leave these advices here, it's up to you to follow them or not. I would recommend using testers only after being
KOed by the moulinette.


-----------------

It runs a series of tests against `ft_printf()` and compares the output with 
the original `printf()`. It works on both linux and mac. On linux and macos the output differs.

If a test detects that the output is wrong, it will print information about
what should've been printed.

It compiles the program with the AddressSanitizer flag, mainly to detect memory leaks
and invalid access. However, it makes the tests run slowly. The option `nosan` disables the
compilation with the address sanitizer, so the tests can run faster. Because the ASAN's LeakSanitizer
doesn't work on Mac, this tester checks for memory leaks using a library called `malloc_count` if running
on Darwin.

If you find any problems, please
[open an issue](https://github.com/paulo-santana/ft_printf_tester/issues/new)

### RTFM

Clone the test repository inside the folder where you generate the `libftprintf.a`
file, then `cd` into it.

The tests are managed maily by a shell script. Here are some command examples:

* `sh test`: run all the tests
* `sh test m`: run all the mandatory tests
* `sh test b1`: run the first bonus tests (`'-0.'` and widths)
* `sh test b2`: run the second bonus tests (`'# +'`)
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
