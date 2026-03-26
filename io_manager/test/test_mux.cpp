#include <iostream>
#include <thread>
#include <chrono>

#include "io_manager/factory/mux_factory.hpp"

int main()
{
    auto mux = create_mux(17, 27, 22);

    if (!mux->init())
    {
        std::cerr << "Erro ao inicializar MUX\n";
        return -1;
    }

    std::cout << "MUX inicializado\n";

    while (true)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            std::cout << "Canal: " << (int)i << std::endl;

            mux->set(i);

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}