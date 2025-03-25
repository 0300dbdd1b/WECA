#ifndef WECA_H
#define WECA_H

#include "platform.h"

#define	MAX_TAPE_SIZE			100
#define	MAX_WINDOW_NAME_SIZE	200
#define	MAX_COMMAND_SIZE		200
#define MAX_GENERATIONS			200


typedef enum BorderType
{
	FULL,
	EMPTY,
	CYCLE
}	BorderType;

typedef struct ElementaryAutomaton
{
	U8			tape[MAX_TAPE_SIZE];
	U16			tapeSize;
	BorderType	borderType;
} ElementaryAutomaton;
typedef ElementaryAutomaton Automaton;

typedef struct Console
{
	U8		active;
	char	input[MAX_COMMAND_SIZE];
	int		lenght;
}	Console;

typedef struct GlobalContext
{
	int					screenWidth;
	int					screenHeight;
	int					windowWidth;
	int					windowHeight;
	char				windowName[MAX_WINDOW_NAME_SIZE];
	ElementaryAutomaton	automaton;
	U8					rule;
	Console				console;

	U8					showGenerations;
	U8					generationCount;
	Automaton			generations[MAX_GENERATIONS];

	int					animationStep;
	int					animationFrameCount;
	int					animationSpeed;
	U8					animationRunning;
} GlobalContext;

extern GlobalContext gContext;

#endif // !WECA_H
