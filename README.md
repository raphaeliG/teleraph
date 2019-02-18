# Teleraph

---

A C++ networking project that allows programmers to make C++ networking applications with ease.
With Teleraph, and just a bit of 

### Installing

Just download the files into your project. It's that easy!

### Using teleraph



### Good networking programming notes

1. Don't start two or more servers binded to the same port on the same machine.\
If you do, behavior of both the servers and the clients connected to them is then undefined.\
for example, the following code will cause undeined behaviour:
```C++
#include "teleraph.h"
using namespace net;

int main()
{
  Server s1("33565");
  Server s2("33565");
  
  /* some code of you using the two servers... */
  
  return 0;
}
```

2. Send as little data as you can. sending a lot of data is known to catch a lot of [bandwidth](https://en.wikipedia.org/wiki/Bandwidth_(computing) "Bandwith (computing)") and causes your program to be called a **fat application**.\
Nobody wants that.
