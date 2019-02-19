# Teleraph

A C++ networking project that allows programmers to make [LAN](https://en.wikipedia.org/wiki/Local_area_network "Local Area Network") C++ networking applications with ease.

---

### Installing

Just download the files of the repository into your project. It's that easy!

---

---

### Using teleraph

The Telegraph project, as you would expect, provides a header file called "teleraph.h", which contains the capabilty of using servers and clients. In order to do so, it gives you a nice little namespace called `net` which contains the `Server` and `Client` classes, so if you'd want to use those classes (which you do, of course), you'd either need to write `net::` before the object name, or write `using namespace net;` at the beginning of your code.

---

### Good networking programming notes - The "Dos and Don'ts"

1. Don't start two or more servers binded to the same port on the same machine.\
If you do, the behavior of both the servers and the clients connected to them is then undefined.\
for example, the following code will cause undeined behaviour:
```C++
#include "teleraph.h"
using namespace net;

int main()
{
    Server s1("33565");
    Server s2("33565");
    //We started two servers that are binded to the same port, on the same machine,
    //and therefore the behaviour of the servers and their clients is undefined.
    
    //some more code using the servers
    return 0;
}
```

2. Send as little data as you can. sending a lot of data is known to catch a lot of [bandwidth](https://en.wikipedia.org/wiki/Bandwidth_(computing) "Bandwith (computing)") and causes your program to be called a **fat application**, Nobody wants that. If you have constant data that isn't going to change throughout the run-time of your program, you'd want to keep it in both your client and your server instead of sending it across the internet. The less data you send, the better networking programmer you are.