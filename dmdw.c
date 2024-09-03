#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 10
#define MAX_TRANSACTIONS 100
#define MAX_ITEMSETS 100
#define MAX_MAPPING 100
#define MAX_KEY_LEN 10

typedef struct {
    char items[MAX_ITEMS][MAX_KEY_LEN];
    int itemCount;
} Transaction;

typedef struct {
    char items[MAX_ITEMS][MAX_KEY_LEN];
    int itemCount;
    int support;
} Itemset;

typedef struct {
    char key[MAX_KEY_LEN];
    Itemset itemset;
} Mapping;

Transaction transactions[MAX_TRANSACTIONS];
int transactionCount = 0;
Itemset frequentItemsets[MAX_ITEMSETS];
int itemsetCount = 0;
Mapping mapping[MAX_MAPPING];
int mappingCount = 0;
Transaction compressedTransactions[MAX_TRANSACTIONS];

void addItemset(char items[MAX_ITEMS][MAX_KEY_LEN], int itemCount, int support) {
    Itemset *itemset = &frequentItemsets[itemsetCount++];
    itemset->itemCount = itemCount;
    itemset->support = support;
    for (int i = 0; i < itemCount; i++) {
        strcpy(itemset->items[i], items[i]);
    }
}

void generateFrequentItemsets(int minSupport) {
    // Generating frequent itemsets of size 1
    for (int i = 0; i < transactionCount; i++) {
        for (int j = 0; j < transactions[i].itemCount; j++) {
            char items[1][MAX_KEY_LEN];
            strcpy(items[0], transactions[i].items[j]);
            int found = 0;
            for (int k = 0; k < itemsetCount; k++) {
                if (strcmp(frequentItemsets[k].items[0], items[0]) == 0) {
                    frequentItemsets[k].support++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                addItemset(items, 1, 1);
            }
        }
    }

    // Filter itemsets by minimum support
    for (int i = 0; i < itemsetCount; i++) {
        if (frequentItemsets[i].support < minSupport) {
            for (int j = i; j < itemsetCount - 1; j++) {
                frequentItemsets[j] = frequentItemsets[j + 1];
            }
            itemsetCount--;
            i--;
        }
    }

    // Generating frequent itemsets of larger sizes
    for (int size = 2; size <= MAX_ITEMS; size++) {
        int prevItemsetCount = itemsetCount;
        for (int i = 0; i < prevItemsetCount; i++) {
            for (int j = i + 1; j < prevItemsetCount; j++) {
                char newItems[MAX_ITEMS][MAX_KEY_LEN];
                int newItemCount = 0;

                for (int k = 0; k < size - 1; k++) {
                    if (strcmp(frequentItemsets[i].items[k], frequentItemsets[j].items[k]) != 0) {
                        break;
                    }
                    strcpy(newItems[newItemCount++], frequentItemsets[i].items[k]);
                }

                if (newItemCount == size - 1) {
                    strcpy(newItems[newItemCount++], frequentItemsets[j].items[size - 2]);
                    int support = 0;
                    for (int k = 0; k < transactionCount; k++) {
                        int match = 1;
                        for (int l = 0; l < newItemCount; l++) {
                            int found = 0;
                            for (int m = 0; m < transactions[k].itemCount; m++) {
                                if (strcmp(transactions[k].items[m], newItems[l]) == 0) {
                                    found = 1;
                                    break;
                                }
                            }
                            if (!found) {
                                match = 0;
                                break;
                            }
                        }
                        if (match) {
                            support++;
                        }
                    }
                    if (support >= minSupport) {
                        addItemset(newItems, newItemCount, support);
                    }
                }
            }
        }
    }
}

void createMapping() {
    for (int i = 0; i < itemsetCount; i++) {
        sprintf(mapping[mappingCount].key, "X%d", mappingCount + 1);
        mapping[mappingCount].itemset = frequentItemsets[i];
        mappingCount++;
    }
}

void compressDataset() {
    for (int i = 0; i < transactionCount; i++) {
        Transaction *compressedTransaction = &compressedTransactions[i];
        compressedTransaction->itemCount = 0;

        for (int j = 0; j < mappingCount; j++) {
            int match = 1;
            for (int k = 0; k < mapping[j].itemset.itemCount; k++) {
                int found = 0;
                for (int l = 0; l < transactions[i].itemCount; l++) {
                    if (strcmp(transactions[i].items[l], mapping[j].itemset.items[k]) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                strcpy(compressedTransaction->items[compressedTransaction->itemCount++], mapping[j].key);
                for (int k = 0; k < mapping[j].itemset.itemCount; k++) {
                    for (int l = 0; l < transactions[i].itemCount; l++) {
                        if (strcmp(transactions[i].items[l], mapping[j].itemset.items[k]) == 0) {
                            for (int m = l; m < transactions[i].itemCount - 1; m++) {
                                strcpy(transactions[i].items[m], transactions[i].items[m + 1]);
                            }
                            transactions[i].itemCount--;
                            l--;
                            break;
                        }
                    }
                }
            }
        }

        for (int j = 0; j < transactions[i].itemCount; j++) {
            strcpy(compressedTransaction->items[compressedTransaction->itemCount++], transactions[i].items[j]);
        }
    }
}

void decompressDataset() {
    for (int i = 0; i < transactionCount; i++) {
        Transaction *decompressedTransaction = &transactions[i];
        decompressedTransaction->itemCount = 0;

        for (int j = 0; j < compressedTransactions[i].itemCount; j++) {
            int found = 0;
            for (int k = 0; k < mappingCount; k++) {
                if (strcmp(compressedTransactions[i].items[j], mapping[k].key) == 0) {
                    for (int l = 0; l < mapping[k].itemset.itemCount; l++) {
                        strcpy(decompressedTransaction->items[decompressedTransaction->itemCount++], mapping[k].itemset.items[l]);
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(decompressedTransaction->items[decompressedTransaction->itemCount++], compressedTransactions[i].items[j]);
            }
        }
    }
}

void calculateCompressionRatio() {
    int originalSize = 0;
    int compressedSize = 0;
    int mappingSize = 0;

    for (int i = 0; i < transactionCount; i++) {
        originalSize += transactions[i].itemCount;
    }

    for (int i = 0; i < transactionCount; i++) {
        compressedSize += compressedTransactions[i].itemCount;
    }

    for (int i = 0; i < mappingCount; i++) {
        mappingSize += mapping[i].itemset.itemCount;
    }

    int totalSize = compressedSize + mappingSize;
    double compressionRatio = ((double)(originalSize - totalSize) / originalSize) * 100;
    printf("Compression Ratio: %.2f%%\n", compressionRatio);
}

void printTransactions(Transaction *transactions, int count) {
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < transactions[i].itemCount; j++) {
            printf("%s ", transactions[i].items[j]);
        }
        printf("\n");
    }
}

int main() {
    // Sample input transactions
    char sampleTransactions[MAX_TRANSACTIONS][MAX_ITEMS][MAX_KEY_LEN] = {
        {"A", "B", "C", "D", "E"},
        {"A", "B", "C", "D", "F"},
        {"A", "B", "C", "D", "E", "G"},
        {"A", "B", "C", "D", "E", "F", "G"}
    };

    transactionCount = 4;

    for (int i = 0; i < transactionCount; i++) {
        for (int j = 0; j < MAX_ITEMS && sampleTransactions[i][j][0] != '\0'; j++) {
            strcpy(transactions[i].items[j], sampleTransactions[i][j]);
            transactions[i].itemCount++;
        }
    }

    int minSupport = 2;

    generateFrequentItemsets(minSupport);
    createMapping();
    compressDataset();
    decompressDataset();

    printf("Original Transactions:\n");
    printTransactions(transactions, transactionCount);

    printf("\nCompressed Transactions:\n");
    printTransactions(compressedTransactions, transactionCount);

    printf("\nDecompressed Transactions:\n");
    printTransactions(transactions, transactionCount);

    calculateCompressionRatio();

    return 0;
}
