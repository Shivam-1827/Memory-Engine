#include <iostream>
#include "memory_engine/object_pool.hpp"

struct User
{
    int id = 0;

    User()
    {
        std::cout << "User Constructor\n";
    }

    ~User()
    {
        std::cout << "User Destructor\n";
    }
};

int main()
{
    std::cout << "[object_pool_example] starting\n";
    std::cout << "  input: ObjectPool<User>(capacity=1), acquire() twice with release between scopes\n";

    memory_engine::ObjectPool<User> pool(1);

    void *firstAddress;

    {
        auto user = pool.acquire();
        firstAddress = user.get();

        std::cout << "  output: first acquire returned " << firstAddress << '\n';
    }

    {
        auto user = pool.acquire();

        std::cout << "  output: second acquire returned " << user.get() << '\n';
        std::cout << "  output: reused address = " << std::boolalpha << (user.get() == firstAddress) << '\n';
    }

    std::cout << "object_pool_example passed\n";
}