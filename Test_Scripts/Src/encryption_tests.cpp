#include "encryption_tests.h"

/*************************************************************************
 * Function to rotate a 32-bit word left by one byte (Left circular shift)
 * Parameters:
 * - word: The 32-bit word to be rotated
 * Returns: The rotated 32-bit word
**************************************************************************/
uint32_t rotateWord(uint32_t word) {
    /* We rotate left by shifting all the bits by 1 byte left (0xAABBCCDD) -> (0xBBCCDD00)
        And then shifting the bits 3 bytes to the right (0xAABBCCDD) -> (0x000000AA)
        We bitwise OR the bytes, so we get (0xBBCCDDAA) which is the rotated word.*/
    return (word << 8) | (word >> 24);
}

/**************************************************************
 * Function to propogate a byte forward through the AES S-box
 * Parameters:
 * - byte: The byte to be substituted
 * Returns: The substituted byte
 ***************************************************************/
uint8_t sBoxForward(uint8_t byte) {
    /* Return the substituted byte from the S-Box */
    return AES_sBox[byte];
}

/**************************************************************
 * Function to back-propogate a byte through the AES S-box
 * Parameters:
 * - byte: The byte to be substituted
 * Returns: The substituted byte
 ***************************************************************/
uint8_t sBoxReverse(uint8_t byte) {
    /* Return the substituted byte from the inverse S-Box */
    return AES_Inverse_sBox[byte];
}

/**************************************************************
 * Function to substitute each byte in a 32-bit word using the AES S-box
 * Parameters:
 * - word: The 32-bit word to be substituted
 * Returns: The substituted 32-bit word
 ***************************************************************/
uint32_t subWord(uint32_t word) {
    /* For each byte in word, substitute it for the SBox of it */
    return (static_cast<uint32_t>(sBoxForward((word >> 24) & 0xFF)) << 24) |
           (static_cast<uint32_t>(sBoxForward((word >> 16) & 0xFF)) << 16) |
           (static_cast<uint32_t>(sBoxForward((word >> 8) & 0xFF)) << 8) |
           (static_cast<uint32_t>(sBoxForward(word & 0xFF)));
}

/**************************************************************
 * Function to generate a round key from the main key#
 * Parameters:
 * - key: The main encryption key as a string containing 0-N 32-bit words
 * - keyLength: The length (N) of the main key in bits (128, 192, or 256)
 * - expandedKey: Reference to a string where the generated round key will be stored
 * Returns: 0 on success, -1 on failure
 **************************************************************/  
int generateExpandedKey(std::string key, uint16_t keyLength, std::string &expandedKey) {
    /* Local variable declarations */
    uint8_t roundsNeeded;
    uint32_t rcon[MAX_ROUNDS_NEEDED]; // Round constant

    /* Validate key length to ensure it is 128, 192 or 256 */
    if (keyLength != 128 && keyLength != 192 && keyLength != 256) {
        std::cerr << "Invalid key length. Must be 128, 192, or 256 bits." << std::endl;
        return -1;
    }

    /* Determine number of rounds based on key length */
    switch (keyLength)
    {
    case 128:
        roundsNeeded = 11;
        break;
    case 192:
        roundsNeeded = 13;
        break;
    case 256:
        roundsNeeded = 15;
        break;
    default:
        break; // Should never reach here due to prior validation
    }
    /* The round constant is a static array that never changes so can be defined */
    rcon[0] = 0x01000000;
    rcon[1] = 0x02000000;
    rcon[2] = 0x04000000;
    rcon[3] = 0x08000000;
    rcon[4] = 0x10000000;
    rcon[5] = 0x20000000;
    rcon[6] = 0x40000000;
    rcon[7] = 0x80000000;
    rcon[8] = 0x1B000000;
    rcon[9] = 0x36000000;
    /* If we need more rounds, they can be generated below */
    for(uint8_t round = 11; round <= roundsNeeded; ++round) {
        /* Generate round constant for this round */
        uint16_t temp_rcon = 0;
        if (rcon[round-2]>>24 < 0x80) {
            temp_rcon = (rcon[round-2] >> 24) * 0x02; // Multiply by 2 after shifting to the right so that the first byte is in the least significant position
        } else {
            temp_rcon = temp_rcon ^ 0x11B; // XOR with 0x11B it will be wrapped when its shifted back to the left
        } 
        rcon[round-1] = static_cast<uint32_t>(temp_rcon) << 24; // Shift back so that the Byte is in the correct position   
    }
    
    /* Key expansion */
    for (uint8_t expandedRound = 0; expandedRound < 4 * roundsNeeded; expandedRound++){
        if (expandedRound < roundsNeeded){
            /* First round keys are just the original key */
            expandedKey.append(key, expandedRound * 4, 4);
        } 
        else if (expandedRound >= roundsNeeded && expandedRound % roundsNeeded == 0){
            /* Every Nth round we do: W(i-N) XOR SubWord(RotWord(W(i-1))) XOR rcon(i/N)*/
            uint32_t tempWord = expandedKey[expandedRound - 1]; // W(i-1)
            tempWord = subWord(rotateWord(tempWord)); // SubWord(RotWord(W(i-1)))
            tempWord = tempWord ^ expandedKey[expandedRound - roundsNeeded]; // XOR with W(i-N)
            tempWord = tempWord ^ rcon[(expandedRound / roundsNeeded) - 1]; // XOR with rcon(i/N)
            expandedKey.append(reinterpret_cast<const char*>(&tempWord), 4); // Append to the expanded key
        }
        else if (expandedRound >= roundsNeeded && expandedRound > 6 && expandedRound % roundsNeeded == 4){
            /* Every 4th round after the first N rounds we do: W(i-N) XOR SubWord(W(i-1)) */
            uint32_t tempWord = expandedKey[expandedRound - 1]; // W(i-1)
            tempWord = subWord(tempWord); // SubWord(W(i-1))
            tempWord = tempWord ^ expandedKey[expandedRound - roundsNeeded]; // XOR with W(i-N)
            expandedKey.append(reinterpret_cast<const char*>(&tempWord), 4); // Append to the expanded key
        }
        else {
            /* All other rounds we do: W(i-N) XOR W(i-1) */
            uint32_t tempWord = expandedKey[expandedRound - 1]; // W(i-1)
            tempWord = tempWord ^ expandedKey[expandedRound - roundsNeeded]; // XOR with W(i-N)
            expandedKey.append(reinterpret_cast<const char*>(&tempWord), 4); // Append to the expanded key
        }
    }
    return 0;
}

int main() {
    uint8_t outputByte;
    uint32_t rotatedWord;
    std::string roundKeys,expandedKey;
    expandedKey = generateExpandedKey("example", 128, roundKeys);
    return 0;
}