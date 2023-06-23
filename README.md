# Sodoku-Solver-with-Heuristics

## 3 Implementations:

1. Basic Backtracking:
   - Randomly select values
   - Randomly select blocks
   - Backtrack when there's no solutions for a block

2. Backtracking + Forward Checking:
   - Basic Backtracking
   - Forward check if assigning a value would cause other blocks to have no solution (if so, eliminate the node to stop it from expanding)
     
3. Backtracking + Forward Checking + Heuristics
   - Backtracking
   - Forward Checking
   - Heuristics on selecting values and blocks:
       1. Find most constrained block(s): block(s) with the fewest valid values
       2. Get the most constaining block: block that constrains the most number of empty blocks
       3. Assign a least constraining value: value that is in the domain of least number of empty blocks

## Results and Performance by difficulty of puzzle:
<img width="719" alt="Screenshot 2023-06-21 at 10 55 02 AM" src="https://github.com/calvinjujuis/Sudoku-Solver-with-Heuristics/assets/83982075/bc224dbc-e07e-4500-9801-5a5b3570043d">
