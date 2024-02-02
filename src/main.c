#define _POSIX_C_SOURCE 200809L // Serve para o getline funcionar
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct // Estrutura de dados do jogador
{
    char *name;
    int games;
    int wins;
    int draws;
    int losses;
} PlayerData;

typedef struct // Estrutura do controlador de jogadores
{
    int numPlayers;
    PlayerData *playersData;
} PlayersController;

typedef struct // Estrutura de dados do tabuleiro
{
    char *playerA;
    char *playerB;
    int round;      // 1 -> Jogador A, 2 -> Jogador B
    int difficulty; // 0 -> Não é jogo automático, 1 -> Jogo automático de nível normal, 2 -> Jogo automático de nível avançado
    int houses[14];
    bool gameStarted; // true -> Jogo iniciado, false -> Jogo não iniciado
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
    printf("L - Ler pontuação jogo\n");
    printf("Enter - Sair\n");
}

bool has_player(const PlayersController *players, const char *name) // Função que verifica se existe tal jogador
{
    if (name == NULL) // Verifica se o nome do jogador é NULL
    {
        return true; // Caso seja, retorna true (me da jeito no desistir)
    }
    for (int i = 0; i < (players->numPlayers); i++)
    {
        if (strcmp(players->playersData[i].name, name) == 0) // Verifica se o nome do jogador é igual ao nome do jogador que estamos a procurar
        {
            return true; // Caso seja, retorna true
        }
    }
    return false; // Caso não seja, retorna false
}

bool are_playing(const BoardController *board, const char *name) // Função que verifica se os jogadores estão jogando
{
    if (name == NULL) // Verifica se o nome do jogador é NULL
    {
        return true; // Caso seja, retorna true (me da jeito no desistir)
    }
    if (strcmp(board->playerA, name) == 0 || strcmp(board->playerB, name) == 0) // Verifica se o nome do jogador é igual ao nome do jogador que estamos a procurar
    {
        return true; // Caso seja, retorna true
    }
    return false; // Caso não seja, retorna false
}

bool can_play(BoardController *board, char *name)
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

void register_player(PlayersController *players, const char *name, const int games, const int wins, const int draws, const int losses) // Função que regista jogadores
{
    players->playersData = realloc(players->playersData, (players->numPlayers + 1) * sizeof(PlayerData)); // Realoca o espaço de memória para o vetor de jogadores
    players->playersData[players->numPlayers].name = malloc(strlen(name) + 1);                            // Aloca o espaço de memória para o nome do jogador
    strcpy(players->playersData[players->numPlayers].name, name);                                         // Copia o nome do jogador para o vetor de jogadores
    players->playersData[players->numPlayers].games = games;                                              // Atribui os jogos, vitorias, empates e derrotas do jogador
    players->playersData[players->numPlayers].wins = wins;
    players->playersData[players->numPlayers].draws = draws;
    players->playersData[players->numPlayers].losses = losses;
    (players->numPlayers)++; // Incrementa o numero de jogadores
}

int compare_players(const PlayerData *a, const PlayerData *b) // Função que compara dois jogadores
{
    if (b->wins == a->wins) // Verifica se o numero de vitorias é igual
    {
        return strcmp(a->name, b->name); // Caso seja, compara os nomes dos jogadores
    }
    return (b->wins - a->wins); // Caso não seja, compara o numero de vitorias dos jogadores
}

int search_id(const PlayersController *players, const char *name) // Função que procura o índice de um jogador
{
    for (int i = 0; i < players->numPlayers; i++)
    {
        if (strcmp(players->playersData[i].name, name) == 0) // Verifica se o nome do jogador é igual ao nome do jogador que estamos a procurar
        {
            return i; // Caso seja, retorna o índice do jogador
        }
    }
    return -1; // Caso não seja, retorna -1
}

void print_players(const PlayersController *players) // Função que lista os jogadores
{
    for (int i = 0; i < players->numPlayers; i++)
    {
        printf("%s %d %d %d %d\n", players->playersData[i].name, players->playersData[i].games, players->playersData[i].wins, players->playersData[i].draws, players->playersData[i].losses);
    }
}

