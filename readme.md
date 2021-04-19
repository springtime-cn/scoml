## socket communication over multi-languages

You can use it to communication over multi-languages very simply. For example, the communication between Python and C++ when using C++ sending an image to Python classification model and receiving the result. In this case, make a server using Python and using C++ code as a client to send image and receive answer. You can simply using the following code to implement it. 

### message structure
![data](imgs/explain.png)
### example1
![data](imgs/example1.png)
### example2
![data](imgs/example2.png)


1. Simple. both send and received messages are string.

2. default maximal 999999999999 Bytes message body, that is maximum 931.3GB in one send or receive. You also can increase the ori_len(oriLen) parameter to support more data transfer at a time.

3. Even tested on Ubuntu18.04, and it should be run well on unix-like systems.

4. Code on other language is comming. 


## Usage

### Server
#### Python3

```Python
from scomlVarSocket import ScomlServerSocket

class Handler(ScomlServerSocket):
    def response(self, msg):
        # You should write your execute code here.
        print("received msg ", msg)
        return b'abc'

hand = Handler()
hand.start(12000)
```

#### C++
C++11 above
```C++
#include <string>                                                                                               
#include <iostream>                                                                                             
#include "varSocket.h"                                                                                          
                                                                                                                
std::string scoml::ServerSocket::response(std::string &msg){
    // You should write your execute code here.
    std::string answer += "abc";
    return answer; 
                                                                                                                
int main(){ 
    std::string ip("localhost");
    std::string port("12000");
    scoml::ServerSocket server(ip, port);
    server.start(); 
    return 0;
}
```


### Client
#### Python3

```Python
from scomlVarSocket import scoml_var_request
answer = scoml_var_request("localhost", 12000, message= "hello world")
print(answer)
```


#### C++
C++11 above
```C++
#include <string>
#include <iostream>
#include "varSocket.h"

int main(){
std::string ip("localhost");
std::string port("12000"); 
std::string msg("abc"); 
std::string answer = scoml::var_request(ip, port, msg);
std::cout << answer << std::endl;
return 0;
```
