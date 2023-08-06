#include <iostream>
#include <vector>
#include<algorithm>

class Item 
{
public:
    int id;
    int size;

    Item(int _id, int _size) : id(_id), size(_size) {}
};

class Bin 
{
public:
    std::vector<Item> items;
    int capacity;

    Bin(int _capacity) : capacity(_capacity) {}

    bool addItem(const Item& item) {
        if (currentCapacity() + item.size <= capacity) 
        {
            items.push_back(item);
            return true;
        }
        return false;
    }

    int currentCapacity() const 
    {
        int usedCapacity = 0;
        for (const auto& item : items) 
        {
            usedCapacity += item.size;
        }
        return usedCapacity;
    }

    bool isFull() const 
    {
        return currentCapacity() == capacity;
    }
};

class Multibin 
{
public:
    Multibin(int maxBins, int incrementStrategy) : maxBins(maxBins), incrementStrategy(incrementStrategy) {}

    void addItem(const Item& item) 
    {
        items.push_back(item);
    }

    bool packItems() 
    {
        int n = 1; 
        while (n <= maxBins) 
        {
            std::vector<Bin> activeBins(n, Bin(binCapacity));
            bool success = packItemsIntoBins(activeBins);
            if (success) 
            {
                bins = activeBins;
                return true;
            }

          
            if (incrementStrategy == 1) 
            {
                n++;
            }
            else {
                n += std::min(incrementStrategy, maxBins - n + 1);
            }
        }

        return false; 
    }

  
    bool packItemsIntoBins(std::vector<Bin>& bins) 
    {
        sort(items.begin(), items.end(), [](const Item& a, const Item& b) 
        {
            return a.size > b.size; // sort items in descending order
        });

        for (const auto& item : items) 
        {
            bool itemPacked = false;
            int bestFitBinIdx = -1;
            int minRemainingSpace = binCapacity + 1; 

            for (int i = 0; i < bins.size(); ++i) 
            {
                int remainingSpace = bins[i].capacity - bins[i].currentCapacity();
                if (remainingSpace >= item.size && remainingSpace < minRemainingSpace) 
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

    void printBins() const 
    {
        for (int i = 0; i < bins.size(); i++) 
        {
            std::cout << "Bin " << i + 1 << ": ";
            for (const auto& item : bins[i].items) 
            {
                std::cout << "Item " << item.id << " (Size: " << item.size << ") ";
            }
            std::cout << std::endl;
        }
    }

private:
    int maxBins;
    int incrementStrategy;
    int binCapacity = 100; 
    std::vector<Item> items;
    std::vector<Bin> bins;
};

int main() 
{

    int maxBins = 4;
    int batchIncrement = 1;

    Multibin multibin(maxBins, batchIncrement);

    multibin.addItem(Item(1, 30)); 
    multibin.addItem(Item(2, 40)); 
    multibin.addItem(Item(3, 20));
    multibin.addItem(Item(4, 21));
    multibin.addItem(Item(5, 18));
    multibin.addItem(Item(6, 2));
    multibin.addItem(Item(7, 11));
    multibin.addItem(Item(8, 5));
    multibin.addItem(Item(9, 7));
    multibin.addItem(Item(10, 43));

    bool success = multibin.packItems();
    if (success) 
    {
        std::cout << "Successfully packed items into bins!" << std::endl;
        multibin.printBins();
    }
    else 
    {
        std::cout << "Failed to find a solution with the maximum number of bins." << std::endl;
    }

    return 0;
}
