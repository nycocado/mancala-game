#define _POSIX_C_SOURCE 200809L // Required for getline to work
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct // Player data structure
{
        char* name;
        int games;
        int wins;
        int draws;
        int losses;
} PlayerData;

typedef struct // Players controller structure
{
        int numPlayers;
        PlayerData* playersData;
} PlayersController;

typedef struct // Board data structure
{
        char* playerA;
        char* playerB;
        int round;      // 1 -> Player A, 2 -> Player B
        int difficulty; // 0 -> Not auto game, 1 -> Normal auto game, 2 ->
                        // Advanced auto game
        int houses[14];
        bool gameStarted; // true -> Game started, false -> Game not started
} BoardController;

void print_title()
{
    printf(" __  __   _   _  _  ___   _   _      _   \n");
    printf("|  \\/  | /_\\ | \\| |/ __| /_\\ | |    /_\\  \n");
    printf("| |\\/| |/ _ \\| .` | (__ / _ \\| |__ / _ \\ \n");
    printf("|_|  |_/_/ \\_\\_|\\_|\\___/_/ \\_\\____/_/ \\_\\ \n");
    printf("\n");
}

void print_commands()
{
    printf("C - Comandos\n");
    printf("RJ - Registar jogador\n");
    printf("LJ - Listar jogadores\n");
    printf("IJ - Iniciar jogo\n");
    printf("IJA - Iniciar jogo automático\n");
    printf("J - Efetuar jogada\n");
    printf("D - Desistir do jogo\n");
    printf("S - Salvar pontuação do jogo\n");
    printf("L - Ler pontuação do jogo\n");
    printf("Enter - Sair\n");
}

bool has_player(
    const PlayersController* players,
    const char* name
) // Checks if such player exists
{
    if (name == NULL) // Checks if player name is NULL
    {
        return true; // Returns true (useful for the resign function)
    }
    for (int i = 0; i < (players->numPlayers); i++)
    {
        if (strcmp(players->playersData[i].name, name) ==
            0) // Checks if player name matches the search name
        {
            return true; // Returns true if found
        }
    }
    return false; // Returns false if not found
}

bool are_playing(
    const BoardController* board,
    const char* name
) // Checks if the players are playing
{
    if (name == NULL) // Checks if player name is NULL
    {
        return true; // Returns true (useful for the resign function)
    }
    if (strcmp(board->playerA, name) == 0 ||
        strcmp(board->playerB, name) ==
            0) // Checks if player name matches either player
    {
        return true; // Returns true if found
    }
    return false; // Returns false if not found
}

bool can_play(BoardController* board, char* name)
{
    if (strcmp(board->playerA, name) == 0 && board->round == 1)
    {
        return true;
    }
    else if (strcmp(board->playerB, name) == 0 && board->round == 2)
    {
        return true;
    }
    return false;
}

void register_player(
    PlayersController* players,
    const char* name,
    const int games,
    const int wins,
    const int draws,
    const int losses
) // Registers players
{
    players->playersData = realloc(
        players->playersData, (players->numPlayers + 1) * sizeof(PlayerData)
    ); // Reallocates memory for the players array
    players->playersData[players->numPlayers].name =
        malloc(strlen(name) + 1); // Allocates memory for the player name
    strcpy(
        players->playersData[players->numPlayers].name, name
    ); // Copies player name to the players array
    players->playersData[players->numPlayers].games =
        games; // Assigns games, wins, draws and losses to the player
    players->playersData[players->numPlayers].wins = wins;
    players->playersData[players->numPlayers].draws = draws;
    players->playersData[players->numPlayers].losses = losses;
    (players->numPlayers)++; // Increments player count
}

int compare_players(
    const PlayerData* a,
    const PlayerData* b
) // Compares two players
{
    if (b->wins == a->wins) // Checks if win count is equal
    {
        return strcmp(a->name, b->name); // If so, compares player names
    }
    return (b->wins - a->wins); // Otherwise, compares win counts
}

