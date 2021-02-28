#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

struct file_mode
{
    char role;
    char *pattern;
    int success;
    int permission_map[3];
};

unsigned int omode;
char numbers[] = { '0', '1', '2', '3', '4', '5', '6', '7' };
mode_t modes[] = { S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH };
mode_t mode;
int i;
char file_mode_group;
char **ppstr;
int a_flag = 0, u_flag = 0, g_flag = 0, o_flag = 0;
int modifier_flag = 0;
int no_role = 0;


int is_all_role(char ch) {
    return ch == 'a';
}
int is_owner_role(char ch) {
    return ch == 'u';
}
int is_group_role(char ch) {
    return ch == 'g';
}
int is_other_role(char ch) {
    return ch == 'o';
}
int is_role_characters(char ch) {
    return is_all_role(ch) || is_owner_role(ch) || is_group_role(ch) || is_other_role(ch);
}

int is_read_permission(char ch) {
    return ch == 'r';
}
int is_write_permission(char ch) {
    return ch == 'w';
}
int is_execute_permission(char ch) {
    return ch == 'x';
}
int is_permission_characters(char ch) {
    return is_read_permission(ch) || is_write_permission(ch) || is_execute_permission(ch);
}

int is_group_modifier(char ch) {
    return ch == '=';
}
int is_public_modifier(char ch) {
    return ch == '+';
}
int is_private_modifier(char ch) {
    return ch == '-';
}
int is_modifier_character(char ch) {
    return is_public_modifier(ch) || is_private_modifier(ch);
}

