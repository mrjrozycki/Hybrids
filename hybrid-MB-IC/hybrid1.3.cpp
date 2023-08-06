#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

//------------------MB-BFD -> FFD-------------

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

    // MB-BFD algorithm with m bins and original bin capacities
    Multibin multibin(m, binCapacity_);
    int stackedItemsCount = 0;

    for (const auto& item : items)
    {
        if (stackedItemsCount >= m)
        {
            // If the threshold of stacked items (m) is reached, switch to the FFD algorithm
            std::vector<Item> remainingItems;
            for (int i = stackedItemsCount; i < items.size(); ++i)
            {
                remainingItems.push_back(items[i]);
            }

            // Reset the bins back to their original capacities for the FFD algorithm
            for (auto& bin : multibin.bins)
            {
                bin = Bin(binCapacity_);
            }

            // Continue with the FFD algorithm to pack the remaining items
            std::vector<Bin>& bins = multibin.bins;
            for (const auto& item : remainingItems)
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
                    // If the item couldn't fit in any existing bins, create a new bin
                    Bin newBin(binCapacity_);
                    newBin.addItem(item);
                    bins.push_back(newBin);
                }
            }

            // Display the packed bins using FFD
            std::cout << "Switched to FFD algorithm after stacking " << m << " items." << std::endl;
            std::cout << "Successfully packed the remaining items into bins using the FFD algorithm!" << std::endl;
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
        }

        multibin.addItem(item);
        stackedItemsCount++;
    }

    // Continue with the MB-BFD algorithm to pack the remaining items
    bool success = multibin.packItems();

    if (success)
    {
        // MB-BFD succeeded
        return true;
    }
    else
    {
        // MB-BFD failed, so we try FFD with original bin capacities from the start
        Multibin multibin_ffd(maxBins_, binCapacity_);
        for (const auto& item : items)
        {
            multibin_ffd.addItem(item);
        }

        bool success_ffd = multibin_ffd.packItems();

        if (success_ffd)
        {
            // FFD succeeded
            return true;
        }
        else
        {
            // Both algorithms failed to pack the items
            std::cout << "Both MB-BFD and FFD algorithms failed to pack the items!" << std::endl;
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
    hybridBinPacking.addItem(Item(25));

    auto start_time = std::chrono::high_resolution_clock::now();
    hybridBinPacking.runHybridAlgorithm();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
    return 0;
}