int search_id(
    const PlayersController* players,
    const char* name
) // Searches for a player's index
{
    for (int i = 0; i < players->numPlayers; i++)
    {
        if (strcmp(players->playersData[i].name, name) ==
            0) // Checks if player name matches the search name
        {
            return i; // Returns the player's index
        }
    }
    return -1; // Returns -1 if not found
}

void print_players(const PlayersController* players) // Lists the players
{
    for (int i = 0; i < players->numPlayers; i++)
    {
        printf(
            "%s %d %d %d %d\n",
            players->playersData[i].name,
            players->playersData[i].games,
            players->playersData[i].wins,
            players->playersData[i].draws,
            players->playersData[i].losses
        );
    }
}

void print_board(const BoardController* board) // Prints the board
{
    printf("%s X %s\n", board->playerA, board->playerB); // Prints player names
    printf("[%d]\t", board->houses[13]); // Prints player B's store
    printf(
        "(%d)\t(%d)\t(%d)\t(%d)\t(%d)\t(%d)\n",
        board->houses[12],
        board->houses[11],
        board->houses[10],
        board->houses[9],
        board->houses[8],
        board->houses[7]
    ); // Prints player B's houses
    printf(
        "\t(%d)\t(%d)\t(%d)\t(%d)\t(%d)\t(%d)\t",
        board->houses[0],
        board->houses[1],
        board->houses[2],
        board->houses[3],
        board->houses[4],
        board->houses[5]
    );                                  // Prints player A's houses
    printf("[%d]\n", board->houses[6]); // Prints player A's store
}

void free_players(
    PlayersController* players
) // Frees memory of the players array
{
    for (int i = 0; i < players->numPlayers; i++)
    {
        free(players->playersData[i].name);
    }
    free(players->playersData);
}

void save_game(PlayersController* players, char* filename) // Saves the game
{
    FILE* f = fopen(filename, "w+");              // Opens file for writing
    for (int i = 0; i < players->numPlayers; i++) // Iterates over players array
    {
        fprintf(
            f,
            "%s %d %d %d %d\n",
            players->playersData[i].name,
            players->playersData[i].games,
            players->playersData[i].wins,
            players->playersData[i].draws,
            players->playersData[i].losses
        ); // Writes player data to file
    }
    fclose(f); // Closes the file
}

void read_game(PlayersController* players, char* filename) // Reads the game
{
    FILE* f = fopen(filename, "r"); // Opens file for reading
    char* line = NULL;              // Variable to hold the read line
    size_t len = 0;
    while (getline(&line, &len, f) != -1) // While there are lines to read
    {
        char* name = strtok(line, " ");      // Splits line by spaces
        int games = atoi(strtok(NULL, " ")); // Converts string to integer
        int wins = atoi(strtok(NULL, " "));
        int draws = atoi(strtok(NULL, " "));
        int losses = atoi(strtok(NULL, " "));
        register_player(
            players, name, games, wins, draws, losses
        ); // Registers the player
    }
    free(line); // Frees the memory allocated for line
    fclose(f);  // Closes the file
}

void start_game(
    const char* nameA,
    const char* nameB,
    BoardController* board,
    const PlayersController* players
) // Starts the game
{
    board->gameStarted = true; // Marks game as started
    board->playerA = players->playersData[search_id(players, nameA)]
                         .name; // Loads player A info
    board->playerB = players->playersData[search_id(players, nameB)]
                         .name; // Loads player B info
    board->round = 1;

    for (int i = 0; i < 14; i++) // Initializes player houses
    {
        if (i == 6 || i == 13)
        {
            board->houses[i] = 0; // Initializes stores with 0 seeds
        }
        else
        {
            board->houses[i] = 4; // Initializes houses with 4 seeds
        }
    }
}

