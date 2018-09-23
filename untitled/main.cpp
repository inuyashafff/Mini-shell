#include <iostream>
int& foo(int x){
    return x;
}
int main() {
    std::cout<<foo(0);
    return foo(0);
}