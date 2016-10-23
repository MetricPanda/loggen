# loggen

Simple category-based logger generator for C/C++.

``` C
int main()
{
  GlobalVerbosity = LOG_EVERYTHING;
  Log(VERBOSE|PLATFORM, "Startup");
  Log(INFO|VIDEO, "Video init");
  Log(AUDIO|ERROR|INIT, "Fatal error");
}
```

Will output:

```
[VERBOSE][PLATFORM][          ]  test/demo.c:   9 Startup
[INFO   ][VIDEO   ][          ]  test/demo.c:  10 Video init
[ERROR  ][AUDIO   ][INIT      ]  test/demo.c:  11 Fatal error
```

## Usage

- Edit the `main()` function and add the log categories and labels you wish to use.
- Build with `make` or compile with a GNU C99 supporting compiler
- Run the `loggen` executable and redirect the output to a blank file like so:
  `./loggen > logger.h`
- Include `logger.h` in your source tree

## Customization

Edit the source code or the generated code to customize the behavior of the logger.


## License

[Public Domain](/LICENSE)
