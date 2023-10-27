#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Rotate around 26 Char Alphabet 
//Each rotation is 1 step
#define ROTATE 26
#define STEP_DIST 1

//Prototypes
void rotorCycle();
int rotorForward();
int rotorBackward();
int findLetterIndex();

struct Rotor {
    const char      *step;
    int             displacement;
    int             rotate;
    const char      *turnover;
    const char      *wheel;
};

//Source for Rotor Info: https://en.wikipedia.org/wiki/Enigma_rotor_details

//ETW or Eintrittzwalze which means Entry Wheel
const char *etw = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
const char *rotors[] = {
    "EKMFLGDQVZNTOWYHXUSPAIBRCJ", //Enigma I, 		I
    "AJDKSIRUXBLHWTMCQGZNPYFVOE", //Enigma I, 		II
    "BDFHJLCPRTXVZNYEIWGAKMUSQO", //Enigma I, 		III
    "ESOVPZJAYQUIRHXLNFTGKDCMWB", //M3 Army,  		IV
    "VZBRGITYUPSDNHLXAWMJQOFECK", //M3 Army,  		V
    "JPGVOUMFYQBENHZRDKASXLICTW", //M3 & M4 Naval,	VI
    "NZJHGRCXMYSWBOUFAIVLPEKQDT", //M3 & M4 Naval,	VII
    "FKQHTLXOCBJSPDZRAMEWNIUYGV"  //M3 & M4 Naval,	VIII
};

const char *notches[] = {
	"Q", //If rotor I steps from Q to R, the next rotor is advanced
	"E", //If rotor II steps from E to F, the next rotor is advanced
	"V", //If rotor III steps from V to W, the next rotor is advanced
	"J", //If rotor IV steps from J to K, the next rotor is advanced 
	"Z", //If rotor V steps from Z to A, the next rotor is advanced
	"ZM", "ZM", "ZM"  	
	//If rotors VI, VII, or VIII step from Z to A, or from M to N the next rotor is advanced
};
const char *notchTurnovers[] = {"R", "F", "W", "K", "A", "AN", "AN", "AN"};

const char *reflectors[] = {
    "EJMZALYXVBWFCRQUONTSPIKHGD", //Reflector A
    "YRUHQSLDPXNGOKMIEBFZCWVJAT", //Reflector B
    "FVPJIAOYEDRZXWGCTKUQSBNMHL"  //Reflector C
};

struct EnigmaMachine {
    struct Rotor    rotors[8];
    int             numRotors;    
    const char      *reflector;
    
};

struct Rotor chooseRotor(struct EnigmaMachine *machine, int chosenRotor) {
    struct Rotor r;
    r.displacement = 0;
    r.rotate = 0;
    r.wheel = rotors[chosenRotor];
    r.turnover = notchTurnovers[chosenRotor];
    r.step = notches[chosenRotor];
    machine -> numRotors++;

    return r;
}

void rotorCycle(struct Rotor *rotor) {
    rotor -> displacement++;
    rotor -> displacement = rotor -> displacement % ROTATE;

    if(findLetterIndex(rotor -> turnover, etw[rotor -> displacement]) >= 0) {
        rotor -> rotate = STEP_DIST;
    }
}

int rotorForward(struct Rotor *rotor, int index) {

    index = (index + rotor -> displacement) % ROTATE;
    index = findLetterIndex(etw, rotor -> wheel[index]);
    index = (ROTATE + index - rotor -> displacement) % ROTATE;

    return index;
}

int rotorBackward(struct Rotor *rotor, int index) {

    index = (index + rotor -> displacement) % ROTATE;
    index = findLetterIndex(rotor -> wheel, etw[index]);
    index = (ROTATE + index - rotor -> displacement) % ROTATE;

    return index;
}

int findLetterIndex(const char *str, int letter) {
    int index;
    int temp;
    char *letterPointer;
    letterPointer = strchr(str, letter);

    if (letterPointer != NULL || letterPointer) {
        temp = (int) (letterPointer - str);
        index = temp;
    }
    else 
        index = -1;

    return index;
}

int main(int argc, char *argv[]) {
    int letter, next;
    struct EnigmaMachine enigma = {}; 

    /*
    Below Configure Rotors

    Rotor                   | Array Value    
    Enigma I, 		I       | 0
    Enigma I, 		II      | 1
    Enigma I, 		III     | 2
    M3 Army,  		IV      | 3
    M3 Army,  		V       | 4
    M3 & M4 Naval,	VI      | 5
    M3 & M4 Naval,	VII     | 6
    M3 & M4 Naval,	VIII    | 7

    For standard configuration, 
    pick any 3 rotors and 1 reflector   
    */
    
    enigma.rotors[0] = chooseRotor(&enigma, 0); //Rotor 1
    enigma.rotors[1] = chooseRotor(&enigma, 1); //Rotor 2
    enigma.rotors[2] = chooseRotor(&enigma, 2); //Rotor 3
    enigma.reflector = reflectors[0]; //Reflector

    printf("CS 428 Final Project :: Anthony Johnson\nEnter Text into Enigma Machine:\n");

    while((letter = getchar()) != -1) {

        if (!isalpha(letter)) {
            printf("%c", letter);
            continue;
        }
        letter = toupper(letter);

        next = findLetterIndex(etw, letter);
        rotorCycle(&enigma.rotors[0]);
        if(findLetterIndex(enigma.rotors[1].step, etw[enigma.rotors[1].displacement]) >= 0 ) 
            rotorCycle(&enigma.rotors[1]);

        for(int i = 0; i < enigma.numRotors - STEP_DIST; i++) {
            letter = etw[enigma.rotors[i].displacement];
            if(enigma.rotors[i].rotate) {
                enigma.rotors[i].rotate = 0;
                rotorCycle(&enigma.rotors[i+STEP_DIST]);
            }
         }

        for(int i = 0; i < enigma.numRotors; i++) 
            next = rotorForward(&enigma.rotors[i], next);
        letter = enigma.reflector[next];
        next = findLetterIndex(etw, letter);

        for(int i = enigma.numRotors - STEP_DIST; i >= 0; i--) 
            next = rotorBackward(&enigma.rotors[i], next);
    
        letter = etw[next];
        putchar(letter);
    }

    return 0;
}