int calc_last_index(
    const int index,
    const BoardController* board,
    const char* name
) // Calculates the last index
{
    int x = index + board->houses[index]; // Calculates the last index

    if (strcmp(name, board->playerA) == 0) // Checks if player is A
    {
        while (x > 12) // Checks if index is greater than 12 (wraps around, 12
                       // avoids opponent's store)
        {
            x -= 13; // Subtracts 13 (13 -> 14 (total houses) - 1 (opponent's
                     // store))
        }
    }
    else if (strcmp(name, board->playerB) == 0) // Checks if player is B
    {
        while (x > 13) // Checks if index is greater than 13 (wraps around, 13
                       // allows player B's store)
        {
            x -= 14; // Subtracts 14 (14 -> total houses)
        }
        if (x == 6)
        {
            x = 7; // Checks if index is player A's store and skips it
        }
    }
    return x; // Returns the last index
}

bool play_again_verify(
    const int index,
    const BoardController* board,
    const char* name
) // Checks if player earns another turn
{
    int finalIndex =
        calc_last_index(index, board, name); // Calculates the last index

    if (strcmp(name, board->playerA) == 0 &&
        finalIndex ==
            6) // Checks if player is A and last index is player A's store
    {
        return true; // Returns true
    }
    else if (
        strcmp(name, board->playerB) == 0 && finalIndex == 13
    ) // Checks if player is B and last index is player B's store
    {
        return true; // Returns true
    }
    return false; // Returns false
}

bool capture_seeds_verify(
    const int index,
    const BoardController* board,
    const char* name
) // Checks if player can capture seeds
{
    int finalIndex = calc_last_index(
        index, board, board->playerA
    ); // Calculates the last index

    if (board->houses[index] == 0) // Checks if house has 0 seeds
    {
        return false; // Returns false
    }
    if (strcmp(name, board->playerA) == 0 && finalIndex >= 0 &&
        finalIndex < 6 &&
        board->houses[finalIndex] == 0) // Checks if player is A, index is a
                                        // player A house, and house has 1 seed
    {
        return true; // Returns true
    }
    else if (
        strcmp(name, board->playerB) == 0 && finalIndex > 6 &&
        finalIndex < 13 && board->houses[finalIndex] == 0
    ) // Checks if player is B, index is a player B house, and house has 1 seed
    {
        return true; // Returns true
    }
    return false; // Returns false
}

void capture_seeds(
    const int finalIndex,
    BoardController* board,
    const char* name
) // Captures seeds
{ // 12 -> 14 (total houses) - 2 (stores) - gives the opponent's equivalent
  // house
    if (strcmp(name, board->playerA) == 0) // Checks if player is A
    {
        board->houses[6] +=
            board->houses[finalIndex] +
            board->houses[12 - finalIndex]; // Assigns seeds to player A's store
    }
    else if (strcmp(name, board->playerB) == 0) // Checks if player is B
    {
        board->houses[13] +=
            board->houses[finalIndex] +
            board->houses[12 - finalIndex]; // Assigns seeds to player B's store
    }

    board->houses[finalIndex] = 0; // Sets house seeds to 0
    board->houses[12 - finalIndex] =
        0; // Sets opponent's equivalent house seeds to 0
}

int end_game_verify(
    const BoardController* board
) // Checks if the game has ended
{
    int countA = 0; // Counts empty houses for player A
    int countB = 0; // Counts empty houses for player B

    for (int i = 0; i < 6; i++)
    {
        if (board->houses[i] == 0) // Checks if player A's house is empty
        {
            countA++; // Increments player A's empty house count
        }
    }
    for (int i = 7; i < 13; i++)
    {
        if (board->houses[i] == 0) // Checks if player B's house is empty
        {
            countB++; // Increments player B's empty house count
        }
    }

    if (countA == 6) // Checks if all player A's houses are empty
    {
        return 1; // Returns 1
    }
    else if (countB == 6) // Checks if all player B's houses are empty
    {
        return 2; // Returns 2
    }
    else // If neither condition is true
    {
        return 0; // Returns 0
    }
}

