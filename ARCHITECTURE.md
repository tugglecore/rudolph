# Architecture

## Algorithm

### Definitions

- Partition: A section of the CSV data where the first by of the section is the first byte of a data cell to be included in the partition and the last byte of the section is the last byte of a data cell to be included in the partition.

### Steps

1. Memory map file.

2. Reader header line.

3. Create partitions of the file equal to the amount of cores on the system by the following algorithm.

- a) Estimate the partition size to be `file data / system's core count`.

- b) Decrement partition size to until reaching a valid end partition boundary.

- b.i) If decrementing partition size finds valid end partition boundary then skip to step c.

- b.ii) if decrementing does not find partition boundary then increment partition size till valid end partition boundary is found.

- b.iii) After finding valid end partition boundary, then resize partition size to be `remaining file data / remaining partitions to be created`.

- c) Repeat steps a - b until last partition where the following algorithm is to be performed for last partition:

- d) The last partition is equal to the remaining file data

4. For each partition:

- a) Parse contents
