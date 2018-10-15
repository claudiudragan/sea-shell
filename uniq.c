#include "includes.h"
#include "uniq.h"
#include <errno.h>
#define SIZE (1<<12)

/* 2. The "uniq" command. Parameters that need to be implemented are: -i, -d, -u */

int parseUniq(char* cmd){
	//Copy for strtok
	char* copy = (char*)malloc(sizeof(char) * strlen(cmd) + 1);

	memcpy(copy, cmd, strlen(cmd) + 1);

	char* tok;
	tok = strtok(copy, " ");

	//Only three args since you can only have uniq + filename + 1 option
	char* args[3];
	int i = 0;
	while(tok != NULL){
		args[i++] = tok;
		tok = strtok(NULL, " ");
	}

	int k = i;
	
	//Checking if command was written properly
	if((args[1][0] == '-') || k > 3){
		printf("\nUsage: uniq [Filename] [-Option]\n Available options: -i, -d, -u\n");
		return 1;
	}

	int file = open(args[1], O_RDONLY);
	
	//Prints an error if there are any when reading the file
	if(errno != 0)
		perror("Error");
	
	//If there are no errors runs command
	if(errno == 0){
		if(args[2]){
			//If there are optional arguments runs command with them otherwise passes null
			char* option = (char*)malloc(sizeof(char) * 3);
			memcpy(option, args[2], 3);

			execUniq(option, &file);
		}else{
			char option = '\0';

			execUniq(&option, &file);
		}
	}
	
	
	return 0;
}

int execUniq(char* option, int* file){
	pid_t pid;
	int fd[2];
	int n;

	if(pipe(fd) < 0){
		printf("Couldn't pipe for uniq");
		return -1;
	}
	
	pid = fork();

	if(pid < 0){
		printf("Couldn't fork for uniq");
		return -1;
	}

	if(pid == 0){
		//Child receives text from parent and runs command
		char* buff = (char*)malloc(sizeof(char) * (SIZE));

		close(fd[1]);
		while((n=read(fd[0], buff, SIZE-1)) > 0){
			buff[n] = '\0';
		}
		close(fd[0]);

		selectUniq(buff, option);
		
		if(buff)
			free(buff);

		exit(0);

	}else{
		//Parent reads file and sends it to child
		char* buff = (char*)malloc(sizeof(char) * (SIZE));

		close(fd[0]);
		while((n = read(*file, buff, SIZE)) > 0){
			write(fd[1], buff, n);
		}
		close(fd[1]);

		wait(NULL);
		
		if(buff)
			free(buff);
	}

	return 0;
}

int selectUniq(char* text, char* option){
	int o = 0;
	int ins = 0;

	//o variable for checking option so we don't run strcmp a million times
	if(strcmp(option, "-u") == 0){
		o = 1;
	}else if(strcmp(option, "-d") == 0){
		o = 2;
	}else if(*option == '\0' || strcmp(option, "-i") == 0){
		o = 3;
	}else{
		printf("Invalid option");
		return -1;
	}

	if(strcmp(option, "-i") == 0)
		ins = 1;


	//Cache for remembering lines to compare to
	char* cache = (char*)malloc(sizeof(char) * 512);

	//Copy of the text for strtok
	char* textcpy = (char*)malloc(sizeof(char) * strlen(text) + 1);

	memcpy(textcpy, text, strlen(text) + 1);

	char* tok = strchr(textcpy, '\n');
	int i = 0;

	//Count lines in file
	while(tok != NULL){
		i++;
		tok = strchr(tok+1, '\n');
	}
	
	//Initialise result array and appearance map for each line
	int n = i;
	char* result[n];
	int counter[n];

	for(i = 0; i < n; i++){
		counter[i] = 0;
	}

	int j = 0;

	char* tok2, *tok3;	

	tok2 = tok3 = textcpy;

	i = 0;

	while((tok3 = strchr(tok2, '\n'))){
		result[i] = (char*)malloc(sizeof(char) * (tok3 - tok2 + 1));
		memcpy(result[i], tok2, tok3 - tok2 + 1);
		result[i++][tok3 - tok2] = '\0';

		tok2 = tok3 + 1;
	}
	
	if(textcpy)
		free(textcpy);

	i = 0;
	cache = result[0];
	counter[0]++;

	//Check for consecutive hits on the same text

	if(ins == 0){
		for(i = 1; i < n; i++){
			if(strcmp(cache, result[i]) == 0){
				counter[j]++;
			}else{
				//if we find a different line, jump j over to the index of that line
				cache = result[i];
				j += counter[j];
				counter[j]++;

			}
		}
	}else{
		for(i = 1; i < n; i++){
			if(strcasecmp(cache, result[i]) == 0){
				counter[j]++;
			}else{
				//if we find a different line, jump j over to the index of that line
				cache = result[i];
				j += counter[j];
				counter[j]++;

			}
		}
	}

	i = 0;
	int first = 1;
	
	

	while(i < n){
		if(o == 1){
			//-u prints only unique lines
			if(counter[i] == 1){
				if(first){
					printf("%s", result[i]);
					first = 0;
				}else{
					printf("\n%s", result[i]);				
				}
			}
		}else if(o == 2){
			//-d prints only duplicate lines
			if(counter[i] > 1){
				if(first){
					printf("%s", result[i]);
					first = 0;
				}else{
					printf("\n%s", result[i]);				
				}
			}
		}else if(o == 3){
			//default prints both unique and duplicate lines only once
			if(counter[i] >= 1){
				if(first){
					printf("%s", result[i]);
					first = 0;
				}else{
					printf("\n%s", result[i]);				
				}
			}
		}
		
		i++;
	}
	
	if(cache)
		free(cache);

	for(i = 0; i < n; i++)
		if(result[i])
			free(result[i]);

	return 0;
}