char **split(const char *names, const char *delim)
{
    char *tmpVal = (char *)malloc(strlen(names) + 1);
    strcpy(tmpVal, names);

    char *str;
    str = strtok(tmpVal, delim);

    int wordCount = 1;
    char **words;
    words = (char**) malloc((wordCount + 1) * sizeof(char*));

    int strSize = strlen(str);  
    char *word = (char*) malloc((strSize + 1) * sizeof(char));
    strcpy(word, str);
    words[0] = word;
    
    while( str != NULL )
    {
        str = strtok(NULL, ",");

        word = NULL;
        if(str != NULL)
        {
            int strSize = strlen(str);         
            word = (char*) malloc((strSize + 1) * sizeof(char));
            strcpy(word, str);
            words[wordCount] = word;
        }
        words[wordCount] = word;

        wordCount++;
        words = (char**) realloc(words, (wordCount + 1) * sizeof(char*));
    }

    return words;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "wrong number of arguments!..\n");
		exit(EXIT_FAILURE);
	}

    if(!isnumber(argv[1][0])) {

        if ((ppstr = split(argv[1], ",")) == NULL) 
        {
            fprintf(stderr, "file mode pattern(s) cannot split!..\n");
            exit(EXIT_FAILURE);
        }

        int counter = 0;
        for (i = 0; ppstr[i] != NULL; ++i)
        {
            counter++;
        }

        struct file_mode filemodes[counter];

        for(i = 0; i < counter; i++) {

            a_flag = 0, u_flag = 0, g_flag = 0, o_flag = 0;
            modifier_flag = 0;
            no_role = 0;

            filemodes[i].success = 0;
            filemodes[i].pattern = ppstr[i];

            int j = 0;
            char first_ch = ppstr[i][j];
            if(is_role_characters(first_ch)) {
                if(is_all_role(first_ch)) {
                    a_flag = 1;
                    file_mode_group = 'a';
                }
                else if(is_owner_role(first_ch)) {
                    u_flag = 1;
                    file_mode_group = 'u';
                }
                else if(is_group_role(first_ch)) {
                    g_flag = 1;
                    file_mode_group = 'g';
                }
                else if(is_other_role(first_ch)) {
                    o_flag = 1;
                    file_mode_group = 'o';
                }
                else{
                    fprintf(stderr, " invalid role value: %c ...\n", first_ch);
                    exit(EXIT_FAILURE);
                }
                j++;
            }
            else if(is_modifier_character(first_ch)) {
                a_flag = 1;
                file_mode_group = 'a';
                if(is_public_modifier(first_ch)) {
                    modifier_flag = 1;
                    no_role = 1;
                }
                else if(is_private_modifier(first_ch)) {
                    modifier_flag = 0;
                    no_role = 1;
                }
                else{
                    fprintf(stderr," invalid modifier value: %c ...\n", first_ch);
                    exit(EXIT_FAILURE);
                }
                j++;
            }
            else {
                fprintf(stderr," file mode pattern is invalid: %s\n", ppstr[i]);
                exit(EXIT_FAILURE);
            }

            filemodes[i].role = file_mode_group;
            for(int k = 0; k < 3; k++) {
                filemodes[i].permission_map[k] = 0;
            }
            
            char cur_ch = ppstr[i][j];
            if( (!no_role && is_group_modifier(cur_ch)) ) {
                j++;
                cur_ch = ppstr[i][j];

                if( !(is_permission_characters(cur_ch) || is_modifier_character(cur_ch)) ) {
                    fprintf(stderr, " invalid modifier or file mode value: %c ...\n", cur_ch);
                    exit(EXIT_FAILURE);
                }
                modifier_flag = 1;
                if(is_private_modifier(cur_ch)) {
                    modifier_flag = 0;
                }
            }
            else if((!no_role && is_modifier_character(cur_ch))) {
                modifier_flag = is_public_modifier(cur_ch);
                j++;
                cur_ch = ppstr[i][j];
            }
            else {
                if(!no_role) {
                    fprintf(stderr, " file mode pattern is invalid: %s\n", ppstr[i]);
                    exit(EXIT_FAILURE);
                }
            }

            filemodes[i].success = 1;
            while(cur_ch != '\0') {
                
                if( !(is_permission_characters(cur_ch) || is_modifier_character(cur_ch)) ) {
                    fprintf(stderr, " file mode pattern is invalid: %s\n", ppstr[i]);
                    exit(EXIT_FAILURE);
                }

                if(is_modifier_character(cur_ch)) {
                    modifier_flag = is_public_modifier(cur_ch);
                    j++;
                    cur_ch = ppstr[i][j];
                }

                if(is_read_permission(cur_ch)) {
                    filemodes[i].permission_map[0] = modifier_flag;
                }
                else if(is_write_permission(cur_ch)) {
                    filemodes[i].permission_map[1] = modifier_flag;
                }
                else if(is_execute_permission(cur_ch)) {
                    filemodes[i].permission_map[2] = modifier_flag;
                }

                j++;
                cur_ch = ppstr[i][j];
                if(is_permission_characters(cur_ch)) {
                    modifier_flag = 1;
                }
            }
        }

        char mode_val_chs[4] = { numbers[0], numbers[0], numbers[0], '\0' };
        for(i = 0; i < counter; i++) {

            if(filemodes[i].success && filemodes[i].role == 'a') {
                
                int mode_val = 0;
                int total = 0;
                mode = 0;

                for(int t = 2; t >=0; t--) {
                    total += pow(2, t) * filemodes[i].permission_map[2-t];
                }
                char mode_val_ch = numbers[total];
                
                mode_val_chs[0] = mode_val_ch;
                mode_val_chs[1] = mode_val_ch;
                mode_val_chs[2] = mode_val_ch;                
            }
        }

        for(i = 0; i < counter; i++) {

            if(filemodes[i].success && filemodes[i].role != 'a') {
                
                int mode_val = 0;
                int total = 0;
                mode = 0;

                for(int t = 2; t >=0; t--) {
                    total += pow(2, t) * filemodes[i].permission_map[2-t];
                }
                char mode_val_ch = numbers[total];
                if(filemodes[i].role == 'u') {
                    mode_val_chs[0] = mode_val_ch;
                }
                else if(filemodes[i].role == 'g') {
                    mode_val_chs[1] = mode_val_ch;
                }
                else if(filemodes[i].role == 'o') {
                    mode_val_chs[2] = mode_val_ch;
                }
            }
        }

        sscanf(mode_val_chs, "%o", &omode);
        
        for (int t = 0; t < 9; ++t) {
            mode |= (omode >> (8 - t) & 1) ? modes[t] : 0;
        }   

        for (int z = 2; z < argc; ++z) {
            if (chmod(argv[z], mode) == -1)
                fprintf(stderr, "%s: %s\n", argv[z], strerror(errno));
        } 

        for (i = 0; ppstr[i] != NULL; ++i)
        {
            free(ppstr[i]);
        }

        free(ppstr);
        return 0;
    }

	if (strlen(argv[1]) > 3) {
		fprintf(stderr, "mode must have maximum 3 octal digits!..\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; argv[1][i] != '\0'; ++i)
		if (argv[1][i] < '0' || argv[1][i] > '7') {
			fprintf(stderr, "invalid octal digit!..\n");
			exit(EXIT_FAILURE);
		}

	sscanf(argv[1], "%o", &omode);
	mode = 0;

	for (i = 0; i < 9; ++i)
		mode |= (omode >> (8 - i) & 1) ? modes[i] : 0;

	for (i = 2; i < argc; ++i)
		if (chmod(argv[i], mode) == -1)
			fprintf(stderr, "%s: %s\n", argv[i], strerror(errno));

	return 0;
}