void print_board(const BoardController *board) // Função que imprime o tabuleiro
{
    printf("%s X %s\n", board->playerA, board->playerB);                                                                                                           // Imprime o nome dos jogadores
    printf("[%d]\t", board->houses[13]);                                                                                                                           // Imprime o poço do jogador B
    printf("(%d)\t(%d)\t(%d)\t(%d)\t(%d)\t(%d)\n", board->houses[12], board->houses[11], board->houses[10], board->houses[9], board->houses[8], board->houses[7]); // Imprime as casas dos jogadores B
    printf("\t(%d)\t(%d)\t(%d)\t(%d)\t(%d)\t(%d)\t", board->houses[0], board->houses[1], board->houses[2], board->houses[3], board->houses[4], board->houses[5]);  // Imprime as casas do jogador A
    printf("[%d]\n", board->houses[6]);                                                                                                                            // Imprime o poço do jogador A
}

void free_players(PlayersController *players) // Função que libera a memoria do vetor de jogadores
{
    for (int i = 0; i < players->numPlayers; i++)
    {
        free(players->playersData[i].name);
    }
    free(players->playersData);
}

void save_game(PlayersController *players, char *filename) // Função que salva o jogo
{
    FILE *f = fopen(filename, "w+");              // Abre o arquivo para escrita
    for (int i = 0; i < players->numPlayers; i++) // Percorre o vetor de jogadores
    {
        fprintf(f, "%s %d %d %d %d\n", players->playersData[i].name, players->playersData[i].games, players->playersData[i].wins, players->playersData[i].draws, players->playersData[i].losses); // Escreve no arquivo os dados do jogador
    }
    fclose(f); // Fecha o arquivo
}

void read_game(PlayersController *players, char *filename) // Função que lê o jogo
{
    FILE *f = fopen(filename, "r"); // Abre o arquivo para leitura
    char *line = NULL;              // Variavel que vai guardar a linha lida
    size_t len = 0;
    while (getline(&line, &len, f) != -1) // Enquanto houver linhas para ler
    {
        char *name = strtok(line, " ");      // Separa a linha por espaços
        int games = atoi(strtok(NULL, " ")); // Converte a string para inteiro
        int wins = atoi(strtok(NULL, " "));
        int draws = atoi(strtok(NULL, " "));
        int losses = atoi(strtok(NULL, " "));
        register_player(players, name, games, wins, draws, losses); // Registra o jogador
    }
    free(line); // Liberta o espaço de memória presente na variável line
    fclose(f);  // Fecha o arquivo
}

void start_game(const char *nameA, const char *nameB, BoardController *board, const PlayersController *players) // Função que inicia o jogo
{
    board->gameStarted = true;                                             // Indica que o jogo foi iniciado
    board->playerA = players->playersData[search_id(players, nameA)].name; // Recebe as informações do jogador A
    board->playerB = players->playersData[search_id(players, nameB)].name; // Recebe as informações do jogador B
    board->round = 1;

    for (int i = 0; i < 14; i++) // Inicializa as casas dos jogadores
    {
        if (i == 6 || i == 13)
        {
            board->houses[i] = 0; // Inicializa os poços com 0 sementes
        }
        else
        {
            board->houses[i] = 4; // Inicializa as casas com 4 sementes
        }
    }
}

int calc_last_index(const int index, const BoardController *board, const char *name) // Função que calcula o ultimo indice
{
    int x = index + board->houses[index]; // Calcula o ultimo indice

    if (strcmp(name, board->playerA) == 0) // Verifica se o jogador é o A
    {
        while (x > 12) // Verifica se o indice é maior que 12 (faz a equivalencia, o 12 é para não ter risco de corrigir para o poço do jogador B)
        {
            x -= 13; // Caso seja, subtrai 13 (13 -> 14 (Numero de casas totais) - 1 (Poço do adversário))
        }
    }
    else if (strcmp(name, board->playerB) == 0) // Verifica se o jogador é o B
    {
        while (x > 13) // Verifica se o indice é maior que 13 (faz a equivalencia, o 13 já permite corrigir para o poço do jogador B)
        {
            x -= 14; // Caso seja, subtrai 14 (14 -> 14 (Numero de casas totais))
        }
        if (x == 6)
        {
            x = 7; // Verifica se o indice é o poço do jogador A e se for, pula essa casa
        }
    }
    return x; // Retorna o ultimo indice
}

