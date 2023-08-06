#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

//------------------MB-BFD -> BC-------------

class Item
{
public:
    Item(int size) : size_(size) {}
    int getSize() const { return size_; }

private:
    int size_;
};

class Bin
{
public:
    Bin(int capacity) : capacity_(capacity), remaining_capacity_(capacity) {}
    bool canFit(const Item& item) const { return remaining_capacity_ >= item.getSize(); }
    void addItem(const Item& item) { remaining_capacity_ -= item.getSize(); items.push_back(item); }
    int getRemainingCapacity() const { return remaining_capacity_; }
    const std::vector<Item>& getItems() const { return items; }

private:
    int capacity_;
    int remaining_capacity_;
    std::vector<Item> items;
};

class Multibin
{
public:
    Multibin(int maxBins, int binCapacity) : maxBins_(maxBins), binCapacity_(binCapacity) {}

    void addItem(const Item& item);
    bool packItems();

    void printBins() const
    {
        for (int i = 0; i < bins.size(); i++)
        {
            std::cout << "Bin " << i + 1 << ": ";
            for (const auto& item : bins[i].getItems())
            {
                std::cout << "Item (Size: " << item.getSize() << ") ";
            }
            std::cout << std::endl;
        }
    }

private:
    int maxBins_;
    int binCapacity_;
    std::vector<Item> items;
public:
    std::vector<Bin> bins;
};

void Multibin::addItem(const Item& item)
{
    items.push_back(item);
}

bool Multibin::packItems()
{
    // Sort items in decreasing order
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getSize() > b.getSize();
        });

    // Initialize m bins
    bins.resize(maxBins_, Bin(binCapacity_));

    for (const auto& item : items)
    {
        bool itemPacked = false;
        for (int i = 0; i < bins.size(); ++i)
        {
            if (bins[i].canFit(item))
            {
                bins[i].addItem(item);
                itemPacked = true;
                break;
            }
        }
        if (!itemPacked)
        {
            // If the item couldn't fit in any existing bins, return false (MB-BFD failed)
            return false;
        }
    }

    // Display the packed bins
    std::cout << "Successfully packed items into bins using the MB-BFD algorithm!" << std::endl;
    printBins();
    return true;
}

class HybridBinPacking
{
public:
    HybridBinPacking(int maxBins, int batchIncrement, int binCapacity)
        : maxBins_(maxBins), batchIncrement_(batchIncrement), binCapacity_(binCapacity) {}

    void addItem(const Item& item);
    bool runHybridAlgorithm();

private:
    int maxBins_;
    int batchIncrement_;
    int binCapacity_;
    std::vector<Item> items;
};

void HybridBinPacking::addItem(const Item& item)
{
    items.push_back(item);
}

bool HybridBinPacking::runHybridAlgorithm()
{
   

    int lowerBound = 0;
    for (const auto& item : items)
    {
        lowerBound += item.getSize();
    }

    int m = std::min(maxBins_, lowerBound);

    // MB-BFD algorithm with reduced bin capacity
    Multibin multibin(m, binCapacity_ / 2); // Reduce the bin capacity for MB-BFD
    for (const auto& item : items)
    {
        multibin.addItem(item);
    }

    bool success = multibin.packItems();

    if (success)
    {
        // MB-BFD succeeded, now restore the original bin capacities for the BC algorithm
        for (auto& bin : multibin.bins)
        {
            bin = Bin(binCapacity_);
        }

        // Continue with the BC algorithm to pack the remaining items
        std::vector<Bin>& bins = multibin.bins;
        std::vector<Item> remainingItems;
        for (int i = m; i < items.size(); ++i)
        {
            remainingItems.push_back(items[i]);
        }

        for (const auto& item : remainingItems)
        {
            bool itemPacked = false;
            for (Bin& bin : bins) {
                if (bins.addItem(item)) {
                    itemPacked = true;
                    break;
                }
            }
            if (!itemPacked) {
                Bin newBin(binCapacity_);
                newBin.addItem(item);
                bins.push_back(newBin);
        }

        // Display the packed bins using BC
        std::cout << "Successfully packed items into bins using the BC algorithm!" << std::endl;
        int binNumber = 1;
        for (const auto& bin : bins)
        {
            std::cout << "Bin " << binNumber << ": ";
            for (const auto& item : bin.getItems())
            {
                std::cout << "Item (Size: " << item.getSize() << ") ";
            }
            std::cout << std::endl;
            binNumber++;
        }

        return true;
        

        return 0;
    }
    else
    {
        // MB-BFD failed, so we try BC with original bin capacities from the start
        Multibin multibin_BC(maxBins_, binCapacity_);
        for (const auto& item : items)
        {
            multibin_BC.addItem(item);
        }

        bool success_BC = multibin_BC.packItems();

        if (success_BC)
        {
            // BC succeeded
            return true;
        }
        else
        {
            // Both algorithms failed to pack the items
            std::cout << "Both MB-BFD and BC algorithms failed to pack the items!" << std::endl;
            return false;
        }
    }
}

int main()
{
    int maxBins = 3;
    int batchIncrement = 1;
    int binCapacity = 100;

    HybridBinPacking hybridBinPacking(maxBins, batchIncrement, binCapacity);

    hybridBinPacking.addItem(Item(30));
    hybridBinPacking.addItem(Item(40));
    hybridBinPacking.addItem(Item(60));
    hybridBinPacking.addItem(Item(70));
    hybridBinPacking.addItem(Item(10));
    hybridBinPacking.addItem(Item(80));

    auto start_time = std::chrono::high_resolution_clock::now();
    hybridBinPacking.runHybridAlgorithm();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Total Execution time: " << duration.count() << " microseconds" << std::endl;
    return 0;
}
