## Setup

### Running

You can compile & run this project using `cmake` & `make`.

Simply run `cmake .` in the root project directory to generate a Makefile, and then run `make` -- this will output an executable named `depthify` that outputs the price-depth snapshot of the orderbook every time an order event warrants a change in the first `n` levels on the bid or the ask side.

You can then use it by passing in the exchange stream into the stdin of the process, and passing a number as a command line argument; for example, to output (to stdout) the first five levels changing for `input1.stream`:
```
cat input1.stream | ./depthify 5
```

#### TL;DR
In the root of your directory:

```
cmake .
make
cat <stream_name> | ./depthify <depth>
```

You can also see the human-readable exchange order information on stderr by setting the `DEBUG` environment variable to true.

viv