void end_game_assign(
    const BoardController* board,
    PlayersController* players
) // Assigns wins, draws and losses
{
    if (board->houses[6] > board->houses[13]) // Checks if player A won
    {
        players->playersData[search_id(players, board->playerB)]
            .losses++; // Assigns loss to player B
        players->playersData[search_id(players, board->playerA)]
            .wins++; // Assigns win to player A
    }
    else if (board->houses[6] < board->houses[13]) // Checks if player B won
    {
        players->playersData[search_id(players, board->playerB)]
            .wins++; // Assigns win to player B
        players->playersData[search_id(players, board->playerA)]
            .losses++; // Assigns loss to player A
    }
    else // If it's a draw
    {
        players->playersData[search_id(players, board->playerB)]
            .draws++; // Assigns draw to player B
        players->playersData[search_id(players, board->playerA)]
            .draws++; // Assigns draw to player A
    }
    players->playersData[search_id(players, board->playerA)]
        .games++; // Assigns game to player A
    players->playersData[search_id(players, board->playerB)]
        .games++; // Assigns game to player B
}

void end_game(
    BoardController* board,
    PlayersController* players
) // Ends the game
{
    if (end_game_verify(board) != 0) // Checks if the game has ended
    {
        if (end_game_verify(board) ==
            1) // Checks if player A's houses are empty
        {
            for (int i = 7; i < 13; i++) // Iterates over player B's houses
            {
                board->houses[13] +=
                    board->houses[i]; // Assigns player B's seeds to player B's
                                      // store
            }
        }
        else if (end_game_verify(board) == 2) // Checks if player B's houses are
                                              // empty
        {
            for (int i = 0; i < 6; i++) // Iterates over player A's houses
            {
                board->houses[6] +=
                    board->houses[i]; // Assigns player A's seeds to player A's
                                      // store
            }
        }
        end_game_assign(board, players); // Assigns wins, draws and losses
        board->gameStarted = false;      // Marks game as ended
    }
}

void resign_game(
    BoardController* board,
    PlayersController* players,
    char* nameA,
    char* nameB
) // Resigns the game
{
    if (nameB == NULL) // Checks if nameB is null
    {
        if (strcmp(nameA, board->playerA) ==
            0) // If so, checks if nameA is player A
        {
            players->playersData[search_id(players, board->playerA)]
                .losses++; // Assigns loss to player A
            players->playersData[search_id(players, board->playerB)]
                .wins++; // Assigns win to player B
        }
        else // Otherwise
        {
            players->playersData[search_id(players, board->playerA)]
                .wins++; // Assigns win to player A
            players->playersData[search_id(players, board->playerB)]
                .losses++; // Assigns loss to player B
        }
    }
    else // If not null
    {
        players->playersData[search_id(players, board->playerA)]
            .losses++; // Assigns loss to player A
        players->playersData[search_id(players, board->playerB)]
            .losses++; // Assigns loss to player B
    }
    players->playersData[search_id(players, board->playerA)]
        .games++; // Assigns game to player A
    players->playersData[search_id(players, board->playerB)]
        .games++;               // Assigns game to player B
    board->gameStarted = false; // Marks game as ended
}

int fix_position(
    int position,
    BoardController* board,
    char* name
) // Corrects house index
{
    if (strcmp(board->playerA, name) == 0) // Checks if player is A
    {
        return position - 1; // Returns corrected house index
    }
    else if (strcmp(board->playerB, name) == 0) // Checks if player is B
    {
        return position + 6; // Returns corrected house index
    }
    return 0; // Returns 0 if neither
}

void fix_table(
    int* index,
    BoardController* board,
    char* name
) // Corrects array loop index
{
    if (strcmp(board->playerA, name) == 0 &&
        *index == 13) // Checks if player is A and index is player B's store
    {
        *index = 0; // Skips that house
    }
    else if (
        strcmp(board->playerB, name) == 0 && *index == 6
    ) // Checks if player is B and index is player A's store
    {
        *index = 7; // Skips that house
    }
    else if (*index == 14) // Checks if index exceeds 14
    {
        *index = 0; // Wraps back to house 0
    }
}

