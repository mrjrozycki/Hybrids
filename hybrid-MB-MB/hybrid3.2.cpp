#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

class Item
{
public:
    Item(int id, int size) : id_(id), size_(size) {}
    int getId() const { return id_; }
    int getSize() const { return size_; }

private:
    int id_;
    int size_;
};

class Bin
{
public:
    Bin(int capacity) : capacity_(capacity), remaining_capacity_(capacity) {}
    bool canFit(const Item& item) const { return remaining_capacity_ >= item.getSize(); }
    bool addItem(const Item& item)
    {
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

class MBBinPacking
{
public:
    MBBinPacking(int binCapacity) : binCapacity_(binCapacity) {}
    void addItem(const Item& item);
    int getNumBins() const { return bins_.size(); }
    const std::vector<Bin>& getBins() const { return bins_; }

private:
    int binCapacity_;
    std::vector<Bin> bins_;
};

void MBBinPacking::addItem(const Item& item)
{
    auto it = std::min_element(bins_.begin(), bins_.end(), [&](const Bin& bin1, const Bin& bin2) {
        return bin1.getRemainingCapacity() < bin2.getRemainingCapacity() && bin1.canFit(item);
        });

    if (it != bins_.end()) {
        it->addItem(item);
    }
    else {
        Bin new_bin(binCapacity_);
        new_bin.addItem(item);
        bins_.push_back(new_bin);
    }
}

bool packItemsIntoBins(std::vector<Item>& items, std::vector<Bin>& bins, int binCapacity)
{
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.getSize() > b.getSize(); // Sort items in descending order
        });

    for (const auto& item : items)
    {
        bool itemPacked = false;
        int bestFitBinIdx = -1;
        int minRemainingSpace = binCapacity + 1;

        for (int i = 0; i < bins.size(); ++i)
        {
            int remainingSpace = bins[i].getRemainingCapacity();
            if (remainingSpace >= item.getSize() && remainingSpace < minRemainingSpace)
            {
                bestFitBinIdx = i;
                minRemainingSpace = remainingSpace;
            }
        }

        if (bestFitBinIdx != -1)
        {
            bins[bestFitBinIdx].addItem(item);
            itemPacked = true;
        }

        if (!itemPacked)
        {
            return false;
        }
    }
    return true;
}

class Multibin
{
public:
    Multibin(int maxBins, int incrementStrategy, int threshold)
        : maxBins_(maxBins), incrementStrategy_(incrementStrategy), threshold_(threshold) {}

    void addItem(const Item& item)
    {
        items_.push_back(item);
    }

    bool packItems()
    {
        int n = 1;
        while (n <= maxBins_)
        {
            int currentBinCapacity = (n <= threshold_) ? n : binCapacity_;
            std::vector<Bin> activeBins(n, Bin(currentBinCapacity));
            bool success = packItemsIntoBins(items_, activeBins, currentBinCapacity);
            if (success)
            {
                bins_ = activeBins;
                return true;
            }

            if (incrementStrategy_ == 1)
            {
                n++;
            }
            else
            {
                n += std::min(incrementStrategy_, maxBins_ - n + 1);
            }
        }

        return false;
    }

    void printBins() const
    {
        for (int i = 0; i < bins_.size(); i++)
        {
            std::cout << "Bin " << i + 1 << ": ";
            for (const auto& item : bins_[i].getItems())
            {
                std::cout << "Item " << item.getId() << " (Size: " << item.getSize() << ") ";
            }
            std::cout << std::endl;
        }
    }

private:
    int maxBins_;
    int incrementStrategy_;
    int threshold_;
    int binCapacity_ = 100;
    std::vector<Item> items_;
    std::vector<Bin> bins_;
};

class HybridMultibin
{
public:
    HybridMultibin(int maxBins, int batchIncrement, int binCapacity, int threshold)
        : maxBins_(maxBins), batchIncrement_(batchIncrement), binCapacity_(binCapacity), threshold_(threshold) {}

    void addItem(const Item& item)
    {
        items_.push_back(item);
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

    bool runHybridAlgorithm()
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        // Calculate the lower bound
        int lowerBound = calculateLowerBound(items_, binCapacity_);

        int currentBins = std::min(2 * lowerBound, maxBins_);

        // Stage 1: MB-BFD Algorithm
        Multibin multibinBFD(currentBins, batchIncrement_, threshold_);

        for (const auto& item : items_)
        {
            multibinBFD.addItem(item);
        }

        bool success = multibinBFD.packItems();
        if (success)
        {
            std::cout << "Successfully packed items into bins using the MB-BFD algorithm!" << std::endl;
            multibinBFD.printBins();
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            std::cout << "Execution time for MB-BFD: " << duration.count() << " microseconds" << std::endl;

            return true;
        }
        else
        {
            std::cout << "\nMB-BFD-based approach failed. Switching to MB-FFD algorithm..." << std::endl;
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            std::cout << "\nExecution time for MB-BFD: " << duration.count() << " microseconds" << std::endl;
        }

        // Stage 2: MB-FFD Algorithm
        MBBinPacking multibinFFD(binCapacity_);
        for (const auto& item : items_)
        {
            multibinFFD.addItem(item);
        }

        if (multibinFFD.getNumBins() <= maxBins_)
        {
            std::cout << "\nSuccessfully packed remaining items into bins using the MB-FFD algorithm!" << std::endl;
            for (int i = 0; i < multibinFFD.getNumBins(); i++)
            {
                std::cout << "Bin " << i + 1 << ": ";
                for (const auto& item : multibinFFD.getBins()[i].getItems())
                {
                    std::cout << "Item " << item.getId() << " (Size: " << item.getSize() << ") ";
                }
                std::cout << std::endl;
            }
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            std::cout << "\nExecution time for MB-FFD: " << duration.count() << " microseconds" << std::endl;
            return true;
        }

        return false;
    }

private:
    int maxBins_;
    int batchIncrement_;
    int binCapacity_;
    int threshold_;
    std::vector<Item> items_;
};



int main()
{
    int maxBins = 3;
    int batchIncrement = 1;
    int binCapacity = 100;
    int thresholdContainerVal = 80; 

    HybridMultibin hybridMultibin(maxBins, batchIncrement, binCapacity, thresholdContainerVal);

    hybridMultibin.addItem(Item(1, 30));
    hybridMultibin.addItem(Item(2, 40));
    hybridMultibin.addItem(Item(3, 60));
    hybridMultibin.addItem(Item(4, 70));
    hybridMultibin.addItem(Item(5, 10));
    hybridMultibin.addItem(Item(6, 80));

    bool success = hybridMultibin.runHybridAlgorithm();

    if (!success)
    {
        std::cout << "Both MB-BFD and MB-FFD algorithms have failed. No feasible solution found." << std::endl;
    }

    return 0;
}
