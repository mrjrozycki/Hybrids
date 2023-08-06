#include <iostream>
#include <vector>

class Item {
public:
    Item(double weight, double value) : weight(weight), value(value) {}

    double getWeight() const { return weight; }
    double getValue() const { return value; }

private:
    double weight;
    double value;
};

class Bin {
public:
    Bin(double capacity) : capacity(capacity), currentWeight(0), totalValue(0) {}

    bool addItem(const Item& item) {
        if (currentWeight + item.getWeight() <= capacity) {
            items.push_back(item);
            currentWeight += item.getWeight();
            totalValue += item.getValue();
            return true;
        }
        return false;
    }

    double getScore() const { return totalValue; }
    const std::vector<Item>& getItems() const { return items; }

private:
    double capacity;
    double currentWeight;
    double totalValue;
    std::vector<Item> items;
};

class BinCentricAlgorithm {
public:
    BinCentricAlgorithm(double binCapacity) : binCapacity(binCapacity) {}

    void addItem(const Item& item) {
        items.push_back(item);
    }

    void runAlgorithm() {
        bins.clear();
        for (const Item& item : items) {
            bool itemAdded = false;
            for (Bin& bin : bins) {
                if (bin.addItem(item)) {
                    itemAdded = true;
                    break;
                }
            }
            if (!itemAdded) {
                Bin newBin(binCapacity);
                newBin.addItem(item);
                bins.push_back(newBin);
            }
        }
    }

    void calculateScores() {
        for (Bin& bin : bins) {
            binScore.push_back(bin.getScore());
        }
    }

    int getNumBinsUsed() const {
        return bins.size();
    }

    std::vector<Bin> getBins() const {
        return bins;
    }

    void printBins() const {
        int binNumber = 1;
        for (const Bin& bin : bins) {
            std::cout << "Bin " << binNumber++ << ": Score=" << bin.getScore() << ", Items=";
            for (const Item& item : bin.getItems()) {
                std::cout << "(Weight=" << item.getWeight() << ", Value=" << item.getValue() << ") ";
            }
            std::cout << std::endl;
        }
    }

    void printBinScores() const {
        int binNumber = 1;
        for (double score : binScore) {
            std::cout << "Bin " << binNumber++ << " Score: " << score << std::endl;
        }
    }

private:
    double binCapacity;
    std::vector<Item> items;
    std::vector<Bin> bins;
    std::vector<double> binScore;
};

int main() {
    // Implementing the usage
    BinCentricAlgorithm algorithm(10.0); // Bin capacity is 10.0

    // Adding individual items
    algorithm.addItem(Item(3.0, 6.0)); 
    algorithm.addItem(Item(4.0, 5.0));
    algorithm.addItem(Item(2.0, 3.0));
    algorithm.addItem(Item(5.0, 8.0));


    algorithm.runAlgorithm();
    algorithm.calculateScores();
    algorithm.printBins();
    algorithm.printBinScores();

    // Now here i tell about the bins that are used and their respective items
    std::cout << "Number of bins used: " << algorithm.getNumBinsUsed() << std::endl;
    std::cout << "Bins Contents:" << std::endl;
    int binNumber = 1;
    for (const Bin& bin : algorithm.getBins()) {
        std::cout << "Bin " << binNumber++ << ": ";
        for (const Item& item : bin.getItems()) {
            std::cout << "(Weight=" << item.getWeight() << ", Value=" << item.getValue() << ") ";
        }
        std::cout << std::endl;
    }

    return 0;
}
