#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>       /* time */

#define NO_INFO 101
#define WRONG_NUMBER 102
#define CORRECT_NUMBER_WRONG_POS 103
#define CORRECT_NUMBER 104

#define NOT_TESTED 201
#define TESTED 202

#define VALUE_FINDING 301
#define POSITION_FINDING 302
#define GAME_WON 303
#define GAME_LOST 304

#define NO_INFO_ON_POSITION 401
#define HALF_CHANCE_CORRECT_POSITION 402
#define WRONG_POSITION 403
#define CORRECT_POSITION 404
#define CANDIDATE_FOR_SAME_POS 405
#define TESTED_POSITION 406

#define TESTING_WITH_WRONG_NUM 501
#define TESTING_WITH_WRONG_NUM_REVERSE_POS 502
#define TEST_RECOVERY 503
#define NO_SPECIAL_MOVE 504
#define TEST_RECOVERY_END 505


#define NUMBER_SIZE 4
using namespace std;

class updateMove {
    public:
    
    bool doValChange;
    bool doPosChange;
    
    int valChangeIndex; // index of next value to change
    int newVal;
    
    int posChangeIndex1;
    int posChangeIndex2;
    int moveID;
    int tempCorrectNumStore; // to hold correct values during special moves
    updateMove(){
        valChangeIndex = 3;
        newVal = 0;
        posChangeIndex1 = 1;
        posChangeIndex2 = 2;
        moveID = NO_SPECIAL_MOVE; // To distinguish special moves
        doValChange = true;
        doPosChange = true;
    }
};

void handleTotalHintChange(int newTot, int oldTot, updateMove previousMove, int* guess, int* previousGuess, int* stateOfNumbers, int positionMemory[10][NUMBER_SIZE]) {
    
    if (newTot - oldTot == (-1)){ // since total hint decreased it means we lost a correct number in previous move
        int correctNumberIndex = previousMove.valChangeIndex;
        int correctNumber = previousGuess[correctNumberIndex]; // we declare the number in position of previous value change is correct
        int wrongNumber = guess[correctNumberIndex];
        stateOfNumbers[correctNumber] = CORRECT_NUMBER_WRONG_POS; // here we found a correct and a wrong number so we update states of numbers
        stateOfNumbers[wrongNumber] = WRONG_NUMBER;
    }
    
    if (newTot - oldTot == (1)){
        int correctNumberIndex = previousMove.valChangeIndex;
        int correctNumber = guess[correctNumberIndex];
        int wrongNumber = previousGuess[correctNumberIndex];
        stateOfNumbers[correctNumber] = CORRECT_NUMBER_WRONG_POS;// here we found a correct and a wrong number so we update states of numbers
        stateOfNumbers[wrongNumber] = WRONG_NUMBER;
        positionMemory[correctNumber][correctNumberIndex] = CORRECT_POSITION;
    }
}

void decideNextValChangeIndex(int* guess, updateMove& nextMove, int* stateOfNumbers){
    // We want to change the wrong numbers in our guess first. If there are no wrong numbers we change numbers with no info
    if (nextMove.doValChange == false ) return;
    int nextValChangeIndexFoundFlag = 0;
    
    for (int i = 0; i < NUMBER_SIZE ; i++ ){
        if (stateOfNumbers[guess[i]] == WRONG_NUMBER) {
            nextValChangeIndexFoundFlag = 1;
            nextMove.valChangeIndex = i;
            break;
        }
    }
    
    if (nextValChangeIndexFoundFlag == 1) return;
    
    for (int i = 0; i < NUMBER_SIZE ; i++ ){
        if (stateOfNumbers[guess[i]] == NO_INFO) {
            nextValChangeIndexFoundFlag = 1;
            nextMove.valChangeIndex = i;
            break;
        }
    }
    if (nextValChangeIndexFoundFlag == 0) nextMove.doValChange = false;
}


void copySameLengthArray(int* destinationArray, int* sourceArray) {
    
    for(int i=0; i < NUMBER_SIZE; i++) {
        
        destinationArray[i] = sourceArray[i];
    }
}

bool boolGuessIncludesNumber(int* guess, int number){
    bool guessIncludesNumber = false;
    for (int i = 0; i < NUMBER_SIZE ; i++ ){ // mark current numbers of guess as tested
        if ( guess[i] == number) {
            guessIncludesNumber = true;
            break;
        }
    }
    return guessIncludesNumber;
}

