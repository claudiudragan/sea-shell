#pragma once
#define RUNNING 1

void analyzeCommand(char* cmd);
void linuxCommand(char* cmd);
void findSymbols(int* p, int* r, char* cmd);
int pipeCommands(int* p, char* cmd);
void removeSpaces(char* source);
char** splitCommand(char* cmd, int* p);
void childLinuxCommand(char* cmd);
void cdCommand(char* cmd);
void redirectCommand(char* cmd, int pip);
void removeLeadingSpace(char* src);
