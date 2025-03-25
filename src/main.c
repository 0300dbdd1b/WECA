
#include "WECA.h"
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>


void DrawGenerations(int generationsCount);
void ComputeGenerations(int count);
ElementaryAutomaton CA_GetNextState(Automaton automaton);
GlobalContext	gContext;
void	InitGlobalContext(GlobalContext *ctx)
{
	ctx->windowHeight			=	700;
	ctx->windowWidth			=	1000;
	ctx->automaton.tapeSize		=	20;
	ctx->rule					=	110;
	ctx->showGenerations		=	0;
	ctx->generationCount		=	10;
	ctx->animationStep			=	0;
	ctx->animationFrameCount	=	0;
	ctx->animationRunning		=	0;
	strcpy(ctx->windowName, "Weca");
	ctx->automaton.borderType	= CYCLE;
	memset(ctx->automaton.tape, 0, MAX_TAPE_SIZE);
}


void	DrawConsole()
{
	if (!gContext.console.active)
		return ;
	int	consoleHeight	=	(gContext.windowHeight / 12);
	Rectangle rec = {	.x		= 0,
						.y		= gContext.windowHeight - consoleHeight,
						.width	= gContext.windowWidth,
						.height	= consoleHeight};
	DrawRectangleRec(rec, BLACK);
	DrawText(	gContext.console.input,
		  		rec.x,
		  		rec.y + rec.height/2,
		  		10,
		  		WHITE);
}

void	CommandSetTape()
{
	gContext.showGenerations = 0;
	char *cmd = gContext.console.input;
	size_t	i = 0;
	size_t	t = 0;
	while (cmd[i++] != ' ')
		continue;
	while (cmd[i] == '0' || cmd[i] == '1')
	{
		if (cmd[i] == '0')
			gContext.automaton.tape[t++] = 0;
		else if (cmd[i] == '1')
			gContext.automaton.tape[t++] = 1;
		i++;
	}
}

void	CommandSetRandomTape()
{
	gContext.showGenerations = 0;
	char *cmd = gContext.console.input;
	size_t	i = 0;
	int		value 	= 0;
	while (cmd[i++] != ' ')
		continue;
	value = atof(&cmd[i]);
	for (int j = 0; j < gContext.automaton.tapeSize; j++)
	{
		gContext.automaton.tape[j] =  (value >= GetRandomValue(0, 1)) ? 1 : 0;
	}
}

void	CommandSetTapeSize()
{
	gContext.showGenerations = 0;
	char *cmd = gContext.console.input;
	size_t	i = 0;
	int		value 	= 0;
	while (cmd[i++] != ' ')
		continue;
	while (cmd[i] >= '0' && cmd[i] <= '9')
	{
		value *= 10;
		value += cmd[i] - '0';
		i++;
	}
	gContext.automaton.tapeSize = value;
}

void	CommandSetSpeed()
{
	gContext.showGenerations = 0;
	char *cmd = gContext.console.input;
	size_t	i = 0;
	int		value 	= 0;
	while (cmd[i++] != ' ')
		continue;
	value = atof(&cmd[i]);
	gContext.animationSpeed = value;
}

void	CommandSetRule()
{
	char *cmd		= gContext.console.input;
	size_t	i		= 0;
	int		value	= 0;
	while (cmd[i++] != ' ')
		continue;
	while (cmd[i] >= '0' && cmd[i] <= '9')
	{
		value *= 10;
		value += cmd[i] - '0';
		i++;
	}
	gContext.rule	= value;
}

void	CommandRun()
{
	gContext.showGenerations = 1;
	char *cmd		= gContext.console.input;
	size_t	i		= 0;
	int		value	= 0;
	while (cmd[i++] != ' ')
		continue;
	while (cmd[i] >= '0' && cmd[i] <= '9')
	{
		value *= 10;
		value += cmd[i] - '0';
		i++;
	}
	gContext.generationCount = value;
	gContext.showGenerations = 1;
	gContext.animationRunning = 1;
	gContext.animationStep = 0;
	gContext.animationFrameCount = 0;
	ComputeGenerations(value);
}

void	HandleConsoleCommand()
{
	printf("HANDLE CONSOLE COMMAND : %s\n", gContext.console.input);
	if (strstr(gContext.console.input, "/RANDOM"))
	{
		CommandSetRandomTape();
	}
	else if (strstr(gContext.console.input, "/TAPESIZE"))
	{
		CommandSetTapeSize();
	}
	else if (strstr(gContext.console.input, "/RULE"))
	{
		CommandSetRule();
	}
	else if (strstr(gContext.console.input, "/SPEED"))
	{
		CommandSetSpeed();
	}
	else if (strstr(gContext.console.input, "/RUN"))
	{
		CommandRun();
	}
	else if (strstr(gContext.console.input, "/TAPE"))
	{
		CommandSetTape();
	}

 
	else
		return;
	return ;
}

