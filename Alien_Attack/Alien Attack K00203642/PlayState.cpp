/*
//  PlayState.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 09/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.

	Modified by Joe O'Regan
		2017/02/16 Added Timer
		2017/02/25 Scores and name saves to file at end of level
*/

#include <iostream>
#include <fstream>
#include "PlayState.h"
#include "GameOverState.h"
#include "PauseState.h"
#include "Game.h"
#include "InputHandler.h"
#include "LevelParser.h"
#include "Level.h"
#include "BulletHandler.h"

const std::string PlayState::s_playID = "PLAY";
std::stringstream timeText;
bool nameEntered = false;

unsigned int lastTime = 0, currentTime, countdownTimer = 0;

void PlayState::update() {
	if (m_loadingComplete && !m_exiting) {
		if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE)) {
			TheGame::Instance()->getStateMachine()->pushState(new PauseState());
		}

		/*        if(TheInputHandler::Instance()->getButtonState(0, 8))
		//        {
		//            TheGame::Instance()->getStateMachine()->pushState(new PauseState());
		/        }
		*/
		TheBulletHandler::Instance()->updateBullets();

		if (TheGame::Instance()->getPlayerLives() == 0) {
			nameEntered = false;
			highScoreUpdate(TheGame::Instance()->getName(), Game::Instance()->getScore());
			TheGame::Instance()->getStateMachine()->changeState(new GameOverState());
		}

		if (pLevel != 0) {
			pLevel->update();
		}
	}
}


void PlayState::highScoreUpdate(std::string name, int score) {
	std::cout << name << " " << score << std::endl;

	const int numScores = 10;	// Number of players names & scores to store
	int eachScore = 0;
	std::string scoreString;

	std::ifstream infile;
	infile.open("scores.txt");

	std::cout << "Reading high scores from file" << std::endl;

	struct PlayerScores {
		std::string name;		// Players name
		int score;				// Player score
	};

	PlayerScores temp;			// Temp struct to swap score positions

	PlayerScores scoreTable[11];// Array of structs to hold player names & scores
								//for (int i = 0; i < 11; i++) {
								//	scoreTable[i] = { "", 0 };
								//}

								// Get the existing scores from the score.txt file
	if (infile.is_open()) {
		while (getline(infile, scoreTable[eachScore].name) && getline(infile, scoreString) && eachScore <= numScores) {
			scoreTable[eachScore].score = stoi(scoreString);
			++eachScore;

			std::cout << "eachScore: " << eachScore << std::endl;
		}

		infile.close();
	}

	std::cout << "eachScore before adding score to array " << eachScore << std::endl;

	// Add the current score to the end of the array
	scoreTable[eachScore].name = name;		// Up to 11th position
	scoreTable[eachScore].score = score;

	std::cout << "name added: " << scoreTable[eachScore].name << " score added " << scoreTable[eachScore].score << std::endl;

	// Sort the structure
	for (int i = 1; i < eachScore + 1; i++) {
		for (int j = 0; j < eachScore + 1 - i; j++) {
			if (scoreTable[j].score < scoreTable[j + 1].score) { // Sort Largest Score To Smallest in the struct
				temp = scoreTable[j];
				scoreTable[j] = scoreTable[j + 1];
				scoreTable[j + 1] = temp;
			}
		}
	}

	if (eachScore < 10) eachScore++; // display extra score if less than 10

									 // Write to scores file
	std::ofstream outfile;
	outfile.open("scores.txt");

	for (int i = 0; i < eachScore; i++) {
		outfile << scoreTable[i].name << std::endl << scoreTable[i].score << std::endl;
	}
}

std::string inputText = "Name";
bool quit = false;
unsigned int readyTextTimer;
bool renderText;
SDL_Event e;

