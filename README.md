# Internet Speed Test

This project is a simple C program that tests internet speed.

## Build

To compile the program, run:

```bash
make
```
After compiling, run the program with:

```bash
./speedtest [options]
```

Options:  
-c <country> : Search servers by country  
-s <server> : Specify server directly  
-d : Run download test (-c or -s required)  
-u : Run upload test (-c or -s required)  
-b : Find best server (-c required)  
-l : Get user location  
-a : Run full automated test (overrides all other options)  
-h : Show help message  

To remove compiled files, run:

```bash
make clean
```