bool play_again_verify(const int index, const BoardController *board, const char *name) // Função que verifica se o jogador tem direito a outra jogada
{
    int finalIndex = calc_last_index(index, board, name); // Calcula o ultimo indice

    if (strcmp(name, board->playerA) == 0 && finalIndex == 6) // Verifica se o jogador é o A e se o ultimo indice é o poço do jogador A
    {
        return true; // Caso seja, retorna true
    }
    else if (strcmp(name, board->playerB) == 0 && finalIndex == 13) // Verifica se o jogador é o B e se o ultimo indice é o poço do jogador B
    {
        return true; // Caso seja, retorna true
    }
    return false; // Caso não seja, retorna false
}

bool capture_seeds_verify(const int index, const BoardController *board, const char *name) // Função que verifica se o jogador pode capturar as sementes
{
    int finalIndex = calc_last_index(index, board, board->playerA); // Calcula o ultimo indice

    if (board->houses[index] == 0) // Verifica se a casa tem 0 sementes
    {
        return false; // Caso seja, retorna false
    }
    if (strcmp(name, board->playerA) == 0 && finalIndex >= 0 && finalIndex < 6 && board->houses[finalIndex] == 0) // Verifica se o jogador é o A e se o indice é uma casa do jogador A e se a casa tem 1 semente
    {
        return true; // Caso seja, retorna true
    }
    else if (strcmp(name, board->playerB) == 0 && finalIndex > 6 && finalIndex < 13 && board->houses[finalIndex] == 0) // Verifica se o jogador é o B e se o indice é uma casa do jogador B e se a casa tem 1 semente
    {
        return true; // Caso seja, retorna true
    }
    return false; // Caso não seja, retorna false
}

void capture_seeds(const int finalIndex, BoardController *board, const char *name) // Função que captura as sementes
{                                                                                  // 12 -> 14 (Numero de casas totais) - 2 (Poços dos jogadores) - Isso nos da a casa equivalente do adversário
    if (strcmp(name, board->playerA) == 0)                                         // Verifica se o jogador é o A
    {
        board->houses[6] += board->houses[finalIndex] + board->houses[12 - finalIndex]; // Atribui as sementes ao poço do jogador A
    }
    else if (strcmp(name, board->playerB) == 0) // Verifica se o jogador é o B
    {
        board->houses[13] += board->houses[finalIndex] + board->houses[12 - finalIndex]; // Atribui as sementes ao poço do jogador B
    }

    board->houses[finalIndex] = 0;      // Atribui 0 sementes a casa
    board->houses[12 - finalIndex] = 0; // Atribui 0 sementes a casa equivalente do adversário
}

int end_game_verify(const BoardController *board) // Função que verifica se o jogo terminou
{
    int countA = 0; // Variavel que conta o numero de casas vazias do jogador A
    int countB = 0; // Variavel que conta o numero de casas vazias do jogador B

    for (int i = 0; i < 6; i++)
    {
        if (board->houses[i] == 0) // Verifica se a casa do jogador A está vazia
        {
            countA++; // Incrementa o numero de casas vazias do jogador A
        }
    }
    for (int i = 7; i < 13; i++)
    {
        if (board->houses[i] == 0) // Verifica se a casa do jogador B está vazia
        {
            countB++; // Incrementa o numero de casas vazias do jogador B
        }
    }

    if (countA == 6) // Verifica se as casas do jogador A estão vazias
    {
        return 1; // Caso seja, retorna 1
    }
    else if (countB == 6) // Verifica se as casas do jogador B estão vazias
    {
        return 2; // Caso seja, retorna 2
    }
    else // Caso nenhuma das condições seja verdadeira
    {
        return 0; // Retorna 0
    }
}

void end_game_assign(const BoardController *board, PlayersController *players) // Função que atribui as vitorias, empates e derrotas
{
    if (board->houses[6] > board->houses[13]) // Verifica se o jogador A ganhou
    {
        players->playersData[search_id(players, board->playerB)].losses++; // Caso seja, atribui a derrota ao jogador B
        players->playersData[search_id(players, board->playerA)].wins++;   // Atribui a vitoria ao jogador A
    }
    else if (board->houses[6] < board->houses[13]) // Verifica se o jogador B ganhou
    {
        players->playersData[search_id(players, board->playerB)].wins++;   // Caso seja, atribui a vitoria ao jogador B
        players->playersData[search_id(players, board->playerA)].losses++; // Atribui a derrota ao jogador A
    }
    else // Caso haja empate
    {
        players->playersData[search_id(players, board->playerB)].draws++; // Caso seja, atribui o empate ao jogador B
        players->playersData[search_id(players, board->playerA)].draws++; // Atribui o empate ao jogador A
    }
    players->playersData[search_id(players, board->playerA)].games++; // Atribui o jogo ao jogador A
    players->playersData[search_id(players, board->playerB)].games++; // Atribui o jogo ao jogador B
}