void standard_play(
    int index,
    BoardController* board,
    char* name
) // Performs a standard move
{
    bool playAgainVerify = play_again_verify(
        index, board, name
    ); // Checks if player earns another turn
    bool captureVerify = capture_seeds_verify(
        index, board, name
    ); // Checks if player can capture seeds

    int seeds = board->houses[index]; // Assigns house seed count to seeds
    board->houses[index] = 0;         // Sets house seeds to 0

    while (seeds > 0) // While there are seeds
    {
        index++;                        // Increments index
        fix_table(&index, board, name); // Corrects house index
        board->houses[index]++;         // Distributes seeds
        seeds--;                        // Decrements seed count
    }

    if (captureVerify == true) // Checks if player can capture seeds
    {
        capture_seeds(index, board, name); // Captures seeds
    }

    if (playAgainVerify == false) // Switches rounds
    {
        if (strcmp(name, board->playerA) ==
            0) // Checks if player is A and has no extra turn
        {
            board->round = 2; // Sets round to 2
        }
        else if (strcmp(name, board->playerB) == 0) // Checks if player is B and
                                                    // has no extra turn
        {
            board->round = 1; // Sets round to 1
        }
    }
}

void auto_game(BoardController* board) // Performs an automatic move
{
    if (board->difficulty == 1) // Checks if game is normal auto difficulty
    {
        for (int i = 1; i < 7; i++) // Iterates over all positions
        {
            int index =
                fix_position(i, board, board->playerB); // Corrects house index
            if (board->houses[index] != 0) // Checks if house has seeds
            {
                standard_play(index, board, board->playerB); // Plays if it does
                break;                                       // Breaks the loop
            }
        }
    }
    else if (board->difficulty == 2) // Checks if game is advanced auto
                                     // difficulty
    {
        bool played = false; // Tracks whether CPU has played

        for (int i = 1; i < 7; i++) // Iterates over all positions
        {
            int index =
                fix_position(i, board, board->playerB); // Corrects house index
            if (capture_seeds_verify(index, board, board->playerB) ==
                true) // Checks if CPU can capture seeds
            {
                standard_play(index, board, board->playerB); // Plays if it can
                played = true; // Marks CPU as played
                break;         // Breaks the loop
            }
            else if (
                play_again_verify(index, board, board->playerB) == true
            ) // Checks if CPU earns another turn
            {
                standard_play(index, board, board->playerB); // Plays if it does
                played = true; // Marks CPU as played
                break;         // Breaks the loop
            }
        }

        if (played == false) // Checks if CPU has not played
        {
            for (int i = 6; i > 0; i--) // Iterates over all positions
            {
                int index = fix_position(
                    i, board, board->playerB
                );                             // Corrects house index
                if (board->houses[index] != 0) // Checks if house has seeds
                {
                    standard_play(
                        index, board, board->playerB
                    );     // Plays if it does
                    break; // Breaks the loop
                }
            }
        }
    }
}

