import os 
os.chdir('data/Falkenauer_CSP/Falkenauer_T')
import glob
import pandas as pd
import numpy as np

# Files are in format called CSP
# First line is the number of items
# Second line is the capacity of the bins
# Next lines are the weights and demands of the items

BIN_CAPACITY = 1000

for i in [60, 120, 249, 501]:
    df_merged = pd.DataFrame()
    for file in glob.glob("*" + str(i) + "*.txt"):
        # Add to pandas dataframe
        df = pd.read_csv(file, sep="\t", header=None, skiprows=2)
        df.columns = ["core", "nb_instances"]
        df["memory"] = 0
        df["inter_degree"] = 0
        df["inter_aff"] = [[] for i in range(len(df))]
        # Rename index
        df.index = df.index + 1
        # Change index name
        df.index.name = "app_id"
        # Change order of columns
        df = df[["nb_instances", "core", "memory", "inter_degree", "inter_aff"]]
        # Merge with files with the same name start
        df_merged = pd.concat([df_merged, df], axis=0)
    # Reset index
    df_merged.reset_index(inplace=True)
    # Drop "app_id" column
    df_merged.drop(["app_id"], axis=1, inplace=True)
    # Rename index
    df_merged.index = df_merged.index + 1
    # Change index name
    df_merged.index.name = "app_id"
    # Save to csv
    df_merged.to_csv("Falkenauer_T_" + str(i) + ".csv", sep="\t")