void end_game(BoardController *board, PlayersController *players) // Função que termina o jogo
{
    if (end_game_verify(board) != 0) // Verifica se o jogo terminou
    {
        if (end_game_verify(board) == 1) // Verifica se as casas do jogador A estão vazias
        {
            for (int i = 7; i < 13; i++) // Percorre as casas do jogador B
            {
                board->houses[13] += board->houses[i]; // Atribui as sementes do jogador B ao poço do jogador B
            }
        }
        else if (end_game_verify(board) == 2) // Verifica se as casas do jogador B estão vazias
        {
            for (int i = 0; i < 6; i++) // Percorre as casas do jogador A
            {
                board->houses[6] += board->houses[i]; // Atribui as sementes do jogador A ao poço do jogador A
            }
        }
        end_game_assign(board, players); // Atribui as vitorias, empates e derrotas
        board->gameStarted = false;      // Indica que o jogo terminou
    }
}

void resign_game(BoardController *board, PlayersController *players, char *nameA, char *nameB) // Função que desiste do jogo
{
    if (nameB == NULL) // Verifica se o nomeB é nulo
    {
        if (strcmp(nameA, board->playerA) == 0) // Se for, verifica se o nomeA é do jogador A
        {
            players->playersData[search_id(players, board->playerA)].losses++; // Atribui a derrota ao jogador A
            players->playersData[search_id(players, board->playerB)].wins++;   // Atribui a vitoria ao jogador B
        }
        else // Caso não seja
        {
            players->playersData[search_id(players, board->playerA)].wins++;   // Atribui a vitoria ao jogador A
            players->playersData[search_id(players, board->playerB)].losses++; // Atribui a derrota ao jogador B
        }
    }
    else // Caso não seja nulo
    {
        players->playersData[search_id(players, board->playerA)].losses++; // Atribui a derrota ao jogador A
        players->playersData[search_id(players, board->playerB)].losses++; // Atribui a derrota ao jogador B
    }
    players->playersData[search_id(players, board->playerA)].games++; // Atribui o jogo ao jogador A
    players->playersData[search_id(players, board->playerB)].games++; // Atribui o jogo ao jogador B
    board->gameStarted = false;                                       // Indica que o jogo terminou
}

int fix_position(int position, BoardController *board, char *name) // Função que corrige o indice da casa
{
    if (strcmp(board->playerA, name) == 0) // Verifica se o jogador é o A
    {
        return position - 1; // Caso seja, retorna o indice da casa
    }
    else if (strcmp(board->playerB, name) == 0) // Verifica se o jogador é o B
    {
        return position + 6; // Caso seja, retorna o indice da casa
    }
    return 0; // Caso não seja, retorna 0
}

void fix_table(int *index, BoardController *board, char *name) // Função que corrige o loop do array
{
    if (strcmp(board->playerA, name) == 0 && *index == 13) // Verifica se o jogador é o A e se o indice é o poço do jogador B
    {
        *index = 0; // Caso seja, ele pula essa casa
    }
    else if (strcmp(board->playerB, name) == 0 && *index == 6) // Verifica se o jogador é o B e se o indice é o poço do jogador A
    {
        *index = 7; // Caso seja, ele pula essa casa
    }
    else if (*index == 14) // Verifica se o indice é maior que 14
    {
        *index = 0; // Caso seja, ele volta para a casa 0
    }
}