int findNewVal(int* stateOfNumbers, int* guess, int* testStateOfNumbers) {
    // This function finds new value to test at our guess.
    int newVal = -1;
    for (int i = 0; i < NUMBER_SIZE ; i++ ){ // mark current numbers of guess as tested
        testStateOfNumbers[guess[i]] = TESTED;
    }
    
    for (int i = 0; i < 10 ; i++ ){
        if ((stateOfNumbers[i] == CORRECT_NUMBER_WRONG_POS) && (!boolGuessIncludesNumber(guess, i))){
            newVal = i;
            return newVal;
        }
    }
    
    
    bool notTestedNumberExist = false; // verify if there are not tested number remains
    for (int i = 0; i < 10 ; i++ ){
        if (boolGuessIncludesNumber(guess,i)) continue;
        if (testStateOfNumbers[i] == NOT_TESTED) {
            notTestedNumberExist = true;
            break;
        }
    }
    
     
    if (notTestedNumberExist){ // find NOT_TESTED number and return it as new Value to try
        int temp;
        while(true){
            temp = rand() % 10;
            if (boolGuessIncludesNumber(guess,temp)) continue;
            if (testStateOfNumbers[temp] == NOT_TESTED) {
                newVal = temp;
                break;
            }
        }
    }
    
    else { // we try numbers which we have no information about, if all numbers are already tested once
        bool noInfoNumberExist = false; // verify if there are no info state numbers 
        for (int i = 0; i < 10 ; i++ ){
            if (boolGuessIncludesNumber(guess,i)) continue;
            if (stateOfNumbers[i] == NO_INFO) {
                noInfoNumberExist = true;
                break;
            }
        }
        
        if (noInfoNumberExist){
            int temp;
            while(true){
                temp = rand() % 10;
                if (boolGuessIncludesNumber(guess,temp)) continue;
                if (stateOfNumbers[temp] == NO_INFO) {
                    newVal = temp;
                    break;
                }
            }
        }
    }
    
    if(newVal == -1) {
        printf("There is no suitable value to try, an error occured during calculations \n"); 
    } else{
        testStateOfNumbers[newVal] = TESTED; // mark new number as tested
    }
    
    return newVal;
}


void applyUpdateMove(updateMove nextMove, int* guess, int* stateOfNumbers, int* testStateOfNumbers, int searchMode) {
    
   // the case of special move 
   if( searchMode == POSITION_FINDING && (nextMove.moveID != NO_SPECIAL_MOVE)) { // Here we test which number of two is in correct position
       guess[nextMove.valChangeIndex] = nextMove.newVal;                        // by changing one of them to a wrong number and check change of positive hint
   }

   if( searchMode == POSITION_FINDING && (nextMove.moveID == NO_SPECIAL_MOVE)) { // Here we look for a hint about positions, in position finding state 
       int temp;                                                                 // all numbers are correct values so we only swap positions.
       temp = guess[nextMove.posChangeIndex1];
       guess[nextMove.posChangeIndex1] = guess[nextMove.posChangeIndex2];
       guess[nextMove.posChangeIndex2] = temp;
   }
    
   if ( nextMove.doValChange == false) return;
   if(searchMode == VALUE_FINDING) { // To find correct values in the number we try to guess we change a digit of our guess
       int newVal = findNewVal(stateOfNumbers, guess, testStateOfNumbers);
       guess[nextMove.valChangeIndex] = newVal; 
   }
   
   
}

void checkWrongNumber(int* guess, int* stateOfNumbers, int positiveHint, int negativeHint){
    // In case we have no hints we mark all numbers in current guess as wrong
    int totalHint = positiveHint + negativeHint;
    if (totalHint == 0){
        for (int i = 0; i < NUMBER_SIZE ; i++ ){
            stateOfNumbers[guess[i]] = WRONG_NUMBER;
        }
        return;
    }
    
    
    if(negativeHint != 0 ) return; // IF negativeHint is 0 and we know the correct numbers, remaining numbers are wrong
    int countOfCorrectNumbersAtCorrectPos = 0;
    for (int i = 0; i < NUMBER_SIZE ; i++ ){
        if(stateOfNumbers[guess[i]] == CORRECT_NUMBER ||stateOfNumbers[guess[i]] == CORRECT_NUMBER_WRONG_POS){
            countOfCorrectNumbersAtCorrectPos++;
        }
    }
    
    if(countOfCorrectNumbersAtCorrectPos == positiveHint){
        for (int i = 0; i < NUMBER_SIZE ; i++ ){
            if(stateOfNumbers[guess[i]] != CORRECT_NUMBER){
                stateOfNumbers[guess[i]] = WRONG_NUMBER;
            }
        }
    }
}

