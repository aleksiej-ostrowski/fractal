# QuickSort and Quasi-Fractality

## About the Project
This repository contains an implementation of the QuickSort algorithm and a computational experiment to investigate the impact of different pivot selection strategies on algorithm efficiency.

The QuickSort algorithm follows the "divide and conquer" strategy, recursively splitting an array into subarrays, thereby forming a quasi-fractal recursive structure. This study aims to analyze how violations of quasi-fractality affect key computational characteristics: the number of recursions and the number of swaps.

## Repository Structure
```
./
├── LICENSE        # Project license
├── main_sort.c    # Source code
├── Makefile       # Build file
├── README.md      # Project description
└── run.sh         # Script to run the experiment
```

## Research Objective
The main objective is to study the impact of pivot selection on algorithm behavior. The experiment analyzes several pivot selection strategies:
- First element of the array
- Random element
- Median element
- Combined method (median + random element)

Each strategy is tested on various input data:
- Random arrays with uniform, exponential, Benford, and Cauchy distributions
- Partially sorted arrays
- Arrays with duplicate elements

## Experiment Description
During the experiment, the following metrics are recorded:
- Number of recursive calls
- Number of element swaps
- Average degree of array ordering

To determine the degree of ordering, the number of inversions is counted, and the pivot is selected based on predefined strategies.

## Conclusions
- Selecting the first element as the pivot yields the worst results, especially for partially sorted data.
- Using the median selection consistently reduces recursion depth.
- The combined method (median + random selection) shows stable improvements over the median selection, reducing the number of swaps.
- Introducing stochastic elements into the algorithm enhances adaptability and prevents performance degradation.

## Running the Code
To compile and run, use the following commands:
```sh
make
./run.sh
```
The `run.sh` script automatically runs the experiment and collects statistics for different pivot selection strategies.

## License
The project is distributed under the GNU GENERAL PUBLIC LICENSE. See the LICENSE file for more details.
