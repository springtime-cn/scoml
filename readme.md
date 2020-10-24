## socket communication over multi-languages


### message structure
![data](imgs/explain.png)
### example1
![data](imgs/example1.png)
### example2
![data](imgs/example2.png)

<p align="center"> 
<img src="imgs/data.pdf">
</p>

1. Simple. both send and received messages are string.

2. default maximal 999999999999 Bytes message body, that is maximum 931.3GB in one send or receive. You also can increase the ori_len(oriLen) parameter to support more data transfer at a time.

3. Even tested on Ubuntu18.04, and it should be run well on unix-like systems.

4. Code on other language is comming. 


## Usage

### Server
#### Python3

```Python
from varSocket import ServerSocket

class Handler(ServerSocket):
    def response(self, msg):
        # You should write your execute code here.
        print("received msg ", msg)
        return msg.upper()

hand = Handler()
hand.start(12000)
```

#### C++
C++11 above
```C++
#include <string>                                                                                               
#include <iostream>                                                                                             
#include "varSocket.h"                                                                                          
                                                                                                                
std::string ServerSocket::response(std::string &msg){
    // You should write your execute code here.
    std::string answer += "abc";
    return answer; 
                                                                                                                
int main(){ 
    std::string ip("localhost");
    std::string port("12000");
    ServerSocket server(ip, port);
    server.start(); 
    return 0;
}
```


### Client
#### Python3

```Python
from varSocket import var_request
answer = var_request("localhost", 12000, message= "hello world")
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
std::string answer = var_request(ip, port, msg);
std::cout << answer << std::endl;
return 0;
```