void PlayState::render() {
	if (!nameEntered) {
		SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 0x00, 0x00, 0x00, 0xFF);								// Clear background
		TheTextureManager::Instance()->draw("enterNameID", 50, 100, 400, 20, TheGame::Instance()->getRenderer());		// Display enter name message
		TheTextureManager::Instance()->loadInputText(inputText, TheGame::Instance()->getRenderer());
		TheTextureManager::Instance()->drawText("inputTextID", 50, 150, TheGame::Instance()->getRenderer());

		renderText = false;		// flag that keeps track of whether we need to update the texture

								/*
								//while (SDL_PollEvent(&e) != 0) {
								//User requests quit
								if (e.type == SDL_QUIT) {
								quit = true;
								}
								//Special key input
								else if (e.type == SDL_KEYDOWN) {
								//Handle backspace
								//if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)	// backspace -> remove the last character from the string
								if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_BACKSPACE) && inputText.length() > 0)
								{
								//lop off character
								inputText.pop_back();
								renderText = true;		// Set the text update flag
								}
								//Handle copy
								//else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)	// Ctrl + c -> copy the text to the clip board
								else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_C) && SDL_GetModState() & KMOD_CTRL)	// Ctrl + c -> copy the text to the clip board
								{
								SDL_SetClipboardText(inputText.c_str());
								}
								//Handle paste
								else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_V) && SDL_GetModState() & KMOD_CTRL)	// Ctrl + v -> get the text from the clipboard
								{
								inputText = SDL_GetClipboardText();
								renderText = true;		// Set the text update flag
								}
								else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RETURN)) {
								TheGame::Instance()->setName(inputText);
								nameEntered = true;
								}
								}

								//Special text input event
								else if (e.type == SDL_TEXTINPUT) {
								//Not copy or pasting
								if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
								//Append character
								inputText += e.text.text;
								renderText = true;
								readyTextTimer = SDL_GetTicks();
								}
								}

								//}
								*/
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			//Special key input
			else if (e.type == SDL_KEYDOWN) {
				//Handle backspace
				if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)	// backspace -> remove the last character from the string
				{
					//lop off character
					inputText.pop_back();
					renderText = true;		// Set the text update flag
				}
				//Handle copy
				else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)	// Ctrl + c -> copy the text to the clip board
				{
					SDL_SetClipboardText(inputText.c_str());
				}
				//Handle paste
				else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)	// Ctrl + v -> get the text from the clipboard
				{
					inputText = SDL_GetClipboardText();
					renderText = true;		// Set the text update flag
				}
				else if (e.key.keysym.sym == SDLK_RETURN) {
					TheGame::Instance()->setName(inputText);
					nameEntered = true;
				}
			}
			//Special text input event
			else if (e.type == SDL_TEXTINPUT) {
				//Not copy or pasting
				if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
					//Append character
					inputText += e.text.text;
					renderText = true;
					readyTextTimer = SDL_GetTicks();
				}
			}
		}


		//Rerender text if needed
		if (renderText) {
			//Text is not empty
			if (inputText != "") {
				//Render new text
				//gInputTextTexture.loadFromRenderedText(inputText.c_str(), textColor);
				TheTextureManager::Instance()->loadInputText(inputText, TheGame::Instance()->getRenderer());
			}
			//Text is empty
			else {
				//Render space texture
				//gInputTextTexture.loadFromRenderedText(" ", textColor);	// Can't render an empty string, so has to be space
				TheTextureManager::Instance()->loadInputText(" ", TheGame::Instance()->getRenderer());
			}
		}
	}
	else if (nameEntered) {
		if (m_loadingComplete) {
			if (pLevel != 0) {
				pLevel->render();
			}

			// Display ready message for 2.5 seconds
			if (SDL_GetTicks() < readyTextTimer + 2500) {
				TheTextureManager::Instance()->loadReadyText("Get Ready " + inputText, TheGame::Instance()->getRenderer());
				TheTextureManager::Instance()->drawText("readyID", 210, 190, TheGame::Instance()->getRenderer());
			}

			gameTimer();	// Calculate and display the game timer

							//TheTextureManager::Instance()->loadFromRenderedText("Level 1", "testxxx", { 255, 255, 255, 255 }, TTF_OpenFont("Fonts/Retro.ttf", 20), TheGame::Instance()->getRenderer());		// Lives in top left corner

			TheTextureManager::Instance()->loadLevelText("Level 1", TheGame::Instance()->getRenderer());
			TheTextureManager::Instance()->drawText("levelID", 150, 0, TheGame::Instance()->getRenderer());

			for (int i = 0; i < TheGame::Instance()->getPlayerLives(); i++) {
				TheTextureManager::Instance()->drawFrame("lives", i * 30, 0, 32, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
			}

			// id, x, y, width, height, currentRow, currentFrame, pRenderer, angle, alpha, flip
			TheTextureManager::Instance()->drawFrame("timerID", 400, 0, 150, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
			//TheTextureManager::Instance()->drawText("testxxx", 100, 100, TheGame::Instance()->getRenderer());

			TheBulletHandler::Instance()->drawBullets();
		}
	}
}

bool PlayState::onEnter() {
	TheGame::Instance()->setPlayerLives(3);

	LevelParser levelParser;
	pLevel = levelParser.parseLevel(TheGame::Instance()->getLevelFiles()[TheGame::Instance()->getCurrentLevel() - 1].c_str());
	readyTextTimer = SDL_GetTicks();
	renderText = true;
	nameEntered = false;	// Input a name

	TheTextureManager::Instance()->loadEnterNameText("Please Enter Your Name:", TheGame::Instance()->getRenderer());


	TheTextureManager::Instance()->load("assets/bullet1.png", "bullet1", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/bullet2.png", "bullet2", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/bullet3.png", "bullet3", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/bullet4.png", "bullet4", TheGame::Instance()->getRenderer());	// 2017/03/14 Angry glider bullet
	TheTextureManager::Instance()->load("assets/lives.png", "lives", TheGame::Instance()->getRenderer());		// Lives in top left corner
																												//bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* rend, bool textWrapped = false);
																												//TheTextureManager::Instance()->load("assets/lives.png", "test", TheGame::Instance()->getRenderer());		// Lives in top left corner
																												//TheTextureManager::Instance()->loadFromRenderedText("Level 1", "testxxx", { 255, 255, 255, 255 }, TTF_OpenFont("Fonts/Retro.ttf", 20), TheGame::Instance()->getRenderer());		// Lives in top left corner

	if (pLevel != 0) {
		m_loadingComplete = true;
	}

	std::cout << "entering PlayState\n";
	return true;
}

bool PlayState::onExit() {
	m_exiting = true;

	TheInputHandler::Instance()->reset();
	TheBulletHandler::Instance()->clearBullets();

	countdownTimer = 0;

	std::cout << "exiting PlayState\n";
	return true;
}


void PlayState::gameTimer() {
	timeText.str("");						// Clear the timer

	currentTime = SDL_GetTicks();			// Set the current time

	if (currentTime > lastTime + 1000) {	// Every 1 second (1000ms)
		lastTime = currentTime;				// Set last time to existing time

		countdownTimer++;					// Increment countdown timer

		std::cout << "Timer: " << countdownTimer << std::endl;
	}

	TheGame::Instance()->setScore(countdownTimer);

	timeText << "Time: " << countdownTimer;	// Set the game timer

	TheTextureManager::Instance()->loadScoreText(timeText.str().c_str(), TheGame::Instance()->getRenderer());	// Player Score
																												//TheTextureManager::Instance()->loadFromRenderedText(timeText.str().c_str(), "timer", { 255, 255, 255, 255 }, TTF_OpenFont("Fonts/Retro.ttf", 12), TheGame::Instance()->getRenderer());		// Lives in top left corner
}