int findWrongVal(int* stateOfNumbers){
    int wrongVal = -1;
    
    for(int i=0; i < 10; i++){
        wrongVal = i;
        if(stateOfNumbers[i] == WRONG_NUMBER || stateOfNumbers[i] == NO_INFO ) break;
    }
    return wrongVal;
}
    
void handlePositionChangeResult(int positiveHint, int oldPositiveHint, int positionMemory[10][NUMBER_SIZE], int* guess, updateMove &nextMove, int* stateOfNumbers, int &searchMode) {
    
    if(nextMove.moveID == TEST_RECOVERY_END){ // at end of test recovery we return to old state so positive hint changes should not be checked
        nextMove.moveID = NO_SPECIAL_MOVE;
        return;
    }
    if (nextMove.moveID != NO_SPECIAL_MOVE) return; // In case of special moves, which are to test position correctness of two numbers this function does  not work.
        
    int posHintChange = positiveHint - oldPositiveHint; //  This function checks change of positiveHint to find hints about true positions of numbers.
    if(positiveHint == 0) {
        int candidateNum;
        for(int i = 0; i < NUMBER_SIZE; i++){
            candidateNum = guess[i];
            positionMemory[candidateNum][i] = WRONG_POSITION;
        }      
    }
    
    if(posHintChange == +2){
        int correctNum1 = guess[nextMove.posChangeIndex1];
        int correctNum2 = guess[nextMove.posChangeIndex2];
        
        positionMemory[correctNum1][nextMove.posChangeIndex1] = CORRECT_POSITION; // since Positive hint increased two we mark current 
        positionMemory[correctNum2][nextMove.posChangeIndex2] = CORRECT_POSITION; // positions as correct for particular numbers.
    }
    
    
    if(posHintChange == -2){
        int correctNum1 = guess[nextMove.posChangeIndex1];
        int correctNum2 = guess[nextMove.posChangeIndex2];
        
        positionMemory[correctNum1][nextMove.posChangeIndex2] = CORRECT_POSITION; // Since positive hint decreased by 2, we know the other position was correct for
        positionMemory[correctNum2][nextMove.posChangeIndex1] = CORRECT_POSITION; // each number.
    }
    
    if(posHintChange == 1){
        int candidateNum1 = guess[nextMove.posChangeIndex1];
        int candidateNum2 = guess[nextMove.posChangeIndex2];
        
        positionMemory[candidateNum1][nextMove.posChangeIndex1] = HALF_CHANCE_CORRECT_POSITION; // + hint increased 1, which means only one number found the correct position
        positionMemory[candidateNum2][nextMove.posChangeIndex2] = HALF_CHANCE_CORRECT_POSITION; // hence we give it half chance
        
     
        nextMove.valChangeIndex = nextMove.posChangeIndex1; // change one of the numbers to test which is in the correct position
        nextMove.newVal = findWrongVal(stateOfNumbers);
        nextMove.moveID = TESTING_WITH_WRONG_NUM;
        nextMove.tempCorrectNumStore = candidateNum1; // hold correct val here 
        
    }
    
    if(posHintChange == -1){
        int candidateNum1 = guess[nextMove.posChangeIndex1];
        int candidateNum2 = guess[nextMove.posChangeIndex2];
        
        positionMemory[candidateNum1][nextMove.posChangeIndex2] = HALF_CHANCE_CORRECT_POSITION; // + hint decreased 1, which means one number was correct on previous position
        positionMemory[candidateNum2][nextMove.posChangeIndex1] = HALF_CHANCE_CORRECT_POSITION; 
    }
    
    
    if(posHintChange == 0){
        if (positiveHint == 0){
            int candidateNum1 = guess[nextMove.posChangeIndex1];
            int candidateNum2 = guess[nextMove.posChangeIndex2];
            
            positionMemory[candidateNum1][nextMove.posChangeIndex1] = WRONG_POSITION; // + hint was 0 and did not change, so previous positions were wrong and those new ones too.
            positionMemory[candidateNum2][nextMove.posChangeIndex2] = WRONG_POSITION;
        }
    }
}