void	ManageConsole()
{
	if (IsKeyPressed(KEY_SLASH) && !gContext.console.active)
	{
		gContext.console.active = 1;
		gContext.console.lenght	= 0;
		gContext.console.input[0] = '\0';
	}
	if (gContext.console.active)
	{
		int key	= GetKeyPressed();
		while (key > 0 && gContext.console.lenght < MAX_COMMAND_SIZE - 1)
		{
			if (key == KEY_BACKSPACE)
			{
				gContext.console.input[gContext.console.lenght - 1]		= '\0';
				gContext.console.lenght--;
			}
			else if (key == KEY_ENTER)
			{
				HandleConsoleCommand();
			}
			else if (key == KEY_ESCAPE)
			{
				gContext.console.active = 0;
			}
			else
			{
				gContext.console.input[gContext.console.lenght++]	= (char)key;
				gContext.console.input[gContext.console.lenght]		= '\0';
			}
			key = GetKeyPressed();
		}
		DrawConsole();
	}
}


void DrawTape()
{
	U16 tapeSize = gContext.automaton.tapeSize;
	float cellWidth = (float)gContext.windowWidth / tapeSize;
	float cellHeight = gContext.windowHeight / 3.0f; // Let's use 1/3 of the screen height

	for (U16 i = 0; i < tapeSize; i++)
	{
		float x = i * cellWidth;
		float y = gContext.windowHeight / 2.0f - cellHeight / 2.0f;

		U8 value = gContext.automaton.tape[i];
		Color fillColor = (value == 1) ? BLACK : RAYWHITE;
		DrawRectangle(x, y, cellWidth, cellHeight, fillColor);
		DrawRectangleLines(x, y, cellWidth, cellHeight, GRAY);
	}
}



void ComputeGenerations(int count)
{
	if (count > MAX_GENERATIONS)
		count = MAX_GENERATIONS;

	Automaton current = gContext.automaton;

	gContext.generations[0] = current;
	gContext.generationCount = count;

	for (int g = 1; g < count; g++)
	{
		current = CA_GetNextState(current);
		gContext.generations[g] = current;
	}
}


void DrawGenerations(int maxGenerations)
{
	const int tapeSize = gContext.automaton.tapeSize;
	const float cellWidth = (float)gContext.windowWidth / tapeSize;
	const float cellHeight = (float)gContext.windowHeight / maxGenerations;

	// Update animation step
	if (gContext.animationRunning)
	{
		gContext.animationFrameCount++;
		if (gContext.animationFrameCount >= gContext.animationSpeed)
		{
			gContext.animationFrameCount = 0;
			if (gContext.animationStep < gContext.generationCount)
				gContext.animationStep++;
			else
				gContext.animationRunning = false;
		}
	}

	// Draw up to current animation step
	int drawCount = gContext.animationRunning ? gContext.animationStep : gContext.generationCount;

	for (int g = 0; g < drawCount; g++)
	{
		for (int i = 0; i < tapeSize; i++)
		{
			float x = i * cellWidth;
			float y = g * cellHeight;

			Color fill = (gContext.generations[g].tape[i] == 1) ? BLACK : RAYWHITE;
			DrawRectangle(x, y, cellWidth, cellHeight, fill);
			DrawRectangleLines(x, y, cellWidth, cellHeight, GRAY);
		}
	}
}









ElementaryAutomaton CA_GetNextState(Automaton automaton)
{
	ElementaryAutomaton nextState;
	nextState.tapeSize = automaton.tapeSize;
	nextState.borderType = automaton.borderType;

	for (U16 i = 0; i < nextState.tapeSize; i++)
	{
		// Voisin gauche
		U8 left = (i == 0)
			? (automaton.borderType == CYCLE ? automaton.tape[nextState.tapeSize - 1] : 0)
			: automaton.tape[i - 1];

		// Cellule centrale
		U8 center = automaton.tape[i];

		// Voisin droit
		U8 right = (i == nextState.tapeSize - 1)
			? (automaton.borderType == CYCLE ? automaton.tape[0] : 0)
			: automaton.tape[i + 1];

		// Code binaire du triplet
		U8 neighborhood = (left << 2) | (center << 1) | right;

		// Extraire le bit correspondant depuis la règle
		U8 result = (gContext.rule >> neighborhood) & 1;

		nextState.tape[i] = result;
	}

	return nextState;
}

int main(int argc, char **argv)
{

	InitGlobalContext(&gContext);
	InitWindow(gContext.windowWidth, gContext.windowHeight, gContext.windowName);
	SetExitKey(KEY_F5);
	SetTargetFPS(30);
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(WHITE);
		if (gContext.showGenerations)
			DrawGenerations(gContext.generationCount);
		else
			DrawTape();
		ManageConsole();
		EndDrawing();
	}
	CloseWindow();
	return 0;
}