void standard_play(int index, BoardController *board, char *name) // Função que realiza uma jogada padrão
{
    bool playAgainVerify = play_again_verify(index, board, name);  // Verifica se o jogador tem direito a outra jogada
    bool captureVerify = capture_seeds_verify(index, board, name); // Verifica se o jogador pode capturar as sementes

    int seeds = board->houses[index]; // Atribui o numero de sementes da casa a variavel seeds
    board->houses[index] = 0;         // Atribui 0 sementes a casa

    while (seeds > 0) // Enquanto houver sementes
    {
        index++;                        // Incrementa o indice
        fix_table(&index, board, name); // Corrige o indice da casa
        board->houses[index]++;         // Distribui as sementes
        seeds--;                        // Decrementa o numero de sementes
    }

    if (captureVerify == true) // Verifica se o jogador pode capturar as sementes
    {
        capture_seeds(index, board, name); // Captura as sementes
    }

    if (playAgainVerify == false) // Troca os rounds
    {
        if (strcmp(name, board->playerA) == 0) // Verifica se o jogador é o A e se ele não tem direito a outra jogada
        {
            board->round = 2; // Atribui o valor 2 ao round
        }
        else if (strcmp(name, board->playerB) == 0) // Verifica se o jogador é o B e se ele não tem direito a outra jogada
        {
            board->round = 1; // Atribui o valor 1 ao round
        }
    }
}

void auto_game(BoardController *board) // Função que realiza uma jogada automática
{
    if (board->difficulty == 1) // Verifica se o jogo é automático de nível normal
    {
        for (int i = 1; i < 7; i++) // Percorre todas as posições
        {
            int index = fix_position(i, board, board->playerB); // Corrige o indice da casa
            if (board->houses[index] != 0)                      // Verifica se a casa tem sementes
            {
                standard_play(index, board, board->playerB); // Caso tenha, joga
                break;                                       // Quebra o loop
            }
        }
    }
    else if (board->difficulty == 2) // Verifica se o jogo é automático de nível avançado
    {
        bool played = false; // Variavel que verifica se a CPU jogou

        for (int i = 1; i < 7; i++) // Percorre todas as posições
        {
            int index = fix_position(i, board, board->playerB);             // Corrige o indice da casa
            if (capture_seeds_verify(index, board, board->playerB) == true) // Verifica se a CPU pode capturar as sementes
            {
                standard_play(index, board, board->playerB); // Caso possa, joga
                played = true;                               // Indica que a CPU jogou
                break;                                       // Quebra o loop
            }
            else if (play_again_verify(index, board, board->playerB) == true) // Verifica se a CPU tem direito a outra jogada
            {
                standard_play(index, board, board->playerB); // Caso tenha, joga
                played = true;                               // Indica que a CPU jogou
                break;                                       // Quebra o loop
            }
        }

        if (played == false) // Verifica se a CPU não jogou
        {
            for (int i = 6; i > 0; i--) // Percorre todas as posições
            {
                int index = fix_position(i, board, board->playerB); // Corrige o indice da casa
                if (board->houses[index] != 0)                      // Verifica se a casa tem sementes
                {
                    standard_play(index, board, board->playerB); // Caso tenha, joga
                    break;                                       // Quebra o loop
                }
            }
        }
    }
}

