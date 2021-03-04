
#include "header.h"

int main()
{
    srand (time(NULL));
    // Pick my number part 
    int myNumber[NUMBER_SIZE];
    createNumberWithDifferentDigits(NUMBER_SIZE, myNumber);
    /*        // uncomment here to see what number code chooses at start for debug.
    printf ("\nMy number is :");
    for(int i = 0; i < NUMBER_SIZE; i++){
        printf ("%d",myNumber[i]);
    }
    */
    // making a guess part
    
    int searchMode = VALUE_FINDING; // There is 2 search modes. Value finding and position finding. We start with value finding to find 
                                // values in number to be guessed. After we find all values (which means total hint sum is 4), we switch
                                // to position finding mode to find correct positions of numbers. Also win and lose conditions are checked
                                // on this variable with GAME_WON and GAME_LOSE
    
    updateMove previousMove;  // Update move is a class for carrying the data of next move.
    updateMove nextMove;
    
    int stateOfNumbers[10] = {NO_INFO,NO_INFO,NO_INFO,NO_INFO,NO_INFO,NO_INFO,NO_INFO,NO_INFO,NO_INFO,NO_INFO}; // Here data about possible numbers hold
                                                                                                            // CORRECT_NUMBER means that number is 
                                                                                                            // in the number to be guessed for example
    int testStateOfNumbers[10] = {NOT_TESTED,NOT_TESTED,NOT_TESTED,NOT_TESTED,NOT_TESTED,NOT_TESTED,NOT_TESTED,NOT_TESTED,NOT_TESTED,NOT_TESTED};
    // testStateOfNumbers holds if a number is tested or not, we check this when we are going to pick a new number to try.
    int guess [NUMBER_SIZE];
    createNumberWithDifferentDigits(NUMBER_SIZE, guess);
    int previousGuess[NUMBER_SIZE];
    
    int positiveHint = 0;
    int negativeHint = 0;
    int totalHint = 0; // This will be sum of two feedbacks
    
    int oldPositiveHint = 0;
    int oldTotalHint = 0;
    
    int positionMemory[10][NUMBER_SIZE]; // Data of previous positions of numbers. Rows corresponds to numbers, and colums corresponds to
                                         // indexes of positions. For ex: positionMemory[1][2] == CORRECT_POSITION means third digit is 
                                         // correct place for number 1. We also have HALF_CHANCE_CORRECT_POSITION which is self explanatory,
                                         // we check it when we are going to pick indexes to swap
                                             
    preparePosMemory(positionMemory);
    
    printf("\nWelcome to my game! Please pick a number and dont tell anyone! \n");
    handlePlayerGuess(myNumber, searchMode);
    if (checkGameEndConditions(searchMode, guess, myNumber)) return 0;

    makeGuessRequestHints(positiveHint, negativeHint, guess, searchMode);
    if (checkGameEndConditions(searchMode, guess, myNumber)) return 0;
    totalHint = negativeHint + positiveHint;

    checkWrongNumber(guess, stateOfNumbers, positiveHint, negativeHint);
    decideNextValChangeIndex(guess, nextMove, stateOfNumbers);
    copySameLengthArray(previousGuess,guess);
    applyUpdateMove(nextMove, guess, stateOfNumbers, testStateOfNumbers, searchMode);
    
    while (true){
        handlePlayerGuess(myNumber, searchMode);
        if (checkGameEndConditions(searchMode, guess, myNumber)) break; // checkGameEndConditions checks end case on searchMode

        previousMove = nextMove;
        oldPositiveHint = positiveHint;
        oldTotalHint = totalHint;
        
        makeGuessRequestHints(positiveHint, negativeHint, guess, searchMode);
        if (checkGameEndConditions(searchMode, guess, myNumber)) break;
        totalHint = negativeHint + positiveHint;
        
        
        if (totalHint == NUMBER_SIZE && searchMode != POSITION_FINDING) { // if we find all the numbers inside the number to be guessed 
            searchMode = POSITION_FINDING;                              // we switch to checking positions
            prepareNextPosSwapIndexes(nextMove, previousMove, guess, positionMemory);
        }
        
        if (searchMode == POSITION_FINDING){  // For different search stages we use different way of functions
            handleSpecialMove(previousMove, nextMove, positiveHint, oldPositiveHint, positionMemory, guess);
            handlePositionChangeResult(positiveHint, oldPositiveHint, positionMemory, guess, nextMove, stateOfNumbers, searchMode);
            prepareNextPosSwapIndexes(nextMove, previousMove, guess, positionMemory);
        }
        
        if (searchMode == VALUE_FINDING) {
            checkWrongNumber(guess, stateOfNumbers, positiveHint, negativeHint);
            handleTotalHintChange(totalHint, oldTotalHint, previousMove, guess, previousGuess, stateOfNumbers, positionMemory);
            decideNextValChangeIndex(guess, nextMove, stateOfNumbers);
        }
        
        copySameLengthArray(previousGuess,guess); // update previousGuess
        applyUpdateMove(nextMove, guess, stateOfNumbers, testStateOfNumbers, searchMode); // the move prepared (nextMove) is applied in this function
    }
    
    return 0;
}