void handleSpecialMove(updateMove previousMove, updateMove &nextMove, int &positiveHint, int &oldPositiveHint, int positionMemory[10][NUMBER_SIZE], int* guess){ 
    
    if (previousMove.moveID == NO_SPECIAL_MOVE) return;
    int posHintChange = positiveHint - oldPositiveHint;
    
    if (previousMove.moveID == TESTING_WITH_WRONG_NUM){ // When we know 1 of 2 numbers is correct, we find the correct one with changing 1 
        if(posHintChange == (-1)){                      // with a already known wrong number
            positionMemory[previousMove.tempCorrectNumStore][previousMove.valChangeIndex] = CORRECT_POSITION;
        } else {
            positionMemory[guess[previousMove.posChangeIndex2]][previousMove.posChangeIndex2] = CORRECT_POSITION;
        }
        
        nextMove.valChangeIndex = previousMove.valChangeIndex; // since the result of test is checked, we start a TEST_RECOVERY move 
        nextMove.newVal = previousMove.tempCorrectNumStore;     // to bring back the correct value we swapped with a wrong value to 
        nextMove.moveID = TEST_RECOVERY;                      // to test if that true value was on correct position
    }
    else if(previousMove.moveID == TEST_RECOVERY){ // If last move was test recovary, test is complete and we can go back to searcging 
        nextMove.moveID = TEST_RECOVERY_END;  //  for true positions.
    }
}

void prepareNextPosSwapIndexes(updateMove &nextMove, updateMove previousMove, int* guess, int positionMemory[10][NUMBER_SIZE]){
    
    if(nextMove.moveID != NO_SPECIAL_MOVE) return;
    
    nextMove.posChangeIndex1 = previousMove.posChangeIndex2;
    
    int numberToSwap = guess[nextMove.posChangeIndex1];
    if (positionMemory[numberToSwap][nextMove.posChangeIndex1] == CORRECT_POSITION) { // If swap index 1 is already CORRECT_POSITION we choose
        int tempNumber;                                                                 // another index
        for (int i =0; i < NUMBER_SIZE; i++){                                                                            
            tempNumber = guess[i];
            if(positionMemory[tempNumber][i] != CORRECT_POSITION){ // if index i is not CORRECT_POSITION for the number in index i, we choose i as
                nextMove.posChangeIndex1 = i;                // a position index to swap
                numberToSwap = guess[nextMove.posChangeIndex1];
            }
        }
    }
    
    for (int i =0; i < NUMBER_SIZE; i++){
        if(i == nextMove.posChangeIndex1) continue;
        if(positionMemory[numberToSwap][i] == CORRECT_POSITION) { // When choosing numbers to swap, we first prefer correct position of the number
            nextMove.posChangeIndex2 = i;                        // then, half chance correct posiiton, and lastly NO_INFO_ON_POSITION.
            return;
        }
    }
    
    for (int i =0; i < NUMBER_SIZE; i++){
        bool alreadyCorrectedPosition = false;
        if(i == nextMove.posChangeIndex1) continue;
        if(positionMemory[numberToSwap][i] != HALF_CHANCE_CORRECT_POSITION) continue;
        for (int j =0; j < NUMBER_SIZE; j++){
            if(positionMemory[guess[j]][i] == CORRECT_POSITION){// dont pick that position if it is some other numbers' position.
                alreadyCorrectedPosition = true;
            } 
            
        }
        if(alreadyCorrectedPosition) continue;
        nextMove.posChangeIndex2 = i;
        return;
    }
    
    for (int i =0; i < NUMBER_SIZE; i++){
        bool alreadyCorrectedPosition = false;
        if(i == nextMove.posChangeIndex1) continue;
        if(positionMemory[numberToSwap][i] != NO_INFO_ON_POSITION) continue;
        for (int j =0; j < NUMBER_SIZE; j++){
            if(positionMemory[guess[j]][i] == CORRECT_POSITION){// dont pick that position if it is some other numbers' position.
                alreadyCorrectedPosition = true;
            } 
            
        }
        if(alreadyCorrectedPosition) continue;
        nextMove.posChangeIndex2 = i;
        return;
    }
}