int main()
{
    PlayersController players;  // Variavel que guarda o numero de jogadores e um ponteiro para o vetor de jogadores
    players.playersData = NULL; // Inicializado a NULL (me da jeito quando for fazer o realloc e verificar se ja foi alocado memoria)
    players.numPlayers = 0;     // Inicializado a 0
    BoardController board;      // Controlador do tabuleiro

    register_player(&players, "CPU", 0, 0, 0, 0); // Registra a CPU

    board.gameStarted = false; // Variavel que verifica se o jogo ja foi iniciado

    char *line = NULL; // Variavel que vai guardar a linha lida
    size_t len = 0;    // Variavel que vai guardar o tamanho da linha lida

    int initialTemp = 0;

    system("clear"); // Limpa a tela
    print_title();   // Imprime o titulo do jogo

    while (true) // Loop infinito
    {
        if (initialTemp == 0) // Verifica se é a primeira vez que o loop é executado
        {
            print_commands(); // Imprime os comandos
            initialTemp = 1;
        }
        printf("\n> "); // Imprime o prompt

        getline(&line, &len, stdin); // Recebe a linha
        system("clear");             // Limpa a tela

        line[strlen(line) - 1] = '\0'; // "RJ Bernardo\n\0" -> "RJ Bernardo\0"
        if (strlen(line) == 0)
        {
            break; // Termina o loop
        }

        print_title();                 // Imprime o titulo do jogo
        if (board.gameStarted == true) // Verifica se o jogo ja foi iniciado
        {
            print_board(&board); // Imprime o tabuleiro
            printf("\n");
        }

        char *operation = strtok(line, " "); // Recebe a operação

        if (strcmp(operation, "C") == 0)
        {
            // Comandos
            print_commands(); // Imprime os comandos
        }
        else if (strcmp(operation, "RJ") == 0)
        {
            // Registar jogador
            char *name = strtok(NULL, " ");
            if (name == NULL)
            {
                printf("Nome inválido.\n");
            }
            else if (has_player(&players, name) == true) // Verifica se o jogador ja esta registado
            {
                printf("Jogador existente.\n");
            }
            else // Caso o jogador não esteja registado
            {
                register_player(&players, name, 0, 0, 0, 0); // Regista o jogador
                printf("Jogador %s registado com sucesso.\n", name);
            }
            name = NULL; // Atribuimos o valor NULL novamente a variavel
        }
        else if (strcmp(operation, "LJ") == 0)
        {
            // Listar jogador
            if (players.numPlayers == 1) // Verifica se tem jogadores registados
            {
                printf("Sem jogadores registados.\n");
            }
            else // Caso tenha jogadores registados, lista os jogadores
            {
                qsort(players.playersData, players.numPlayers, sizeof(PlayerData), (int (*)(const void *, const void *))compare_players); // Ordena os jogadores
                print_players(&players);                                                                                                  // Lista os jogadores
            }
        }
        else if (strcmp(operation, "IJ") == 0)
        {
            // Iniciar jogo
            char *nameA = strtok(NULL, " ");
            char *nameB = strtok(NULL, " ");

            if (board.gameStarted == true) // Verifica se o jogo ja foi iniciado
            {
                printf("Existe um jogo em curso.\n");
            }
            else if (has_player(&players, nameA) == false || has_player(&players, nameB) == false) // Verifica se os jogadores existem
            {
                printf("Jogador inexistente.\n");
            }
            else // Caso o jogo não tenha sido iniciado
            {
                board.difficulty = 0;                       // Atribui o valor 0 a dificuldade (significa que não é jogo automático
                start_game(nameA, nameB, &board, &players); // Inicia o jogo
                print_board(&board);                        // Imprime o tabuleiro
                printf("\n");
                printf("Jogo iniciado com sucesso.\n");
            }
        }
        else if (strcmp(operation, "IJA") == 0)
        {
            // Iniciar jogo automático
            char *nameA = strtok(NULL, " ");
            char *nameB = "CPU";
            char *difficulty = strtok(NULL, " ");

            if (board.gameStarted == true) // Verifica se o jogo ja foi iniciado
            {
                printf("Existe um jogo em curso.\n");
            }
            else if (has_player(&players, nameA) == false) // Verifica se o jogador existe
            {
                printf("Jogador inexistente.\n");
            }
            else if (strcmp(difficulty, "Normal") != 0 && strcmp(difficulty, "Avançado") != 0) // Verifica a dificuldade
            {
                printf("Dificuldade inválida.\n");
            }
            else
            {
                if (strcmp(difficulty, "Normal") == 0) // Verifica a dificuldade
                {
                    board.difficulty = 1; // Caso seja normal, atribui o valor 1 a dificuldade
                }
                else if (strcmp(difficulty, "Avançado") == 0) // Verifica a dificuldade
                {
                    board.difficulty = 2; // Caso seja avançado, atribui o valor 2 a dificuldade
                }
                start_game(nameA, nameB, &board, &players); // Inicia o jogo
                print_board(&board);                        // Imprime o tabuleiro
                printf("\n");
                printf("Jogo automático de nível %s iniciado com sucesso.\n", difficulty);
            }
        }
        else if (strcmp(operation, "J") == 0)
        {
            // Efetuar Jogada
            char *name = strtok(NULL, " ");
            int position = atoi(strtok(NULL, " "));

            if (board.gameStarted == false) // Verifica se o jogo ja foi iniciado
            {
                printf("Não existe jogo em curso.\n");
            }
            else if (has_player(&players, name) == false) // Verifica se o jogador existe
            {
                printf("Jogador inexistente.\n");
            }
            else if (are_playing(&board, name) == false) // Verifica se o jogador está jogando
            {
                printf("Jogador não participa no jogo em curso.\n");
            }
            else if (can_play(&board, name) == false) // Verifica se é a vez do jogador
            {
                printf("Não é a vez do jogador %s.\n", name);
            }
            else if (position < 1 || position > 6) // Verifica se a casa é válida
            {
                printf("Casa inválida.\n");
            }
            else if (board.houses[fix_position(position, &board, name)] == 0) // Verifica se a casa tem sementes
            {
                printf("Casa sem sementes.\n");
            }
            else
            {
                int index = fix_position(position, &board, name);              // Corrige o indice da casa
                bool playAgainVerify = play_again_verify(index, &board, name); // Verifica se o jogador tem direito a outra jogada

                if (board.difficulty == 0 || (board.difficulty != 0 && board.round == 1)) // Verifica se o jogo é automático ou se o round é 1
                {
                    standard_play(index, &board, name); // Se for, realiza uma jogada padrão
                }
                while (board.round == 2 && board.difficulty != 0) // Verifica se o round é 2 e se o jogo não é automático
                {
                    auto_game(&board); // Se for, realiza uma jogada automática até a rodada mudar
                }

                end_game(&board, &players); // Verifica se o jogo terminou, e se terminou, atribui as vitorias, empates e derrotas

                if (board.gameStarted == false) // Caso tenha terminado
                {
                    printf("Jogo terminado.\n");
                    printf("%s %d\n", board.playerA, board.houses[6]);
                    printf("%s %d\n", board.playerB, board.houses[13]);
                }
                else
                {
                    if (playAgainVerify == true && strcmp(name, "CPU") != 0) // Verifica se o jogador tem direito a outra jogada (e se não é a CPU)
                    {
                        printf("O jogador %s tem direito a outra jogada.\n", name);
                    }
                    else if (strcmp(name, "CPU") != 0) // Caso não tenha direito a outra jogada (e se não é a CPU)
                    {
                        printf("Jogada efetuada com sucesso.\n");
                    }
                }
            }
        }
        else if (strcmp(operation, "D") == 0)
        {
            // Desistir do jogo
            char *nameA = strtok(NULL, " ");
            char *nameB = strtok(NULL, " ");

            if (board.gameStarted == false) // Verifica se o jogo ja foi iniciado
            {
                printf("Não existe jogo em curso.\n");
            }
            else if (has_player(&players, nameA) == false || has_player(&players, nameB) == false) // Verifica se os jogadores existem
            {
                printf("Jogador inexistente.\n");
            }
            else if (are_playing(&board, nameA) == false || are_playing(&board, nameB) == false) // Verifica se os jogadores estão jogando
            {
                printf("Jogador não participa no jogo em curso.\n");
            }
            else // Caso o jogo não tenha sido iniciado
            {
                resign_game(&board, &players, nameA, nameB); // Desiste do jogo
                printf("Jogo terminado com sucesso.\n");
            }
        }
        else if (strcmp(operation, "G") == 0)
        {
            // Gravar
            char *filename = strtok(NULL, " ");
            save_game(&players, filename); // Salva o jogo
            printf("Jogo gravado com sucesso.\n");
        }
        else if (strcmp(operation, "L") == 0)
        {
            // Ler
            if (board.gameStarted == true) // Verifica se o jogo ja foi iniciado
            {
                printf("Existe um jogo em curso.\n");
            }
            else // Caso o jogo não tenha sido iniciado
            {
                free_players(&players);             // Liberta a memoria do vetor de jogadores
                players.playersData = NULL;         // Atribui o valor NULL ao vetor de jogadores
                players.numPlayers = 0;             // Reinciializa o numero de jogadores
                char *filename = strtok(NULL, " "); // Separa a linha por espaços
                read_game(&players, filename);      // Lê o jogo
                printf("Jogo lido com sucesso.\n");
            }
        }
        else
        {
            printf("Instrução inválida.\n");
        }
        free(line);  // Liberta o espaço de memória presente na variável line
        line = NULL; // Atribuimos o valor NULL novamente a variavel
    }
    if (line != NULL) // Caso a variavel line seja diferente de NULL libertamos essa memoria
    {
        free(line);
    }
    free_players(&players); // Liberta a memoria do vetor de jogadores
    return 0;
}