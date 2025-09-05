#include <iostream>
#include <cstdint>
#include <cstring>
#define MAX_ROUNDS_NEEDED 15
/**************************************************************
 * Function to generate a round key from the main key#
 * Parameters:
 * - key: The main encryption key as a string containing 0-N 32-bit words
 * - keyLength: The length (N) of the main key in bits (128, 192, or 256)
 * - roundKey: Reference to a string where the generated round key will be stored
 * Returns: 0 on success, -1 on failure
 **************************************************************/  
int generateRoundKeys(std::string key, uint16_t keyLength, std::string &roundKeys) {
    /* Local variable declarations */
    uint8_t roundsNeeded;
    uint8_t rcon[MAX_ROUNDS_NEEDED][4]; // Round constant

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

    for(uint8_t round = 1; round <= roundsNeeded; ++round) {
        /* Generate round constant for this round */
        memset(&rcon[round-1][1], 0, sizeof(uint8_t) * 3);
        if (round == 1) {
            rcon[round-1][0] = 0x01;
        } else if (rcon[round-2][0] < 0x80) {
            rcon[round-1][0] = rcon[round-2][0] * 0x02; // Multiply by 2
        } else {
            rcon[round-1][0] = static_cast<uint8_t>(static_cast<uint16_t>(rcon[round-2][0] * 0x02) ^ 0x11B); // XOR with 0x11B at 16 bit operation, then cast to 8 bit to wrap it.
        }    
    }
    
    printf("Rounds needed: %d\n", roundsNeeded);
    printf("Rcon values:\n");
    for (uint8_t i = 0; i < roundsNeeded - 1; ++i) {
        printf(
            "Round %d: %02X (%d) %02X (%d) %02X (%d) %02X (%d)\n",
            i + 1,
            rcon[i][0], rcon[i][0],
            rcon[i][1], rcon[i][1],
            rcon[i][2], rcon[i][2],
            rcon[i][3], rcon[i][3]
        );
    }
    return 0;
}

int main() {
    std::string roundKeys;
    generateRoundKeys("example", 128, roundKeys);
}