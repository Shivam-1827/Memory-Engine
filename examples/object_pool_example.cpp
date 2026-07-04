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
    memory_engine::ObjectPool<User> pool(1);

    void *firstAddress;

    {
        auto user = pool.acquire();
        firstAddress = user.get();

        std::cout << firstAddress << '\n';
    }

    {
        auto user = pool.acquire();

        std::cout << user.get() << '\n';
    }
}