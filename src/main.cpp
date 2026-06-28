#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "memory_manager.h"
#include "cache.h"

using namespace std;

int main()
{
    MemoryManager mem;

    Cache L1(64, 16, 1);  // 64B cache, 16B block, direct mapped
    Cache L2(256, 16, 4); // 256B cache, 4-way set associative

    int l1_to_l2 = 0;
    int l2_to_mem = 0;

    cout << "Memory Management Simulator\n";
    cout << "Type 'help' to see commands\n";

    string line;
    while (true)
    {
        cout << "> ";
        getline(cin, line);

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit")
        {
            break;
        }

        else if (cmd == "help")
        {
            cout << "\nCommands:\n";
            cout << "init memory <size>\n";
            cout << "set allocator <first_fit | best_fit | worst_fit | buddy>\n";
            cout << "malloc <size>\n";
            cout << "free <id>\n";
            cout << "dump memory\n";
            cout << "stats\n";
            cout << "access <address>\n";
            cout << "exit\n\n";
        }

        else if (cmd == "init")
        {
            string temp;
            int size;
            ss >> temp >> size;
            mem.init(size);

            cout << "Memory initialized with size " << size << "\n";
        }

        else if (cmd == "set")
        {
            string temp, type;
            ss >> temp >> type;

            if (type == "first_fit")
                mem.setAllocator(AllocatorType::FIRST_FIT);
            else if (type == "best_fit")
                mem.setAllocator(AllocatorType::BEST_FIT);
            else if (type == "worst_fit")
                mem.setAllocator(AllocatorType::WORST_FIT);
            else if (type == "buddy")
                mem.setAllocator(AllocatorType::BUDDY);
            else
            {
                cout << "Unknown allocator\n";
                continue;
            }

            cout << "Allocator set to " << type << "\n";
        }

        else if (cmd == "malloc")
        {
            int size;
            ss >> size;

            int address;
            int id = mem.allocate(size, address);

            if (id == -1)
                cout << "Allocation failed\n";
            else
                cout << "Allocated block id=" << id
                     << " at address=0x"
                     << hex << setw(4) << setfill('0') << address
                     << dec << "\n";
        }

        else if (cmd == "free")
        {
            int id;
            ss >> id;
            if (mem.freeBlock(id))
                cout << "Block " << id << " freed\n";
            else
                cout << "Invalid block id\n";
        }

        else if (cmd == "dump")
        {
            string temp;
            ss >> temp;
            mem.dumpMemory();
        }

        else if (cmd == "stats")
        {
            mem.printStats();

            cout << "\nCache Stats:\n";

            int l1_hits = L1.getHits();
            int l1_misses = L1.getMisses();
            int l2_hits = L2.getHits();
            int l2_misses = L2.getMisses();

            cout << "L1 Hits: " << l1_hits
                 << " Misses: " << l1_misses << "\n";

            if (l1_hits + l1_misses > 0)
                cout << "L1 Hit Ratio: "
                     << (double)l1_hits / (l1_hits + l1_misses) << "\n";

            cout << "L2 Hits: " << l2_hits
                 << " Misses: " << l2_misses << "\n";

            if (l2_hits + l2_misses > 0)
                cout << "L2 Hit Ratio: "
                     << (double)l2_hits / (l2_hits + l2_misses) << "\n";

            cout << "\nMiss Propagation:\n";
            cout << "L1 Misses forwarded to L2: " << l1_to_l2 << "\n";
            cout << "L2 Misses forwarded to Memory: " << l2_to_mem << "\n";
        }

        else if (cmd == "access")
        {
            int address;
            ss >> address;

            if (!L1.access(address))
            {
                l1_to_l2++;

                if (!L2.access(address))
                {
                    l2_to_mem++;
                }
            }
        }
        else
        {
            cout << "Unknown command. Type 'help'.\n";
        }
    }

    cout << "Exiting simulator...\n";
    return 0;
}
