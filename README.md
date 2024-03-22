
# Amadeus Chess Engine

Welcome to the repository of Amadeus, a chess engine that adheres to the Universal Chess Interface (UCI) specification, designed and developed with performance and simplicity in mind. Written entirely in C, Amadeus leverages the [Spark](https://github.com/stefanoskapa/spark.git) move generator—a companion project—to efficiently generate possible moves and employs a minimax algorithm with alpha-beta pruning alongside a handcrafted evaluation function to make intelligent decisions on the chessboard.

## Features

- **UCI Compatibility**: Fully compatible with chess interfaces that support the UCI protocol, allowing for easy integration and use.
- **Minimax Algorithm with Alpha-Beta Pruning**: Employs a time-tested decision-making process enhanced with alpha-beta pruning to efficiently evaluate possible moves.
- **Handcrafted Evaluation Function**: Utilizes a custom evaluation function designed to assess chess positions and guide the engine towards winning strategies.
- **Spark Move Generator**: Integrates with the Spark move generator for efficient move generation and board evaluation.

## Technologies Used

Amadeus is written in C, making it highly efficient and portable across different computing environments that support C compilers.

## Installation Instructions

To get started with Amadeus, follow these simple steps:

1. Ensure you have a C compiler, `git` and `make` installed on your system.
2. Clone this repository to your local machine:
3. ```git clone https://github.com/stefanoskapa/amadeus.git```  
4. Navigate to the cloned repository directory and run `make`

This will compile the source code and generate an executable for the Amadeus chess engine.

## Usage

Amadeus operates according to the UCI protocol, which means it can be used directly through standard UCI commands or integrated with any chess GUI that supports UCI engines. To start the engine in console mode, simply run the compiled executable. From there, you can input UCI commands to interact with Amadeus.

### Play Against Amadeus Online

You can challenge the latest build of Amadeus on Lichess by visiting [Blue Mongoose's Lichess Profile](https://lichess.org/@/Blue_Mongoose).

## Credits

Special thanks to the open-source community, [chessprogramming.org](https://www.chessprogramming.org) and the various resources available that have provided guidance and inspiration for this project.

## Contact Information

For questions, suggestions, or further discussions regarding Amadeus, please feel free to contact me at koutsouflakis.stefanos@gmail.com


