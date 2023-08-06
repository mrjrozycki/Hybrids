#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include<chrono>

//-------------------IC-BFD -> MB-FFD----------------

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
    bool addItem(const Item& item) {
        if (!canFit(item))
        {
            return false;
        }
        remaining_capacity_ -= item.getSize();
        items.push_back(item);
        return true;
    }
    bool isFull() const { return remaining_capacity_ == 0; }
    int getRemainingCapacity() const { return remaining_capacity_; }
    const std::vector<Item>& getItems() const { return items; }

private:
    int capacity_;
    int remaining_capacity_;
    std::vector<Item> items;
};

bool packItemsIC(const std::vector<Item>& items, std::vector<Bin>& bins, int binCapacity, int thresholdItems)
{
    // Sort items in decreasing order
    std::vector<Item> sortedItems = items;
    std::sort(sortedItems.begin(), sortedItems.end(), [](const Item& a, const Item& b)
        {
            return a.getSize() > b.getSize();
        });

    int itemsPacked = 0;
    for (const auto& item : sortedItems)
    {
        // Best Fit logic for IC-BFD: Find the bin with the smallest remaining capacity
        int bestBinIdx = -1;
        int smallestRemainingCapacity = binCapacity + 1; // Initialize to a value greater than the bin capacity

        for (int i = 0; i < bins.size(); ++i)
        {
            if (bins[i].canFit(item) && bins[i].getRemainingCapacity() < smallestRemainingCapacity)
            {
                bestBinIdx = i;
                smallestRemainingCapacity = bins[i].getRemainingCapacity();
            }
        }

        if (bestBinIdx != -1)
        {
            // Item fits in an existing bin with the best fit, so add it to that bin
            bins[bestBinIdx].addItem(item);
        }
        else
        {
            // Item couldn't fit in any existing bin, so create a new bin and add it
            Bin newBin(binCapacity);
            newBin.addItem(item);
            bins.push_back(newBin);
        }

        itemsPacked++;
        if (itemsPacked >= thresholdItems)
        {
            // Threshold reached, switch to the second stage algorithm
            return false;
        }
    }

    return true;
}

bool packItemsIntoBins(std::vector<Item>& items, std::vector<Bin>& bins, int binCapacity)
{
    // Sort items in decreasing order
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b)
        {
            return a.getSize() > b.getSize();
        });

    for (const auto& item : items)
    {
        bool itemPacked = false;
        for (auto& bin : bins)
        {
            if (!bin.isFull() && bin.addItem(item))
            {
                itemPacked = true;
                break;
            }
        }
        if (!itemPacked)
        {
            return false;
        }
    }
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

int calculateLowerBound(const std::vector<Item>& items, int binCapacity)
{
    int sumItemSizes = 0;
    for (const auto& item : items)
    {
        sumItemSizes += item.getSize();
    }
    return static_cast<int>(std::round(static_cast<double>(sumItemSizes) / binCapacity));
}

bool HybridBinPacking::runHybridAlgorithm()
{
    auto start_time = std::chrono::high_resolution_clock::now();// start the measure

    int lowerBound = calculateLowerBound(items, binCapacity_);
    int m = std::min(maxBins_, lowerBound);

    // Stage 1: IC-BFD Algorithm with threshold based on the number of items
    int thresholdItems = m * binCapacity_; // Example: Threshold based on the number of bins times bin capacity
    std::vector<Bin> binsIC;
    bool successIC = packItemsIC(items, binsIC, binCapacity_, thresholdItems);

    if (successIC)
    {
        // IC-BFD succeeded
        std::cout << "Successfully packed items into bins using the IC-BFD algorithm!" << std::endl;
        for (int i = 0; i < binsIC.size(); i++)
        {
            std::cout << "Bin " << i + 1 << ": ";
            for (const auto& item : binsIC[i].getItems())
            {
                std::cout << "Item (Size: " << item.getSize() << ") ";
            }
            std::cout << std::endl;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        std::cout << "Execution time for IC-BFD: " << duration.count() << " microseconds" << std::endl;

        return true;
    }

    // IC-BFD threshold reached, proceed to stage 2: MB-FFD
    // Initialize MB-FFD with remaining items after IC-BFD
    std::vector<Bin> binsMB = binsIC;
    items.clear();
    for (const auto& binIC : binsIC)
    {
        const auto& itemsInBinIC = binIC.getItems();
        for (const auto& item : itemsInBinIC)
        {
            items.push_back(item);
        }
    }

    // Stage 2: MB-FFD Algorithm with varying max bins
    int maxBinsMB = m; // Start with the lower bound as the initial max bins
    double maxBinsThreshold = 2.0 * static_cast<double>(lowerBound);

    while (maxBinsMB <= maxBinsThreshold)
    {
        binsMB.resize(maxBinsMB, Bin(binCapacity_));
        bool successMB = packItemsIntoBins(items, binsMB, binCapacity_); // Use MB-FFD logic

        if (successMB)
        {
            // MB-FFD succeeded with current maxBinsMB
            std::cout << "Successfully packed items into bins using the MB-FFD algorithm with maxBins=" << maxBinsMB << "!" << std::endl;
            for (int i = 0; i < binsMB.size(); i++)
            {
                std::cout << "Bin " << i + 1 << ": ";
                for (const auto& item : binsMB[i].getItems())
                {
                    std::cout << "Item (Size: " << item.getSize() << ") ";
                }
                std::cout << std::endl;
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            std::cout << "Execution time for MB-FFD: " << duration.count() << " microseconds" << std::endl;
            return true;
        }

        maxBinsMB += batchIncrement_;
    }

    // Both IC-BFD and MB-FFD failed
    std::cout << "Both IC-BFD and MB-FFD algorithms have failed. No feasible solution found." << std::endl;
    return false;
}

int main()
{
    int maxBins = 3;
    int batchIncrement = 1;
    int binCapacity = 100;

    HybridBinPacking hybridBinPacking(maxBins, batchIncrement, binCapacity);

    hybridBinPacking.addItem(Item(10));
    hybridBinPacking.addItem(Item(21));
    hybridBinPacking.addItem(Item(44));

    bool success = hybridBinPacking.runHybridAlgorithm();

    if (!success) 
    {
        std::cout << "Both IC-BFD and MB-FFD algorithms have failed. No feasible solution found." << std::endl;
    }

    return 0;
}