void preparePosMemory(int positionMemory[10][NUMBER_SIZE]){
    for (int i = 0; i < 10; i++){
        for (int j = 0; j< NUMBER_SIZE; j++){
            positionMemory[i][j] = NO_INFO_ON_POSITION;
        }
    }
}

void createNumberWithDifferentDigits(int numberOfDigits, int* numberArray) {
    for(int i = 0; i < NUMBER_SIZE; i++){
        bool uniqueNumberFoundFlag;
        while(true){
            uniqueNumberFoundFlag = true;
            numberArray[i] = rand() % 10;
            for (int j = 0; j<i; j++ ){
                if (numberArray[i] == numberArray[j]){
                    uniqueNumberFoundFlag = false;
                    break;
                }
                if (j == (i - 1)) {
                    uniqueNumberFoundFlag = true;
                }
            }
            if (uniqueNumberFoundFlag) break;
        }
    }
}

bool checkHintCorrectness(string hint){
    int hintLength = 0;
    for(int i = 0; hint[i] != '\0'; i++){
        hintLength++;
    }
    if (hintLength != 1 ){
        printf("Your hint shold be 1 digit long, please enter again: \n");
        return false;
    }
    if( (int)hint[0] - 48 < 0 || (int)hint[0] - 48 > 10 ) { 
        printf("Your hint cannot be negative. You should enter magnitude of the hint. Please enter again: \n");
        return false;
    }
    return true;
}

void makeGuessRequestHints(int &positiveHint, int &negativeHint, int* guess, int &searchMode){
    printf("\nMy guess is: ");
    for(int i = 0; i<NUMBER_SIZE; i++){
        printf ("%d",guess[i]);
    } 
    string hintInputHolder;
    printf("\nPlease give positive hint: \n"); // take feed back
    while(true){
        cin>>hintInputHolder;
        if (checkHintCorrectness(hintInputHolder)) break;
    }
    positiveHint = (int)hintInputHolder[0] - 48;
    if (positiveHint == NUMBER_SIZE) searchMode = GAME_WON;

    printf("\nPlease give negative hint: \n");
    while(true){
        cin>>hintInputHolder;
        if (checkHintCorrectness(hintInputHolder)) break;
    }
    negativeHint = (int)hintInputHolder[0] - 48;
}

int* takePlayersGuess(int* guessOfPlayer) {

    string inputStrHolder;
    printf("\nTry to guess my number: \n");
    
    while(true){
        cin >> inputStrHolder;
        int inputLength = 0;
        for(int i = 0; inputStrHolder[i] != '\0'; i++){
            inputLength++;
        }
        if (inputLength != NUMBER_SIZE) {
            printf("Your guess should be %d digits long. Please enter again:  \n", NUMBER_SIZE);
            continue;
        }
        break;
    }
    
    for (int i=0; i < NUMBER_SIZE ; i++){
        guessOfPlayer[i] = (int)inputStrHolder[i] - 48;
    }
}

void giveHintToPlayer(int* myNumber, int* guessOfPlayer, int &searchMode) {
    int negativeHint = 0;
    int positiveHint = 0;
    for (int i = 0; i < NUMBER_SIZE ; i++){
        for ( int j = 0; j < NUMBER_SIZE; j++) {
            if (myNumber[j] == guessOfPlayer [i]){
                if (i == j) positiveHint ++;
                else negativeHint ++;
            }
        }
    }
    if(positiveHint == NUMBER_SIZE){
        searchMode = GAME_LOST;
        return;
    }
    printf("Your hints are:   +%d  -%d   \n", positiveHint,negativeHint);
}

void handlePlayerGuess(int* myNumber, int &searchMode) {
    int guessOfPlayer[NUMBER_SIZE];
    takePlayersGuess(guessOfPlayer);
    giveHintToPlayer(myNumber, guessOfPlayer, searchMode);
}

bool checkGameEndConditions(int searchMode, int* guess, int* myNumber){
    if (searchMode == GAME_WON){
        printf("Your number is:  ");
        for(int i = 0; i<NUMBER_SIZE; i++){
            printf ("%d",guess[i]);
        }
        printf("\nIt seems I have won. Thanks for playing... \n");
        return true;
    }
    else if (searchMode == GAME_LOST){
        printf("My number was:  ");
        for(int i = 0; i<NUMBER_SIZE; i++){
            printf ("%d",myNumber[i]);
        }
        printf("\nYou have won. Congratulations, thanks for playing... \n");
        return true;
    }
    else return false;
}