int main()
{
    PlayersController
        players; // Holds player count and pointer to players array
    players.playersData = NULL; // Initialized to NULL (useful for realloc and
                                // checking if memory was allocated)
    players.numPlayers = 0;     // Initialized to 0
    BoardController board;      // Board controller

    register_player(&players, "CPU", 0, 0, 0, 0); // Registers the CPU

    board.gameStarted = false; // Marks game as not started

    char* line = NULL; // Variable to hold the read line
    size_t len = 0;    // Variable to hold the read line length

    int initialTemp = 0;

    system("clear"); // Clears the screen
    print_title();   // Prints the game title

    while (true) // Infinite loop
    {
        if (initialTemp == 0) // Checks if it's the first loop iteration
        {
            print_commands(); // Prints commands
            initialTemp++;    // Increments variable
        }
        printf("\n> "); // Prints prompt

        getline(&line, &len, stdin); // Reads the line
        system("clear");             // Clears the screen

        line[strlen(line) - 1] = '\0'; // "RJ Bernardo\n\0" -> "RJ Bernardo\0"
        if (strlen(line) == 0)
        {
            break; // Exits the loop
        }

        print_title(); // Prints the game title

        char* operation = strtok(line, " "); // Gets the operation

        if (strcmp(operation, "C") == 0)
        {
            // Comandos
            print_commands(); // Prints commands
        }
        else if (strcmp(operation, "RJ") == 0)
        {
            // Registar jogador
            char* name = strtok(NULL, " ");
            if (name == NULL)
            {
                printf("Nome inválido.\n");
            }
            else if (has_player(&players, name) == true) // Checks if player is
                                                         // already registered
            {
                printf("Jogador existente.\n");
            }
            else // If player is not registered
            {
                register_player(
                    &players, name, 0, 0, 0, 0
                ); // Registers the player
                printf("Jogador %s registado com sucesso.\n", name);
            }
            name = NULL; // Resets variable to NULL
        }
        else if (strcmp(operation, "LJ") == 0)
        {
            // Listar jogador
            if (players.numPlayers ==
                1) // Checks if there are registered players
            {
                printf("Sem jogadores registados.\n");
            }
            else // If there are registered players, lists them
            {
                qsort(
                    players.playersData,
                    players.numPlayers,
                    sizeof(PlayerData),
                    (int (*)(const void*, const void*))compare_players
                );                       // Sorts players
                print_players(&players); // Lists players
            }
        }
        else if (strcmp(operation, "IJ") == 0)
        {
            // Iniciar jogo
            char* nameA = strtok(NULL, " ");
            char* nameB = strtok(NULL, " ");

            if (board.gameStarted == true) // Checks if game is already started
            {
                printf("Existe um jogo em curso.\n");
            }
            else if (
                has_player(&players, nameA) == false ||
                has_player(&players, nameB) == false
            ) // Checks if players exist
            {
                printf("Jogador inexistente.\n");
            }
            else // If game has not started
            {
                board.difficulty = 0; // Sets difficulty to 0 (not auto game)
                start_game(nameA, nameB, &board, &players); // Starts the game
                printf("Jogo iniciado com sucesso.\n");
            }
        }
        else if (strcmp(operation, "IJA") == 0)
        {
            // Start automatic game
            char* nameA = strtok(NULL, " ");
            char* nameB = "CPU";
            char* difficulty = strtok(NULL, " ");

            if (board.gameStarted == true) // Checks if game is already started
            {
                printf("Existe um jogo em curso.\n");
            }
            else if (has_player(&players, nameA) == false) // Checks if player
                                                           // exists
            {
                printf("Jogador inexistente.\n");
            }
            else if (
                strcmp(difficulty, "Normal") != 0 &&
                strcmp(difficulty, "Avançado") != 0
            ) // Checks difficulty
            {
                printf("Dificuldade inválida.\n");
            }
            else
            {
                if (strcmp(difficulty, "Normal") ==
                    0) // Checks if difficulty is normal
                {
                    board.difficulty = 1; // Sets difficulty to 1 for normal
                }
                else if (strcmp(difficulty, "Avançado") == 0) // Checks if
                                                              // difficulty is
                                                              // advanced
                {
                    board.difficulty = 2; // Sets difficulty to 2 for advanced
                }
                start_game(nameA, nameB, &board, &players); // Starts the game
                printf(
                    "Jogo automático de nível %s iniciado com sucesso.\n",
                    difficulty
                );
            }
        }
        else if (strcmp(operation, "J") == 0)
        {
            // Efetuar Jogada
            char* name = strtok(NULL, " ");
            int position = atoi(strtok(NULL, " "));

            if (board.gameStarted == false) // Checks if game has started
            {
                printf("Não existe jogo em curso.\n");
            }
            else if (has_player(&players, name) == false) // Checks if player
                                                          // exists
            {
                printf("Jogador inexistente.\n");
            }
            else if (are_playing(&board, name) == false) // Checks if player is
                                                         // in the game
            {
                printf("Jogador não participa no jogo em curso.\n");
            }
            else if (can_play(&board, name) == false) // Checks if it's the
                                                      // player's turn
            {
                printf("Não é a vez do jogador %s.\n", name);
            }
            else if (position < 1 || position > 6) // Checks if house is valid
            {
                printf("Casa inválida.\n");
            }
            else if (
                board.houses[fix_position(position, &board, name)] == 0
            ) // Checks if house has seeds
            {
                printf("Casa sem sementes.\n");
            }
            else
            {
                int index = fix_position(
                    position, &board, name
                ); // Corrects house index
                bool playAgainVerify = play_again_verify(
                    index, &board, name
                ); // Checks if player earns another turn

                if (board.difficulty == 0 ||
                    (board.difficulty != 0 &&
                     board.round ==
                         1)) // Checks if game is manual or round is 1
                {
                    standard_play(
                        index, &board, name
                    ); // Performs a standard move
                }
                while (board.round == 2 &&
                       board.difficulty !=
                           0) // Checks if round is 2 and game is auto
                {
                    auto_game(
                        &board
                    ); // Performs auto moves until round changes
                }

                end_game(
                    &board, &players
                ); // Checks if game ended and assigns wins, draws and losses

                if (board.gameStarted == false) // If game ended
                {
                    printf("Jogo terminado.\n");
                    printf("%s %d\n", board.playerA, board.houses[6]);
                    printf("%s %d\n", board.playerB, board.houses[13]);
                }
                else
                {
                    if (playAgainVerify == true &&
                        strcmp(name, "CPU") !=
                            0) // Checks if player earns another turn (and is
                               // not CPU)
                    {
                        printf(
                            "O jogador %s tem direito a outra jogada.\n", name
                        );
                    }
                    else if (strcmp(name, "CPU") != 0) // If no extra turn (and
                                                       // is not CPU)
                    {
                        printf("Jogada efetuada com sucesso.\n");
                    }
                }
            }
        }
        else if (strcmp(operation, "D") == 0)
        {
            // Desistir do jogo
            char* nameA = strtok(NULL, " ");
            char* nameB = strtok(NULL, " ");

            if (board.gameStarted == false) // Checks if game has started
            {
                printf("Não existe jogo em curso.\n");
            }
            else if (
                has_player(&players, nameA) == false ||
                has_player(&players, nameB) == false
            ) // Checks if players exist
            {
                printf("Jogador inexistente.\n");
            }
            else if (
                are_playing(&board, nameA) == false ||
                are_playing(&board, nameB) == false
            ) // Checks if players are in the game
            {
                printf("Jogador não participa no jogo em curso.\n");
            }
            else // If game has started
            {
                resign_game(&board, &players, nameA, nameB); // Resigns the game
                printf("Jogo terminado com sucesso.\n");
            }
        }
        else if (strcmp(operation, "G") == 0)
        {
            // Gravar
            char* filename = strtok(NULL, " ");
            save_game(&players, filename); // Saves the game
            printf("Jogo gravado com sucesso.\n");
        }
        else if (strcmp(operation, "L") == 0)
        {
            // Ler
            if (board.gameStarted == true) // Checks if game has started
            {
                printf("Existe um jogo em curso.\n");
            }
            else // If game has not started
            {
                free_players(&players);     // Frees players array memory
                players.playersData = NULL; // Resets players array to NULL
                players.numPlayers = 0;     // Reinitializes player count
                char* filename = strtok(NULL, " "); // Splits line by spaces
                read_game(&players, filename);      // Reads the game
                printf("Jogo lido com sucesso.\n");
            }
        }
        else
        {
            printf("Instrução inválida.\n");
        }
        free(line);  // Frees the memory allocated for line
        line = NULL; // Resets variable to NULL

        if (board.gameStarted == true) // Checks if game has started
        {
            printf("\n");
            print_board(&board); // Prints the board
            printf("\n");
        }
    }
    if (line != NULL) // Frees line memory if not null
    {
        free(line);
    }
    free_players(&players); // Frees players array memory
    return 0;
}
